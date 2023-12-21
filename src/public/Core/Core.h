#pragma once
#include <iostream>
#include <Windows.h>

class Core {
private:
	static Core* m_instance;

	HWND m_hwnd;
public:
	Core();
	void Init();

	void SetHWND(HWND& hwnd);

	static Core* GetInstance();
};