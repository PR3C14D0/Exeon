#pragma once
#include <iostream>
#include <imgui/imgui.h>
#include "Fonts/Roboto.h"

class Editor {
private:
	static Editor* m_instance;
	bool m_bMenuOpen;
public:
	Editor();
	void Init();
	void Update();

	static Editor* GetInstance();
};