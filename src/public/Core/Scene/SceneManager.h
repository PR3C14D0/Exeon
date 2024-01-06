#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "Core/Scene.h"

class SceneManager {
private:
	static SceneManager* m_instance;
public:
	static SceneManager* GetInstance();
	SceneManager();

	void Init();
	void Update();
};