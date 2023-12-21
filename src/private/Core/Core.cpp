#include "Core/Core.h"

Core* Core::m_instance;

Core::Core() {
	this->m_hwnd = NULL;
}

void Core::Init() {

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