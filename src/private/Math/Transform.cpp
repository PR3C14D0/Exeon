#include "Math/Transform.h""

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