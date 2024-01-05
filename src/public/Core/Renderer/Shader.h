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

class Shader {
private:
	void D3D12Shader(const char* shader, const char* vertexShader, const char* pixelShader);

	Renderer* m_renderer;

	LPVOID m_pBuffer;
	UINT m_nBufferLength;

public:
	Shader(const char* shader, const char* vertexShader, const char* pixelShader);
	~Shader() = default;

	UINT GetBuffer(LPVOID& pBuffer);
};