#include "Core/Renderer/D3D12.h"

D3D12::D3D12() : Renderer::Renderer() {
	this->m_nBackBuffers = 2;
	this->m_nCurrentFence = 0;
	this->m_hFence = NULL;
	this->m_vsyncState = VSYNC::ENABLED;
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
	
	D3D12_COMMAND_QUEUE_DESC queueDesc = { };
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(this->m_dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(this->m_queue.GetAddressOf())));
	ThrowIfFailed(this->m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->m_alloc.GetAddressOf())));
	ThrowIfFailed(this->m_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_alloc.Get(), nullptr, IID_PPV_ARGS(this->m_list.GetAddressOf())));

	ThrowIfFailed(this->m_list->Close());

	DXGI_SWAP_CHAIN_DESC1 scDesc = { };
	scDesc.Width = this->m_nWidth;
	scDesc.Height = this->m_nHeight;
	scDesc.BufferCount = this->m_nBackBuffers;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SampleDesc.Count = 1;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	{
		ComPtr<IDXGISwapChain1> tempSc;
		ThrowIfFailed(this->m_factory->CreateSwapChainForHwnd(
			this->m_queue.Get(),
			this->m_hwnd,
			&scDesc,
			nullptr,
			nullptr,
			tempSc.GetAddressOf()
		));

		tempSc.As(&this->m_sc);
	}

	//this->m_sc->SetFullscreenState(TRUE, nullptr);

	this->m_rtvHeap = new DescriptorHeap(2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);

	for (int i = 0; i < this->m_nBackBuffers; i++) {
		ComPtr<ID3D12Resource> buff;
		ThrowIfFailed(this->m_sc->GetBuffer(i, IID_PPV_ARGS(buff.GetAddressOf())));

		this->m_backBuffers.push_back(buff);

		Descriptor rtv = this->m_rtvHeap->GetDescriptor(i);
		this->m_dev->CreateRenderTargetView(buff.Get(), nullptr, rtv.cpuHandle);
		buff->SetName(L"Back Buffer");
	}

	this->m_nActualBackBuffer = this->m_sc->GetCurrentBackBufferIndex();

	ThrowIfFailed(this->m_dev->CreateFence(this->m_nCurrentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(this->m_fence.GetAddressOf())));
	this->m_nCurrentFence++;

	std::cout << "[DEBUG] Initializing Depth buffer" << std::endl;
	this->InitDepth();
	std::cout << "[DEBUG] Depth buffer initialized" << std::endl;
}

void D3D12::InitDepth() {
	this->m_dsvHeap = new DescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);
	Descriptor dsv = this->m_dsvHeap->GetDescriptor(0);

	D3D12_RESOURCE_DESC depthBuffDesc = { };
	depthBuffDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBuffDesc.SampleDesc.Count = 8;
	depthBuffDesc.Width = this->m_nWidth;
	depthBuffDesc.Height = this->m_nHeight;
	depthBuffDesc.MipLevels = 1;
	depthBuffDesc.DepthOrArraySize = 1;
	depthBuffDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthBuffDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_HEAP_PROPERTIES heapProps = { };
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	
	D3D12_CLEAR_VALUE dsvClear = { };
	dsvClear.Format = depthBuffDesc.Format;
	dsvClear.DepthStencil.Depth = 1.f;
	dsvClear.DepthStencil.Depth = 1.f;

	ThrowIfFailed(this->m_dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthBuffDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClear,
		IID_PPV_ARGS(this->m_depthBuffer.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = { };
	dsvDesc.Format = depthBuffDesc.Format;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	
	this->m_dev->CreateDepthStencilView(
		this->m_depthBuffer.Get(),
		&dsvDesc,
		dsv.cpuHandle
	);

	this->m_depthBuffer->SetName(L"Depth buffer");
}

void D3D12::Update() {
	Renderer::Update();
	ThrowIfFailed(this->m_alloc->Reset());
	ThrowIfFailed(this->m_list->Reset(this->m_alloc.Get(), nullptr));

	Shader* tempShader = new Shader("Shader.hlsl", "VertexMain", "PixelMain");

	ComPtr<ID3D12Resource> actualBuffer = this->m_backBuffers[this->m_nActualBackBuffer];
	this->ResourceBarrier(actualBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Descriptor rtv = this->m_rtvHeap->GetDescriptor(this->m_nActualBackBuffer);
	this->m_list->OMSetRenderTargets(1, &rtv.cpuHandle, FALSE, nullptr);
	this->m_list->ClearRenderTargetView(rtv.cpuHandle, RGBA { 0.f, 0.f, 0.f, 1.f }, 0, nullptr);

	this->ResourceBarrier(actualBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	ThrowIfFailed(this->m_list->Close());

	ID3D12CommandList* commandLists[] = {
		this->m_list.Get()
	};

	this->m_queue->ExecuteCommandLists(_countof(commandLists), commandLists);

	this->m_sc->Present(this->m_vsyncState, 0);
	this->m_nActualBackBuffer = this->m_sc->GetCurrentBackBufferIndex();

	this->WaitFrame();
}

void D3D12::WaitFrame() {
	UINT nFence = this->m_nCurrentFence;
	this->m_nCurrentFence++;

	this->m_queue->Signal(this->m_fence.Get(), nFence);

	if (nFence > this->m_fence->GetCompletedValue()) {
		this->m_hFence = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		ThrowIfFailed(this->m_fence->SetEventOnCompletion(nFence, this->m_hFence));
		WaitForSingleObject(this->m_hFence, INFINITE);
	}

	return;
}

void D3D12::ResourceBarrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState) {
	D3D12_RESOURCE_BARRIER barrier = { };
	barrier.Transition.pResource = resource.Get();
	barrier.Transition.StateBefore = oldState;
	barrier.Transition.StateAfter = newState;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	this->m_list->ResourceBarrier(1, &barrier);

	return;
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

void D3D12::GetDevice(ComPtr<ID3D12Device1>& dev) {
	this->m_dev.CopyTo(&dev);
}

void D3D12::GetCommandList(ComPtr<ID3D12GraphicsCommandList>& list) {
	this->m_list.CopyTo(&list);
}