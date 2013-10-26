
#ifndef CFG_WND_H
#define CFG_WND_H

#pragma once

#include "window.h"
#include "ClassEx.h"
#include "resource.h"
#include "data.h"
#include <list>

class CfgWnd : public Window
{
public:
	CfgWnd():_clsName(_T("QrCfgWnd")), _fCreated(false),
		_fUpdateMode(false), hBtnBrowse(0), _whichItem(-1){}
	~CfgWnd(){}

	void init(HINSTANCE hInst, HWND hwndParent){
		Window::init(hInst, hwndParent);
		ClassEx cls(hInst, _clsName.c_str(), IDI_STAR, 0, WndProc);
		if (!cls.Register())	throw _T("Failed to Register CfgWnd");
	}
	bool Create();
	bool isCreated()	const	{	return _fCreated;	}

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnCreate(LPCREATESTRUCT lpCreateStruct);

	void showIcon();
	bool IsLegalCombination(KEYDATA &keyData);

	void doSave();
	void doBrowse();

	void OnDropFiles(HDROP hDrop);
private:
	TString _clsName;
	TString _filePath;
	bool _fCreated;

	bool _fUpdateMode;
	int _whichItem;

	std::list<KEYDATA> keyList;

	HWND hBtnBrowse;
	HWND hBtnOk;
	HWND hwndIcon;
	HWND cbAlt, cbCtl, cbSft;
	HWND cbKey;
};

#endif