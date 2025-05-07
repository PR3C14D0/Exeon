#pragma once
#include <iostream>

struct Vector3 {
	float x, y, z;
	Vector3() = default;
	Vector3(float x, float y, float z);
	Vector3(const Vector3& v);

	Vector3 operator+(const Vector3& v);
	Vector3 operator+(float v);

	Vector3 operator-(const Vector3& v);
	Vector3 operator-(float v);

	Vector3 operator*(const Vector3& v);
	Vector3 operator*(float v);
};