//===============================================================================
//#include <WindowsX.h>
#include "TrayApp.h"
#include "../../QuickRun/ClassEx.h"
#include "../../QuickRun/myMessage.h"
#include "resource.h"
#include "../../keybdHook/keybdHook/kbHook.h"
#include <shlwapi.h>
//===============================================================================
#ifdef _DEBUG
#pragma comment(lib, "../../Debug/keybdHook.lib")
#else
#pragma comment(lib, "../../Release/keybdHook.lib")
#endif

#pragma comment(lib, "shlwapi.lib")
//===============================================================================

void TrayApp::init(HINSTANCE hInst, HWND hwndParent)
{
	Window::init(hInst, hwndParent);

	ClassEx cls(hInst, _clsName.c_str(), IDI_APP, 0, WndProc);
	if (!cls.Register())
		throw _T("Register Class Failed");

	_hWnd = CreateEx(WS_EX_TOOLWINDOW, _clsName.c_str(), _T("TrayWnd"),
		WS_OVERLAPPED, 0, 0, 0, 0, 0, (LPVOID)this);

	if (!_hWnd)	throw _T("CreateWindow Failed");

	::UpdateWindow(_hWnd);
	InitTray();
	ShowTray();
	if (!InstallHook(_hWnd))
	{
		MsgBox(_T("安装键盘钩子失败!!!"), _T("出错了"), MB_ICONERROR | MB_ICONSTOP);
		Destroy();
	}

	_infoWnd.init(_hinst, _hWnd);
}

LRESULT CALLBACK TrayApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_NCCREATE :
		{
			TrayApp *pApp = (TrayApp *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			pApp->_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

	default:
		return ((TrayApp *)::GetWindowLongPtr(hWnd, GWL_USERDATA))->runProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK TrayApp::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{

	case WM_COMMAND:
		OnCommand(int(LOWORD(wParam)), HWND(lParam), UINT(HIWORD(wParam)));
		break;

	case WM_COPYDATA:
		{
			PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT) lParam;
			KEYDATA keyData = *(KEYDATA*)pCDS->lpData;

			_infoWnd.showWindow();

			_infoWnd.SendMsg(MM_SHOWINFO, 0, (LPARAM)&keyData);
		}
		break ;

	case WM_CLOSE:
		OnClose();
		break;

	case WM_DESTROY:
		{
			HWND hwndMain = FindWindow(_T("QuickRun"), NULL);
			if (hwndMain)	SendMessage(hwndMain, MM_TRAYQUIT, 0, 0);

			ShowTray(false);
			InstallHook(hWnd, FALSE);
			_infoWnd.Destroy();
			PostQuitMessage(0);
		}
		break;

	case MM_TRAY:
		{
			if (wParam != IDI_APP)	return 0;

			switch(lParam)
			{
//			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:

				doRkeyMenu();
				return 0;
			}
		}
		return 0;

	default:	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void TrayApp::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDM_SHOW_MAINAPP:
		{
			TCHAR exePath[MAX_PATH];
			GetModuleFileName(_hinst, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
			lstrcat(exePath, _T("\\QuickRun.exe"));
			if (!::PathFileExists(exePath))
			{
				MsgBox(_T("请确保程序存在且名字为[QuickRun.exe]"), _T("文件丢失"), MB_ICONERROR);
				break;
			}

			ShellExecute(NULL, _T("open"), exePath, NULL, NULL, SW_SHOW);
		}
		break;

	case IDM_EXIT:	OnClose();	break;
	}
}

void TrayApp::OnClose()
{
	Destroy();
}

 void TrayApp::ShowTray(bool bShow/* = true*/)
 {
	 if (bShow)
		 ::Shell_NotifyIcon(NIM_ADD, &_nid);
	 else
		 ::Shell_NotifyIcon(NIM_DELETE, &_nid);
 }

 void TrayApp::doRkeyMenu()
 {
	 HMENU hMenu = 0;
	 HMENU hMenuTP = 0;

	 hMenu = ::LoadMenu(_hinst, MAKEINTRESOURCE(IDR_MENU_RKEY));
	 if (!hMenu)	return;

	 hMenuTP = ::GetSubMenu(hMenu, 0);
	 if (!hMenuTP)	return;

	 MENUITEMINFO mii = {0};

	 mii.cbSize = sizeof(MENUITEMINFO);
	 mii.fMask = MIIM_BITMAP;

	 mii.hbmpItem = LoadBitmap(_hinst, MAKEINTRESOURCE(IDB_MAIN_APP));;
	 SetMenuItemInfo(hMenuTP, IDM_SHOW_MAINAPP, MF_BYCOMMAND, &mii);

	 mii.hbmpItem = LoadBitmap(_hinst, MAKEINTRESOURCE(IDB_CLOSE));;
	 SetMenuItemInfo(hMenuTP, IDM_EXIT, MF_BYCOMMAND, &mii);

	 POINT pt;

	 ::GetCursorPos(&pt);

	 ::TrackPopupMenuEx(hMenuTP, TPM_BOTTOMALIGN, pt.x, pt.y, _hWnd, 0);

	 ::DestroyMenu(hMenu);
 }

 void TrayApp::InitTray()
 {
	 ::RtlSecureZeroMemory(&_nid, sizeof(NOTIFYICONDATA));

	 _nid.cbSize = sizeof(NOTIFYICONDATA);
	 _nid.dwInfoFlags = NIIF_INFO;
	 _nid.dwState = NIS_SHAREDICON;
	 _nid.hWnd = _hWnd;
	 _nid.uID = IDI_APP;
	 _nid.hIcon = LoadIcon(_hinst, MAKEINTRESOURCE(IDI_APP));
	 _nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	 _nid.uCallbackMessage = MM_TRAY;

	 lstrcpy(_nid.szTip, TEXT("Quick Run Tray"));
 }