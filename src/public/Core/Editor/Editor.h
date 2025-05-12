#pragma once
#include <iostream>
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <DXMath/DirectXMath.h>
#include "Fonts/Roboto.h"
#include "Core/Scene/SceneManager.h"
#include "Util.h"

class Editor {
private:
	static Editor* m_instance;
	bool m_bMenuOpen;
	SceneManager* m_sceneMgr;
	GameObject* m_inspectorObj;

	UINT m_nWidth;
	UINT m_nHeight;

	WVP m_wvp;
public:
	Editor();
	void Init(UINT nWidth, UINT nHeight);
	void Update();

	static Editor* GetInstance();
};