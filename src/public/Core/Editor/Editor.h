#pragma once
#include <iostream>
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <DXMath/DirectXMath.h>
#include "Fonts/Roboto.h"
#include "Core/Scene/SceneManager.h"
#include "Console.h"
#include "Util.h"
#include <IconsFontAwesome5.h>

class Editor {
private:
	static Editor* m_instance;
	bool m_bMenuOpen;
	SceneManager* m_sceneMgr;
	GameObject* m_inspectorObj;

	UINT m_nWidth;
	UINT m_nHeight;

	WVP m_wvp;

	Console* m_console;

	bool m_bLocation;
	bool m_bRotation;
	bool m_bScale;
	ImGuizmo::OPERATION m_guizmoOp;
public:
	Editor();
	void Init(UINT nWidth, UINT nHeight);
	void Update();

	static Editor* GetInstance();
};