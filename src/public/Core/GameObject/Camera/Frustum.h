#pragma once
#include <iostream>
#include <DXMath/DirectXMath.h>

using namespace DirectX;

class Frustum {
private:
    XMVECTOR m_planes[6];
public:
    Frustum() = default;

    void Update(const XMMATRIX& View, const XMMATRIX& Projection) {
        XMMATRIX viewProj = XMMatrixMultiply(View, Projection);
        XMFLOAT4X4 vp;
        XMStoreFloat4x4(&vp, viewProj);

        // Left plane
        m_planes[0] = DirectX::XMVectorSet(
            vp._14 + vp._11,
            vp._24 + vp._21,
            vp._34 + vp._31,
            vp._44 + vp._41
        );

        // Right plane
        m_planes[1] = DirectX::XMVectorSet(
            vp._14 - vp._11,
            vp._24 - vp._21,
            vp._34 - vp._31,
            vp._44 - vp._41
        );

        // Top plane
        m_planes[2] = DirectX::XMVectorSet(
            vp._14 - vp._12,
            vp._24 - vp._22,
            vp._34 - vp._32,
            vp._44 - vp._42
        );

        // Bottom plane
        m_planes[3] = DirectX::XMVectorSet(
            vp._14 + vp._12,
            vp._24 + vp._22,
            vp._34 + vp._32,
            vp._44 + vp._42
        );

        // Near plane
        m_planes[4] = DirectX::XMVectorSet(
            vp._13,
            vp._23,
            vp._33,
            vp._43
        );

        // Far plane
        m_planes[5] = DirectX::XMVectorSet(
            vp._14 - vp._13,
            vp._24 - vp._23,
            vp._34 - vp._33,
            vp._44 - vp._43
        );

        for (int i = 0; i < 6; ++i) {
            m_planes[i] = DirectX::XMVector4Normalize(m_planes[i]);
        }
    }

    bool CheckBox(const XMFLOAT3& min, const XMFLOAT& max) {
        for (UINT i = 0; i < 6; i++) {
            XMVECTOR plane = this->m_planes[i];

            XMFLOAT3 normal(
                XMVectorGetX(plane),
                XMVectorGetY(plane),
                XMVectorGetZ(plane)
            );

            float planeD = XMVectorGetW(plane);

            /* Find near point (P) and far point (N) */
            XMFLOAT3 p, n;

            if (normal.x >= 0.f) {
                p.x = min,x;
                n.x = max.x;
            } else {
                p.x = max.x;
                n.x = min.x;
            }

            if (normal.y >= 0.f) {
                p.y = min.y;
                n.y = max.y;
            } else {
                p.y = max.y;
                n.y = min.y;
            }

            if (normal.z >= 0.f) {
                p.z = min.z;
                n.z = max.z;
            } else {
                p.z = max.z;
                n.z = min.z;
            }

            float d_near = normal.x * n.x + normal.y * n.y + normal.z * n.z + planeD;

            if (d_near < 0.f) {
                float d_far = normal.x * p.x + normal.y * p.y + normal.z * p.z + planeD;

                if (d_far < 0.f) {
                    return false;
                }
            }
        }

        return true;
    }
};