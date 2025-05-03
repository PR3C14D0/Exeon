#include "Core/Renderer/ResourceManager.h"
#include "Core/Core.h"
#include "Core/Renderer/D3D12.h"

ResourceManager* ResourceManager::m_instance;

ResourceManager::ResourceManager() {
	this->m_core = Core::GetInstance();
	this->m_renderer = this->m_core->GetRenderer();
	if (D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Impl(d3d12);
	}
}

void ResourceManager::D3D12Impl(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	
	ThrowIfFailed(this->m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->m_alloc.GetAddressOf())));
	this->m_alloc->SetName(L"ResourceManager command allocator");

	ThrowIfFailed(this->m_dev->CreateCommandList(1, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_alloc.Get(), nullptr, IID_PPV_ARGS(this->m_list.GetAddressOf())));
	this->m_list->SetName(L"ResourceManager Graphics Command List");
}

void ResourceManager::Init() {
}

void ResourceManager::LoadTexture(ID3D12Resource** resource) {

}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}