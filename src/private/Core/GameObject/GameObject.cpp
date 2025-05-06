#include "Core/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->m_name = name;
	this->m_input = Input::GetInstance();
	this->m_time = Time::GetInstance();
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

void GameObject::Render() {
	for (Component* component : this->m_components) {
		if (Mesh* mesh = dynamic_cast<Mesh*>(component)) {
			mesh->Render();
		}
	}
}