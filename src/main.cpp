#include "common.h"
#include "main.h"
#include "ShellTray.h"
#include "ProcessMonitor.h"
#include "RoExec-Launcher.h"

HANDLE runningMutex = NULL;

BOOL AlreadyRunning() {
	HANDLE mutex = CreateMutex(NULL, FALSE, L"RoExec-AutoLauncher-Singleton");

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return TRUE;
	}

	runningMutex = mutex;

	return FALSE;
}

int main() {
	if (AlreadyRunning()) {
		MessageBoxA(NULL, "Auto-launcher already running!", "Error", MB_OK);
		return EXIT_FAILURE;
	}

	if (!validateEnviroment()) {
		MessageBoxA(NULL, "Place this executable into the folder with your loader!", "Error", MB_OK);
		return EXIT_FAILURE;
	}
	startProcessLoop();
	std::cout << "You can now launch roblox as normal. We will launch ro-exec for you!" << std::endl;
	std::cout << "This process will go into your tray, to stop auto-launcher click on the icon and click exit." << std::endl;
	Sleep(6000);
	FreeConsole();
	Sleep(100);
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	HWND hwnd = createWindow();

	if (!hwnd) {
		return EXIT_FAILURE;
	}

	AddTrayIcon(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	RemoveTrayIcon(hwnd);
	CloseHandle(runningMutex);
	return EXIT_SUCCESS;
}