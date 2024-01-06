#include "Core/SceneManager.h"

SceneManager*  SceneManager::m_instance;

SceneManager::SceneManager() {

}

void SceneManager::Init() {

}

void SceneManager::Update() {

}

SceneManager* SceneManager::GetInstance() {
	if (SceneManager::m_instance == nullptr)
		SceneManager::m_instance = new SceneManager();
	return SceneManager::m_instance;
}