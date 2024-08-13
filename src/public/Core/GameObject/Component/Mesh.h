#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

#include "Core/GameObject/Component/Component.h"

using namespace Microsoft::WRL;

/* Our core forward declaration */
class Core;
class Renderer;
class D3D12;

class Mesh : public Component {
private:
	Core* m_core;

	Renderer* m_renderer;

	void D3D12Init(D3D12* renderer);
public:
	Mesh(std::string name);

	void Init();
};