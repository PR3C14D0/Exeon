#pragma once
#include <iostream>
#include <Windows.h>

typedef float RGBA[4];

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr))
		throw std::exception();
	return;
}