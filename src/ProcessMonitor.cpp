#include "common.h"
#include "ProcessMonitor.h"
#include "RoExec-Launcher.h"

bool IsRobloxRunning() {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return FALSE;
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
					GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName));
				}
			}

			if (!_wcsicmp(L"RobloxPlayerBeta.exe", szProcessName)) {
				CloseHandle(hProcess);
				return TRUE;
			}

			CloseHandle(hProcess);
		}
	}

	return FALSE;
}

void mainLoop() {
	bool isRobloxFound = false;

	while (!isRobloxFound) {
		isRobloxFound = IsRobloxRunning();
		if (isRobloxFound) {
			launchRoExec();
		}
		else {
			Sleep(1000);
		}
	}

	while (isRobloxFound) {
		isRobloxFound = IsRobloxRunning();
		if (isRobloxFound) {
			Sleep(1000);
		}
	}

	mainLoop();
}

void startProcessLoop() {
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainLoop, NULL, NULL, NULL);
}