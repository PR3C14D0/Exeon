/*
	WIC texture loading from memory got from DirectXTK12.
	Special thanks to the DirectXTK12 and developers.

	GitHub repository: https://github.com/microsoft/DirectXTK12
*/

#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <DirectXTK/ResourceUploadBatch.h>
#include <DirectXTK/WICTextureLoader.h>

using namespace Microsoft::WRL;

/* Forward declarations */

class Core;
class Renderer;
class D3D12;

class ResourceManager {
private:
	static ResourceManager* m_instance;
	Core* m_core;
	Renderer* m_renderer;

	ComPtr<ID3D12Device1> m_dev;

	void D3D12Impl(D3D12* renderer);
public:
	ResourceManager();

	void Init();

	void LoadTexture(const uint8_t* pData, DWORD dwDataSize, ID3D12Resource** resource);
	static ResourceManager* GetInstance();
};