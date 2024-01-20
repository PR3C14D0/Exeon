#include "Core/Renderer/Shader.h"
#include "Core/Core.h"

Shader::Shader(const char* shader, const char* vertexShader, const char* pixelShader) {
	this->m_pVertexBuffer = nullptr;
	this->m_pPixelBuffer = nullptr;
	this->m_nVertexBufferLength = 0;
	this->m_nPixelBufferLength = 0;

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

	this->m_pVertexBuffer = vertexBlob->GetBufferPointer();
	this->m_nVertexBufferLength = vertexBlob->GetBufferSize();
	this->m_pPixelBuffer = pixelBlob->GetBufferPointer();
	this->m_nPixelBufferLength = pixelBlob->GetBufferSize();
}

UINT Shader::GetBuffer(SHADER_BUFFER type, LPVOID& pBuffer) {
	if (type == SHADER_BUFFER::VERTEX) {
		pBuffer = this->m_pVertexBuffer;
		return this->m_nVertexBufferLength;
	}
	else if (type == SHADER_BUFFER::PIXEL) {
		pBuffer = this->m_pPixelBuffer;
		return this->m_nPixelBufferLength;
	}

	return 0;
}