#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

#include "Core/GameObject/Component/Component.h"
#include "Core/GameObject/Component/Mesh.h"

class GameObject {
private:
	std::string m_name;

	std::vector<Component*> m_components;
	Mesh* m_mesh;
public:
	GameObject(std::string name);

	void Init();
	void Update();
};