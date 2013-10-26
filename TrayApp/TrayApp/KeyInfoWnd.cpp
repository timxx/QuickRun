//===============================================================================
#include "KeyInfoWnd.h"
#include "../../QuickRun/ClassEx.h"
#include "resource.h"
#include "../../QuickRun/myMessage.h"
#include <GdiPlus.h>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Comctl32.lib")
//===============================================================================
#define INFO_WND_WIDTH	(int)200
#define INFO_WND_HEIGHT	(int)100

#define TIMERID_SHOW	1
#define TIMERID_ALPHA	2

//using namespace Gdiplus;
Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
ULONG_PTR gdiplusToken; 
//===============================================================================
KeyInfoWnd::~KeyInfoWnd()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

void KeyInfoWnd::init(HINSTANCE hInst, HWND hwndParent)
{
	Window::init(hInst, hwndParent);

	ClassEx cls(hInst, _clsName.c_str(), 0, 0, WndProc);
	if (!cls.Register())
		throw _T("Register Class Failed");

	_hWnd = CreateEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
		_clsName.c_str(), _T("TrayWnd"),
		WS_POPUP, 0, 0, INFO_WND_WIDTH, INFO_WND_HEIGHT, 0, (LPVOID)this);

	if (!_hWnd)	throw _T("CreateWindow Failed");

	SetLayeredWindowAttributes(_hWnd, 0, 255*(100 - _alpha)/100, LWA_ALPHA);

	AdjustPos();
	//showWindow();
	::UpdateWindow(_hWnd);

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

LRESULT CALLBACK KeyInfoWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_NCCREATE :
		{
			KeyInfoWnd *pApp = (KeyInfoWnd *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			pApp->_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

	default:
		return ((KeyInfoWnd *)::GetWindowLongPtr(hWnd, GWL_USERDATA))->runProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK KeyInfoWnd::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		OnPaint();
		break;
		
	case MM_SHOWINFO:
		{
			_keyData = *(KEYDATA*)lParam;
			if (_show)
				KillTimer(hWnd, TIMERID_SHOW);
			_show = true;
			SetTimer(_hWnd, TIMERID_SHOW, 1000, NULL);
			SetTimer(_hWnd, TIMERID_ALPHA, 10, NULL);
			_beginTime = ::GetTickCount();
			InvalidateRect(FALSE);
		}
		break;

	case WM_MOUSEMOVE:
		if (!_fHovered)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = _hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 5;
			_fHovered = _TrackMouseEvent(&tme);
		}

		break;

	case WM_MOUSEHOVER:
		_stopTime = GetTickCount();
		_alpha = 10;
		SetLayeredWindowAttributes(_hWnd, 0, 255*(100 - _alpha)/100, LWA_ALPHA);

		//if (_beginTime != 0 && _show)
		//	KillTimer(hWnd, TIMERID_SHOW);
		//else if(_beginTime != 0 && !_show)
		//	KillTimer(hWnd, TIMERID_ALPHA);
		break;

	case WM_MOUSELEAVE:
		_stopTime = GetTickCount() - _stopTime;
		//SetLayeredWindowAttributes(_hWnd, 0, 255*(100 - _alpha)/100, LWA_ALPHA);
		//if (_beginTime != 0 && _show)
		//	SetTimer(hWnd, TIMERID_SHOW, 1000, NULL);
		//else if(_beginTime != 0 && !_show)
		//	SetTimer(hWnd, TIMERID_ALPHA, 10, NULL);

		_fHovered = FALSE;
		break;

	case WM_TIMER:

		if (_fHovered)	break;

		if (wParam == TIMERID_SHOW)
		{
			DWORD dwTime = GetTickCount() - _beginTime;
			if (dwTime >= 7000 + _stopTime)
			{
				KillTimer(hWnd, TIMERID_SHOW);
				hideWindow();
			}
			else if (dwTime >= 5000 + _stopTime && _show)
			{
				_show = false;
				SetTimer(hWnd, TIMERID_ALPHA, 10, NULL);
			}
		}
		else if (wParam == TIMERID_ALPHA)
		{
			if (_show)
			{
				if (_alpha <= 10)
					KillTimer(hWnd, TIMERID_ALPHA);
				else
					SetLayeredWindowAttributes(_hWnd, 0, 255*(100 - (--_alpha))/100, LWA_ALPHA);
			}
			else
			{
				if (_alpha >= 99)
					KillTimer(hWnd, TIMERID_ALPHA);
				else
					SetLayeredWindowAttributes(_hWnd, 0, 255*(100 - (++_alpha))/100, LWA_ALPHA);
			}
		}
		break;

	case WM_WININICHANGE:
		AdjustPos();
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDB_X)
			hideWindow();
		break;

	case WM_CREATE:
		{
			Rect rect;
			GetWindowRect(&rect);

			hBtnX = CreateWindow(TEXT("Button"), TEXT("X"),
				WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
				rect.right - 15, 5, 15, 15,
				hWnd, (HMENU)IDB_X, _hinst, NULL);
		}
		break;

	default:	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void KeyInfoWnd::AdjustPos()
{
	int iScrWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScrHeight = GetSystemMetrics(SM_CYSCREEN);

	typedef HWND (WINAPI *GETTASKMANWINDOW)();
	GETTASKMANWINDOW GetTaskmanWindow = (GETTASKMANWINDOW)GetProcAddress(
		GetModuleHandle(_T("user32.dll")), "GetTaskmanWindow");
	HWND hWnd = GetTaskmanWindow();
	if (hWnd == NULL)
		return ;

	Rect rcWA;
	Rect rcTb;

	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWA, 0);
	::GetWindowRect(hWnd, &rcTb);

	int x = 0, y = 0;

	if (rcWA.left > 0) //◊Û
	{
		x = rcTb.Width();
		y = iScrHeight - 10 - INFO_WND_HEIGHT;
	}
	else if (rcWA.top > 0) //…œ
	{
		x = iScrWidth - INFO_WND_WIDTH  - 10;
		y = rcTb.Height();
	}
	else if (rcWA.Width() < iScrWidth)	//”“
	{
		x = iScrWidth - rcTb.Width() - INFO_WND_WIDTH;
		y = iScrHeight - 10 - INFO_WND_HEIGHT;
	}
	else if (rcWA.Height() < iScrHeight)	//œ¬
	{
		x = iScrWidth - INFO_WND_WIDTH  - 10;
		y = iScrHeight - INFO_WND_HEIGHT - rcTb.Height();
	}
	else
	{
		x = iScrWidth - INFO_WND_WIDTH - 10;
		y = iScrHeight - INFO_WND_HEIGHT - 10;
	}

	moveTo(x, y);
}

void KeyInfoWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(_hWnd, &ps);

	Rect rect;
	GetClientRect(&rect);

	Gdiplus::Rect gRect(0, 0, rect.Width(), rect.Height());

	Gdiplus::Color cr2(255, 83, 155, 245);
	Gdiplus::Color cr1(255, 187, 216, 251);

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::LinearGradientBrush linGrBrush(gRect, cr1, cr2, (Gdiplus::REAL)(90 - 45));
	graphics.FillRectangle(&linGrBrush, gRect);

	DrawInfo(_keyData);

	::EndPaint(_hWnd, &ps);
}

void KeyInfoWnd::DrawInfo(const KEYDATA &keyData)
{
	HDC hdc = GetDC(_hWnd);
	if (!hdc)	return ;
	SetTextColor(hdc, RGB(128, 0, 255));
	SetBkMode(hdc, TRANSPARENT);

	TString keyStr = MakeKeyStr(keyData.fAlt, keyData.fCtrl, keyData.fShift, keyData.key);

	HFONT hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("ÀŒÃÂ"));
	HGDIOBJ hOldFont = SelectObject(hdc, hFont);
	Rect rect;
	GetClientRect(&rect);
	rect.left = 10;
	rect.top = 30;

	TextOut(hdc, 10, 10, keyStr.c_str(), keyStr.length());

	SetTextColor(hdc, RGB(20, 45, 250));
	//TextOut(hdc, 10, 35, keyData.szName, lstrlen(keyData.szName));
	DrawText(hdc, keyData.szName, lstrlen(keyData.szName), &rect, DT_LEFT | DT_WORDBREAK);
	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);

	SHFILEINFO sfi = {0};
	if (SUCCEEDED(SHGetFileInfo(keyData.exePath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON)))
	{
		::DrawIcon(hdc, 10, 60, sfi.hIcon);
	}

	ReleaseDC(_hWnd, hdc);
}

TString KeyInfoWnd::MakeKeyStr(bool fAlt, bool fCtrl, bool fShift, const TCHAR *key)
{
	TCHAR keyStr[20] = {0};

	if (fAlt)	lstrcat(keyStr, _T("Alt+"));
	if (fCtrl)	lstrcat(keyStr, _T("Ctrl+"));
	if (fShift)	lstrcat(keyStr, _T("Shift+"));

	lstrcat(keyStr, key);

	return keyStr;
}