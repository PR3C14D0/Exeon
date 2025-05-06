#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"

class Camera : public GameObject {
public:
	Camera(std::string name);

	void Init() override;
	void Update() override;
};