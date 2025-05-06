#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

#include "Core/GameObject/Component/Component.h"
#include "Core/GameObject/Component/Mesh.h"
#include "Math/Transform.h"
#include "Core/Input/Input.h"
#include "Core/Time.h"

class GameObject {
	friend class Scene;
private:
	std::string m_name;

	std::vector<Component*> m_components;
protected:
	Input* m_input;
	Time* m_time;
public:
	Transform transform;
	GameObject(std::string name);

	virtual void Init();
	virtual void Update();
	void Render();
};