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
}

void ResourceManager::Init() {

}

void ResourceManager::LoadTexture(const uint8_t* pData, DWORD dwDataSize, ID3D12Resource** resource) {
	DirectX::ResourceUploadBatch batch(this->m_dev.Get());
	batch.Begin(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ThrowIfFailed(DirectX::CreateWICTextureFromMemory(this->m_dev.Get(), batch, pData, dwDataSize, resource));
	D3D12* d3d12 = reinterpret_cast<D3D12*>(this->m_renderer);
	batch.End(d3d12->m_queue.Get());
}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}