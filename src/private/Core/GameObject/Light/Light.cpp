#include "Core/GameObject/Light/Light.h"
#include "Core/Core.h"

Light::Light(std::string name) : GameObject::GameObject(name) {
    /* White by default */
    this->m_lightColor[0] = 1.f;
    this->m_lightColor[1] = 1.f;
    this->m_lightColor[2] = 1.f;
    this->m_lightColor[3] = 1.f;
    this->m_core = Core::GetInstance();
    this->m_renderer = this->m_core->GetRenderer();
}

void Light::Init() {
    GameObject::Init();

    if (D3D12* renderer = reinterpret_cast<D3D12*>(this->m_renderer)) {
        Transform transform = this->transform;
        XMVECTOR eye = XMVectorSet(
                transform.location.x,
                transform.location.y,
                transform.location.z,
                1.0f
            );

        float pitch = XMConvertToRadians(transform.rotation.x);
        float yaw = XMConvertToRadians(transform.rotation.y);

        XMVECTOR forward = XMVectorSet(
            cosf(pitch) * sinf(yaw),
            -sinf(pitch),
            -cosf(pitch) * cosf(yaw),
            0.0f
        );

        XMVECTOR at = XMVectorAdd(eye, forward);

        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        this->m_lightBuffer.View = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));
        this->m_lightBuffer.Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
			    XMConvertToRadians(70.f),
			    static_cast<float>(renderer->m_nWidth) / static_cast<float>(renderer->m_nHeight),
			    0.001f,
			    3000.f));
    }

}

void Light::Update() {
    GameObject::Update();
}
