/*
	WIC texture loading from memory got from DirectXTK12.
	Special thanks to the DirectXTK12 and developers.

	GitHub repository: https://github.com/microsoft/DirectXTK12
*/

#pragma once
#include <iostream>
#include <Windows.h>
#include <wincodec.h>
#include <d3d12.h>
#include <wrl.h>

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

	ComPtr<IWICImagingFactory2> m_factory;
	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12GraphicsCommandList> m_list;
	ComPtr<ID3D12CommandAllocator> m_alloc;

	void D3D12Impl(D3D12* renderer);
public:
	ResourceManager();

	void Init();

	void LoadTexture(const uint8_t* pData, DWORD dwDataSize, ID3D12Resource** resource);
	static ResourceManager* GetInstance();
};