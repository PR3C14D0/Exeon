#pragma once
#include <iostream>
#include <Windows.h>
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/D3D12.h"
#include "Scene/SceneManager.h"
#include "Renderer/ResourceManager.h"
#include "Runtime/MonoScript.h"

class Core {
private:
	static Core* m_instance;

	HWND m_hwnd;

	Renderer* m_renderer;
	
	SceneManager* m_sceneManager;
	ResourceManager* m_resMgr;

	UINT m_nWidth;
	UINT m_nHeight;

	MonoScript* m_mono;
public:
	Core();
	void Init();

	void SetHWND(HWND& hwnd);

	static Core* GetInstance();

	void GetWindowSize(UINT& nWidth, UINT& nHeight);

	void MainLoop();

	Renderer* GetRenderer();
};