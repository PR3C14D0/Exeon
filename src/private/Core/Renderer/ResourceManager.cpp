#include "Core/Renderer/ResourceManager.h"

ResourceManager* ResourceManager::m_instance;

ResourceManager::ResourceManager() {

}

void ResourceManager::LoadTexture(ID3D12Resource** resource) {

}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}