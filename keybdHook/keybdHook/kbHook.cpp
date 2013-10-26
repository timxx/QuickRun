//================================================================
#define HOOKAPI extern "C" __declspec(dllexport)

#include "kbHook.h"
#include <list>
#include <Shlwapi.h>
#include <tchar.h>

#pragma comment(lib, "shlwapi.lib")
//================================================================

static HHOOK ghHook;
static BOOL fHookInstalled;
extern HINSTANCE ghDLLInst;
extern HWND ghWnd;
extern BOOL gfShow;
std::list<KEYDATA> keyList;
//================================================================
int APIENTRY InstallHook (HWND hWnd, BOOL fInstall/* = TRUE*/)
{
    int nReturn = 1;
	ghWnd = hWnd;
//	if (hWnd == NULL)	MessageBox(0, L"NULL hWnd", L"test", 0);
    if (fInstall == fHookInstalled)
        return 0;

    if (fInstall)
    {
        ghHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardHook, ghDLLInst, 0);

        if (!ghHook)
            return 0;

        fHookInstalled = TRUE;
    }
    else
    {
        nReturn = UnhookWindowsHookEx(ghHook);
        fHookInstalled = FALSE;
    }
    return nReturn;
}

LRESULT CALLBACK KeyboardHook(int nCode, WORD wParam, DWORD lParam)
{
	if(nCode != HC_ACTION)
		return CallNextHookEx(ghHook, nCode, wParam, lParam);

	if (((wParam >='0' && wParam <='9')	||
		(wParam >='A' && wParam <= 'Z')	||
		(wParam >= VK_F1 && wParam <= VK_F12)) &&
		(HIWORD(lParam) & KF_UP)==0)
	{
		bool fAlt	= (GetKeyState(VK_MENU)<0);
		bool fCtrl	= (GetKeyState(VK_CONTROL)<0);
		bool fShift = (GetKeyState(VK_SHIFT)<0);

		if (!(wParam >= VK_F1 && wParam <= VK_F12) &&
			!fAlt && !fCtrl && !fShift
			)
			return CallNextHookEx(ghHook, nCode, wParam, lParam);

		if (!ReadKeyList())
		{
			TCHAR errInfo[256];
			DWORD dwErr = GetLastError();
			wsprintf(errInfo, _T("读取快捷键配置文件失败，错误代码: %ld"), dwErr);

			if (dwErr == 2 || dwErr == 3)
			{
				if (!gfShow)
				{
					gfShow = TRUE;
					lstrcpy(errInfo, _T("快捷键文件不存在！请检查key.qrk是否存在于程序目录下的Data文件夹下！\r\n ")
						_T("注：此提示下次不会再弹出"));
					MessageBox(GetActiveWindow(), errInfo, _T("错误"), MB_ICONERROR);
				}
			}
			else if (dwErr != 0)
			{
				if (gfShow)	gfShow = FALSE;

				MessageBox(GetActiveWindow(), errInfo, _T("错误"), MB_ICONERROR);
			}

			return CallNextHookEx(ghHook, nCode, wParam, lParam);
		}
		for (std::list<KEYDATA>::iterator it = keyList.begin();
			it != keyList.end(); it++)
		{
			if (Str2Key(it->key) == wParam	&&
				it->fAlt == fAlt		&&
				it->fCtrl == fCtrl		&&
				it->fShift == fShift
				)
			{
				if (!PathFileExists(it->exePath))
				{
					TCHAR szInfo[MAX_PATH + 50];
					wsprintf(szInfo, _T("路径[%s]不存在！！！"), it->exePath);
					MessageBox(ghWnd, szInfo, _T("文件不存在"), MB_ICONERROR);
				}
				else
				{
					TCHAR szDir[MAX_PATH];
					lstrcpy(szDir, it->exePath);
					PathRemoveFileSpec(szDir);
					int cmdShow = SW_SHOWNORMAL;
					switch(it->showType)
					{
					case MAX:	cmdShow = SW_SHOWMAXIMIZED;	break;
					case MIN:	cmdShow = SW_SHOWMINIMIZED;	break;
					case HIDE:	cmdShow = SW_HIDE;			break;
					default:	cmdShow = SW_SHOWNORMAL;
					}
					ShellExecute(0, _T("open"), it->exePath, it->szParam, szDir, cmdShow);
					//if (ghWnd == NULL)	MessageBox(0, L"NULL Wnd", L"est", 0);
					COPYDATASTRUCT cds = {0};
					cds.dwData = 0;
					cds.lpData = &(*it);
					cds.cbData = sizeof(KEYDATA);
					//Must be use SendMessage!!!
					SendMessage(ghWnd, WM_COPYDATA, (WPARAM)ghDLLInst, (LPARAM)&cds);
					//wsprintf(szDir, L"%ld", GetLastError());
					//MessageBox(ghWnd, szDir, L"test", 0);
				}
				break;
			}
		}//end for
	}
	return CallNextHookEx(ghHook, nCode, wParam, lParam);
}

UINT Str2Key(const TCHAR * key)
{
	/* 0~9 A~Z */
	if (lstrlen(key) == 1)
		return key[0];

	/* F1~F9 */
	else if (lstrlen(key) == 2)
		return VK_F1 + key[1] - '0' - 1;

	/* F10~F12 */
	else if (lstrlen(key) == 3)
		return VK_F10 + key[2] - '0';

	return -1;
}

bool ReadKeyList()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(ghDLLInst, szPath, MAX_PATH);

	PathRemoveFileSpec(szPath);
	lstrcat(szPath, _T("\\Data\\key.qrk"));

	if (!PathFileExists(szPath))
	{
		//MessageBox(0, szPath, _T("File Not Found!"), 0);
		return false;
	}

	HANDLE hFile;

	hFile = CreateFile(szPath, GENERIC_READ,\
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,\
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)	return false;

	int len = sizeof(KEYDATA);
	int cout = GetFileSize(hFile, NULL) / len;

	keyList.clear();

	DWORD dwRead;
	for (int i=0; i<cout; i++)
	{
		KEYDATA keyData;

		if (!ReadFile(hFile, &keyData, len, &dwRead, NULL))
		{
			CloseHandle(hFile);
			return false;
		}

		keyList.push_back(keyData);
	}
	CloseHandle(hFile);
	return true;
}