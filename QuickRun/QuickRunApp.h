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
	void AddKeyToListView(KEYDATA &keyData);	//���һ������list view
	bool WriteToFile();							//���б���Ϣ���浽�ļ�
	bool ReadToList();							//��ȡ�ļ��е���Ϣ
	bool IsKeyExists();
	bool SetAutoRun(bool fSet = true);
	void ShowPopupMenu();
	TString IniFilePath();
	bool SetLvImagelist();						//����list view��Ŀͼ��
	BOOL SaveLvColumnInfo();					//����list view header ctrl��Ϣ
	BOOL GetLvColumnInfo();

	void SortColumn(int index);					//����

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
	static TCHAR _hdrName[3][20];	//�б����ϵ�����
};

#endif