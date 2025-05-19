#include "Core/Core.h"
#include "Subsystem/Discord.h"

Core* Core::m_instance;

Core::Core() {
	this->m_hwnd = NULL;
	this->m_renderer = nullptr;
	this->m_sceneManager = SceneManager::GetInstance();
	this->m_resMgr = nullptr;
	this->m_mono = nullptr;
	this->m_status = GAME_STATUS::STOPPED;
	this->m_discord = Discord::GetInstance();
}

void Core::Init() {
	RECT rect;
	GetWindowRect(this->m_hwnd, &rect);

	this->m_nWidth = rect.right - rect.left;
	this->m_nHeight = rect.bottom - rect.top;

	this->m_renderer = new D3D12();
	m_renderer->Init(this->m_hwnd);
	this->m_resMgr = ResourceManager::GetInstance();
	this->m_resMgr->Init();

	this->m_mono = MonoScript::GetInstance();
	this->m_mono->Init();
	this->m_discord->Init();
}

void Core::SetHWND(HWND& hwnd) {
	this->m_hwnd = hwnd;

}

void Core::MainLoop() {
	this->m_sceneManager->Update();
	this->m_renderer->Update();
	this->m_mono->Update();
}

void Core::GetWindowSize(UINT& nWidth, UINT& nHeight) {
	nWidth = this->m_nWidth;
	nHeight = this->m_nHeight;
}

Core* Core::GetInstance() {
	if (Core::m_instance == nullptr)
		Core::m_instance = new Core();
	return Core::m_instance;
}

Renderer* Core::GetRenderer() {
	return this->m_renderer;
}