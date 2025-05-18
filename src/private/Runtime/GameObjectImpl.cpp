#include "Runtime/GameObjectImpl.h"

extern "C" uintptr_t GameObject_GetTransform(uintptr_t ptr) {
    GameObject* obj = reinterpret_cast<GameObject*>(ptr);
    uintptr_t pTransform = reinterpret_cast<uintptr_t>(&obj->transform);

    return pTransform;
}

extern "C" uintptr_t Transform_GetLocation(uintptr_t ptr) {
    Transform* trasnform = reinterpret_cast<Transform*>(ptr);
    uintptr_t pLocation = reinterpret_cast<uintptr_t>(&trasnform->location);
    return pLocation;
}