#include "Core/Renderer/Renderer.h"

Renderer::Renderer() {
	this->m_hwnd = NULL;
	this->m_nWidth = 0;
	this->m_nHeight = 0;
}

void Renderer::Init(HWND hwnd) {
	this->m_hwnd = hwnd;

	RECT rect;
	GetWindowRect(hwnd, &rect);
	this->m_nWidth = rect.right - rect.left;
	this->m_nHeight = rect.bottom - rect.top;
}

void Renderer::Update() {

}