#pragma once

#include <iostream>

class Component {
protected:
	std::string m_name;
public:
	explicit Component(std::string name);

	virtual void Init();
	virtual void Update();
};