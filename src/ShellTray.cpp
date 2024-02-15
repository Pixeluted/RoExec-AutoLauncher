#include "common.h"
#include "ShellTray.h"

void AddTrayIcon(HWND hwnd) {
	NOTIFYICONDATA iconData = {};
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = hwnd;
	iconData.uID = 696969;
	iconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	iconData.uCallbackMessage = WM_TRAYICON;
	iconData.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	lstrcpy(iconData.szTip, TEXT("Ro-exec AutoLauncher"));
	Shell_NotifyIcon(NIM_ADD, &iconData);
}

void RemoveTrayIcon(HWND hwnd) {
	NOTIFYICONDATA iconData = {};
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = hwnd;
	iconData.uID = 696969;
	Shell_NotifyIcon(NIM_DELETE, &iconData);
}

void ShowContextMenu(HWND hwnd, POINT pt) {
	HMENU hMenu = CreatePopupMenu();
	if (hMenu) {
		InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_EXIT, TEXT("Exit"));
		
		SetMenuDefaultItem(hMenu, ID_EXIT, FALSE);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
		DestroyMenu(hMenu);
	}
}

BOOL handleWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		RemoveTrayIcon(hwnd);
		PostQuitMessage(0);
		return TRUE;
	case WM_TRAYICON:
		if (LOWORD(lParam) == WM_RBUTTONUP) {
			POINT curPoint;
			GetCursorPos(&curPoint);
			ShowContextMenu(hwnd, curPoint);
			return TRUE;
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_EXIT) {
			PostQuitMessage(0);
			return TRUE;
		}
	}

	return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (handleWindowProc(hwnd, uMsg, wParam, lParam))
		return TRUE;

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND createWindow() {
	LPCWSTR className = L"RoExecAutoLauncher";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = className;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(0, className, L"Tray App", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

	return hwnd;
}