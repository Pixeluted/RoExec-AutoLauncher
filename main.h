#pragma once
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "launcher.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_EXIT 1 

void launchRoExec();