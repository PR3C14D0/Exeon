#include "Core/Scene/Scene.h"

Scene::Scene(std::string name) {
	this->name = name;
}

void Scene::Init() {
	for (GameObject* object : this->m_gameObjects) {
		object->Init();
	}
}

void Scene::Update() {
	for (GameObject* object : this->m_gameObjects) {
		object->Update();
	}
}

void Scene::AddGameObject(GameObject* object) {
	this->m_gameObjects.push_back(object);
}