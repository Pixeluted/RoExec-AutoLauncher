#include "main.h"

std::wstring executablePath;

namespace fs = std::filesystem;

void AddTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 100;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    lstrcpy(nid.szTip, TEXT("Ro-exec AutoLauncher"));
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 100;
    Shell_NotifyIcon(NIM_DELETE, &nid);
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
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
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void inRightEnviroment() {
    if (fs::exists("launch.cfg")) {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(L"*.exe", &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            MessageBoxA(NULL, "Couldn't find the launcher!", "Error", MB_OK);
            exit(1);
        }
        else {
            do {
                wchar_t fullPath[MAX_PATH];
                GetCurrentDirectory(MAX_PATH, fullPath);
                wcscat_s(fullPath, L"\\");
                wcscat_s(fullPath, findFileData.cFileName);

                executablePath = fullPath;

                break;
            } while (FindNextFile(hFind, &findFileData) != 0);

            FindClose(hFind);
        }
    }
    else {
        MessageBoxA(NULL, "Make sure you placed this executable into folder with launcher!", "Error", MB_OK);
        exit(1);
    }
}

void launchRoExec() {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    LPWSTR path = const_cast<LPWSTR>(executablePath.c_str());

    BOOL result = CreateProcess(
        path,      
        NULL,      
        NULL,      
        NULL,     
        FALSE,    
        0,     
        NULL,     
        NULL,      
        &si,       
        &pi
    );

    if (!result) {
        MessageBoxA(NULL, "Failed to launch ro-exec!", "Error", MB_OK);
        exit(1);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    inRightEnviroment();
    std::cout << "You can now launch Roblox! We will auto-launch ro-exec!" << std::endl;
    std::cout << "To stop the auto-launcher, go into the system tray, right-click, and click on Exit." << std::endl;
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)startCheckingLoop, NULL, NULL, NULL);
    Sleep(5000);

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    LPCWSTR className = L"RoExecAutoLauncher";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;

    if (!RegisterClass(&wc)) {
        return -1;
    }

    HWND hwnd = CreateWindowEx(0, className, L"Tray App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

    AddTrayIcon(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveTrayIcon(hwnd);
    return 0;
}
