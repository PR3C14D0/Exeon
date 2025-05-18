#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"
#include <mono/jit/jit.h>

extern "C" {
    uintptr_t GameObject_GetTransform(uintptr_t ptr);
    uintptr_t Transform_GetLocation(uintptr_t ptr);
}