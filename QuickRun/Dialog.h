
#ifndef DIALOG_H
#define DIALOG_H

#pragma once

#include "window.h"

class Dialog : public Window
{
public:
	Dialog() : _fModeless(false), _isCreated(false)	
	{	_hWnd = NULL;	}

	~Dialog()
	{
		if(_hWnd)	
			::SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)NULL);
		_hWnd = NULL;
		_isCreated = false;
	}

	virtual void create(UINT uID, LPVOID lParam = 0);
	virtual void doModal(UINT uID, LPVOID lParam = 0);

	bool	IsCreated()		{	return _isCreated;	}

	virtual void	Destroy()
	{	
		if (!_hWnd)	return;

		if (_fModeless)
			::DestroyWindow(_hWnd);
		else
			::EndDialog(_hWnd, _nResult);

		_isCreated = false;
	}

protected:

	static	BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;


	void SetDlgIcon(UINT uIconID)
	{
		if (!_hWnd)	return ;
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(_hWnd, GWLP_HINSTANCE);
		HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(uIconID));

		SendMsg(WM_SETICON, TRUE,  (LPARAM)hIcon);
		SendMsg(WM_SETICON, FALSE, (LPARAM)hIcon);
	}

	HWND HwndFromId(int id)	{	return ::GetDlgItem(_hWnd, id);		}	//由控件ID返回句柄

	void ShowCtrl(int id)	{	::ShowWindow(HwndFromId(id), SW_SHOW);	}	//显示控件
	void HideCtrl(int id)	{	::ShowWindow(HwndFromId(id), SW_HIDE);	}	//隐藏控件

	void EnableCtrl(int id)	{	::EnableWindow(HwndFromId(id), TRUE);	}	//控件有效
	void DisableCtrl(int id){	::EnableWindow(HwndFromId(id), FALSE);	}	//控件无效

	void CheckButton(int id)	{	::CheckDlgButton(_hWnd, id, BST_CHECKED);	}
	void UnCheckButton(int id)	{	::CheckDlgButton(_hWnd, id, BST_UNCHECKED);	}

	bool IsButtonChecked(int nIDButton)
		{	return ::IsDlgButtonChecked(_hWnd, nIDButton)==BST_CHECKED ? true : false;	}

	BOOL CheckRadio(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
		{	return ::CheckRadioButton(_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton);	}

	void FocusCtrl(int id)	{	::SetFocus(HwndFromId(id));				}	//使控件取得焦点

	TString GetItemText(int id);
	void	SetItemText(int id, LPCTSTR lpText)	{	::SetWindowText(HwndFromId(id), lpText);	}

	void SetLimitText(int id, int max)		{	::SendMessage(HwndFromId(id), EM_SETLIMITTEXT, max, 0);	}
	void SetLimitText(HWND hEdit, int max)	{	::SendMessage(hEdit, EM_SETLIMITTEXT, max, 0);		}
protected:
	LPVOID _pData;

private:
	bool _fModeless;	//当前对话是无模式还是模式
	INT_PTR _nResult;//创建模式对话框返回的

	bool _isCreated;
};

#endif