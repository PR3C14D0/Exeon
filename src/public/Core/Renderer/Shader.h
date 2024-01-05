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
	void D3D12Shader(D3D12* renderer, const char* shader, const char* vertexShader, const char* pixelShader);

	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12GraphicsCommandList> m_list;
	Renderer* m_renderer;
public:
	Shader(const char* shader, const char* vertexShader, const char* pixelShader);
	~Shader() = default;
};