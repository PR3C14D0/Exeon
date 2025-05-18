#include "Core/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->m_name = name;
	this->m_input = Input::GetInstance();
	this->m_time = Time::GetInstance();
	this->transform = Transform();
	this->transform.scale = Vector3(1.f, 1.f, 1.f);
}

void GameObject::Init() {
	//this->transform.Translate(0.f, 0.f, 0.f);
	for (Component* component : this->m_components) {
		component->Init();
	}
}

void GameObject::Update() {
	for (Component* component : this->m_components) {
		component->Update();
	}
}

Component* GameObject::GetComponent(std::string name) {
	return nullptr;
}

void GameObject::Render() {
	for (Component* component : this->m_components) {
		if (Mesh* mesh = dynamic_cast<Mesh*>(component)) {
			mesh->Render();
		}
	}
}