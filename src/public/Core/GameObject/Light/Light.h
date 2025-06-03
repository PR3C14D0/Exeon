#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"
#include "Util.h"
#include <DXMath/DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;
using namespace DirectX;

struct LightBuffer {
    XMMATRIX View;
    XMMATRIX Projection;
    XMFLOAT4 LightColor;
};

class Core;

class Light : public GameObject {
private:
    Core* m_core;
    Renderer* m_renderer;

    ComPtr<ID3D12Resource> m_depth;
    UINT m_nDepthIndex;

    ComPtr<ID3D12RootSignature> m_rootSig;
    ComPtr<ID3D12PipelineState> m_plState;

    ComPtr<ID3D12Device1> m_dev;
    ComPtr<ID3D12GraphicsCommandList> m_list;

    void InitConstantBuffers(D3D12* renderer);
    void InitPipeline(D3D12* renderer);

    Shader* m_shader;
public:
    RGBA m_lightColor;

    LightBuffer m_lightBuffer;
    WVP m_wvp;

    Light(std::string name);

    void Init() override;
    void Update() override;
    void Render() override;
};