#include "Core/Renderer/Renderer.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "Util.h"
#include <vector>

using namespace Microsoft::WRL;

#define safe_release(p) { if( (p) ) { (p)->Release(); (p) = nullptr; } }

class D3D12 : public Renderer {
private:
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGIAdapter1> m_adapter;

	ComPtr<IDXGISwapChain1> m_sc;

	void GetMostCapableAdapter();

	D3D_FEATURE_LEVEL GetMaxFeatureLevel();
public:
	D3D12();
	
	void Init(HWND hwnd) override;
	void Update() override;
};