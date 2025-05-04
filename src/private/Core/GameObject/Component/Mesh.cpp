#include "Core/GameObject/Component/Mesh.h"
#include "Core/Core.h"
#include "Core/Renderer/ResourceManager.h"

Mesh::Mesh(std::string name) : Component::Component(name) {
	this->m_core = Core::GetInstance();

	this->m_dev = nullptr;
	this->m_list = nullptr;

	this->m_renderer = this->m_core->GetRenderer();

	this->m_bMeshLoaded = false;
}

void Mesh::Init() {
	Component::Init();


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

	spdlog::debug("{0}: Vertex Buffer Object initialized", this->m_name);
}

void Mesh::Update() {
	Component::Update();


}

void Mesh::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);

	this->UploadVertices();
}

/*
	Load a model from an FBX file with assimp.
*/
void Mesh::LoadModel(std::string filename) {
	if (this->m_bMeshLoaded) return;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, NULL);

	for (UINT i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Vertex> vertices;

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
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
			const aiTexture* texture = scene->GetEmbeddedTexture(texPath.C_Str());
			ResourceManager* resMgr = ResourceManager::GetInstance();
			ComPtr<ID3D12Resource> resource;
			resMgr->LoadTexture((BYTE*)texture->pcData, texture->mWidth, texture->mFilename.C_Str(), resource);
			this->m_textures[i] = resource;
		}
	}
}