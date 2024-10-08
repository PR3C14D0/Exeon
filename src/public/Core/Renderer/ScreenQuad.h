#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Util.h"
#include "Core/Renderer/Descriptor.h"
#include "Core/Renderer/Shader.h"

using namespace Microsoft::WRL;

class Core;
class Renderer;
class D3D12;

class ScreenQuad {
private:
	Core* m_core;
	void D3D12Init(D3D12* renderer);
	void D3D12Render(D3D12* renderer);

	Renderer* m_renderer;
	
	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	ComPtr<ID3D12Resource> m_VBO;
	ComPtr<ID3D12Resource> m_IBO;

	D3D12_VERTEX_BUFFER_VIEW m_VBV;
	D3D12_INDEX_BUFFER_VIEW m_IBV;

	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12GraphicsCommandList> m_list;

	UINT m_nRTVIndex;
	Descriptor m_rtvDescriptor;
	ComPtr<ID3D12Resource> m_rtvBuff;

	ComPtr<ID3D12RootSignature> m_rootSig;
	ComPtr<ID3D12PipelineState> m_plState;
	
	Shader* m_shader;
public:
	ScreenQuad();

	void GetResource(ComPtr<ID3D12Resource>& res);
	
	void Init();
	void Render();
};