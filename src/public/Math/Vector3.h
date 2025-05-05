#pragma once
#include <iostream>


struct Vector3 {
	float x, y, z;

	Vector3 operator+(Vector3 v);
	Vector3 operator+(float v);

	Vector3 operator-(Vector3 v);
	Vector3 operator-(float v);

	Vector3 operator*(Vector3 v);
	Vector3 operator*(float v);
};