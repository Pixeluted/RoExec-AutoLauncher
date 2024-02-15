#pragma once
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_EXIT 1 

void AddTrayIcon(HWND hwnd);
void RemoveTrayIcon(HWND hwnd);
BOOL handleWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createWindow();