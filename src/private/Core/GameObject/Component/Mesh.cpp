#include "Core/GameObject/Component/Mesh.h"
#include "Core/Core.h"

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
}