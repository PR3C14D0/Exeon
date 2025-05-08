#include "Core/GameObject/Component/Mesh.h"
#include "Core/Core.h"
#include "Core/Renderer/ResourceManager.h"
#include "Core/Renderer/DescriptorHeap.h"
#include "Core/Scene/SceneManager.h"


Mesh::Mesh(std::string name, Transform* parentTransform) : Component::Component(name) {
	this->m_core = Core::GetInstance();
	this->m_sceneMgr = SceneManager::GetInstance();

	this->m_dev = nullptr;
	this->m_list = nullptr;

	this->m_renderer = this->m_core->GetRenderer();

	this->m_bMeshLoaded = false;
	this->m_nTotalVertices = 0;
	this->m_nSamplerIndex = -1; // I put -1 cuz 0 can actually be occupied.
	this->m_nWvpIndex = -1;
	this->m_nTotalVertices = 0;
	this->m_shader = nullptr;

	this->m_transform = parentTransform;

	UINT nWidth, nHeight = 0;
	this->m_core->GetWindowSize(nWidth, nHeight);

	this->m_wvp.World = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(this->m_transform->location.x, this->m_transform->location.y, this->m_transform->location.z));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->m_transform->rotation.x)));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(this->m_transform->rotation.y)));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(this->m_transform->rotation.z)));

	this->m_wvp.View = XMMatrixTranspose(XMMatrixIdentity());
	this->m_wvp.Projection = XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(70.f), static_cast<float>(nWidth) / static_cast<float>(nHeight), 0.01f, 3000.f));
}

void Mesh::Init() {
	Component::Init();
	Transform cameraTransform = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera()->transform;
	this->m_wvp.View = XMMatrixTranspose(XMMatrixIdentity());
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(cameraTransform.rotation.x)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(cameraTransform.rotation.y)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(cameraTransform.rotation.z)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixTranslation(cameraTransform.location.x, cameraTransform.location.y, cameraTransform.location.z));


	if (D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Init(d3d12);
	}
}

void Mesh::UploadVertices() {
	D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer);

	if (!d3d12) {
		spdlog::error("{0}: Failed uploading vertices. D3D12 Renderer not initialized.", this->m_name);
		throw std::exception();
		return;
	}

	spdlog::debug("{0}: Uploading vertices to GPU.", this->m_name);
	for (std::pair<UINT, std::vector<Vertex>> object : this->m_vertices) {
		ComPtr<ID3D12Resource> VBO;
		d3d12->CreateBuffer(&object.second[0], object.second.size() * sizeof(Vertex), VBO);
		VBO->SetName(L"StaticMesh VBO");
		spdlog::info("{0}: {1:d} vertices uploaded for mesh {2:d}", this->m_name, object.second.size(), object.first);

		this->m_VBOs[object.first] = VBO;
	}

	for (std::pair<UINT, std::vector<UINT>> object : this->m_indices) {
		ComPtr<ID3D12Resource> IBO;
		d3d12->CreateBuffer(&object.second[0], object.second.size() * sizeof(UINT), IBO);
		IBO->SetName(L"StaticMesh IBO");
		spdlog::info("{0}: {1:d} indices uploaded for mesh {2:d}", this->m_name, object.second.size(), object.first);

		this->m_IBOs[object.first] = IBO;
	}

	spdlog::debug("{0}: Vertex Buffer Objects initialized", this->m_name);
	spdlog::debug("{0}: Index Buffer Objects initialized", this->m_name);

	for (std::pair<UINT, ComPtr<ID3D12Resource>> VBO : this->m_VBOs) {
		D3D12_VERTEX_BUFFER_VIEW vbv = { };
		vbv.BufferLocation = VBO.second->GetGPUVirtualAddress();
		vbv.SizeInBytes = this->m_vertices[VBO.first].size() * sizeof(Vertex);
		vbv.StrideInBytes = sizeof(Vertex);

		this->m_VBVs.push_back(vbv);
	}

	for (std::pair<UINT, ComPtr<ID3D12Resource>> IBO : this->m_IBOs) {
		D3D12_INDEX_BUFFER_VIEW ibv = { };
		ibv.BufferLocation = IBO.second->GetGPUVirtualAddress();
		ibv.SizeInBytes = this->m_indices[IBO.first].size() * sizeof(UINT);
		ibv.Format = DXGI_FORMAT_R32_UINT;

		this->m_IBVs.push_back(ibv);
	}
}

void Mesh::InitConstantBuffer() {
	D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer);

	UINT nWVPSize = (sizeof(this->m_wvp) + 255) & ~255;

	d3d12->CreateBuffer(&this->m_wvp, nWVPSize, this->m_wvpRes);
	this->m_wvpRes->SetName(L"Mesh WVP Constant buffer");

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = { };
	cbvDesc.BufferLocation = this->m_wvpRes->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = nWVPSize;

	this->m_cbv_srvHeap->Allocate(1);
	this->m_nWvpIndex = this->m_cbv_srvHeap->GetLastDescriptorIndex();
	Descriptor wvpDesc = this->m_cbv_srvHeap->GetDescriptor(this->m_nWvpIndex);

	this->m_dev->CreateConstantBufferView(&cbvDesc, wvpDesc.cpuHandle);
}

void Mesh::UpdateConstantBuffer() {
	UINT nWVPSize = (sizeof(this->m_wvp) + 255) & ~255;
	this->m_wvp.World = XMMatrixTranspose(XMMatrixIdentity());
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->m_transform->rotation.x)));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(this->m_transform->rotation.y)));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(this->m_transform->rotation.z)));
	this->m_wvp.World *= XMMatrixTranspose(XMMatrixTranslation(
		this->m_transform->location.x, 
		this->m_transform->location.y,
		this->m_transform->location.z));

	Transform cameraTransform = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera()->transform;
	this->m_wvp.View = XMMatrixTranspose(XMMatrixIdentity());
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(cameraTransform.rotation.x)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(cameraTransform.rotation.y)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(cameraTransform.rotation.z)));
	this->m_wvp.View *= XMMatrixTranspose(XMMatrixTranslation(
		cameraTransform.location.x, 
		cameraTransform.location.y, 
		cameraTransform.location.z));
	
	
	PVOID pData;
	ThrowIfFailed(this->m_wvpRes->Map(0, nullptr, &pData));
	memcpy(pData, &this->m_wvp, nWVPSize);
	this->m_wvpRes->Unmap(0, nullptr);

	return;
}

void Mesh::Update() {
	Component::Update();

}

void Mesh::Render() {
	this->UpdateConstantBuffer();
	this->m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->m_list->SetPipelineState(this->m_plState.Get());
	this->m_list->SetGraphicsRootSignature(this->m_rootSig.Get());

	Descriptor wvpDesc = this->m_cbv_srvHeap->GetDescriptor(this->m_nWvpIndex);

	this->m_list->SetGraphicsRootDescriptorTable(0, this->m_samplerDescriptor.gpuHandle);
	this->m_list->SetGraphicsRootDescriptorTable(3, wvpDesc.gpuHandle);

	int i = 0;
	for (D3D12_VERTEX_BUFFER_VIEW vbv : this->m_VBVs) {
		D3D12_INDEX_BUFFER_VIEW ibv = this->m_IBVs[i];
		this->m_list->IASetVertexBuffers(0, 1, &vbv);
		this->m_list->IASetIndexBuffer(&ibv);

		UINT nTextureIndex = this->m_textureIndices[i];
		UINT nMetalIndex = this->m_nORMIndices[i];
		Descriptor texDesc = this->m_cbv_srvHeap->GetDescriptor(nTextureIndex);
		Descriptor metalDesc = this->m_cbv_srvHeap->GetDescriptor(nMetalIndex);
		this->m_list->SetGraphicsRootDescriptorTable(1, texDesc.gpuHandle);
		this->m_list->SetGraphicsRootDescriptorTable(2, metalDesc.gpuHandle);

		this->m_list->DrawIndexedInstanced(this->m_indices[i].size(), 1, 0, 0, 0);
		i++;
	}
}

void Mesh::InitPipeline() {
	this->m_shader = new Shader("GBufferPass.hlsl", "VertexMain", "PixelMain");

	LPVOID lpVertex, lpPixel = nullptr;
	UINT nVertexSize = this->m_shader->GetBuffer(SHADER_BUFFER::VERTEX, lpVertex);
	UINT nPixelSize = this->m_shader->GetBuffer(SHADER_BUFFER::PIXEL, lpPixel);

	CD3DX12_DESCRIPTOR_RANGE albedoRange;
	CD3DX12_DESCRIPTOR_RANGE ORMRange;
	CD3DX12_DESCRIPTOR_RANGE samplerRange;
	CD3DX12_DESCRIPTOR_RANGE wvpRange;
	albedoRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	ORMRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
	wvpRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_ROOT_PARAMETER albedoParam;
	CD3DX12_ROOT_PARAMETER ORMParam;
	CD3DX12_ROOT_PARAMETER samplerParam;
	CD3DX12_ROOT_PARAMETER wvpParam;
	albedoParam.InitAsDescriptorTable(1, &albedoRange, D3D12_SHADER_VISIBILITY_PIXEL);
	ORMParam.InitAsDescriptorTable(1, &ORMRange, D3D12_SHADER_VISIBILITY_PIXEL);
	samplerParam.InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
	wvpParam.InitAsDescriptorTable(1, &wvpRange, D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_ROOT_PARAMETER rootParams[] = {
		samplerParam,
		albedoParam,
		ORMParam,
		wvpParam
	};

	D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
	rootDesc.pParameters = rootParams;
	rootDesc.NumParameters = _countof(rootParams);
	rootDesc.pStaticSamplers = nullptr;
	rootDesc.NumStaticSamplers = 0;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> rsBlob, rsErrBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, rsBlob.GetAddressOf(), rsErrBlob.GetAddressOf()));

	if (rsErrBlob) {
		spdlog::error("{0}: Error serializing root signature. {1}", this->m_name, (char*)rsErrBlob->GetBufferPointer());
		return;
	}

	ThrowIfFailed(this->m_dev->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(this->m_rootSig.GetAddressOf())));

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL }
	};

	D3D12_INPUT_LAYOUT_DESC layout = { };
	layout.pInputElementDescs = elements;
	layout.NumElements = _countof(elements);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
	plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	plDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	plDesc.DepthStencilState.DepthEnable = TRUE;
	plDesc.DepthStencilState.StencilEnable = FALSE;
	plDesc.InputLayout = layout;
	plDesc.VS.pShaderBytecode = lpVertex;
	plDesc.VS.BytecodeLength = nVertexSize;
	plDesc.PS.pShaderBytecode = lpPixel;
	plDesc.PS.BytecodeLength = nPixelSize;
	plDesc.pRootSignature = this->m_rootSig.Get();
	plDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	plDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	plDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	plDesc.RasterizerState.FrontCounterClockwise = TRUE;
	plDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.RTVFormats[1] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.RTVFormats[2] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.RTVFormats[3] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.NumRenderTargets = 4;
	plDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	plDesc.SampleDesc.Count = 8;
	plDesc.SampleMask = UINT32_MAX;

	spdlog::debug("Vertex Shader Size: {}", nVertexSize);
	spdlog::debug("Pixel Shader Size: {}", nPixelSize);

	ThrowIfFailed(this->m_dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->m_plState.GetAddressOf())));
	this->m_plState->SetName(L"Mesh pipeline state");

	spdlog::debug("{0}: Pipeline state created.", this->m_name);
}

void Mesh::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);

	this->UploadVertices();
	this->InitPipeline();
	this->InitSampler(renderer);
	this->InitConstantBuffer();
}

void Mesh::InitSampler(D3D12* renderer) {
	this->m_cbv_srvHeap = renderer->m_cbvSrvHeap;
	UINT nNumTextures = this->m_textures.size();
	UINT nNumORMTextures = this->m_ORMTextures.size();
	this->m_cbv_srvHeap->Allocate(nNumTextures + nNumORMTextures);
	UINT nLastIndex = m_cbv_srvHeap->GetLastDescriptorIndex();
	UINT nFirstIndex = nLastIndex - nNumTextures;

	UINT nActualIndex = nFirstIndex;
	for (std::pair<UINT, ComPtr<ID3D12Resource>> resource : this->m_textures) {
		if (nActualIndex > nLastIndex) break;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = resource.second->GetDesc().Format;

		Descriptor resDesc = m_cbv_srvHeap->GetDescriptor(nActualIndex);
		this->m_textureIndices[resource.first] = nActualIndex;

		this->m_dev->CreateShaderResourceView(resource.second.Get(), &srvDesc, resDesc.cpuHandle);
		nActualIndex++;
	}

	for (std::pair<UINT, ComPtr<ID3D12Resource>> resource : this->m_ORMTextures) {
		if (nActualIndex > nLastIndex) break;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = resource.second->GetDesc().Format;

		Descriptor resDesc = m_cbv_srvHeap->GetDescriptor(nActualIndex);
		this->m_nORMIndices[resource.first] = nActualIndex;

		this->m_dev->CreateShaderResourceView(resource.second.Get(), &srvDesc, resDesc.cpuHandle);
		nActualIndex++;
	}

	renderer->m_samplerHeap->Allocate(1);
	this->m_nSamplerIndex = renderer->m_samplerHeap->GetLastDescriptorIndex();
	this->m_samplerDescriptor = renderer->m_samplerHeap->GetDescriptor(this->m_nSamplerIndex);

	D3D12_SAMPLER_DESC samplerDesc = { };
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 1;

	this->m_dev->CreateSampler(&samplerDesc, this->m_samplerDescriptor.cpuHandle);
}

/*
	Load a model from an FBX file with assimp.
*/
void Mesh::LoadModel(std::string filename) {
	if (this->m_bMeshLoaded) {
		spdlog::error("{0}: Tried to load another file when meshes already loaded", this->m_name);
		return;
	};

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, NULL);

	for (UINT i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Vertex> vertices;
		this->m_nTotalVertices += mesh->mNumVertices;

		for (UINT x = 0; x < mesh->mNumVertices; x++) {
			aiVector3D vec = mesh->mVertices[x];
			RGB pos = { vec.x, vec.y, vec.z };
			RGB normal = { 0.f, 0.f, 0.f };
			RG uv = { 0.f, 0.f };

			if (mesh->HasNormals()) {
				aiVector3D nml = mesh->mNormals[x];
				normal[0] = nml.x;
				normal[1] = nml.y;
				normal[2] = nml.z;
			}

			if (mesh->HasTextureCoords(0)) {
				aiVector3D texCoord = mesh->mTextureCoords[0][x];
				uv[0] = texCoord.x;
				uv[1] = texCoord.y;
			}

			Vertex vert = { {pos[0], pos[1], pos[2]}, {normal[0], normal[1], normal[2]}, {uv[0], uv[1]} };
			vertices.push_back(vert);
		}

		if (mesh->HasFaces()) {
			std::vector<UINT> indices;
			for (int x = 0; x < mesh->mNumFaces; x++) {
				const aiFace& face = mesh->mFaces[x];
				UINT nNumIndices = face.mNumIndices;

				for (int f = 0; f < nNumIndices; f++) {
					indices.push_back(face.mIndices[f]);
				}

			}
			this->m_indices[i] = indices;
		}

		this->m_vertices[i] = vertices;

		aiString texPath;
		aiString metalPath;
		if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
			const aiTexture* texture = scene->GetEmbeddedTexture(texPath.C_Str());
			if (texture != nullptr) {
				ResourceManager* resMgr = ResourceManager::GetInstance();
				ComPtr<ID3D12Resource> resource;
				resMgr->LoadTexture((BYTE*)texture->pcData, texture->mWidth, texture->mFilename.C_Str(), resource);
				resource->SetName(L"Mesh Base color");
				this->m_textures[i] = resource;
			}
		}

		if (material->GetTextureCount(aiTextureType_METALNESS) > 0 && material->GetTexture(aiTextureType_METALNESS, 0, &metalPath) == AI_SUCCESS) {
			const aiTexture* texture = scene->GetEmbeddedTexture(metalPath.C_Str());

			if (texture != nullptr) {
				ResourceManager* resMgr = ResourceManager::GetInstance();
				ComPtr<ID3D12Resource> resource;
				resMgr->LoadTexture((BYTE*)texture->pcData, texture->mWidth, texture->mFilename.C_Str(), resource);
				resource->SetName(L"Mesh Metallic Roughness");
				this->m_ORMTextures[i] = resource;
			}
		}
	}
}