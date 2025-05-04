#include "Core/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->m_name = name;
	
}

void GameObject::Init() {
	this->m_mesh = new Mesh("StaticMeshComponent");
	this->m_components.push_back(this->m_mesh);
	this->m_mesh->LoadModel("f16.fbx");
	for (Component* component : this->m_components) {
		component->Init();
	}
}

void GameObject::Update() {
	for (Component* component : this->m_components) {
		component->Update();
	}
}