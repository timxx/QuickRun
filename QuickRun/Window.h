//========================================================================================================

#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <Windows.h>
#include <commctrl.h>
#include <tchar.h>

#include "TString.h"
#include "Rect.h"
#include "Point.h"

class Window
{
public:
	Window():_hWnd(0), _parentWnd(0), _hinst(0){}
	~Window(){}

	virtual void init(HINSTANCE hInst, HWND hwndParent)
	{
		_hinst = hInst;
		_parentWnd = hwndParent;
	}

	WNDPROC Subclass(WNDPROC newProc)
	{	
		::SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);
		return (WNDPROC)::SetWindowLongPtr(_hWnd, GWLP_WNDPROC, (LONG)newProc);	
	}

	HWND getSelf() const	{	return _hWnd;	}
	HWND getParent() const	{	return _parentWnd;	}

	HWND SetFocus()			{	return ::SetFocus(_hWnd);	}

	HINSTANCE getHinst() const	{	return _hinst;	}
	HINSTANCE HwndToHinst(HWND hWnd)	{	return (HINSTANCE) ::GetWindowLongPtr(hWnd, GWLP_HINSTANCE);	}

	int getTextLength()	{	return ::GetWindowTextLength(_hWnd);	}

	void setTitle(const TString &title)	{	::SetWindowText(_hWnd, title.c_str());	}
	void getTitle(TString &title)
	{
		int len = getTextLength();
		TCHAR *text = new TCHAR[len+1];

		::GetWindowText(_hWnd, text, len+1);

		title = text;
	}
	TString getTitle()	{	TString t;	getTitle(t);	return t;	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
	{	return ::SetWindowPos(_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);	}

	bool SaveWindow(const TCHAR *lpszSavePath, const TCHAR *lpszSection, const TCHAR *lpszKey){
		WINDOWPLACEMENT wpm = {0};
		wpm.length = sizeof(WINDOWPLACEMENT);
		if (!::GetWindowPlacement(_hWnd, &wpm))	return false;
		return ::WritePrivateProfileStruct(lpszSection, lpszKey, &wpm, sizeof(WINDOWPLACEMENT), lpszSavePath) ? true : false;
	}
	bool RestoreWindow(const TCHAR *lpszSavePath, const TCHAR *lpszSection, const TCHAR *lpszKey){
		WINDOWPLACEMENT wpm = {0};

		if (!::GetPrivateProfileStruct(lpszSection, lpszKey, &wpm, sizeof(WINDOWPLACEMENT), lpszSavePath))
			return false;

		//prevent hide window
		if (wpm.showCmd == SW_HIDE)
			wpm.showCmd = SW_SHOWNORMAL;

		return ::SetWindowPlacement(_hWnd, &wpm) ? true : false;
	}
	void moveTo(int x, int y)	{	SetWindowPos(0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);	}
	void resizeTo(int nWidth, int nHeight)	{	SetWindowPos(0, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER);	}

	void setTopMost(bool fSet = true)	{	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);	}

	void hideWindow()	{	::ShowWindow(_hWnd, SW_HIDE);	}
	void showWindow()	{	::ShowWindow(_hWnd, SW_SHOW);	}

	void disable()		{	::EnableWindow(_hWnd, FALSE);	}
	void enable()		{	::EnableWindow(_hWnd, TRUE);	}

	void setAlphaValue(int value)	{	::SetLayeredWindowAttributes(_hWnd, 0, (100-value)*255/100, LWA_ALPHA);	}

	int MsgBox(TString text, TString caption, UINT uType = MB_OK)
	{	return ::MessageBox(_hWnd ? _hWnd : ::GetActiveWindow(), text.c_str(), caption.c_str(), uType);	}

	LRESULT SendMsg(UINT uMsg, WPARAM wParam = 0U, LPARAM lParam = 0L)	{	return ::SendMessage(_hWnd, uMsg, wParam, lParam);	}

	virtual HWND Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		int x = CW_USEDEFAULT, int y = 0, int nWidth = CW_USEDEFAULT, int nHeight = 0,
		UINT uMenuID = 0, LPVOID lpParam = 0)
	{
		return _hWnd = ::CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth,\
			nHeight, _parentWnd, (HMENU)uMenuID, _hinst, lpParam);
	}

	virtual HWND CreateEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		int x = CW_USEDEFAULT, int y = 0, int nWidth = CW_USEDEFAULT, int nHeight = 0,
		UINT uMenuID = 0, LPVOID lpParam = 0)
	{
		return _hWnd = ::CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth,\
			nHeight, _parentWnd, (HMENU)uMenuID, _hinst, lpParam);
	}

	virtual void Destroy()		{	::DestroyWindow(_hWnd);		}

	void centerWnd(HWND hWndParent)
	{
		Rect rcParent;
		::GetWindowRect(hWndParent, &rcParent);

		int x = rcParent.left + (rcParent.Width() - getWidth())/2;
		int y = rcParent.top + (rcParent.Height() - getHeight())/2;

		moveTo(x, y);
	}
	
	DWORD getStyle()	{	return (DWORD)::GetWindowLongPtr(_hWnd, GWL_STYLE);		}
	DWORD getExStyle()	{	return (DWORD)::GetWindowLongPtr(_hWnd, GWL_EXSTYLE);	}

	BOOL InvalidateRect(const RECT *lpRect = 0, BOOL bErase = TRUE){
		return ::InvalidateRect(_hWnd, lpRect, bErase);
	}
	BOOL GetClientRect(RECT *lpRect)	{	return ::GetClientRect(_hWnd, lpRect);	}
	BOOL GetWindowRect(RECT *lpRect)	{	return ::GetWindowRect(_hWnd, lpRect);	}

	BOOL ClientToScreen(LPPOINT lpPoint)	{	return ::ClientToScreen(_hWnd, lpPoint);	}
	BOOL ScreenToClient(LPPOINT lpPoint)	{	return ::ScreenToClient(_hWnd, lpPoint);	}

	enum getType {	window, client	};

	int getWidth(getType type = window)	{
		Rect rc;
		if (type == window)	GetWindowRect(&rc);
		else				GetClientRect(&rc);
		return rc.Width();
	}
	int getHeight(getType type = window)	{
		Rect rc;
		if (type == window)	GetWindowRect(&rc);
		else				GetClientRect(&rc);
		return rc.Height();
	}

	HFONT GetFont()	{	return (HFONT)SendMsg(WM_GETFONT);	}

	HMENU GetMenu()	{	return ::GetMenu(_hWnd);	}
	HMENU LoadMenu(LPCTSTR lpMenuName)	{	return ::LoadMenu(_hinst, lpMenuName);	}
	HMENU LoadMenu(UINT nIDResource)	{	return ::LoadMenu(_hinst, MAKEINTRESOURCE(nIDResource));	}

	bool CreateToolTip(HWND hwndParent, HWND hwndTo, LPTSTR pText)
	{    
		if (!hwndParent || !hwndTo || !pText)	return false;

		HWND hwndTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_BALLOON,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hwndParent, NULL, (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE), NULL);

		if (!hwndTip)	return false;

		TOOLINFO toolInfo = { 0 };
		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.hwnd = hwndParent;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)hwndTo;
		toolInfo.lpszText = pText;
		SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

		return true;
	}
protected:
	HWND _hWnd;
	HWND _parentWnd;

	HINSTANCE _hinst;
};

#endif