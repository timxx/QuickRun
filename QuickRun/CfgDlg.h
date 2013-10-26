
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

	void showIcon();	//��Ŀ��·��ȡ��ͼ�겢��ʾ
	bool IsLegalCombination(KEYDATA &keyData);	//���õ���ϼ��Ƿ����Ҫ��
	TCHAR *PathToName(const TCHAR *lpszPath);	//·�������ļ���
	void InitNewKey();

	void doSave();		//������[ȷ��]��Ϣ
	void doBrowse();	//ѡ��Ŀ��
	void doOptional(bool fShow = false);
	void doRestoreKey(KEYDATA keyData);	//����keyData��ʼ���Ի���

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