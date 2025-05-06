#include <iostream>
#include <Windows.h>
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>
#include "Core/Core.h"
#include "resource.h"
#include "Core/Input/Input.h"

bool g_bQuit = false;
Input* g_input = Input::GetInstance();

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DebugConsole();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#ifndef NDEBUG
	DebugConsole();
#endif // NDEBUG

	std::cout << "Exeon debug console" << std::endl;

	const char CLASS_NAME[] = "Exeon";
	const char WINDOW_NAME[] = "Exeon";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	RegisterClass(&wc);
		
	HWND hwnd = CreateWindowEx(
		NULL,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);

	if (hwnd)
		ShowWindow(hwnd, nShowCmd);
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug);
#endif
	spdlog::debug("Window created");

	g_input->SetHWND(hwnd);
	Core* core = Core::GetInstance();
	core->SetHWND(hwnd);

	core->Init();
	spdlog::debug("Core initialized");

	MSG msg = { };
	spdlog::debug("Starting main loop");
	while (!g_bQuit) {
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		core->MainLoop();
	}

	return FALSE;
}

void DebugConsole() {
	AllocConsole();
	SetConsoleTitle("Exeon Debug console");
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	g_input->Callback(hwnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_DESTROY:
		g_bQuit = true;
		PostQuitMessage(0);
		return 0;
	}
	g_input->Close();


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}