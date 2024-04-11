#include "Core/Renderer/DescriptorHeap.h"
#include "Core/Core.h"

DescriptorHeap::DescriptorHeap(UINT nDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, bool bShaderVisible) {
	this->m_nDescriptors = nDescriptors;
	this-> m_bShaderVisible = bShaderVisible;

	Core* core = Core::GetInstance();
	Renderer* pRenderer = core->GetRenderer();
	
	if (D3D12* renderer = dynamic_cast<D3D12*>(pRenderer)) {
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { };
		heapDesc.Type = type;
		heapDesc.NumDescriptors = nDescriptors;
		heapDesc.Flags = this->m_bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		renderer->GetDevice(this->m_dev);

		ThrowIfFailed(this->m_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(this->m_heap.GetAddressOf())));

		this->m_nIncrement = this->m_dev->GetDescriptorHandleIncrementSize(type);
		this->m_cpuHandle = this->m_heap->GetCPUDescriptorHandleForHeapStart();
		this->m_gpuHandle = this->m_bShaderVisible ? this->m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0x00 };
	}
}

void DescriptorHeap::Allocate(UINT nDescriptors) {
	if (this->m_dev) {
		UINT nOldCount = this->m_nDescriptors;
		this->m_nDescriptors += nDescriptors;
		ComPtr<ID3D12DescriptorHeap> heap;
		
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = this->m_heap->GetDesc();
		heapDesc.NumDescriptors = this->m_nDescriptors;

		ThrowIfFailed(this->m_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(heap.GetAddressOf())));
		this->m_dev->CopyDescriptorsSimple(nOldCount, heap->GetCPUDescriptorHandleForHeapStart(), this->m_heap->GetCPUDescriptorHandleForHeapStart(), heapDesc.Type);

		this->m_nIncrement = this->m_dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		this->m_cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		this->m_gpuHandle = this->m_bShaderVisible ? heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0x00 };

		this->m_heap.Swap(heap);
		heap->Release();
	}
}

Descriptor DescriptorHeap::GetDescriptor(UINT nIndex) {
	SIZE_T pCPU = this->m_cpuHandle.ptr;
	SIZE_T pGPU = this->m_gpuHandle.ptr;

	pCPU += nIndex * this->m_nIncrement;
	pGPU += nIndex * this->m_nIncrement;

	Descriptor retDesc{ pCPU, pGPU };
	return retDesc;
}

UINT DescriptorHeap::GetDescriptorCount() {
	return this->m_nDescriptors;
}