#pragma once
#include <iostream>
#include <vector>
#include <map>

class Scene {
public:
	std::string name;

	Scene(std::string name);

	void Init();
	void Update();
};