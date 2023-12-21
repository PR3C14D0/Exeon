#include "Core/Renderer/D3D12.h"

D3D12::D3D12() : Renderer::Renderer() {
	
}

void D3D12::Init(HWND hwnd) {
	Renderer::Init(hwnd);

	CreateDXGIFactory

	DXGI_SWAP_CHAIN_DESC1 scDesc = { };
}

void D3D12::Update() {
	Renderer::Update();
}
void D3D12::GetMostCapableAdapter(ComPtr<IDXGIAdapter>& adapter) {

}