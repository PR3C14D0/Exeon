#pragma once
#include <iostream>

class MonoScript {
private:
	static MonoScript* m_instance;
public:
	MonoScript();

	void Init();
	void Update();

	static MonoScript* GetInstance();
};