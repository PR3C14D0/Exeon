#include "Core/GameObject/Camera/EditorCamera.h"

EditorCamera::EditorCamera(std::string name) : Camera(name) {
	this->m_fSensX = 5.f;
	this->m_fSensY = 5.f;

	this->m_fSpeed = 25.f;
}

void EditorCamera::Init() {
	Camera::Init();
}

void EditorCamera::Update() {
	Camera::Update();

	if(this->m_input->GetButtonDown(MOUSE_BUTTON::RIGHT)) {
		this->m_input->ShowCursor(false);
		
		this->transform.Rotate(this->m_input->deltaY * this->m_fSensY * -1 * this->m_time->deltaTime, this->m_input->deltaX * this->m_fSensX * this->m_time->deltaTime, 0.f);

		if (this->m_input->GetKeyDown('w')) {
			this->transform.Translate(this->transform.Forward() * this->m_fSpeed * this->m_time->deltaTime);
		}

		if (this->m_input->GetKeyDown('s')) {
			this->transform.Translate(this->transform.Forward() * this->m_fSpeed * -1 * this->m_time->deltaTime);
		}

		if (this->m_input->GetKeyDown('d')) {
			this->transform.Translate(this->transform.Right() * this->m_fSpeed * this->m_time->deltaTime);
		}

		if (this->m_input->GetKeyDown('a')) {
			this->transform.Translate(this->transform.Right() * this->m_fSpeed * -1 * this->m_time->deltaTime);
		}
	}
	else {
		this->m_input->ShowCursor(true);
	}
}