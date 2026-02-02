// localization.cpp
#include "stdafx.h"
#include "localization.h"
#include "resource.h"

// Global instance set in DllMain
HINSTANCE g_hInst = nullptr;

// Default UI language (optional)
LANGID g_uiLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);


// -----------------------------------------------------------------------------
// Internal helper – loads string from THIS DLL
// -----------------------------------------------------------------------------
static std::wstring LoadLocalized(UINT id)
{
    wchar_t buf[1024]{};

    // Load from THIS DLL, not from EXE
    int len = LoadStringW(g_hInst, id, buf, _countof(buf));

    if (len > 0)
        return std::wstring(buf, len);

    return L""; // no fallback needed, Windows handles language fallback
}

// -----------------------------------------------------------------------------
// Version 1: returns std::wstring
// -----------------------------------------------------------------------------
std::wstring LocGet(UINT id)
{
    return LoadLocalized(id);
}

// -----------------------------------------------------------------------------
// Version 2: returns LPCWSTR (for MessageBox, SetWindowTextW, etc.)
// -----------------------------------------------------------------------------
LPCWSTR LocGet_CStr(UINT id)
{
    static thread_local std::wstring buffer;
    buffer = LoadLocalized(id);
    return buffer.c_str();
}

// -----------------------------------------------------------------------------
// Changes the active UI language
// -----------------------------------------------------------------------------
void LocSetLanguage(LANGID newLang)
{
    g_uiLanguage = newLang;
}

// -----------------------------------------------------------------------------
// Returns the preferred language based on system settings
// -----------------------------------------------------------------------------
LANGID GetPreferredLanguage()
{
    return MAKELANGID(GetUserDefaultUILanguage(), SUBLANG_DEFAULT);
}