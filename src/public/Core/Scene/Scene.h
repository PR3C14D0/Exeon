#pragma once
#include <iostream>
#include <vector>
#include <map>

#include "Core/GameObject/GameObject.h"

class Scene {
private:
	std::vector<GameObject*> m_gameObjects;
public:
	std::string name;

	Scene(std::string name);

	void Init();
	void Update();

	void AddGameObject(GameObject* object);
};