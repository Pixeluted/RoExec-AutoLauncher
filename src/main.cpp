#include "common.h"
#include "main.h"
#include "ShellTray.h"

int main() {
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
	return EXIT_SUCCESS;
}