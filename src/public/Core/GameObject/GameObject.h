#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

#include "Core/GameObject/Component/Component.h"
#include "Core/GameObject/Component/Mesh.h"
#include "Math/Transform.h"
#include "Core/Input/Input.h"

class GameObject {
	friend class Scene;
private:
	std::string m_name;

	std::vector<Component*> m_components;
	Mesh* m_mesh;
protected:
	Input* m_input;
public:
	Transform transform;
	GameObject(std::string name);

	virtual void Init();
	virtual void Update();
	void Render();
};