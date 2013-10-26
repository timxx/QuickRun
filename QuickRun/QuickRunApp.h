//========================================================================================================

#ifndef QUICK_RUN_APP_H
#define QUICK_RUN_APP_H

#pragma once

#include <list>

#include "Window.h"
#include "Button.h"
#include "ListView.h"
#include "CfgDlg.h"
#include "data.h"

class QuickRunApp : public Window
{
public:
	QuickRunApp():_clsName(_T("QuickRun")), _fStarted(false){
		RtlSecureZeroMemory(&lvColHdrInfo, sizeof(LVCOLUMNHDRINFO));
	}
	~QuickRunApp(){}

	void init(HINSTANCE hInst, HWND hwndParent);

	TString getClassName()	const	{	return _clsName;	}

	bool IsDlgMsg(MSG *lpMsg) {	return cfgDlg.IsCreated() && IsDialogMessage(cfgDlg.getSelf(), lpMsg);	}
protected:
	void AddKeyToListView(KEYDATA &keyData);	//添加一个键到list view
	bool WriteToFile();							//将列表信息保存到文件
	bool ReadToList();							//读取文件中的信息
	bool IsKeyExists();
	bool SetAutoRun(bool fSet = true);
	void ShowPopupMenu();
	TString IniFilePath();
	bool SetLvImagelist();						//设置list view项目图标
	BOOL SaveLvColumnInfo();					//保存list view header ctrl信息
	BOOL GetLvColumnInfo();

	void SortColumn(int index);					//排序

	void OnCreate(LPCREATESTRUCT lpCS);
	void OnCommand(int id, HWND hwndCtl, UINT codeNotify);
	void OnUpdate(KEYDATA *lpKeyData, int index);
	void OnDropFiles(HDROP hDrop);

	void doNew();
	void doUpdate();
	void doDel();
	void doStart();

	static DWORD WINAPI ReadListProc(LPVOID lParam);
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	TString _clsName;

	ListView	listView;
	CfgDlg		cfgDlg;

	std::list<KEYDATA> keyList;
	typedef std::list<KEYDATA>::iterator IT;

	bool _fStarted;

	LVCOLUMNHDRINFO lvColHdrInfo;
	static TCHAR _hdrName[3][20];	//列表栏上的文字
};

#endif