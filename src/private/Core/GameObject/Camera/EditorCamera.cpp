#include "Core/GameObject/Camera/EditorCamera.h"

EditorCamera::EditorCamera(std::string name) : Camera(name) {

}

void EditorCamera::Init() {
	Camera::Init();
}

void EditorCamera::Update() {
	Camera::Update();

	if(this->m_input->GetButtonDown(MOUSE_BUTTON::RIGHT)) {
		this->m_input->ShowCursor(false);
		
		this->transform.Rotate(this->m_input->deltaY, this->m_input->deltaX, 0.f);

		if (this->m_input->GetKeyDown('w')) {
			this->transform.Translate(this->transform.Forward() * 0.1f);
		}

		if (this->m_input->GetKeyDown('s')) {
			this->transform.Translate(this->transform.Forward() * -0.1f);
		}

		if (this->m_input->GetKeyDown('d')) {
			this->transform.Translate(this->transform.Right() * 0.1f);
		}

		if (this->m_input->GetKeyDown('a')) {
			this->transform.Translate(this->transform.Right() * -0.1f);
		}
	}
	else {
		this->m_input->ShowCursor(true);
	}
}