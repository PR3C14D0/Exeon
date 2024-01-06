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
	
	renderer->CreateTexture(renderer->m_nWidth, renderer->m_nHeight, 8, this->m_rtvBuff);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtvDesc.Texture2D.MipSlice = 1;
	rtvDesc.Texture2D.PlaneSlice = 1;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	renderer->m_dev->CreateRenderTargetView(this->m_rtvBuff.Get(), &rtvDesc, this->m_rtvDescriptor.cpuHandle);

	this->m_shader = new Shader("LightPass.hlsl", "VertexMain", "PixelMain");
}

void ScreenQuad::Render() {
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Render(renderer);
	}
} 

void ScreenQuad::D3D12Render(D3D12* renderer) {
	
}