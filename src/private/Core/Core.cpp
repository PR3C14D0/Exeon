#include "Core/Core.h"

Core* Core::m_instance;

Core::Core() {

}

void Core::Init() {

}

Core* Core::GetInstance() {
	if (Core::m_instance == nullptr)
		Core::m_instance = new Core();
	return Core::m_instance;
}