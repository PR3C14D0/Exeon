#pragma once
#include <iostream>
#include "Vector3.h"
#include <DXMath/DirectXMath.h>

using namespace DirectX;

struct Transform {
	Vector3 location;
	Vector3 rotation;	
	Vector3 scale;

	Transform();

	void Translate(Vector3 v);
	void Translate(float x, float y, float z);

	void Rotate(Vector3 v);
	void Rotate(float x, float y, float z);
	Vector3 Forward();
	Vector3 Right();

	Vector3 RotatePoint(Vector3 point);
};