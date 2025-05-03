#include "Core/Renderer/ResourceManager.h"
#include "Core/Core.h"
#include "Core/Renderer/D3D12.h"

ResourceManager* ResourceManager::m_instance;

/* From DirectXTK12 */
struct WICTranslate {
	const GUID& wic;
	DXGI_FORMAT format;
	constexpr WICTranslate(const GUID& wg, DXGI_FORMAT fmt) noexcept :
		wic(wg),
		format(fmt) { }
};

struct WICConvert
{
	const GUID& source;
	const GUID& target;

	constexpr WICConvert(const GUID& src, const GUID& tgt) noexcept :
		source(src),
		target(tgt) {
	}
};

constexpr WICTranslate g_WICFormats[] =
{
	{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

	{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
	{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

	{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM },

	{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
	{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

	{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
	{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

	{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
	{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
	{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
	{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

	{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

	{ GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
};

constexpr WICConvert g_WICConvert[] =
{
	// Note target GUID in this conversion table must be one of those directly supported formats (above).

	{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

	{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

	{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

	{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT
	{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT

	{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

	{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

	{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

	{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

	{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT

	{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT

	{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

	{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT

	{ GUID_WICPixelFormat96bppRGBFixedPoint,   GUID_WICPixelFormat96bppRGBFloat }, // DXGI_FORMAT_R32G32B32_FLOAT

	// We don't support n-channel formats
};

DXGI_FORMAT WICToDXGI(const GUID& guid) noexcept {
	for (size_t i = 0; i < std::size(g_WICFormats); ++i) {
		if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0) {
			return g_WICFormats[i].format;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}



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

	UINT nWidth, nHeight;
	ThrowIfFailed(frame->GetSize(&nWidth, &nHeight));

	/* https://learn.microsoft.com/en-us/windows/win32/api/d3d12video/ns-d3d12video-d3d12_video_size_range */
	size_t maxSize = size_t(D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);

	UINT width = nWidth;
	UINT height = nHeight;

	/* If image is bigger than the max size allowed, clamp it. */
	if (nWidth > maxSize || nHeight > maxSize) {
		const float aspectRatio = static_cast<float>(nHeight) / static_cast<float>(nWidth);
		if (nWidth > nHeight) {
			width = static_cast<UINT>(maxSize);
			height = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxSize) * aspectRatio));
		}
		else {
			height = static_cast<UINT>(maxSize);
			width = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxSize) * aspectRatio));
		}
	}

	WICPixelFormatGUID pixelFormat;
	ThrowIfFailed(frame->GetPixelFormat(&pixelFormat));

	WICPixelFormatGUID convertGUID;
	memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &pixelFormat, sizeof(GUID));

	DXGI_FORMAT format = WICToDXGI(pixelFormat);

	/* Bits per pixel */
	UINT bpp = 0;

	if (format == DXGI_FORMAT_UNKNOWN) {
		for (size_t i = 0; i < std::size(g_WICConvert); ++i) {
			if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0) {
				memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &g_WICConvert[i].target, sizeof(GUID));

				format = WICToDXGI(convertGUID);
				ComPtr<IWICComponentInfo> cInfo;
				ThrowIfFailed(this->m_factory->CreateComponentInfo(convertGUID, cInfo.GetAddressOf()));

				WICComponentType type;
				ThrowIfFailed(cInfo->GetComponentType(&type));

				if (type != WICPixelFormat) {
					std::cout << "[ERROR] ResourceManager: Error getting Bits per pixel." << std::endl;
					return;
				}

				ComPtr<IWICPixelFormatInfo> pfInfo;
				ThrowIfFailed(cInfo.As(&pfInfo));

				ThrowIfFailed(pfInfo->GetBitsPerPixel(&bpp));

				break;
			}
		}
	}
	else {
		ComPtr<IWICComponentInfo> cInfo;
		ThrowIfFailed(this->m_factory->CreateComponentInfo(pixelFormat, cInfo.GetAddressOf()));

		WICComponentType type;
		ThrowIfFailed(cInfo->GetComponentType(&type));

		if (type != WICPixelFormat) {
			std::cout << "[ERROR] ResourceManager: Error getting Bits per pixel." << std::endl;
			return;
		}

		ComPtr<IWICPixelFormatInfo> pfInfo;
		ThrowIfFailed(cInfo.As(&pfInfo));

		ThrowIfFailed(pfInfo->GetBitsPerPixel(&bpp));
	}

	
}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::m_instance == nullptr)
		ResourceManager::m_instance = new ResourceManager();
	return ResourceManager::m_instance;
}