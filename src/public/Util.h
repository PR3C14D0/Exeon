#pragma once
#include <iostream>
#include <Windows.h>

typedef float RGBA[4];
typedef float RGB[3];
typedef float RG[2];

struct Vertex {
	RGB xyz;
	RG uv;
};

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr))
		throw std::exception();
	return;
}

inline LPCWSTR MultiByteToUnicode(const char* data) {
	UINT nUnicodeSize = MultiByteToWideChar(CP_ACP, NULL, data, -1, nullptr, NULL);
	wchar_t* unicodeData = new wchar_t[nUnicodeSize];

	MultiByteToWideChar(CP_ACP, NULL, data, -1, unicodeData, nUnicodeSize);

	return unicodeData;
}