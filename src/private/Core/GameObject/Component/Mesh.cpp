#include "Core/GameObject/Component/Mesh.h"
#include "Core/Core.h"

Mesh::Mesh(std::string name) : Component::Component(name) {
	this->m_core = Core::GetInstance();

	this->m_dev = nullptr;
	this->m_list = nullptr;

	this->m_renderer = nullptr;
}

void Mesh::Init() {
	Component::Init();

	this->m_renderer = this->m_core->GetRenderer();

	if (D3D12* d3d12 = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Init(d3d12);
	}
}

void Mesh::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);


}