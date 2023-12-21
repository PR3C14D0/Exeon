#include "Core/Core.h"

Core* Core::m_instance;

Core::Core() {
	this->m_hwnd = NULL;
	this->m_renderer = nullptr;
}

void Core::Init() {
	this->m_renderer = new D3D12();
	m_renderer->Init(this->m_hwnd);
}

void Core::SetHWND(HWND& hwnd) {
	this->m_hwnd = hwnd;
}

void Core::MainLoop() {

}

Core* Core::GetInstance() {
	if (Core::m_instance == nullptr)
		Core::m_instance = new Core();
	return Core::m_instance;
}