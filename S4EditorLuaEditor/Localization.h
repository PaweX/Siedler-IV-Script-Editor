// localization.h
#pragma once

#include <string>
#include <windows.h>

// Global variable holding the currently active UI language
// Initialized to English (neutral) by default
extern LANGID g_uiLanguage;
extern HINSTANCE g_hInst;

// -----------------------------------------------------------------------------
// Returns localized string as std::wstring
// Use this when you need full string object (e.g. for manipulation)
std::wstring LocGet(UINT id);

// -----------------------------------------------------------------------------
// Returns localized string directly as LPCWSTR (for Windows API calls)
LPCWSTR LocGet_CStr(UINT id);

// Optional: changes the UI language (rarely needed after window creation)
void LocSetLanguage(LANGID newLang);

// Returns the preferred language based on system settings
LANGID GetPreferredLanguage();