#include <iostream>
#include <Windows.h>
#include "Core/Core.h"

bool g_bQuit = false;

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
	std::cout << "[DEBUG] Window created" << std::endl;

	Core* core = Core::GetInstance();
	core->SetHWND(hwnd);

	core->Init();
	std::cout << "[DEBUG] Core initialized" << std::endl;

	MSG msg = { };

	std::cout << "[DEBUG] Starting main loop" << std::endl;
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
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		g_bQuit = true;
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}