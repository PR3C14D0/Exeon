#include "Core/Renderer/D3D12.h"

D3D12::D3D12() : Renderer::Renderer() {
	
}

void D3D12::Init(HWND hwnd) {
	Renderer::Init(hwnd);

	UINT nFactoryFlags = 0;
#ifndef NDEBUG
	nFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	ComPtr<ID3D12Debug6> debug;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())));

	debug->EnableDebugLayer();
	debug->SetEnableGPUBasedValidation(TRUE);
#endif

	ThrowIfFailed(CreateDXGIFactory2(nFactoryFlags, IID_PPV_ARGS(this->m_factory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 scDesc = { };
}

void D3D12::Update() {
	Renderer::Update();
}
void D3D12::GetMostCapableAdapter(ComPtr<IDXGIAdapter>& adapter) {

}