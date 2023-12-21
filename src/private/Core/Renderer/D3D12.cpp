#include "Core/Renderer/D3D12.h"

D3D12::D3D12() : Renderer::Renderer() {
	
}

void D3D12::Init(HWND hwnd) {
	Renderer::Init(hwnd);

	UINT nFactoryFlags = 0;
#ifndef NDEBUG
	nFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	ComPtr<ID3D12Debug1> debug;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())));

	debug->EnableDebugLayer();
	debug->SetEnableGPUBasedValidation(TRUE);
#endif

	ThrowIfFailed(CreateDXGIFactory2(nFactoryFlags, IID_PPV_ARGS(this->m_factory.GetAddressOf())));

	this->GetMostCapableAdapter();
	D3D_FEATURE_LEVEL maxFeatureLevel = this->GetMaxFeatureLevel();

	ThrowIfFailed(D3D12CreateDevice(this->m_adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(this->m_dev.GetAddressOf())));

	ThrowIfFailed(this->m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->m_alloc.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 scDesc = { };
}

void D3D12::Update() {
	Renderer::Update();
}

void D3D12::GetMostCapableAdapter() {
	std::vector<ComPtr<IDXGIAdapter1>> adapters;
	{
		ComPtr<IDXGIAdapter1> adapter;
		for (int i = 0; this->m_factory->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++) {
			adapters.push_back(adapter);
		}
	}

	ComPtr<ID3D12Device> tempDevice;
	for (ComPtr<IDXGIAdapter1> adapter : adapters) {
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
			D3D_FEATURE_LEVEL_11_0, 
			IID_PPV_ARGS(tempDevice.GetAddressOf()
			)))) {
			this->m_adapter = adapter;
		}
	}

	return;
}

D3D_FEATURE_LEVEL D3D12::GetMaxFeatureLevel() {
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_2
	};

	D3D_FEATURE_LEVEL minFeatureLevel = featureLevels[0];

	ComPtr<ID3D12Device> tempDevice;
	ThrowIfFailed(
		D3D12CreateDevice(
			this->m_adapter.Get(),
			minFeatureLevel,
			IID_PPV_ARGS(tempDevice.GetAddressOf())
		)
	);

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureData = { };
	featureData.NumFeatureLevels = _countof(featureLevels);
	featureData.pFeatureLevelsRequested = featureLevels;

	ThrowIfFailed(tempDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureData, sizeof(featureData)));

	return featureData.MaxSupportedFeatureLevel;
}