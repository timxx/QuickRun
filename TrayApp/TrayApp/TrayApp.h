//===============================================================================
#ifndef TRAY_APP_H
#define TRAY_APP_H
//===============================================================================
#pragma once
//===============================================================================
#include "../../QuickRun/Window.h"
#include "KeyInfoWnd.h"
//===============================================================================
class TrayApp : public Window
{
public:
	TrayApp():_clsName(_T("QuickRunTray")){}
	~TrayApp(){}

	void init(HINSTANCE hInst, HWND hwndParent);
	TString getClassName()	const	{	return _clsName;	}

protected:
	void OnCommand(int id, HWND hwndCtl, UINT codeNotify);
	void OnClose();
	void InitTray();
	void ShowTray(bool bShow = true);
	void doRkeyMenu();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	TString _clsName;
	KeyInfoWnd _infoWnd;
	NOTIFYICONDATA _nid;
};

#endif