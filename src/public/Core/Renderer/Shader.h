#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;

class Core;
class Renderer;
class D3D12;

enum SHADER_BUFFER {
	VERTEX = 0,
	PIXEL = 1
};

class Shader {
private:
	void D3D12Shader(const char* shader, const char* vertexShader, const char* pixelShader);

	Renderer* m_renderer;

	LPVOID m_pVertexBuffer;
	UINT m_nVertexBufferLength;

	ComPtr<ID3DBlob> m_vertexBlob, m_pixelBlob;

	LPVOID m_pPixelBuffer;
	UINT m_nPixelBufferLength;

public:
	Shader(const char* shader, const char* vertexShader, const char* pixelShader);
	~Shader() = default;

	UINT GetBuffer(SHADER_BUFFER type, LPVOID& pBuffer);
};