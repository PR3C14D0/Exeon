#pragma once
#include <iostream>
#include <Windows.h>
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/D3D12.h"
#include "Scene/SceneManager.h"

class Core {
private:
	static Core* m_instance;

	HWND m_hwnd;

	Renderer* m_renderer;
	
	SceneManager* m_sceneManager;
public:
	Core();
	void Init();

	void SetHWND(HWND& hwnd);

	static Core* GetInstance();


	void MainLoop();

	Renderer* GetRenderer();
};