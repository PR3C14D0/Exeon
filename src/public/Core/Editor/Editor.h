#pragma once
#include <iostream>
#include <imgui/imgui.h>
#include "Fonts/Roboto.h"
#include "Core/Scene/SceneManager.h"

class Editor {
private:
	static Editor* m_instance;
	bool m_bMenuOpen;
	SceneManager* m_sceneMgr;
public:
	Editor();
	void Init();
	void Update();

	static Editor* GetInstance();
};