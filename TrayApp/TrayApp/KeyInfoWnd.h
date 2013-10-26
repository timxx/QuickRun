//===============================================================================
#pragma once

#ifndef KEY_INFO_WND_H
#define KEY_INFO_WND_H

#define IDB_X	3000
//===============================================================================
#include "../../QuickRun/Window.h"
#include "../../QuickRun/data.h"
//===============================================================================

class KeyInfoWnd : public Window
{
public:
	KeyInfoWnd():_clsName(_T("QuickRunInfoWnd")), _beginTime(0), _stopTime(0),
		_alpha(80), _show(false), _fHovered(FALSE), hBtnX(0)
	{}
	~KeyInfoWnd();

	void init(HINSTANCE hInst, HWND hwndParent);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void AdjustPos();
	void OnPaint();
	void DrawInfo(const KEYDATA &keyData);
	TString MakeKeyStr(bool fAlt, bool fCtrl, bool fShift, const TCHAR *key);
private:
	TString _clsName;
	DWORD _beginTime;
	DWORD _stopTime;
	int _alpha;
	bool _show;
	KEYDATA _keyData;

	BOOL _fHovered;
	HWND hBtnX;
};

#endif