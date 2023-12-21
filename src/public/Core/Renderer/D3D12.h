#include "Core/Renderer/Renderer.h"
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class D3D12 : public Renderer {
private:

public:
	D3D12();
	
	void Init() override;
	void Update() override;
};