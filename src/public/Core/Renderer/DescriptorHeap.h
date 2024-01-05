#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include "Util.h"
#include "Core/Renderer/Descriptor.h"

using namespace Microsoft::WRL;

class DescriptorHeap {
private:
	UINT m_nDescriptors;
	
	ComPtr<ID3D12Device1> m_dev;

	ComPtr<ID3D12DescriptorHeap> m_heap;

	bool m_bShaderVisible;

	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;

	UINT m_nIncrement;
public:
	DescriptorHeap(UINT nDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, bool bShaderVisible);

	Descriptor GetDescriptor(UINT nIndex);

	void Allocate(UINT nDescriptors);

	UINT GetDescriptorCount();
};