#include "Core/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->m_name = name;
}

void GameObject::Init() {
	for (Component* component : this->m_components) {
		component->Init();
	}
}

void GameObject::Update() {
	for (Component* component : this->m_components) {
		component->Update();
	}
}