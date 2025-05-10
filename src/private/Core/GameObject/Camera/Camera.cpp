#include "Core/GameObject/Camera/Camera.h"

Camera::Camera(std::string name) : GameObject::GameObject(name) {

}

void Camera::Init() {
	GameObject::Init();


}

void Camera::Update() {
	GameObject::Update();
	if (this->transform.rotation.x >= 89.f) {
		this->transform.rotation.x = 89.f;
	}
	if (this->transform.rotation.x <= -89.f) {
		this->transform.rotation.x = -89.f;
	}
}