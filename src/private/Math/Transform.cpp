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

Vector3 Transform::Forward() {
	Vector3 v = Vector3(0, 0.f, 1.f);
	return this->RotatePoint(v);
}

Vector3 Transform::Right() {
	Vector3 v = Vector3(1.f, 0.f, 0.f);
	return this->RotatePoint(v);
}

Vector3 Transform::RotatePoint(Vector3 point) {
	XMMATRIX rot = XMMatrixTranspose(XMMatrixIdentity());
	rot *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->rotation.x)));
	rot *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(this->rotation.y)));
	rot *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(this->rotation.z)));

	XMVECTOR rotToPoint = XMVector4Transform(XMVectorSet(point.x, point.y, point.z, 1.f), rot);
	XMFLOAT4 rotToPointFloat;
	XMStoreFloat4(&rotToPointFloat, rotToPoint);

	return Vector3(rotToPointFloat.x, rotToPointFloat.y, rotToPointFloat.z);
}