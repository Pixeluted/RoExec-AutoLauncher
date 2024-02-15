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

		do {
			wchar_t fullPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, fullPath);
			wcscat_s(fullPath, L"\\");
			wcscat_s(fullPath, findFileData.cFileName);

			loaderPath = fullPath;

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
		0,
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