#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class DescriptorHeap {
public:
	DescriptorHeap(UINT nDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
};