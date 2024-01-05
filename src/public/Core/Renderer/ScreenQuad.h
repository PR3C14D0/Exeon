#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Util.h"

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
public:
	ScreenQuad();
	
	void Init();
	void Render();
};