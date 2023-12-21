#pragma once
#include <iostream>
#include <Windows.h>

class Renderer {
protected:
	HWND m_hwnd;
public:
	explicit Renderer();
	virtual void Init(HWND hwnd);
	virtual void Update();
};