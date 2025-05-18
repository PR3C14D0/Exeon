#pragma once
#include <iostream>
#include <mono/jit/jit.h>
#include "Core/Scene/SceneManager.h"

extern "C" {
    uintptr_t SceneManager_GetScene(MonoString* name);
    bool Scene_ObjectExists(const uintptr_t ptr, MonoString* name);
    uintptr_t Scene_GetObject(const uintptr_t ptr, MonoString* name);
}
