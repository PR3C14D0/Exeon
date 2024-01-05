#include "Core/Renderer/Shader.h"
#include "Core/Core.h"

Shader::Shader() {
	Core* core = Core::GetInstance();
	Renderer* renderer = core->GetRenderer();

	if (D3D12* d3d12Renderer = dynamic_cast<D3D12*>(renderer)) {
		this->D3D12Shader(d3d12Renderer);
	}
}

void Shader::D3D12Shader(D3D12* renderer) {
}