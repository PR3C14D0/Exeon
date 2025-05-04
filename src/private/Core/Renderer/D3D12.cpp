#include "Core/Renderer/D3D12.h"
#include "Core/Renderer/ScreenQuad.h"
#include "Core/Scene/SceneManager.h"

D3D12::D3D12() : Renderer::Renderer() {
	this->m_nBackBuffers = 2;
	this->m_nCurrentFence = 0;
	this->m_hFence = NULL;
	this->m_vsyncState = VSYNC::ENABLED;

	this->m_nAlbedoIndex = 0;
	this->m_nUVIndex = 0;
	this->m_nPositionIndex = 0;
	this->sceneMgr = SceneManager::GetInstance();
	this->m_samplerHeap = nullptr;
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

	/* Initialize a new Sampler descriptor heap with 1 descriptor for the moment */
	this->m_samplerHeap = new DescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, true);

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

	spdlog::debug("Initializing Depth buffer");
	this->InitDepth();
	spdlog::debug("Depth buffer initialized");

	this->CreateTexture(this->m_nWidth, this->m_nHeight, 8, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, this->m_albedoBuff);
	this->CreateTexture(this->m_nWidth, this->m_nHeight, 8, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, this->m_uvBuff);
	this->CreateTexture(this->m_nWidth, this->m_nHeight, 8, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, this->m_positionBuff);

	this->m_albedoBuff->SetName(L"Albedo");
	this->m_uvBuff->SetName(L"Normal");
	this->m_positionBuff->SetName(L"Position");
	
	this->m_nAlbedoIndex = this->m_rtvHeap->GetDescriptorCount();
	this->m_nUVIndex = this->m_nAlbedoIndex + 1;
	this->m_nPositionIndex = this->m_nUVIndex + 1;

	D3D12_RENDER_TARGET_VIEW_DESC GBufferDesc = { };
	GBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	GBufferDesc.Texture2D.MipSlice = 1;
	GBufferDesc.Texture2D.PlaneSlice = 1;
	GBufferDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	this->m_rtvHeap->Allocate(3);

	Descriptor albedoDesc = this->m_rtvHeap->GetDescriptor(this->m_nAlbedoIndex);
	Descriptor UVDesc = this->m_rtvHeap->GetDescriptor(this->m_nUVIndex);
	Descriptor positionDesc =  this->m_rtvHeap->GetDescriptor(this->m_nPositionIndex);

	this->m_dev->CreateRenderTargetView(this->m_albedoBuff.Get(), &GBufferDesc, albedoDesc.cpuHandle);
	this->m_dev->CreateRenderTargetView(this->m_uvBuff.Get(), &GBufferDesc, UVDesc.cpuHandle);
	this->m_dev->CreateRenderTargetView(this->m_positionBuff.Get(), &GBufferDesc, positionDesc.cpuHandle);

	this->ResourceBarrier(this->m_albedoBuff.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	this->ResourceBarrier(this->m_uvBuff.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	this->ResourceBarrier(this->m_positionBuff.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// We'll allocate 3 for our ScreenQuad.
	this->m_cbvSrvHeap = new DescriptorHeap(3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

	this->m_screenQuad = new ScreenQuad();
	this->m_screenQuad->Init();
	this->InitGBufferShader();

	ZeroMemory(&this->m_viewport, sizeof(D3D12_VIEWPORT));
	this->m_viewport.Width = this->m_nWidth;
	this->m_viewport.Height = this->m_nHeight;
	this->m_viewport.MaxDepth = 1.f;

	ZeroMemory(&this->m_scissor, sizeof(D3D12_RECT));
	this->m_scissor.left = 0;
	this->m_scissor.top = 0;
	this->m_scissor.right = this->m_nWidth;
	this->m_scissor.bottom = this->m_nHeight;

	ThrowIfFailed(this->m_list->Close());

	ID3D12CommandList* lists[] = {
		this->m_list.Get()
	};
	this->m_queue->ExecuteCommandLists(1, lists);
	this->WaitFrame();
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
	this->m_depthBuffer->SetName(L"Depth buffer");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = { };
	dsvDesc.Format = depthBuffDesc.Format;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	
	this->m_dev->CreateDepthStencilView(
		this->m_depthBuffer.Get(),
		&dsvDesc,
		dsv.cpuHandle
	);
}

void D3D12::Update() {
	Renderer::Update();
	ThrowIfFailed(this->m_alloc->Reset());
	ThrowIfFailed(this->m_list->Reset(this->m_alloc.Get(), nullptr));

	ComPtr<ID3D12Resource> actualBuffer = this->m_backBuffers[this->m_nActualBackBuffer];
	this->ResourceBarrier(actualBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Descriptor albedoDesc = this->m_rtvHeap->GetDescriptor(this->m_nAlbedoIndex);
	Descriptor UVDesc = this->m_rtvHeap->GetDescriptor(this->m_nUVIndex);
	Descriptor positionDesc = this->m_rtvHeap->GetDescriptor(this->m_nPositionIndex);
	Descriptor dsvDesc = this->m_dsvHeap->GetDescriptor(0);
	this->m_list->ClearRenderTargetView(albedoDesc.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);
	this->m_list->ClearRenderTargetView(UVDesc.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);
	this->m_list->ClearRenderTargetView(positionDesc.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);
	this->m_list->ClearDepthStencilView(dsvDesc.cpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0.f, 0, nullptr);

	this->m_list->RSSetViewports(1, &this->m_viewport);
	this->m_list->RSSetScissorRects(1, &this->m_scissor);

	D3D12_CPU_DESCRIPTOR_HANDLE gbuffers[] = {
		albedoDesc.cpuHandle,
		UVDesc.cpuHandle,
		positionDesc.cpuHandle
	};
	this->m_list->OMSetRenderTargets(_countof(gbuffers), gbuffers, FALSE, &dsvDesc.cpuHandle);

	ID3D12DescriptorHeap* descriptorHeaps[] = {
		this->m_cbvSrvHeap->m_heap.Get(),
		this->m_samplerHeap->m_heap.Get()
	};
	this->m_list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	this->sceneMgr->Render();

	Descriptor rtv = this->m_rtvHeap->GetDescriptor(this->m_nActualBackBuffer);
	this->m_list->OMSetRenderTargets(1, &rtv.cpuHandle, FALSE, nullptr);
	this->m_list->ClearRenderTargetView(rtv.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);

	this->ResourceBarrier(actualBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	ID3D12DescriptorHeap* sqDescriptorHeaps[] = {
		this->m_cbvSrvHeap->m_heap.Get()
	};
	this->m_list->SetDescriptorHeaps(_countof(sqDescriptorHeaps), sqDescriptorHeaps);

	/* We only call this method once per frame. */
	this->m_screenQuad->Render();

	ComPtr<ID3D12Resource> sqRes = nullptr;
	this->m_screenQuad->GetResource(sqRes);

	this->ResourceBarrier(sqRes, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	this->ResourceBarrier(this->m_backBuffers[this->m_nActualBackBuffer], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST);

	this->m_list->ResolveSubresource(this->m_backBuffers[this->m_nActualBackBuffer].Get(), 0, sqRes.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);

	this->ResourceBarrier(sqRes, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	this->ResourceBarrier(this->m_backBuffers[this->m_nActualBackBuffer], D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);

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

void D3D12::InitGBufferShader() {
	this->m_gBufferShader = new Shader("GBufferPass.hlsl", "VertexMain", "PixelMain");
	void* vertexShader, *pixelShader;
	UINT nVertexShaderLength = this->m_gBufferShader->GetBuffer(SHADER_BUFFER::VERTEX, vertexShader);
	UINT nPixelShaderLength = this->m_gBufferShader->GetBuffer(SHADER_BUFFER::PIXEL, pixelShader);

	D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootDesc.pStaticSamplers = nullptr;
	rootDesc.NumStaticSamplers = 0;

	ComPtr<ID3DBlob> rootBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootBlob.GetAddressOf(), nullptr));
	ThrowIfFailed(this->m_dev->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(this->m_rootSig.GetAddressOf())));
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

void D3D12::CreateBuffer(void* pData, UINT nLength, ComPtr<ID3D12Resource>& resource) {
	D3D12_RESOURCE_DESC buffDesc = { };
	buffDesc.DepthOrArraySize = 1;
	buffDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	buffDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	buffDesc.Height = 1;
	buffDesc.Width = nLength;
	buffDesc.MipLevels = 1;
	buffDesc.SampleDesc.Count = 1;
	buffDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES heapProps = { };
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	ThrowIfFailed(this->m_dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&buffDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf())
	));

	PVOID pMap = nullptr;
	resource->Map(0, nullptr, &pMap);
	memcpy(pMap, pData, nLength);
	resource->Unmap(0, nullptr);

	return;
}

void D3D12::CreateTexture(UINT nWidth, UINT nHeight, UINT nSampleCount, D3D12_RESOURCE_FLAGS Flags, ComPtr<ID3D12Resource>& resource) {
	D3D12_RESOURCE_DESC texDesc = { };
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.SampleDesc.Count = nSampleCount;
	texDesc.Width = nWidth;
	texDesc.Height = nHeight;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Flags = Flags;
	
	D3D12_HEAP_PROPERTIES heapProps = { };
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue = { };
	clearValue.Color[0] = 0.f;
	clearValue.Color[1] = 0.f;
	clearValue.Color[2] = 0.f;
	clearValue.Color[3] = 1.f;
	clearValue.Format = texDesc.Format;

	ThrowIfFailed(this->m_dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clearValue,
		IID_PPV_ARGS(resource.GetAddressOf())
	));
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