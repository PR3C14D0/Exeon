#include "Core/Core.h"

Core* Core::m_instance;

Core::Core() {
	this->m_hwnd = NULL;
	this->m_renderer = nullptr;
	this->m_sceneManager = SceneManager::GetInstance();
}

void Core::Init() {
	this->m_renderer = new D3D12();
	m_renderer->Init(this->m_hwnd);
	m_sceneManager->Init();
}

void Core::SetHWND(HWND& hwnd) {
	this->m_hwnd = hwnd;
}

void Core::MainLoop() {
	this->m_renderer->Update();
	this->m_sceneManager->Update();
}

Core* Core::GetInstance() {
	if (Core::m_instance == nullptr)
		Core::m_instance = new Core();
	return Core::m_instance;
}

Renderer* Core::GetRenderer() {
	return this->m_renderer;
}