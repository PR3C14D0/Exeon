#include "Core/Scene/Scene.h"

Scene::Scene(std::string name) {
	this->name = name;
	this->m_go = new GameObject("SampleObject");
	this->AddGameObject(m_go);
	
	this->m_editorCamera = new Camera("EditorCamera");
	this->AddGameObject(this->m_editorCamera);
}

void Scene::Init() {
	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		object.second->Init();
	}
}

void Scene::Update() {
	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		object.second->Update();
	}
}

void Scene::Render() {
	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		object.second->Render();
	}
}

bool Scene::ObjectExists(std::string name) {
	if (this->m_gameObjects.size() > 0) {
		if (this->m_gameObjects.count(name) > 0)
			return true;
		else
			spdlog::error("Scene#{0}: GameObject {1} not found in the scene.", this->name, name);
	}
	else {
		spdlog::error("Scene#{0}: No GameObjects added on the Scene.", this->name);
	}

	return false;
}

void Scene::AddGameObject(GameObject* object) {
	if (!object)
		return;

	if (this->ObjectExists(object->m_name)) {
		spdlog::error("Scene#{0}: A GameObject with the name {1} already exists in the scene.", this->name, object->m_name);
		return;
	}

	if (Camera* cam = dynamic_cast<Camera*>(object))
		this->m_cameras[object->m_name] = cam;

	this->m_gameObjects[object->m_name] = object;
}