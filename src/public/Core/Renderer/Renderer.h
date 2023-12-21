#pragma once
#include <iostream>
#include <Windows.h>

class Renderer {
protected:
	HWND m_hwnd;
public:
	explicit Renderer() = default;
	virtual void Init(HWND hwnd);
	virtual void Update();
};