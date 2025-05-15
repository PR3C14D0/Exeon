#include "Runtime/MonoScript.h"


MonoScript* MonoScript::m_instance;

MonoScript::MonoScript() {

}

void MonoScript::Init() {

}

void MonoScript::Update() {

}

MonoScript* MonoScript::GetInstance() {
	if (MonoScript::m_instance == nullptr) {
		MonoScript::m_instance = new MonoScript();
	}
	return MonoScript::m_instance;
}