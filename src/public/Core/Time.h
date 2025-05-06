#pragma once
#include <iostream>
#include <time.h>

class Time {
private:
	static Time* m_instance;
public:
	Time();
	static Time* GetInstance();
	float lastTime;
	float currentTime;
	float deltaTime;
};