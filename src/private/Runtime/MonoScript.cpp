#include "Runtime/MonoScript.h"

MonoScript* MonoScript::m_instance;
MonoDomain* MonoScript::m_domain;
MonoAssembly* MonoScript::m_asm;
MonoImage* MonoScript::m_image;

MonoScript::MonoScript() {
	this->m_domain = nullptr;
	this->m_asm = nullptr;
	this->m_image = nullptr;
	this->m_filename = "Managed/ExeonScript.dll";
}

extern "C" void Console_Debug(MonoString* msg) {
	const char* nativeMsg = mono_string_to_utf8(msg);

	Console::GetInstance()->Debug(nativeMsg);
	mono_free((void*)nativeMsg);
}

extern "C" uintptr_t SceneManager_GetScene(MonoString* name) {
	const char* nativeName = mono_string_to_utf8(name);
	Scene* scene = SceneManager::GetInstance()->GetScene(nativeName);

	mono_free((void*)nativeName);

	return reinterpret_cast<uintptr_t>(scene);
}

void MonoScript::Init() {
	mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");
	this->m_domain = mono_jit_init("ExeonDomain");

	if (!this->m_domain) {
		spdlog::error("Failed initializing Mono domain");
		return;
	}

	spdlog::debug("Mono domain initialized");

	char fullPath[MAX_PATH];
	GetFullPathName(this->m_filename.c_str(), MAX_PATH, fullPath, nullptr);

	this->m_asm = mono_domain_assembly_open(this->m_domain, fullPath);
	if (!this->m_asm) {
		spdlog::error("Failed to read assembly {0}", this->m_filename);
		return;
	}

	spdlog::debug("Mono domain assembly open");

	this->m_image = mono_assembly_get_image(this->m_asm);

	MonoClass* gameScriptClass = mono_class_from_name(this->m_image, "Exeon", "ExeonScript");
	MonoMethod* initMethod = mono_class_get_method_from_name(gameScriptClass, "Init", 0);
	if(!initMethod) {
		spdlog::error("Error calling Init Method");
		return;
	}

	mono_add_internal_call("Exeon.Console::Debug", (const void*)Console_Debug);
	mono_add_internal_call("Exeon.SceneManager::GetScene_Impl", (const void*)SceneManager_GetScene);
	mono_runtime_invoke(initMethod, nullptr, nullptr, nullptr);
}

void MonoScript::Update() {

}

MonoScript* MonoScript::GetInstance() {
	if (MonoScript::m_instance == nullptr)
		MonoScript::m_instance = new MonoScript();
	return MonoScript::m_instance;
}