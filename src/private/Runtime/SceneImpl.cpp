#include "Runtime/SceneImpl.h"

extern "C" uintptr_t SceneManager_GetScene(MonoString* name) {
    char* nativeName = mono_string_to_utf8(name);
    Scene* scene = SceneManager::GetInstance()->GetScene(nativeName);

    mono_free(nativeName);

    return reinterpret_cast<uintptr_t>(scene);
}

extern "C" bool Scene_ObjectExists(const uintptr_t ptr, MonoString* name) {
    char* nativeName = mono_string_to_utf8(name);
    Scene* scene = reinterpret_cast<Scene*>(ptr);

    mono_free(nativeName);

    return scene->ObjectExists(nativeName);
}

extern "C" uintptr_t Scene_GetObject(const uintptr_t ptr, MonoString* name) {
    char* nativeName = mono_string_to_utf8(name);
    Scene* scene = reinterpret_cast<Scene*>(ptr);

    GameObject* obj = scene->GetObject(nativeName);

    mono_free(nativeName);
    return reinterpret_cast<uintptr_t>(obj);
}
