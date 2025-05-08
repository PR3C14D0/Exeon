#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <DirectXTK/d3dx12.h>
#include <wrl.h>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
#include "Util.h"

#include "Core/GameObject/Component/Component.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Descriptor.h"
#include "Math/Transform.h"

using namespace Microsoft::WRL;

/* Our core forward declaration */
class Core;
class Renderer;
class D3D12;
class DescriptorHeap;
class SceneManager;

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
	std::map<UINT, std::vector<UINT>> m_indices;
	std::map <UINT, ComPtr<ID3D12Resource>> m_textures;
	std::map <UINT, ComPtr<ID3D12Resource>> m_ORMTextures;
	std::map<UINT, UINT> m_textureIndices;
	std::map<UINT, UINT> m_nORMIndices;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VBVs;
	std::vector<D3D12_INDEX_BUFFER_VIEW> m_IBVs;

	std::map<UINT, ComPtr<ID3D12Resource>> m_VBOs;
	std::map<UINT, ComPtr<ID3D12Resource>> m_IBOs;
	Shader* m_shader;

	ComPtr<ID3D12PipelineState> m_plState;

	ComPtr<ID3D12RootSignature> m_rootSig;

	DescriptorHeap* m_cbv_srvHeap;
	Descriptor m_samplerDescriptor;
	UINT m_nSamplerIndex;

	UINT m_nTotalVertices;

	ComPtr<ID3D12Resource> m_wvpRes;
	WVP m_wvp;
	UINT m_nWvpIndex;
	void InitConstantBuffer();
	void UpdateConstantBuffer();

	void UploadVertices();
	void InitPipeline();
	void InitSampler(D3D12* renderer);

	Transform* m_transform;
	SceneManager* m_sceneMgr;
public:
	Mesh(std::string name, Transform* parentTransform);

	void Init();
	void Update();

	void Render();

	void LoadModel(std::string filename);
};