#pragma once

#include <iostream>

class Component {
private:
	std::string m_name;
public:
	Component(std::string name);

	void Init() = default;
	void Update() = default;
};