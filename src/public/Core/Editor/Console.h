#pragma once
#include <iostream>
#include <imgui/imgui.h>

class Console {
private:
	static Console* m_instance;
	std::string m_log;
public:
	Console();

	void Init();
	void Update();

	void Debug(std::string msg);
	
	static Console* GetInstance();
};

