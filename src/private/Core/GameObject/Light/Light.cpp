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

    if (D3D12* renderer = reinterpret_cast<D3D12*>(this->m_renderer)) {
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

        ThrowIfFailed(renderer->m_dev->CreateCommittedResource(
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

        renderer->m_dev->CreateDepthStencilView(this->m_depth.Get(), &dsvDesc, descriptor.cpuHandle);
    }

}

void Light::InitPipeline(D3D12* renderer) {
    CD3DX12_DESCRIPTOR_RANGE wvpRange = { };
    wvpRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_ROOT_PARAMETER wvpParam;
    wvpParam.InitAsConstantBufferView(0, D3D12_SHADER_VISIBILITY_VERTEX);

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
    this->m_lightBuffer.View = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));
    this->m_lightBuffer.Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
            XMConvertToRadians(70.f),
            static_cast<float>(renderer->m_nWidth) / static_cast<float>(renderer->m_nHeight),
            0.001f,
            3000.f));
}


void Light::Update() {
    GameObject::Update();
    this->InitConstantBuffers(dynamic_cast<D3D12*>(this->m_renderer));
}

void Light::Render() {

}
