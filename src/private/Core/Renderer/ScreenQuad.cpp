#include "Core/Renderer/ScreenQuad.h"
#include "Core/Core.h"

ScreenQuad::ScreenQuad() {
	this->m_core = Core::GetInstance();
	this->m_renderer = nullptr;
	this->m_shader = nullptr;
}

void ScreenQuad::Init() {
	this->m_renderer = this->m_core->GetRenderer();

	this->m_vertices = {
		{{-1.f, 1.f, 0.f}, {0.f, 0.f}},
		{{1.f, 1.f, 0.f}, {1.f, 0.f}},
		{{1.f, -1.f, 0.f}, {1.f, 1.f}},
		{{-1.f, -1.f, 0.f}, {0.f, 1.f}},
	};

	this->m_indices = {
		0, 1, 3,
		3, 1, 2
	};
	
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Init(renderer);
	}

}

void ScreenQuad::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);

	renderer->CreateBuffer(&this->m_vertices[0], this->m_vertices.size() * sizeof(Vertex), this->m_VBO);
	renderer->CreateBuffer(&this->m_indices[0], this->m_indices.size() * sizeof(UINT), this->m_IBO);

	this->m_VBV.BufferLocation = this->m_VBO->GetGPUVirtualAddress();
	this->m_VBV.SizeInBytes = this->m_vertices.size() * sizeof(Vertex);
	this->m_VBV.StrideInBytes = sizeof(Vertex);

	this->m_IBV.BufferLocation = this->m_IBO->GetGPUVirtualAddress();
	this->m_IBV.Format = DXGI_FORMAT_R32_UINT;
	this->m_IBV.SizeInBytes = this->m_indices.size() * sizeof(UINT);

	this->m_nRTVIndex = renderer->m_rtvHeap->GetDescriptorCount();
	renderer->m_rtvHeap->Allocate(1);

	this->m_rtvDescriptor = renderer->m_rtvHeap->GetDescriptor(this->m_nRTVIndex);
	
	renderer->CreateTexture(renderer->m_nWidth, renderer->m_nHeight, 8, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, this->m_rtvBuff);
	renderer->ResourceBarrier(this->m_rtvBuff.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtvDesc.Texture2D.MipSlice = 1;
	rtvDesc.Texture2D.PlaneSlice = 1;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	renderer->m_dev->CreateRenderTargetView(this->m_rtvBuff.Get(), &rtvDesc, this->m_rtvDescriptor.cpuHandle);

	this->m_shader = new Shader("LightPass.hlsl", "VertexMain", "PixelMain");

	Descriptor srvDesc = renderer->m_cbvSrvHeap->GetDescriptor(0);

	D3D12_DESCRIPTOR_RANGE gbufferRange = { };
	gbufferRange.NumDescriptors = 3;
	gbufferRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	gbufferRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	D3D12_ROOT_DESCRIPTOR_TABLE rootTable = { };
	rootTable.NumDescriptorRanges = 1;
	rootTable.pDescriptorRanges = &gbufferRange;

	D3D12_ROOT_PARAMETER rootParams = { };
	rootParams.DescriptorTable = rootTable;
	rootParams.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParams.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
	rootDesc.NumParameters = 1;
	rootDesc.pParameters = &rootParams;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootDesc.pStaticSamplers = nullptr;
	rootDesc.NumStaticSamplers = 0;

	ComPtr<ID3DBlob> rootBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootBlob.GetAddressOf(), nullptr));

	ThrowIfFailed(renderer->m_dev->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(this->m_rootSig.GetAddressOf())));

	void* vShader, *pShader;
	UINT nVertexLength = this->m_shader->GetBuffer(SHADER_BUFFER::VERTEX, vShader);
	UINT nPixelLength = this->m_shader->GetBuffer(SHADER_BUFFER::PIXEL, pShader);

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
	plDesc.VS.pShaderBytecode = vShader;
	plDesc.VS.BytecodeLength = nVertexLength;
	plDesc.PS.pShaderBytecode = pShader;
	plDesc.PS.BytecodeLength = nPixelLength;
	plDesc.pRootSignature = this->m_rootSig.Get();
	plDesc.DepthStencilState.DepthEnable = TRUE;
	plDesc.DepthStencilState.StencilEnable = FALSE;
	plDesc.InputLayout.NumElements = _countof(elements);
	plDesc.InputLayout.pInputElementDescs = elements;
	plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	plDesc.NodeMask = 0;
	plDesc.SampleDesc.Count = 8;
	plDesc.SampleMask = UINT32_MAX;
	plDesc.NumRenderTargets = 1;
	plDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;

	ThrowIfFailed(this->m_dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->m_plState.GetAddressOf())));
}

void ScreenQuad::Render() {
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Render(renderer);
	}
} 

void ScreenQuad::D3D12Render(D3D12* renderer) {
	this->m_list->OMSetRenderTargets(1, &this->m_rtvDescriptor.cpuHandle, FALSE, nullptr);
	this->m_list->SetPipelineState(this->m_plState.Get());
	this->m_list->ClearRenderTargetView(this->m_rtvDescriptor.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);

	this->m_list->IASetVertexBuffers(0, 1, &this->m_VBV);
	this->m_list->IASetIndexBuffer(&this->m_IBV);
	this->m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->m_list->DrawIndexedInstanced(this->m_indices.size(), 1, 0, 0, 0);
}