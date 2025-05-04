#include "Core/Scene/SceneManager.h"

SceneManager* SceneManager::m_instance;

SceneManager::SceneManager() {
	this->m_sampleScene = new Scene("SampleScene");
	this->m_actualScene = m_sampleScene;
}

void SceneManager::Init() {
	this->m_actualScene->Init();
}

void SceneManager::Update() {
	if (this->m_actualScene) {
		this->m_actualScene->Update();
	}
}

void SceneManager::AddScene(Scene* scene) {
	if (this->m_scenes.count(scene->name) > 0) {
		MessageBox(NULL, "Scene with that name already exists", "Error", MB_OK | MB_ICONERROR);
		spdlog::error("Scene with name {0} already exists", scene->name);
		return;
	}

	this->m_scenes[scene->name] = scene;
}

Scene* SceneManager::GetScene(std::string name) {
	if (!(this->m_scenes.count(name) > 0)) {
		MessageBox(NULL, "Scene not found", "Error", MB_OK | MB_ICONERROR);
		spdlog::error("Scene with name {0} not found", name);
		return nullptr;
	}

	return this->m_scenes[name];
}

void SceneManager::LoadScene(std::string name) {
	if (!(this->m_scenes.count(name) > 0)) {
		MessageBox(NULL, "Scene not found", "Error", MB_OK | MB_ICONERROR);
		spdlog::error("Scene not found");
		return;
	}

	this->m_actualScene = this->m_scenes[name];
}

SceneManager* SceneManager::GetInstance() {
	if (SceneManager::m_instance == nullptr)
		SceneManager::m_instance = new SceneManager();
	return SceneManager::m_instance;
}