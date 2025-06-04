#include "Core/GameObject/Light/Light.h"
#include "Core/Core.h"

Light::Light(std::string name) : GameObject::GameObject(name) {
    /* White by default */
    this->m_lightColor[0] = 1.f;
    this->m_lightColor[1] = 1.f;
    this->m_lightColor[2] = 1.f;
    this->m_lightColor[3] = 1.f;
    this->m_core = Core::GetInstance();
    this->m_renderer = this->m_core->GetRenderer();
}

void Light::Init() {
    GameObject::Init();
    this->transform.Translate(1.f, 1.f, 0.f);

    if (D3D12* renderer = reinterpret_cast<D3D12*>(this->m_renderer)) {
        renderer->GetDevice(this->m_dev);
        renderer->GetCommandList(this->m_list);

        this->InitConstantBuffers(renderer);

        this->m_shader = new Shader("ShadowPass.hlsl", "VertexMain", "PixelMain");
        renderer->m_dsvHeap->Allocate(1);
        this->m_nDepthIndex = renderer->m_dsvHeap->GetLastDescriptorIndex();

        D3D12_RESOURCE_DESC depthBuffDesc = { };
        depthBuffDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthBuffDesc.SampleDesc.Count = 8;
        depthBuffDesc.Width = renderer->m_nWidth;
        depthBuffDesc.Height = renderer->m_nHeight;
        depthBuffDesc.MipLevels = 1;
        depthBuffDesc.DepthOrArraySize = 1;
        depthBuffDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        depthBuffDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        D3D12_HEAP_PROPERTIES heapProps = { };
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_CLEAR_VALUE dsvClear = { };
        dsvClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvClear.DepthStencil.Depth = 1.f;
        dsvClear.DepthStencil.Stencil = 0.f;

        ThrowIfFailed(this->m_dev->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthBuffDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &dsvClear,
            IID_PPV_ARGS(this->m_depth.GetAddressOf())
            ));

        this->m_depth->SetName(L"Shadow Map");

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = { };
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

        Descriptor descriptor = renderer->m_dsvHeap->GetDescriptor(this->m_nDepthIndex);

        this->m_dev->CreateDepthStencilView(this->m_depth.Get(), &dsvDesc, descriptor.cpuHandle);

        this->InitPipeline(renderer);
    }

}

void Light::InitPipeline(D3D12* renderer) {
    CD3DX12_DESCRIPTOR_RANGE wvpRange = { };
    wvpRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_ROOT_PARAMETER wvpParam;
    wvpParam.InitAsDescriptorTable(1, &wvpRange, D3D12_SHADER_VISIBILITY_VERTEX);

    D3D12_ROOT_PARAMETER rootParams[] = {
        wvpParam
    };

    D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootDesc.NumParameters = _countof(rootParams);
    rootDesc.pParameters = rootParams;
    rootDesc.NumStaticSamplers = 0;
    rootDesc.pStaticSamplers = nullptr;

    ComPtr<ID3DBlob> rootBlob, errBlob;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootBlob.GetAddressOf(), errBlob.GetAddressOf()));

    if (errBlob) {
        spdlog::error("Light: Root signature error {0}", errBlob->GetBufferPointer());
        return;
    }

    ThrowIfFailed(this->m_dev->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(this->m_rootSig.GetAddressOf())));

    D3D12_INPUT_ELEMENT_DESC elements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL }
    };

    LPVOID lpVertex = nullptr;
    UINT nVertexSize = this->m_shader->GetBuffer(SHADER_BUFFER::VERTEX, lpVertex);

    D3D12_INPUT_LAYOUT_DESC layout = { };
    layout.pInputElementDescs = elements;
    layout.NumElements = _countof(elements);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
    plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    plDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    plDesc.DepthStencilState.DepthEnable = TRUE;
    plDesc.DepthStencilState.StencilEnable = FALSE;
    plDesc.InputLayout = layout;
    plDesc.VS.pShaderBytecode = lpVertex;
    plDesc.VS.BytecodeLength = nVertexSize;
    // plDesc.PS.pShaderBytecode = lpPixel;
    // plDesc.PS.BytecodeLength = nPixelSize;
    plDesc.pRootSignature = this->m_rootSig.Get();
    plDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    plDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    plDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    plDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    plDesc.RasterizerState.FrontCounterClockwise = FALSE;
    plDesc.NumRenderTargets = 0;
    plDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    plDesc.SampleDesc.Count = 8;
    plDesc.SampleMask = UINT32_MAX;

    ThrowIfFailed(this->m_dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->m_plState.GetAddressOf())));
}

void Light::InitConstantBuffers(D3D12* renderer) {
    Transform transform = this->transform;
    XMVECTOR eye = XMVectorSet(
            transform.location.x,
            transform.location.y,
            transform.location.z,
            1.0f
        );

    float pitch = XMConvertToRadians(transform.rotation.x);
    float yaw = XMConvertToRadians(transform.rotation.y);

    XMVECTOR forward = XMVectorSet(
        cosf(pitch) * sinf(yaw),
        -sinf(pitch),
        -cosf(pitch) * cosf(yaw),
        0.0f
    );

    XMVECTOR at = XMVectorAdd(eye, forward);

    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    this->m_wvp.View = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));
    this->m_wvp.Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
            XMConvertToRadians(70.f),
            static_cast<float>(renderer->m_nWidth) / static_cast<float>(renderer->m_nHeight),
            0.001f,
            3000.f));
}


void Light::Update() {
    GameObject::Update();
}

void Light::Render() {
    this->InitConstantBuffers(dynamic_cast<D3D12*>(this->m_renderer));

    if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
        Descriptor dsvDesc = renderer->m_dsvHeap->GetDescriptor(this->m_nDepthIndex);
        this->m_list->ClearDepthStencilView(dsvDesc.cpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0.f, 0, nullptr);
        this->m_list->OMSetRenderTargets(0, nullptr, FALSE, &dsvDesc.cpuHandle);
        this->m_list->SetPipelineState(this->m_plState.Get());
        this->m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        this->m_list->SetGraphicsRootSignature(this->m_rootSig.Get());
    }

}
