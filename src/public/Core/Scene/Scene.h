#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <spdlog/spdlog.h>

#include "Core/GameObject/GameObject.h"
#include "Core/GameObject/Camera/Camera.h"
#include "Core/GameObject/Camera/EditorCamera.h"

class Scene {
	friend class Editor;
private:
	GameObject* m_go;

	std::map<std::string, GameObject*> m_gameObjects;
	std::map<std::string, Camera*> m_cameras;
	Camera* m_currentCamera;

	EditorCamera* m_editorCamera;
public:
	std::string name;

	Scene(std::string name);

	void Init();
	void Update();

	void Render();

	void SetCurrentCamera(std::string cameraName);
	Camera* GetCurrentCamera();

	GameObject* GetObject(std::string name);

	void AddGameObject(GameObject* object);
	bool ObjectExists(std::string name);
	bool CameraExists(std::string name);
};