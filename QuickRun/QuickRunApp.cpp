//========================================================================================================
//#pragma warning(disable : 4995)

#include <Shlwapi.h>

#include "QuickRunApp.h"
#include "ClassEx.h"
#include "resource.h"
#include "myMessage.h"
#include "File.h"
#include "AboutDlg.h"
#include "common.h"

#pragma comment(lib, "shlwapi.lib")

TCHAR QuickRunApp::_hdrName[3][20] = {TEXT("显示名称"), TEXT("快捷键"), TEXT("目标路径")};

//========================================================================================================
void QuickRunApp::init(HINSTANCE hInst, HWND hwndParent)
{
	Window::init(hInst, hwndParent);


	ClassEx cls(hInst, _clsName.c_str(), IDI_MAIN, IDR_MENU_MAIN, WndProc);
	if (!cls.Register())
		throw _T("Register Class Failed");

	_hWnd = CreateEx(WS_EX_ACCEPTFILES, _clsName.c_str(), _T("QuickRun"),
		WS_OVERLAPPEDWINDOW, 0, 0, 500, 250, 0, (LPVOID)this);

	if (!_hWnd)	throw _T("CreateWindow Failed");

	if (!RestoreWindow(IniFilePath().c_str(), _T("QuickRunApp"), _T("Window")))
		centerWnd(GetDesktopWindow());

	showWindow();
	::UpdateWindow(_hWnd);

	if (IsKeyExists())
	{
		CheckMenuItem(GetMenu(), IDM_AUTORUN, MF_BYCOMMAND | MF_CHECKED);
	}
	if (::FindWindow(_T("QuickRunTray"), NULL))
	{
		CheckMenuItem(GetMenu(), IDM_START, MF_BYCOMMAND | MF_CHECKED);
		_fStarted = true;
	}

	cfgDlg.init(hInst, _hWnd);
}

LRESULT CALLBACK QuickRunApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_NCCREATE :
		{
			QuickRunApp *pApp = (QuickRunApp *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			pApp->_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

	default:
		return ((QuickRunApp *)::GetWindowLongPtr(hWnd, GWL_USERDATA))->runProc(hWnd, uMsg, wParam, lParam);
	}
}
bool sortUp = true;
bool cmpName(const KEYDATA &lhs, const KEYDATA &rhs){
	if (sortUp)
		return lstrcmpi(lhs.szName, rhs.szName)<0? true:false;
	else
		return lstrcmpi(lhs.szName, rhs.szName)<0? false:true;
}

bool cmpKey(const KEYDATA &lhs, const KEYDATA &rhs){

	TCHAR key1[30] = {0};
	TCHAR key2[30] = {0};

	if (lhs.fAlt)
		StringCchCat(key1, 30, _T("Alt+"));
	if (lhs.fCtrl)
		StringCchCat(key1, 30, _T("Ctrl+"));
	if (lhs.fShift)
		StringCchCat(key1, 30, _T("Shift+"));

	StringCchCat(key1, 30, lhs.key);

	if (rhs.fAlt)
		StringCchCat(key2, 30, _T("Alt+"));
	if (rhs.fCtrl)
		StringCchCat(key2, 30, _T("Ctrl+"));
	if (rhs.fShift)
		StringCchCat(key2, 30, _T("Shift+"));

	StringCchCat(key2, 30, rhs.key);

	if (sortUp)
		return lstrcmpi(key1, key2)<0?true:false;
	else
		return lstrcmpi(key1, key2)<0?false:true;
}

bool cmpPath(const KEYDATA &lhs, const KEYDATA &rhs){
	if (sortUp)
		return lstrcmpi(lhs.exePath, rhs.exePath)<0?true:false;
	else
		return lstrcmpi(lhs.exePath, rhs.exePath)<0?false:true;
}

LRESULT CALLBACK QuickRunApp::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		OnCreate(LPCREATESTRUCT(lParam));
		break;

	case WM_SIZE:
		listView.resizeTo(LOWORD(lParam), HIWORD(lParam));
		listView.SetColumnWidth(0, 110);
		listView.SetColumnWidth(1, 110);
		listView.SetColumnWidth(2);
		break;

	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_RCLICK:
				ShowPopupMenu();
				break;

			case LVN_COLUMNCLICK:
				{
					int whichItem = ((NM_LISTVIEW *)lParam)->iSubItem;
					if (whichItem > 2)
						break;

					SortColumn(whichItem);
				}
				break;
 			}
		}
		break;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
		break;

	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		break;

	case WM_CLOSE:
		Destroy();
		break ;

	case WM_DESTROY:
		SaveWindow(IniFilePath().c_str(), _T("QuickRunApp"), _T("Window"));
		SaveLvColumnInfo();
		PostQuitMessage(0);
		break;

	case MM_ADDKEY:
		{
			KEYDATA *lpKeyData = (KEYDATA*)lParam;
			keyList.push_back(*lpKeyData);
			AddKeyToListView(*lpKeyData);
			WriteToFile();
			lvColHdrInfo.sortType = !lvColHdrInfo.sortType;
			SortColumn(lvColHdrInfo.sortIndex);

			if (keyList.size() == 1 && _fStarted == false)
			{
				int sel = MsgBox(_T("您还未开启热键系统，是否现在开启？"),
					_T("友情提醒"), MB_ICONQUESTION | MB_ICONINFORMATION | MB_YESNO);

				if (sel == IDYES)
					SendMsg(WM_COMMAND, MAKEWPARAM(IDM_START, 0));
			}
		}
		break;

	case MM_UPDATE:
		OnUpdate((KEYDATA*)lParam, int(wParam));
		lvColHdrInfo.sortType = !lvColHdrInfo.sortType;
		SortColumn(lvColHdrInfo.sortIndex);
		break;

	case MM_TRAYQUIT:
		CheckMenuItem(GetMenu(), IDM_START, MF_BYCOMMAND | MF_UNCHECKED);
		_fStarted = false;
		break;

	default:	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void QuickRunApp::AddKeyToListView(KEYDATA &keyData)
{
	LVITEM lvItem = {0};
	TCHAR key[30] = {0};

	if (keyData.fAlt)
		StringCchCat(key, 30, _T("Alt+"));
	if (keyData.fCtrl)
		StringCchCat(key, 30, _T("Ctrl+"));
	if (keyData.fShift)
		StringCchCat(key, 30, _T("Shift+"));

	StringCchCat(key, 30, keyData.key);
	
	lvItem.cchTextMax = MAX_PATH;
	lvItem.iItem = listView.GetItemCount();

	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;

	if (lvColHdrInfo.colIndex[0] == 0)
	{
		lvItem.pszText = keyData.szName;

		listView.InsertItem(&lvItem);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[1], key);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[2], keyData.exePath);

	}
	else if (lvColHdrInfo.colIndex[0] == 1)
	{
		lvItem.pszText =key;
		listView.InsertItem(&lvItem);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[0], keyData.szName);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[1], keyData.exePath);

	}
	else if (lvColHdrInfo.colIndex[0] == 2)
	{
		lvItem.pszText = keyData.exePath;
		listView.InsertItem(&lvItem);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[0], keyData.szName);
		listView.SetItemText(lvItem.iItem, lvColHdrInfo.colIndex[1], key);
	}
}

bool QuickRunApp::WriteToFile()
{
	//if (keyList.empty())
	//	return true;

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(_hinst, szPath, MAX_PATH);

	PathRemoveFileSpec(szPath);
	StringCchCat(szPath, MAX_PATH, _T("\\Data"));

	if (!PathFileExists(szPath))
	{
		if (!CreateDirectory(szPath, NULL))
		{
			TString strInfo(100, _T("Failed to CreateDirectory with code: %d"), GetLastError());
			MsgBox(strInfo, _T("Error"), MB_ICONERROR);
			return false;
		}
	}
	StringCchCat(szPath, MAX_PATH, _T("\\key.qrk"));
	File file(szPath, File::write);

	for (std::list<KEYDATA>::iterator it = keyList.begin();
		it != keyList.end(); it++)
	{
		if (!file.Write(&(*it), sizeof(KEYDATA)))
			return false;
	}

	return true;
}

bool QuickRunApp::ReadToList()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(_hinst, szPath, MAX_PATH);

	PathRemoveFileSpec(szPath);
	StringCchCat(szPath, MAX_PATH, _T("\\Data\\key.qrk"));

	if (!PathFileExists(szPath))
		return true;

	File file(szPath, File::read);

	int len = sizeof(KEYDATA);
	int cout = file.GetSize() / len;

	for (int i=0; i<cout; i++)
	{
		KEYDATA keyData;

		if (!file.Read(&keyData, len))
			return false;

		keyList.push_back(keyData);
	}

	return true;
}

bool QuickRunApp::IsKeyExists()
{
	HKEY hKey;
	long lResult = ERROR_SUCCESS;

	lResult = ::RegCreateKeyEx(HKEY_CURRENT_USER,\
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL,\
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hKey, NULL);

	lResult = ::RegQueryValueEx(hKey, _T("Quick_Run_Tray"), 0, NULL, NULL, NULL);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND)
		{
			::RegCloseKey(hKey);
			return false;
		}
	}

	::RegCloseKey(hKey);

	return true;
}

bool QuickRunApp::SetAutoRun(bool fSet)
{
	HKEY hKey;
	long lResult = ERROR_SUCCESS;
	bool fOk = false;

	lResult = ::RegCreateKeyEx(HKEY_CURRENT_USER,\
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL,\
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hKey, NULL);

	if (lResult != ERROR_SUCCESS)
		goto _exit;

	if (fSet)
	{
		TCHAR exePath[MAX_PATH];
		GetModuleFileName(_hinst, exePath, MAX_PATH);
		PathRemoveFileSpec(exePath);

		StringCchCat(exePath, MAX_PATH, _T("\\TrayApp.exe"));

		if (!PathFileExists(exePath))
			goto _exit;

		lResult = ::RegSetValueEx(hKey, _T("Quick_Run_Tray"),
			0, REG_SZ, (const BYTE*)exePath, (lstrlen(exePath)+1)*sizeof(TCHAR));
	}
	else
	{
		if (IsKeyExists())
			lResult = ::RegDeleteValue(hKey, _T("Quick_Run_Tray"));
	}

	if (lResult == ERROR_SUCCESS)
		fOk = true;

_exit:
	::RegCloseKey(hKey);

	return fOk;
}

void QuickRunApp::ShowPopupMenu()
{
	HMENU hMenu = 0;
	HMENU hMenuTP = 0;

	hMenu = LoadMenu(IDR_MENU_RKEY);
	hMenuTP = ::GetSubMenu(hMenu, 0);

	if (!hMenu)	return ;

	if (listView.GetSelectionMark() == -1)
	{
		::EnableMenuItem(hMenuTP, IDM_DEL, MF_BYCOMMAND | MF_DISABLED);
		::EnableMenuItem(hMenuTP, IDM_UPDATE, MF_BYCOMMAND | MF_DISABLED);
	}

	if (listView.GetItemCount() == 0)
		::EnableMenuItem(hMenuTP, IDM_DEL_ALL, MF_BYCOMMAND | MF_DISABLED);

	POINT pt;
	::GetCursorPos(&pt);
	::TrackPopupMenuEx(hMenuTP, TPM_BOTTOMALIGN, pt.x, pt.y, _hWnd, 0);

	::DestroyMenu(hMenu);
}

void QuickRunApp::OnCreate(LPCREATESTRUCT lpCS)
{
	listView.init(getHinst(), _hWnd);

	if (!listView.Create())
		throw _T("Create ListView Failed");

	listView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |\
		LVS_EX_TRACKSELECT | LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER |\
		LVS_EX_HEADERDRAGDROP);

	listView.resizeTo(getWidth(client), getHeight(client));

	if (!GetLvColumnInfo())
	{
		lvColHdrInfo.colIndex[0] = 0;
		lvColHdrInfo.colIndex[1] = 1;
		lvColHdrInfo.colIndex[2] = 2;
		lvColHdrInfo.sortIndex = 0;
		lvColHdrInfo.sortType = 0;
	}

// 	TCHAR   *szString[] = {_T("显示名称"), _T("快捷键"), _T("程序路径")};
	LV_COLUMN   lvColumn = {0};

	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	lvColumn.pszText = _hdrName[lvColHdrInfo.colIndex[0]];
	listView.InsertColumn(0, &lvColumn);

	lvColumn.pszText = _hdrName[lvColHdrInfo.colIndex[1]];
	listView.InsertColumn(1, &lvColumn);

	lvColumn.pszText = _hdrName[lvColHdrInfo.colIndex[2]];
	listView.InsertColumn(2, &lvColumn);

	// 	for(int i = 0; i < 3; i++)
// 	{
// 		lvColumn.pszText = szString[i];
// 		listView.InsertColumn(i, &lvColumn);
// 	}

 	ReadToList();
	lvColHdrInfo.sortType = !lvColHdrInfo.sortType;
	SortColumn(lvColHdrInfo.sortIndex);

	//用线程显示图标太慢了……
//	HANDLE hThread = CreateThread(NULL, 0, ReadListProc, this, 0, 0);

//	CloseHandle(hThread);
}

void QuickRunApp::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDM_NEW:		doNew();		break;
	case IDM_UPDATE:	doUpdate();		break;
	case IDM_DEL:		doDel();		break;

	case IDM_DEL_ALL:
		if (MsgBox(_T("是否删除全部记录？"), _T("请确认"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			listView.DeleteAllItems();
			keyList.clear();
			WriteToFile();
		}
		break;

	case IDM_AUTORUN:
		if (GetMenuState(GetMenu(), IDM_AUTORUN, MF_BYCOMMAND) == MF_CHECKED)
		{
			if (SetAutoRun(false))
				CheckMenuItem(GetMenu(), IDM_AUTORUN, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else
		{
			if (SetAutoRun())
				CheckMenuItem(GetMenu(), IDM_AUTORUN, MF_BYCOMMAND | MF_CHECKED);
		}

		break;

	case IDM_START:	doStart();			break;

	case IDM_ABOUT:
		{
			AboutDlg aboutDlg;
			aboutDlg.init(_hinst, _hWnd);
			aboutDlg.doModal(IDD_ABOUT);
		}
		break;

	case IDM_HELP:
		{
			TCHAR szPath[MAX_PATH];
			GetModuleFileName(_hinst, szPath, MAX_PATH);
			PathRemoveFileSpec(szPath);
			StringCchCat(szPath, MAX_PATH, _T("\\QuickRun.CHM"));

			if (!::PathFileExists(szPath))
			{
				MsgBox(_T("帮助文件丢失！"), _T("文件丢失"), MB_ICONERROR);
				break;
			}
			ShellExecute(NULL, _T("open"), szPath, NULL, NULL, SW_SHOW);
		}
		break;

	case IDM_EXIT:	SendMsg(WM_CLOSE);	break;

	case IDM_PASTE:
		{
			if (OpenClipboard(_hWnd))
			{
				if (IsClipboardFormatAvailable(CF_HDROP))
				{
					HDROP hDrop = (HDROP)GetClipboardData(CF_HDROP);
					if (hDrop)
					{
						//OnDropFiles(hDrop);
						SendMsg(WM_DROPFILES, (WPARAM)hDrop);
					}
				}
				else if (IsClipboardFormatAvailable(CF_TEXT))
				{
					HGLOBAL  hglb;
					LPSTR    lpStr;

					hglb = GetClipboardData(CF_TEXT); 
					if (hglb != NULL) 
					{ 
						lpStr = (LPSTR)GlobalLock(hglb); 
						if (lpStr != NULL) 
						{
							TCHAR *ptStr = NULL;
#ifdef UNICODE
							ptStr = A2U(lpStr);
#else
							ptStr = lpStr;
#endif
							if (::PathFileExists(ptStr))
							{
								doNew();
								cfgDlg.SendMsg(MM_PASTEPATH, 0, reinterpret_cast<LPARAM>(ptStr));
							}
							GlobalUnlock(hglb); 
						} 
					} 
				}
				else if (IsClipboardFormatAvailable(CF_UNICODETEXT))
				{
					HGLOBAL  hglb;
					LPWSTR    lpStr;

					hglb = GetClipboardData(CF_UNICODETEXT); 
					if (hglb != NULL) 
					{ 
						lpStr = (LPWSTR)GlobalLock(hglb); 
						if (lpStr != NULL) 
						{
							TCHAR *ptStr = NULL;
#ifndef UNICODE
							ptStr = U2A(lpStr);
#else
							ptStr = lpStr;
#endif
							if (::PathFileExists(ptStr))
							{
								doNew();
								cfgDlg.SendMsg(MM_PASTEPATH, 0, reinterpret_cast<LPARAM>(ptStr));
							}
							GlobalUnlock(hglb); 
						} 
					} 
				}
				CloseClipboard();
			}
		}
		break;
	}
}

void QuickRunApp::OnUpdate(KEYDATA *lpKeyData, int index)
{
	TCHAR key[30] = {0};

	if (lpKeyData->fAlt)
		StringCchCat(key, 30, _T("Alt+"));
	if (lpKeyData->fCtrl)
		StringCchCat(key, 30, _T("Ctrl+"));
	if (lpKeyData->fShift)
		StringCchCat(key, 30, _T("Shift+"));

	StringCchCat(key, 30, lpKeyData->key);

	listView.SetItemText(index, lvColHdrInfo.colIndex[0], lpKeyData->szName);
	listView.SetItemText(index, lvColHdrInfo.colIndex[1], key);
	listView.SetItemText(index, lvColHdrInfo.colIndex[2], lpKeyData->exePath);

	int i = 0;
	for (std::list<KEYDATA>::iterator it = keyList.begin();
		it != keyList.end(); it++, i++)
	{
		if (i == index)
		{
			*it = *lpKeyData;
			break;
		}
	}
	WriteToFile();
}

void QuickRunApp::doNew()
{
	if (cfgDlg.IsCreated())
	{
		cfgDlg.SetFocus();
	}
	else
	{
		cfgDlg.create(IDD_CFG);
	}

	cfgDlg.SendMsg(MM_NEW, 0, (LPARAM)&keyList);
}

void QuickRunApp::doUpdate()
{
	int curSel = listView.GetSelectionMark();
	if (curSel == -1)	return ;

	int i = 0;
	KEYDATA keyData;
	for (std::list<KEYDATA>::iterator it = keyList.begin();
		it != keyList.end(); it++, i++)
	{
		if (i == curSel)
		{
			keyData = *it;
			break;
		}
	}
	if (cfgDlg.IsCreated())
	{
		cfgDlg.SetFocus();
	}
	else
	{
		cfgDlg.create(IDD_CFG);
	}
	cfgDlg.SendMsg(MM_UPDATE, curSel, (LPARAM)&keyData);
}

void QuickRunApp::doDel()
{
	int curSel = listView.GetSelectionMark();
	if (curSel == -1)	return ;

	TCHAR szKey[20];
	TCHAR szExePath[MAX_PATH];
	TCHAR szMsg[MAX_PATH + 120];

	listView.GetItemText(curSel, lvColHdrInfo.colIndex[1], szKey, 20);
	listView.GetItemText(curSel, lvColHdrInfo.colIndex[2], szExePath, MAX_PATH);

	StringCchPrintf(szMsg, MAX_PATH + 120, _T("确定要删除热键为：%s\r\n路径为：%s\r\n的项目吗？"),\
		szKey, szExePath);

	if (MsgBox(szMsg, _T("请确认"), MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		listView.DeleteItem(curSel);
		int i = 0;
		for (std::list<KEYDATA>::iterator it = keyList.begin();
			it != keyList.end(); it++, i++)
		{
			if (i == curSel)
			{
				keyList.erase(it);
				break;
			}
		}
		WriteToFile();
	}
}

void QuickRunApp::doStart()
{
	_fStarted = !_fStarted;
	CheckMenuItem(GetMenu(), IDM_START, MF_BYCOMMAND | (_fStarted ? MF_CHECKED : MF_UNCHECKED));
	if (_fStarted)
	{
		TCHAR exePath[MAX_PATH];
		GetModuleFileName(_hinst, exePath, MAX_PATH);
		PathRemoveFileSpec(exePath);

		StringCchCat(exePath, MAX_PATH, _T("\\TrayApp.exe"));

		if (!PathFileExists(exePath))
			MsgBox(_T("程序丢失！将不能正常工作！"), _T("文件不完整"), MB_ICONERROR);
		else
			::ShellExecute(NULL, _T("open"), exePath, NULL, NULL, SW_HIDE);
	}
	else
	{
		HWND hWndTray = ::FindWindow(_T("QuickRunTray"), NULL);
		if (hWndTray)
			::SendMessage(hWndTray, WM_CLOSE, 0, 0);
	}
}

TString QuickRunApp::IniFilePath()
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

void QuickRunApp::OnDropFiles(HDROP hDrop)
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
		doNew();
		cfgDlg.SendMsg(WM_DROPFILES, (WPARAM)hDrop, 0);
	}
	delete [] filePath;

	::DragFinish(hDrop);
}

bool QuickRunApp::SetLvImagelist()
{
	int count = keyList.size();
	if ( count < 1)
		return false;

	HIMAGELIST himl = NULL;

	himl = ImageList_Create(16, 16, ILC_COLOR32, count, 0);
	if (himl == NULL)
		return false;

	SHFILEINFO sfi = {0};

	for (IT it = keyList.begin(); it != keyList.end(); ++it)
	{
		if(!::PathFileExists(it->exePath))
			sfi.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		else
			SHGetFileInfo(it->exePath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON);

		if ((ImageList_AddIcon(himl, sfi.hIcon) == -1))
			return false;
	}

	if (ImageList_GetImageCount(himl) < count)
		return false;

// 	if (!listView.SetImageList(himl, LVSIL_SMALL))
// 		return false;

	listView.SetImageList(himl, LVSIL_SMALL);

	for(int index=0; index<count; index++)
	{
		LVITEM lvi = {0};

		lvi.iItem = index;
		lvi.mask = LVIF_IMAGE;

// 		if (listView.GetItem(&lvi))
// 		{
			lvi.iImage = index;
			if (!listView.SetItem(&lvi))
				return false;
// 		}
	}

	return true;
}

DWORD WINAPI QuickRunApp::ReadListProc(LPVOID lParam)
{
	QuickRunApp *pApp = (QuickRunApp*)lParam;

	pApp->ReadToList();
	pApp->lvColHdrInfo.sortType = !pApp->lvColHdrInfo.sortType;
	pApp->SortColumn(pApp->lvColHdrInfo.sortIndex);

	return 1;
}

BOOL QuickRunApp::SaveLvColumnInfo()
{
	TString iniFile = IniFilePath();
	if (iniFile.empty())
		return FALSE;

	return WritePrivateProfileStruct(TEXT("QuickRunApp"), TEXT("ColumnSettings"),\
		(LPVOID)&lvColHdrInfo, sizeof(LVCOLUMNHDRINFO), iniFile.c_str());
}

BOOL QuickRunApp::GetLvColumnInfo()
{
	TString iniFile = IniFilePath();
	if (iniFile.empty() || !::PathFileExists(iniFile.c_str()))
	{
		lvColHdrInfo.colIndex[0] = 0;
		lvColHdrInfo.colIndex[1] = 1;
		lvColHdrInfo.colIndex[2] = 2;
		lvColHdrInfo.sortIndex = 0;
		lvColHdrInfo.sortType = 0;

		return TRUE;
	}

	return GetPrivateProfileStruct(TEXT("QuickRunApp"), TEXT("ColumnSettings"),\
		&lvColHdrInfo, sizeof(LVCOLUMNHDRINFO), iniFile.c_str());
}

void QuickRunApp::SortColumn(int index)
{
	if (index > 2)
		return ;

	HWND hLvHdr = listView.GetHeader();
	if (!hLvHdr)	return ;

	HDITEM hdi = {0};
	hdi.mask = HDI_FORMAT;

	for (int i=0; i<3; i++)
	{
		if (i == index)
		{
			if (lvColHdrInfo.sortIndex != index)
			{
				hdi.fmt = HDF_SORTUP | HDF_STRING;
				sortUp = true;
				lvColHdrInfo.sortType = 0;
				lvColHdrInfo.sortIndex = index;
			}
			else
			{
				if (lvColHdrInfo.sortType == 0)
				{
					hdi.fmt = HDF_SORTDOWN | HDF_STRING;
					sortUp = false;
					lvColHdrInfo.sortType = 1;
				}
				else
				{
					hdi.fmt = HDF_SORTUP | HDF_STRING;
					sortUp = true;
					lvColHdrInfo.sortType = 0;
				}
			}

		}
		else
			hdi.fmt	 = HDF_STRING;

		Header_SetItem(hLvHdr, i, &hdi);
	}

	switch(index)
	{
	case 0:	keyList.sort(cmpName);	break;
	case 1:	keyList.sort(cmpKey);	break;
	case 2:	keyList.sort(cmpPath);	break;
	}

	listView.DeleteAllItems();

	for(std::list<KEYDATA>::iterator it = keyList.begin();
		it != keyList.end(); ++it)
		AddKeyToListView(*it);

	SetLvImagelist();
	WriteToFile();

	SaveLvColumnInfo();
}