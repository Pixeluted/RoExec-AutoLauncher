#include "common.h"
#include "ProcessMonitor.h"
#include "RoExec-Launcher.h"

std::vector<DWORD> lastRobloxPIDs;

bool IsPIDInHistory(DWORD pid) {
	auto it = std::find(lastRobloxPIDs.begin(), lastRobloxPIDs.end(), pid);

	return it != lastRobloxPIDs.end();
}

void AddPIDToHistoryList(DWORD newPID) {
	auto it = std::find(lastRobloxPIDs.begin(), lastRobloxPIDs.end(), newPID);
	if (it == lastRobloxPIDs.end()) {
		if (lastRobloxPIDs.size() >= 50) {
			lastRobloxPIDs.erase(lastRobloxPIDs.begin());
		}
		lastRobloxPIDs.push_back(newPID);
	}
}

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
			if (!hProcess) {
				continue;
			}

			if (nullptr != hProcess) {
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
					GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName));
				}
			}

			if (!_wcsicmp(L"RobloxPlayerBeta.exe", szProcessName) && IsPIDInHistory(aProcesses[i]) == FALSE) {
				AddPIDToHistoryList(aProcesses[i]);
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

	mainLoop();
}

void startProcessLoop() {
	HANDLE threadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainLoop, NULL, NULL, NULL);
	if (!threadHandle) {
		MessageBoxA(NULL, "Failed to start checking loop!", "Error", MB_OK);
		exit(1);
	}

	CloseHandle(threadHandle);
}