#include "Core/Editor/Console.h"

Console* Console::m_instance;

Console::Console() {

}

void Console::Init() {
	this->Debug("Test for logs");
	this->Debug("Test #2");
}

void Console::Update() {
	ImGui::SetNextWindowPos(ImVec2{ 370.f, 694.f });
	ImGui::SetNextWindowSize(ImVec2{ 1200.f, 280.f });
	ImGui::Begin("Console");
	ImGui::Text(this->m_log.c_str());
	ImGui::End();
}

void Console::Debug(std::string msg) {
	std::string log = this->m_log + "\n" + "[DEBUG] " +  msg;
	this->m_log = log;
	return;
}

Console* Console::GetInstance() {
	if (Console::m_instance == nullptr)
		Console::m_instance = new Console();
	return Console::m_instance;
}