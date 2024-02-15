#include "launcher.h"

bool IsProcessRunning(const wchar_t* processName) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return false;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            if (nullptr != hProcess) {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }
            }

            if (!_wcsicmp(processName, szProcessName)) {
                CloseHandle(hProcess);
                return true;
            }

            CloseHandle(hProcess);
        }
    }

    return false;
}

void startCheckingLoop() {
    bool isRobloxFound = false;

    while (!isRobloxFound) {
        isRobloxFound = IsProcessRunning(L"RobloxPlayerBeta.exe");
        if (isRobloxFound) {
            launchRoExec();
        }
        else {
            Sleep(1000);
        }
    }

    while (isRobloxFound) {
        isRobloxFound = IsProcessRunning(L"RobloxPlayerBeta.exe");
        if (!isRobloxFound) {
            startCheckingLoop();
        }
        else {
            Sleep(1000);
        }
    }
}