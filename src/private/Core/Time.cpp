#include "Core/Time.h"

Time* Time::m_instance;

Time::Time() {
	this->currentTime = 0.f;
	this->lastTime = 0.f;
	this->deltaTime = 0.f;
}

Time* Time::GetInstance() {
	if (Time::m_instance == nullptr)
		Time::m_instance = new Time();
	return Time::m_instance;
}