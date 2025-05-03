#pragma once
#include <iostream>
#include <Windows.h>
#include <wincodec.h>
#include <d3d12.h>

/* Forward declarations */
class Core;

class ResourceManager {
private:
	static ResourceManager* m_instance;

	ComPtr<IWICImagingFactory> m_factory;
public:
	ResourceManager();

	void LoadTexture(ID3D12Resource** resource);
	ResourceManager* GetInstance();
};