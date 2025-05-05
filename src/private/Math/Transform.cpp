#include "Math/Transform.h""

Transform::Transform() {
	this->location = Vector3(0.f, 0.f, 0.f);
	this->rotation = Vector3(0.f, 0.f, 0.f);
	this->scale = Vector3(0.f, 0.f, 0.f);
}

void Transform::Translate(Vector3 v) {
	this->location = location + v;
}

void Transform::Translate(float x, float y, float z) {
	this->location = location + Vector3(x, y,z);
}

void Transform::Rotate(Vector3 v) {
	this->rotation = rotation + v;
}

void Transform::Rotate(float x, float y, float z) {
	this->rotation = rotation + Vector3(x, y, z);
}

Vector3 Transform::RotatePoint(Vector3 point) {
	return Vector3();
}