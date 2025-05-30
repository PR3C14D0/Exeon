#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"
#include "Util.h"
#include <DXMath/DirectXMath.h>

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
public:
    RGBA m_lightColor;

    LightBuffer m_lightBuffer;

    Light(std::string name);

    void Init() override;
    void Update() override;
};