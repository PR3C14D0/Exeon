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
		

	}
	else {
		this->m_input->ShowCursor(true);
	}
}