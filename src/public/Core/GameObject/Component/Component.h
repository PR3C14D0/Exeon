#pragma once

#include <iostream>

class Component {
	friend class Editor;
protected:
	std::string m_name;
public:
	explicit Component(std::string name);

	virtual void Init();
	virtual void Update();
};