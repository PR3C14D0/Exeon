#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <map>
#include "Core/Scene/Scene.h"
#include <spdlog/spdlog.h>

class SceneManager {
private:
	static SceneManager* m_instance;

	std::map<std::string, Scene*> m_scenes;
	Scene* m_actualScene;

	Scene* m_sampleScene;
public:
	static SceneManager* GetInstance();
	SceneManager();

	void Init();
	void Update();

	Scene* GetScene(std::string name);
	void AddScene(Scene* scene);

	void LoadScene(std::string name);
};