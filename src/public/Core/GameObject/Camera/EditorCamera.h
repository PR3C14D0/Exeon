#pragma once
#include "Camera.h"

class EditorCamera : public Camera {
private:
	float m_fSensX;
	float m_fSensY;

	float m_fSpeed;
public:
	EditorCamera(std::string name);

	void Init() override;
	void Update() override;
};