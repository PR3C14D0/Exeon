#pragma once
#include <iostream>
#include "Vector3.h"

struct Transform {
	Vector3 location;
	Vector3 rotation;	
	Vector3 position;

	void Translate(Vector3 v);
	void Translate(float x, float y, float z);

	void Rotate(Vector3 v);
	void Rotate(float x, float y, float z);
};