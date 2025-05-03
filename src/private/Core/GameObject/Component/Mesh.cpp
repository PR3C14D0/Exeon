#include "Core/GameObject/Component/Mesh.h"
#include "Core/Core.h"
#include "Core/Renderer/ResourceManager.h"

Mesh::Mesh(std::string name) : Component::Component(name) {
	this->m_core = Core::GetInstance();

	this->m_dev = nullptr;
	this->m_list = nullptr;

	this->m_renderer = nullptr;

	this->m_bMeshLoaded = false;
}

void Mesh::Init() {
	Component::Init();

	this->m_renderer = this->m_core->GetRenderer();

	if (D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Init(d3d12);
	}
}

void Mesh::Update() {
	Component::Update();


}

void Mesh::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);


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

		this->m_vertices[i] = vertices;

		aiString texPath;
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
			const aiTexture* texture = scene->GetEmbeddedTexture(texPath.C_Str());
			ResourceManager* resMgr = ResourceManager::GetInstance();
			ComPtr<ID3D12Resource> resource;
			resMgr->LoadTexture((BYTE*)texture->pcData, texture->mWidth, resource.GetAddressOf());

			std::cout << "ASD" << std::endl;
		}
	}
}