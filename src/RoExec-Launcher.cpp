#include "common.h"
#include "RoExec-Launcher.h"

namespace fs = std::filesystem;

BOOL validateEnviroment() {
	if (fs::exists("launch.cfg")) {
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(L"*.exe", &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			MessageBoxA(NULL, "Failed to find loader!", "Error", MB_OK);
			return FALSE;
		}

		wchar_t ownPath[MAX_PATH];
		if (!GetModuleFileName(NULL, ownPath, MAX_PATH)) {
			MessageBoxA(NULL, "Failed to get own executable name!", "Error", MB_OK);
			FindClose(hFind);
			return FALSE;
		}

		do {
			wchar_t fullPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, fullPath);
			wcscat_s(fullPath, L"\\");
			wcscat_s(fullPath, findFileData.cFileName);

			if (_wcsicmp(fullPath, ownPath) == 0) {
				continue;
			}

			loaderPath = fullPath;
			std::wcout << L"Found loader: " << fullPath << std::endl;

			break;
		} while (FindNextFile(hFind, &findFileData) != 0);

		return TRUE;
	}
	else {
		return FALSE;
	}
}

void launchRoExec() {
	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	PROCESS_INFORMATION processInformation;

	LPWSTR loaderPathConverted = const_cast<LPWSTR>(loaderPath.c_str());

	BOOL sucessfullyLaunched = CreateProcess(
		loaderPathConverted,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&startupInfo,
		&processInformation
	);

	if (!sucessfullyLaunched) {
		MessageBoxA(NULL, "Failed to launch ro-exec!", "Error", MB_OK);
		return;
	}

	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);
}