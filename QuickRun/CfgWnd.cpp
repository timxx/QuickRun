

#include "CfgWnd.h"
#include "myMessage.h"
#include "data.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

const int BTN_BROWSE_ID = 100;
const int BTN_OK_ID		= 101;
const int CB_ALT_ID = 102;
const int CB_CTL_ID = 103;
const int CB_SFT_ID = 104;

bool CfgWnd::Create()
{
	_hWnd = CreateEx(WS_EX_ACCEPTFILES, _clsName.c_str(), _T("快捷键设置"), \
		WS_SYSMENU | WS_OVERLAPPED, 0, 0, 235, 150, 0, (LPVOID)this);

	if (!_hWnd)	return false;

	_fCreated  = true;
	showWindow();

	return true;
}

LRESULT CALLBACK CfgWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_NCCREATE :
		{
			CfgWnd *pWnd = (CfgWnd *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			pWnd->_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWnd);
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

	default:
		return ((CfgWnd *)::GetWindowLongPtr(hWnd, GWL_USERDATA))->runProc(hWnd, uMsg, wParam, lParam);
	}
}
#include<WindowsX.h>
LRESULT CALLBACK CfgWnd::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:

		OnCreate((LPCREATESTRUCT)lParam);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == BTN_BROWSE_ID)
		{
			doBrowse();
		}
		else if (LOWORD(wParam) == BTN_OK_ID)
		{
			doSave();
			SendMsg(WM_CLOSE);
		}
		else
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;

	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		break;

	case WM_CLOSE:
		_fCreated = false;
		::SetFocus(getParent());
		_filePath.clear();
		Destroy();
		break;

	case MM_NEW:
		{
			if (!_filePath.empty())
			{
				int sel = MsgBox(_T("当前项目未保存，是否保存再修改新项目？"),
					_T("请确认"), MB_ICONQUESTION | MB_YESNO);

				if (sel == IDYES)
					doSave();
			}
			_fUpdateMode = false;
			keyList = *(std::list<KEYDATA>*)lParam;

			SendMessage(cbAlt, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(cbCtl, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(cbSft, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(cbKey, CB_SETCURSEL, 15, 0);

			_filePath.clear();
			SendMessage(hwndIcon, STM_SETICON, (WPARAM)NULL, 0);

		}

		break;

	case MM_UPDATE:
		{
			if (!_filePath.empty())
			{
				int sel = MsgBox(_T("当前项目未保存，是否保存再新增项目？"),
					_T("请确认"), MB_ICONQUESTION | MB_YESNO);

				if (sel == IDYES)
					doSave();
			}

			KEYDATA *pKeyData = (KEYDATA*)lParam;
			_whichItem = (int)wParam;
			_fUpdateMode = true;

			SendMessage(cbAlt, BM_SETCHECK, pKeyData->fAlt ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(cbCtl, BM_SETCHECK, pKeyData->fCtrl ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(cbSft, BM_SETCHECK, pKeyData->fShift ? BST_CHECKED : BST_UNCHECKED, 0);

			int index = SendMessage(cbKey, CB_FINDSTRING, -1, (LPARAM)pKeyData->key);
			if (index != CB_ERR)
				SendMessage(cbKey, CB_SETCURSEL, index, 0);

			_filePath = pKeyData->exePath;
			showIcon();
		}
		break;

	default:	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void CfgWnd::showIcon()
{
	if(!::PathFileExists(_filePath.c_str()))	return ;

	SHFILEINFO sfi = {0};
	SHGetFileInfo(_filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON);

	SendMessage(hwndIcon, STM_SETICON, (WPARAM)sfi.hIcon, 0);

	CreateToolTip(getSelf(), hwndIcon, (TCHAR*)_filePath.c_str());
}

bool CfgWnd::IsLegalCombination(KEYDATA &keyData)
{
	TCHAR key[30] = {0};

	if (keyData.fAlt)
		lstrcat(key, _T("Alt+"));
	if (keyData.fCtrl)
		lstrcat(key, _T("Ctrl+"));
	if (keyData.fShift)
		lstrcat(key, _T("Shift+"));

	lstrcat(key, keyData.key);

	TCHAR *sysHotKey[] = {
		_T("Ctrl+X"),
		_T("Ctrl+C"),
		_T("Ctrl+V"),
		_T("Ctrl+Z"),
		_T("Ctrl+F"),
		_T("Ctrl+A"),
		_T("Ctrl+S"),
		_T("Ctrl+N"),
		_T("Ctrl+F"),
		_T("Alt+F4"),
		_T("Alt+F12")
	};

	for (int i=0; i<11; i++)
	{
		if (lstrcmpi(key, sysHotKey[i]) == 0)
			return false;
	}

	return true;
}

void CfgWnd::doSave()
{
	KEYDATA keyData;
	if (_filePath.empty())
	{
		MsgBox(_T("请选择一个可执行文件（*.exe)！"), _T("路径不能为空"), MB_ICONINFORMATION);
		return  ;
	}
	keyData.fCtrl	= (SendMessage(cbCtl, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
	keyData.fShift	= (SendMessage(cbSft, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
	keyData.fAlt	= (SendMessage(cbAlt, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;

	int sel = SendMessage(cbKey, CB_GETCURSEL, 0, 0);
	SendMessage(cbKey, CB_GETLBTEXT, sel, (LPARAM)keyData.key);

	if (lstrlen(keyData.key)==1 &&
		!keyData.fCtrl &&
		!keyData.fShift &&
		!keyData.fAlt
		)
	{
		MsgBox(_T("不能单独设置数字、字母为热键！"), _T("友情提醒"), MB_ICONINFORMATION);
		return ;
	}

	if (!IsLegalCombination(keyData))
	{
		MsgBox(_T("该热键已被系统使用，请设置其它热键。"), _T("友情提醒"), MB_ICONINFORMATION);
		return ;
	}

	lstrcpy(keyData.exePath, _filePath.c_str());

	bool fKeyExists = false;
	bool fPathExists = false;
	int index = 0;
	for (std::list<KEYDATA>::iterator it = keyList.begin();
		it != keyList.end(); it++, index++)
	{
		if (it->fAlt == keyData.fAlt &&
			it->fCtrl == keyData.fCtrl &&
			it->fShift == keyData.fShift &&
			lstrcmpi(it->key, keyData.key)==0
			) //key exists
		{
			fKeyExists = true;
			if (lstrcmpi(it->exePath, keyData.exePath)==0)
				fPathExists = true;

			break;
		}
	}

	if (fKeyExists && !_fUpdateMode)	//热键重复
	{
		if (fPathExists)
		{
			MsgBox(_T("该项目已存在，不能增加重复记录"), _T("友情提醒"), MB_ICONINFORMATION);
			return ;
		}
		int sel = MsgBox(_T("该热键已经设置过！是否覆盖原有记录？\r\n")
			_T("是－覆盖\r\n否－不保存或重新设置。"),
			_T("请确认"), MB_YESNO | MB_ICONQUESTION);

		if (sel == IDYES)
			SendMessage(getParent(), MM_UPDATE, index, (LPARAM)&keyData);
		else
			return ;
	}
	else 
	{
		if (_fUpdateMode)
			SendMessage(getParent(), MM_UPDATE, _whichItem, (LPARAM)&keyData);
		else
			SendMessage(getParent(), MM_ADDKEY, 0, (LPARAM)&keyData);
	}
}

void CfgWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	const int btnWidth	= 90;
	const int btnHeight = 30;

	hBtnBrowse = ::CreateWindow(_T("button"), _T("选择程序"),\
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		getWidth(client)-btnWidth-10, (getHeight(client)-btnHeight)/2, \
		btnWidth, btnHeight, _hWnd, (HMENU)BTN_BROWSE_ID, lpCreateStruct->hInstance, 0);

	hBtnOk = ::CreateWindowW(_T("button"), _T("确定"),\
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		getWidth(client)-btnWidth-10, (getHeight(client)-btnHeight)/2+btnHeight+5, \
		btnWidth, btnHeight, _hWnd, (HMENU)BTN_OK_ID, lpCreateStruct->hInstance, 0);

	hwndIcon = ::CreateWindow(_T("static"), _T(""), 
		WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_ICON | SS_NOTIFY, 
		30, (getHeight(client)-48)/2+15, 48, 48, _hWnd, 0, lpCreateStruct->hInstance, 0);

	cbAlt = ::CreateWindow(_T("BUTTON"), _T("Alt"), BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 
		5, 10, 50, 20, _hWnd, (HMENU)CB_ALT_ID, lpCreateStruct->hInstance, 0);

	cbCtl = ::CreateWindow(_T("BUTTON"), _T("Ctrl"), BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 
		55, 10, 50, 20, _hWnd, (HMENU)CB_CTL_ID, lpCreateStruct->hInstance, 0);

	cbSft = ::CreateWindow(_T("BUTTON"), _T("Shift"), BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 
		105, 10, 50, 20, _hWnd, (HMENU)CB_SFT_ID, lpCreateStruct->hInstance, 0);

	cbKey = ::CreateWindow(_T("COMBOBOX"), _T("A"), 
		CBS_DROPDOWNLIST | CBS_UPPERCASE | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 
		170, 10, 47, 25, _hWnd, (HMENU)0, lpCreateStruct->hInstance, 0);

	::SendMessage(cbKey, CB_LIMITTEXT, 3, 0);

	for (TCHAR c = _T('A'); c<=_T('Z'); c++)
	{
		TCHAR ss[2] = {0};
		ss[0] = c;
		SendMessage(cbKey, CB_ADDSTRING, 0, (LPARAM)ss);
	}
	for (TCHAR c = _T('0'); c<=_T('9'); c++)
	{
		TCHAR ss[2] = {0};
		ss[0] = c;
		SendMessage(cbKey, CB_ADDSTRING, 0, (LPARAM)ss);
	}

	for (int i = 1; i<=12; i++)
	{
		TCHAR ss[4] = {0};
		wsprintf(ss, _T("F%d"), i);
		SendMessage(cbKey, CB_ADDSTRING, 0, (LPARAM)ss);
	}

	::SendMessage(cbKey, CB_SETCURSEL, 15, 0);

	CheckDlgButton(_hWnd, CB_CTL_ID, BST_CHECKED);
	CheckDlgButton(_hWnd, CB_SFT_ID, BST_CHECKED);

	if (_filePath.empty())
		CreateToolTip(_hWnd, hwndIcon, (TCHAR*)_T("请通过拖动或单击[选择程序]按钮选择程序"));
	else
		CreateToolTip(_hWnd, hwndIcon, (TCHAR*)_filePath.c_str());
}

void CfgWnd::doBrowse()
{
	OPENFILENAME	ofn;
	SYSTEMTIME		st;
	TCHAR filePath[MAX_PATH] = {0};

	::RtlSecureZeroMemory(&ofn, sizeof(ofn));
	::RtlSecureZeroMemory(&st, sizeof(SYSTEMTIME));

	ofn.hInstance 	 = getHinst();
	ofn.hwndOwner 	 = _hWnd;
	ofn.Flags 		 = OFN_OVERWRITEPROMPT;
	ofn.lpstrFile	 = filePath;
	ofn.nMaxFile 	 = MAX_PATH;
	ofn.lStructSize  = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = _T("程序文件(*.exe)\0*.exe\0\0");
	ofn.lpstrTitle   = _T("选择一个程序");

	if (::GetOpenFileName(&ofn))
	{
		_filePath = filePath;
		showIcon();
	}
}
void CfgWnd::OnDropFiles(HDROP hDrop)
{
	::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	int len = ::DragQueryFile(hDrop, 0, NULL, 0);

	TCHAR *filePath = new TCHAR[len+1];

	::DragQueryFile(hDrop, 0, filePath, len+1);

	TCHAR *ext = ::PathFindExtension(filePath);

	if (lstrcmpi(ext, _T(".exe"))!=0)
		MsgBox(_T("请拖动可执行文件到此！"), _T("非法文件"), MB_ICONINFORMATION);
	else
	{
		_filePath = filePath;
		showIcon();
	}
	delete [] filePath;

	::DragFinish(hDrop);
}
