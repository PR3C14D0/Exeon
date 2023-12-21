#pragma once
#include <iostream>
#include <Windows.h>

class Core {
private:
	static Core* m_instance;
public:
	Core();
	void Init();

	static Core* GetInstance();
};