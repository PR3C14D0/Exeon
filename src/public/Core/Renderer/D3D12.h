#include "Core/Renderer/Renderer.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "Util.h"
#include <vector>
#include "Core/Renderer/DescriptorHeap.h"

using namespace Microsoft::WRL;

class D3D12 : public Renderer {
private:
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGIAdapter1> m_adapter;

	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12CommandQueue> m_queue;
	ComPtr<ID3D12GraphicsCommandList> m_list;
	ComPtr<ID3D12CommandAllocator> m_alloc;

	ComPtr<IDXGISwapChain1> m_sc;

	void GetMostCapableAdapter();

	D3D_FEATURE_LEVEL GetMaxFeatureLevel();
public:
	D3D12();
	
	void Init(HWND hwnd) override;
	void Update() override;

	void GetDevice(ComPtr<ID3D12Device1>& dev);
};