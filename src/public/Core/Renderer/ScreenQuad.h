#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class Core;

class ScreenQuad {
private:
	Core* m_core;
public:
	ScreenQuad();
	
	void Init();
	void Render();
};