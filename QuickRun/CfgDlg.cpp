
#include <shlwapi.h>
#include <shlobj.h>

#include "CfgDlg.h"
#include "resource.h"
#include "myMessage.h"
#include "QuickRunApp.h"

#pragma comment(lib, "shlwapi.lib")

BOOL CALLBACK CfgDlg::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		OnInitDlg(HWND(wParam), lParam);
		return TRUE;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
		break;

	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		break;

// 	case WM_KEYDOWN:
// 		MsgBox(L"s", L"s");
// 		break;

	case WM_CLOSE:
		//::SetFocus(getParent());
		_filePath.clear();
		hideWindow();
		Sleep(200);
		SaveShowOptional();
		doOptional(true);
		SaveWindow(IniFilePath().c_str(), _T("CfgDlg"), _T("Window"));
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

			InitNewKey();
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
			doRestoreKey(*pKeyData);
		}
		break;

	case MM_PASTEPATH:
		{
			_filePath = (TCHAR*)lParam;
			SetItemText(IDE_NAME, PathToName((TCHAR*)lParam));
			showIcon();
		}
		break;
	}
	return FALSE;
}

BOOL CfgDlg::OnInitDlg(HWND hwndFocus, LPARAM lParam)
{
	SetDlgIcon(IDI_STAR);
	RestoreWindow(IniFilePath().c_str(), _T("CfgDlg"), _T("Window"));

	_prevHeight = getHeight();

	SetLimitText(IDE_NAME, MAX_PATH);
	SetLimitText(IDE_PARAM, MAX_PATH);

	if (!GetShowOptional())
		doOptional();

	HWND cbKey = HwndFromId(IDC_KEY);
	
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
		StringCchPrintf(ss, 4, _T("F%d"), i);
		SendMessage(cbKey, CB_ADDSTRING, 0, (LPARAM)ss);
	}

	TCHAR *szShowType[] = {_T("普通方式"), _T("最大化"), _T("最小化"),
		_T("隐藏")
	};

	for (int i=0; i<4; i++)
		::SendDlgItemMessage(_hWnd, IDC_SHOWTYPE, CB_ADDSTRING, 0, (LPARAM)szShowType[i]);

	InitNewKey();

	if (_filePath.empty())
		CreateToolTip(_hWnd, HwndFromId(IDS_ICON), (TCHAR*)_T("请通过拖动或单击[选择程序]按钮选择程序"));
	else
		CreateToolTip(_hWnd, HwndFromId(IDS_ICON), (TCHAR*)_filePath.c_str());

	return TRUE;
}


void CfgDlg::doBrowse()
{
/*
	OPENFILENAME	ofn;
	TCHAR filePath[MAX_PATH] = {0};

	::RtlSecureZeroMemory(&ofn, sizeof(ofn));

	ofn.hInstance 	 = getHinst();
	ofn.hwndOwner 	 = _hWnd;
	ofn.Flags 		 = OFN_OVERWRITEPROMPT;
	ofn.lpstrFile	 = filePath;
	ofn.nMaxFile 	 = MAX_PATH;
	ofn.lStructSize  = sizeof(OPENFILENAME);
	ofn.lpstrFilter  = _T("程序文件(*.exe)\0*.exe\0所有格式(*.*)\0*.*\0\0");
	ofn.lpstrTitle   = _T("选择一个程序");

	if (::GetOpenFileName(&ofn))
	{
		_filePath = filePath;
		SetItemText(IDE_NAME, PathToName(filePath));
		showIcon();
	}
*/

	BROWSEINFO   bi = {0};       
	TCHAR	dispName[MAX_PATH],  pFileName[MAX_PATH];       
	ITEMIDLIST   *   pidl;       

	bi.hwndOwner		 =   _hWnd;       
	bi.pszDisplayName    =   dispName;       
	bi.lpszTitle		 =   TEXT("请选择一个目标");       
	bi.ulFlags			 =   BIF_BROWSEINCLUDEFILES | BIF_EDITBOX;       

	pidl = ::SHBrowseForFolder(&bi);
	if (pidl != NULL)
	{
		if (::SHGetPathFromIDList(pidl, pFileName))
		{
			_filePath = pFileName;
			SetItemText(IDE_NAME, PathToName(pFileName));
			showIcon();
		}
	}
}

void CfgDlg::doOptional(bool fShow/* = false*/)
{
	if (fShow)
	{
		SetItemText(IDB_MORE, _T("-"));
		ShowCtrl(IDS_MORE);
		HideCtrl(IDS_LINE);
		FocusCtrl(IDE_NAME);

		resizeTo(getWidth(), _prevHeight);
	}
	else
	{
		SetItemText(IDB_MORE, _T("+"));
		HideCtrl(IDS_MORE);
		ShowCtrl(IDS_LINE);

		Rect rectDlg, rectLine;

		GetWindowRect(&rectDlg);
		::GetWindowRect(HwndFromId(IDS_LINE), &rectLine);

		Point pt(rectLine.left, rectLine.top);
		ScreenToClient(&pt);

		resizeTo(getWidth(), GetSystemMetrics(SM_CYSIZE) + pt.y + 20);

	}
}

TCHAR *CfgDlg::PathToName(const TCHAR *lpszPath)
{
	TCHAR *pszTitleEx;
	int len = ::GetFileTitle(lpszPath, NULL, 0);
	pszTitleEx = new TCHAR[len];
	GetFileTitle(lpszPath, pszTitleEx, len);
 
	if (len > 4 && pszTitleEx[len - 1 - 1 - 3] == '.')
	{
		TCHAR *pszTitle = new TCHAR[len - 4];
		_tcsncpy_s(pszTitle, len - 4, pszTitleEx, len -1 - 4);

		pszTitle[len -1 - 4] = '\0';
		delete [] pszTitleEx;

		return pszTitle;
	}
	return pszTitleEx;
}

void CfgDlg::OnDropFiles(HDROP hDrop)
{
	::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	int len = ::DragQueryFile(hDrop, 0, NULL, 0);

	TCHAR *filePath = new TCHAR[len+1];

	::DragQueryFile(hDrop, 0, filePath, len+1);

	TCHAR *ext = ::PathFindExtension(filePath);

// 	if (lstrcmpi(ext, _T(".exe"))!=0)
// 		MsgBox(_T("请拖动可执行文件到此！"), _T("非法文件"), MB_ICONINFORMATION);
// 	else
	{
		_filePath = filePath;
		SetItemText(IDE_NAME, PathToName(filePath));
		showIcon();
	}
	delete [] filePath;

	::DragFinish(hDrop);
}

void CfgDlg::showIcon()
{
	if(!::PathFileExists(_filePath.c_str()))	return ;

	SHFILEINFO sfi = {0};
	SHGetFileInfo(_filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON);

	SendMessage(HwndFromId(IDS_ICON), STM_SETICON, (WPARAM)sfi.hIcon, 0);

	CreateToolTip(getSelf(), HwndFromId(IDS_ICON), (TCHAR*)_filePath.c_str());
}

bool CfgDlg::IsLegalCombination(KEYDATA &keyData)
{
	TCHAR key[30] = {0};

	if (keyData.fAlt)
		StringCchCat(key, 30, _T("Alt+"));
	if (keyData.fCtrl)
		StringCchCat(key, 30, _T("Ctrl+"));
	if (keyData.fShift)
		StringCchCat(key, 30, _T("Shift+"));

	StringCchCat(key, 30, keyData.key);

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

void CfgDlg::doSave()
{
	KEYDATA keyData;
	if (_filePath.empty())
	{
		//MsgBox(_T("请选择一个可执行文件（*.exe)！"), _T("路径不能为空"), MB_ICONINFORMATION);
		MsgBox(_T("请选择一个有效的目标路径！"), _T("路径不能为空"), MB_ICONINFORMATION);
		return  ;
	}

	keyData.fAlt	= IsButtonChecked(IDC_ALT);
	keyData.fCtrl	= IsButtonChecked(IDC_CTRL);
	keyData.fShift	= IsButtonChecked(IDC_SHIFT);

	int sel = SendMessage(HwndFromId(IDC_KEY), CB_GETCURSEL, 0, 0);
	SendMessage(HwndFromId(IDC_KEY), CB_GETLBTEXT, sel, (LPARAM)keyData.key);

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

	StringCchCopy(keyData.exePath, MAX_PATH, _filePath.c_str());
	StringCchCopy(keyData.szName, MAX_PATH, GetItemText(IDE_NAME).c_str());
	StringCchCopy(keyData.szParam, MAX_PATH, GetItemText(IDE_PARAM).c_str());
	keyData.showType = (RunType)SendDlgItemMessage(_hWnd, IDC_SHOWTYPE, CB_GETCURSEL, 0, 0);

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

	SendMsg(WM_CLOSE);
}

void CfgDlg::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
	SendMsg(DM_SETDEFID, IDB_OK);

	switch(id)
	{
	case IDB_BROWSE:
		doBrowse();
		break;

	case IDB_OK:
		doSave();
		break;

	case IDB_MORE:
		if (GetItemText(IDB_MORE) == _T("+"))
			doOptional(true);
		else
			doOptional();

		break;
	}
}

void CfgDlg::InitNewKey()
{
	UnCheckButton(IDC_ALT);
	CheckButton(IDC_CTRL);
	CheckButton(IDC_SHIFT);

	::SendMessage(HwndFromId(IDC_KEY), CB_SETCURSEL, 15, 0);

	::SendDlgItemMessage(_hWnd, IDS_ICON, STM_SETICON, NULL, 0);

	SetItemText(IDE_NAME, _T(""));
	SetItemText(IDE_PARAM, _T(""));

	::SendDlgItemMessage(_hWnd, IDC_SHOWTYPE, CB_SETCURSEL, 0, 0);

	_filePath.clear();
}

void CfgDlg::doRestoreKey(KEYDATA keyData)
{
	SendMessage(HwndFromId(IDC_ALT), BM_SETCHECK, keyData.fAlt ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(HwndFromId(IDC_CTRL), BM_SETCHECK, keyData.fCtrl ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(HwndFromId(IDC_SHIFT), BM_SETCHECK, keyData.fShift ? BST_CHECKED : BST_UNCHECKED, 0);

	int index = SendMessage(HwndFromId(IDC_KEY), CB_FINDSTRING, -1, (LPARAM)keyData.key);
	if (index != CB_ERR)
		SendMessage(HwndFromId(IDC_KEY), CB_SETCURSEL, index, 0);

	_filePath = keyData.exePath;
	showIcon();

	SetItemText(IDE_NAME, keyData.szName);
	SetItemText(IDE_PARAM, keyData.szParam);

	SendDlgItemMessage(_hWnd, IDC_SHOWTYPE, CB_SETCURSEL, keyData.showType, 0);
}

TString CfgDlg::IniFilePath()
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(_hinst, szPath, MAX_PATH);

	PathRemoveFileSpec(szPath);

	StringCchCat(szPath, MAX_PATH, _T("\\Data"));

	if (!::PathFileExists(szPath))
		CreateDirectory(szPath, NULL);

	StringCchCat(szPath, MAX_PATH, _T("\\config.ini"));

	return szPath;
}

void CfgDlg::SaveShowOptional()
{
	if (GetItemText(IDB_MORE) == _T("+"))
		::WritePrivateProfileString(_T("CfgDlg"), _T("ShowOptional"), _T("0"), IniFilePath().c_str());
	else
		::WritePrivateProfileString(_T("CfgDlg"), _T("ShowOptional"), _T("1"), IniFilePath().c_str());
}

bool CfgDlg::GetShowOptional()
{
	if (!::PathFileExists(IniFilePath().c_str()))
		return false;

	return ::GetPrivateProfileInt(_T("CfgDlg"), _T("ShowOptional"),
		0, IniFilePath().c_str()) == 1 ? true : false;
}