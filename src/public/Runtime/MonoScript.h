#pragma once
#include <iostream>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <spdlog/spdlog.h>
#include "Core/Editor/Editor.h"

class MonoScript {
private:
	static MonoScript* m_instance;
	static MonoDomain* m_domain;
	static MonoAssembly* m_asm;
	static MonoImage* m_image;
	
	std::string m_filename;
public:
	MonoScript();

	void Init();
	void Update();

	static MonoScript* GetInstance();
};