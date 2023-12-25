#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>

struct Descriptor {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};