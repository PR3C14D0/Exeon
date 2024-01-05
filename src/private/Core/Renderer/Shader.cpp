#include "Core/Renderer/Shader.h"
#include "Core/Core.h"

Shader::Shader(const char* shader, const char* vertexShader, const char* pixelShader) {
	Core* core = Core::GetInstance();
	this->m_renderer = core->GetRenderer();

	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Shader(renderer, shader, vertexShader, pixelShader);
	}
}

void Shader::D3D12Shader(D3D12* renderer, const char* shader, const char* vertexShader, const char* pixelShader) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);

	D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootDesc.NumParameters = 0;
	rootDesc.pParameters = nullptr;
	rootDesc.NumStaticSamplers = 0;
	rootDesc.pStaticSamplers = nullptr;

	ComPtr<ID3DBlob> rootBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootBlob.GetAddressOf(), nullptr));

	ComPtr<ID3D12RootSignature> rootSig;
	ThrowIfFailed(this->m_dev->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(rootSig.GetAddressOf())));

	ComPtr<ID3DBlob> vertexBlob, pixelBlob;
	ComPtr<ID3DBlob> vertexError, pixelError;

	LPCWSTR wShader = MultiByteToUnicode(shader);
	D3DCompileFromFile(wShader, nullptr, nullptr, vertexShader, "vs_5_1", NULL, NULL, vertexBlob.GetAddressOf(), vertexError.GetAddressOf());
	D3DCompileFromFile(wShader, nullptr, nullptr, pixelShader, "ps_5_1", NULL, NULL, pixelBlob.GetAddressOf(), pixelError.GetAddressOf());

	if (vertexError) {
		std::cout << "[ERROR] " << (char*)vertexError->GetBufferPointer() << std::endl;
		delete this;
		return;
	}

	if (pixelError) {
		std::cout << "[ERROR] " << (char*)pixelError->GetBufferPointer() << std::endl;
		delete this;
		return;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = { };
	
}