#pragma once
#include <Windows.h>
HWND initWindow(HINSTANCE);
extern HWND g_hOurWindow;
extern HWND g_hS4EditorMainWindow; // Main window of S4 Editor

extern HWND FindMainWindowOfCurrentProcess();

// Our window sizes:
extern int windowWidth, windowHeight;