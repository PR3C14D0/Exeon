#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"
#include <mono/jit/jit.h>



extern "C" {
    struct CVector3 {
        float x;
        float y;
        float z;
    };

    uintptr_t GameObject_GetTransform(uintptr_t ptr);
    CVector3 Transform_GetLocation(uintptr_t ptr);
    void Transform_Translate(uintptr_t ptr, float x, float y, float z);
}