#pragma once
#include "Camera.h"

class EditorCamera : public Camera {
public:
	EditorCamera(std::string name);

	void Init() override;
	void Update() override;
};