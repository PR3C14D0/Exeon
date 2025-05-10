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

void ResourceManager::GetResource(std::string resource, ComPtr<ID3D12Resource>& res) {
	if (this->ResourceExists(resource)) {
		res = this->m_resources[resource];
	}
	else {
		spdlog::error("Resource{0} not found", resource);
	}
}

bool ResourceManager::ResourceExists(std::string resource) {
	bool bRet = false;
	
	/* 
		Check if the `m_resources` map size is bigger than 0 (not empty)
		Then, check if there's a resource with the specified name. 
	*/
	if (this->m_resources.size() > 0) {
		if (this->m_resources.count(resource) > 0) {
			bRet = true;
		}
	}

	return bRet;
}

void ResourceManager::LoadTexture(const uint8_t* pData, DWORD dwDataSize, std::string texName, ComPtr<ID3D12Resource>& resource) {
	if (!this->AddResource(texName, resource)) {
		resource = this->m_resources[texName];
		return;
	}


	DirectX::ResourceUploadBatch batch(this->m_dev.Get());
	batch.Begin(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ThrowIfFailed(DirectX::CreateWICTextureFromMemory(this->m_dev.Get(), batch, pData, dwDataSize, resource.GetAddressOf()));
	D3D12* d3d12 = reinterpret_cast<D3D12*>(this->m_renderer);
	batch.End(d3d12->m_queue.Get());

	this->m_resources[texName] = resource;
}

void ResourceManager::LoadTextureFile(std::string texName, ComPtr<ID3D12Resource>& resource) {
	if (!this->AddResource(texName, resource)) {
		resource = this->m_resources[texName];
		return;
	}

	LPCWSTR wTexName = MultiByteToUnicode(texName.c_str());


	DirectX::ResourceUploadBatch batch(this->m_dev.Get());
	batch.Begin(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ThrowIfFailed(DirectX::CreateWICTextureFromFile(this->m_dev.Get(), batch, wTexName, resource.GetAddressOf()));
	D3D12* d3d12 = reinterpret_cast<D3D12*>(this->m_renderer);
	batch.End(d3d12->m_queue.Get());

	this->m_resources[texName] = resource;
}

bool ResourceManager::AddResource(std::string resource, ComPtr<ID3D12Resource>& res) {
	if (ResourceExists(resource)) {
		spdlog::error("Resource with name {0} already exists. Using the one already uploaded.", resource);
		return false;
	}

	spdlog::debug("Adding resource {0} to the resource manager.", resource);

	return true;
}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}