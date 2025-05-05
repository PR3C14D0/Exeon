#include "Math/Vector3.h"

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(const Vector3& v) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

Vector3 Vector3::operator+(const Vector3& v) {
	return Vector3(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3 Vector3::operator+(float v) {
	return Vector3(this->x + v, this->y + v, this->z + v);
}

Vector3 Vector3::operator-(const Vector3& v) {
	return Vector3(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector3 Vector3::operator-(float v) {
	return Vector3(this->x - v, this->y - v, this->z - v);
}

Vector3 Vector3::operator*(const Vector3& v) {
	return Vector3(this->x * v.x, this->y * v.y, this->z * v.z);
}

Vector3 Vector3::operator*(float v) {
	return Vector3(this->x * v, this->y * v, this->z * v);
}