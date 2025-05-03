#include "Core/Renderer/ResourceManager.h"
#include "Core/Core.h"
#include "Core/Renderer/D3D12.h"

ResourceManager* ResourceManager::m_instance;

ResourceManager::ResourceManager() {
	this->m_core = Core::GetInstance();
	this->m_renderer = this->m_core->GetRenderer();
	if (D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Impl(d3d12);
	}
}

BOOL WINAPI InitWICFactory(PINIT_ONCE, PVOID, PVOID* factory) noexcept {
	return SUCCEEDED(CoCreateInstance(
		CLSID_WICImagingFactory2,
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory2),
		factory
	)) ? TRUE : FALSE;
}

void ResourceManager::D3D12Impl(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	
	ThrowIfFailed(this->m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->m_alloc.GetAddressOf())));
	this->m_alloc->SetName(L"ResourceManager command allocator");
	std::cout << "[DEBUG] ResourceManager: ID3D12CommandAllocator initialized" << std::endl;


	ThrowIfFailed(this->m_dev->CreateCommandList(1, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_alloc.Get(), nullptr, IID_PPV_ARGS(this->m_list.GetAddressOf())));
	this->m_list->SetName(L"ResourceManager Graphics Command List");
	std::cout << "[DEBUG] ResourceManager: ID3D12GraphicsCommandList initialized" << std::endl;

	static INIT_ONCE initOnce = INIT_ONCE_STATIC_INIT;
	IWICImagingFactory2* factory = nullptr;
	InitOnceExecuteOnce(&initOnce, InitWICFactory, nullptr, reinterpret_cast<LPVOID*>(&factory));
	this->m_factory.Attach(factory);

	std::cout << "[DEBUG] ResourceManager: IWICImagingFactory2 initialized" << std::endl;
}

void ResourceManager::Init() {

}

void ResourceManager::LoadTexture(const uint8_t* pData, DWORD dwDataSize, ID3D12Resource** resource) {
	if (resource) {
		*resource = nullptr;
	}

	if (!this->m_factory) {
		std::cout << "[ERROR] ResourceManager: IWICImagingFactory2 not initialized." << std::endl;
		return;
	}

	ComPtr<IWICStream> stream;

	ThrowIfFailed(this->m_factory->CreateStream(stream.GetAddressOf()));

	ThrowIfFailed(stream->InitializeFromMemory(const_cast<uint8_t*>(pData), dwDataSize));

	std::cout << "[DEBUG] ResourceManager: Stream initialized for texture loading." << std::endl;

	ComPtr<IWICBitmapDecoder> decoder;
	ThrowIfFailed(this->m_factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf()));

	std::cout << "[DEBUG] ResourceManager: Bitmap decoder initialized for texture loading." << std::endl;

	ComPtr<IWICBitmapFrameDecode> frame;
	ThrowIfFailed(decoder->GetFrame(0, frame.GetAddressOf()));

	std::cout << "[DEBUG] ResourceManager: Bitmap frame initialized for texture loading." << std::endl;

	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA initData;
}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}