#include "Core/Renderer/Renderer.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "Util.h"
#include <vector>
#include "Core/Renderer/DescriptorHeap.h"
#include "Core/Renderer/Shader.h"

using namespace Microsoft::WRL;

enum VSYNC {
	DISABLED = 0,
	ENABLED = 1,
	MEDIUM = 2
};

class D3D12 : public Renderer {
	friend class ScreenQuad;
private:
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGIAdapter1> m_adapter;

	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12CommandQueue> m_queue;
	ComPtr<ID3D12GraphicsCommandList> m_list;
	ComPtr<ID3D12CommandAllocator> m_alloc;

	ComPtr<IDXGISwapChain3> m_sc;

	std::vector<ComPtr<ID3D12Resource>> m_backBuffers;
	ComPtr<ID3D12Resource> m_depthBuffer;

	UINT m_nActualBackBuffer;

	void GetMostCapableAdapter();

	D3D_FEATURE_LEVEL GetMaxFeatureLevel();

	ComPtr<ID3D12Fence> m_fence;
	UINT m_nCurrentFence;
	HANDLE m_hFence;
	void WaitFrame();

	DescriptorHeap* m_rtvHeap;
	DescriptorHeap* m_dsvHeap;

	ComPtr<ID3D12Resource> m_albedoBuff;
	ComPtr<ID3D12Resource> m_uvBuff;
	ComPtr<ID3D12Resource> m_positionBuff;

	UINT m_nAlbedoIndex;
	UINT m_nUVIndex;
	UINT m_nPositionIndex;

	UINT m_nBackBuffers;

	void ResourceBarrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState);

	VSYNC m_vsyncState;

	void InitDepth();

public:
	D3D12();
	
	void Init(HWND hwnd) override;
	void Update() override;

	void GetDevice(ComPtr<ID3D12Device1>& dev);
	void GetCommandList(ComPtr<ID3D12GraphicsCommandList>& list);

	void CreateBuffer(void* pData, UINT nLength, ComPtr<ID3D12Resource>& resource);
	void CreateTexture(UINT nWidth, UINT nHeight, UINT nSampleCount, D3D12_RESOURCE_FLAGS Flags, ComPtr<ID3D12Resource>& resource);
};