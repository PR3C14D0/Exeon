#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
#include "Util.h"

#include "Core/GameObject/Component/Component.h"

using namespace Microsoft::WRL;

/* Our core forward declaration */
class Core;
class Renderer;
class D3D12;

class Mesh : public Component {
private:
	Core* m_core;

	Renderer* m_renderer;

	void D3D12Init(D3D12* renderer);

	ComPtr<ID3D12Device1> m_dev;
	ComPtr<ID3D12GraphicsCommandList> m_list;

	std::vector<ComPtr<ID3D12Resource>> m_buffers;

	bool m_bMeshLoaded;

	std::map<UINT, std::vector<Vertex>> m_vertices;
public:
	Mesh(std::string name);

	void Init();
	void Update();

	void LoadModel(std::string filename);
};