#include "Runtime/GameObjectImpl.h"

extern "C" uintptr_t GameObject_GetTransform(uintptr_t ptr) {
    GameObject* obj = reinterpret_cast<GameObject*>(ptr);
    uintptr_t pTransform = reinterpret_cast<uintptr_t>(&obj->transform);

    return pTransform;
}

extern "C" CVector3 Transform_GetLocation(uintptr_t ptr) {
    Transform* transform = reinterpret_cast<Transform*>(ptr);
    Vector3 loc = transform->location;
    CVector3 vec(loc.x, loc.y, loc.z);
    return vec;
}

extern "C" void Transform_Translate(uintptr_t ptr, float x, float y, float z) {
    Transform* transform = reinterpret_cast<Transform*>(ptr);
    transform->Translate(x, y, z);
    return;
}