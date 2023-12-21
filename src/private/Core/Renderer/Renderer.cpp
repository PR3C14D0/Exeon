#include "Core/Renderer/Renderer.h"

Renderer::Renderer() {
	this->m_hwnd = NULL;
}

void Renderer::Init(HWND hwnd) {
	this->m_hwnd = hwnd;
}

void Renderer::Update() {

}