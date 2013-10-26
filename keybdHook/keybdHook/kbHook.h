//================================================================
#ifndef KB_HOOK_H
#define KB_HOOK_H

#include <Windows.h>
#include "../../QuickRun/data.h"
//================================================================
#ifndef HOOKAPI
#define HOOKAPI extern "C" __declspec(dllimport)
#endif
//================================================================
HOOKAPI int APIENTRY InstallHook(HWND hWnd, BOOL fInstall = TRUE );

LRESULT CALLBACK KeyboardHook(int nCode, WORD wParam, DWORD lParam);
UINT Str2Key(const TCHAR * key);
bool ReadKeyList();
//================================================================
#endif
//////////////////////////////////////////////////////////////////