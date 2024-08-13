#pragma once
#include <iostream>
#include <Windows.h>

class GameObject {
private:
	std::string m_name;
public:
	GameObject(std::string name);
};