#include "Core/Scene/Scene.h"

Scene::Scene(std::string name) {
	this->name = name;
	this->m_go = new GameObject("SampleObject");
	this->m_gameObjects.push_back(this->m_go);
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

void Scene::Render() {
	for (GameObject* object : this->m_gameObjects) {
		object->Render();
	}
}

void Scene::AddGameObject(GameObject* object) {
	this->m_gameObjects.push_back(object);
}