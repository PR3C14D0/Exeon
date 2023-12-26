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
		this->m_dev->CopyDescriptorsSimple(nOldCount, this->m_heap->GetCPUDescriptorHandleForHeapStart(), heap->GetCPUDescriptorHandleForHeapStart(), heapDesc.Type);

		this->m_heap.Swap(heap);
		heap->Release();

		this->m_nIncrement = this->m_dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	}
}

Descriptor DescriptorHeap::GetDescriptor(UINT nIndex) {
	return {
		this->m_heap->GetCPUDescriptorHandleForHeapStart().ptr + (nIndex)*this->m_nIncrement,
		this->m_heap->GetGPUDescriptorHandleForHeapStart().ptr + (nIndex)*this->m_nIncrement,
	};
}