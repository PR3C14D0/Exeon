#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>

class Core;
class D3D12;

class Shader {
private:
	void D3D12Shader(D3D12* renderer);
public:
	Shader();
};