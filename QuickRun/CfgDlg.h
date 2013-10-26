
#ifndef CFG_DIALOG_H
#define CFG_DIALOG_H

#pragma once

#include <list>
#include "dialog.h"
#include "data.h"

class CfgDlg : public Dialog
{
public:
	CfgDlg():_fUpdateMode(false), _whichItem(0), _prevHeight(0){}
	~CfgDlg(){}

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void showIcon();	//由目标路径取得图标并显示
	bool IsLegalCombination(KEYDATA &keyData);	//设置的组合键是否符合要求
	TCHAR *PathToName(const TCHAR *lpszPath);	//路径返回文件名
	void InitNewKey();

	void doSave();		//处理单击[确定]消息
	void doBrowse();	//选择目标
	void doOptional(bool fShow = false);
	void doRestoreKey(KEYDATA keyData);	//根据keyData初始化对话框

	TString IniFilePath();
	void SaveShowOptional();
	bool GetShowOptional();

	BOOL OnInitDlg(HWND hwndFocus, LPARAM lParam);
	void OnDropFiles(HDROP hDrop);
	void OnCommand(int id, HWND hwndCtl, UINT codeNotify);

private:
	bool _fUpdateMode;
	int _whichItem;

	TString _filePath;
	std::list<KEYDATA> keyList;

	int _prevHeight;
};

#endif