#include "Core/Renderer/Shader.h"
#include "Core/Core.h"

Shader::Shader(const char* shader, const char* vertexShader, const char* pixelShader) {
	this->m_pBuffer = nullptr;
	this->m_nBufferLength = 0;

	Core* core = Core::GetInstance();
	this->m_renderer = core->GetRenderer();

	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Shader(shader, vertexShader, pixelShader);
	}
}

void Shader::D3D12Shader(const char* shader, const char* vertexShader, const char* pixelShader) {
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

	this->m_pBuffer = vertexBlob->GetBufferPointer();
	this->m_nBufferLength = vertexBlob->GetBufferSize();
}

UINT Shader::GetBuffer(LPVOID& pBuffer) {
	pBuffer = this->m_pBuffer;
	return this->m_nBufferLength;
}