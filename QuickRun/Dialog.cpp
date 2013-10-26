

#include "Dialog.h"

void Dialog::create(UINT uID, LPVOID lParam/* = 0*/)
{
	_pData = lParam;

	_hWnd = ::CreateDialogParam(_hinst, MAKEINTRESOURCE(uID), _parentWnd, (DLGPROC)dlgProc, (LPARAM)this);

	if(!_hWnd)	return ;

	_fModeless = true;
	_isCreated = true;

	showWindow();
}

void Dialog::doModal(UINT uID, LPVOID lParam/* = 0*/)
{
	_pData = lParam;
	_isCreated = true;
	_nResult = ::DialogBoxParam(_hinst,  MAKEINTRESOURCE(uID), _parentWnd, (DLGPROC)dlgProc, (LPARAM)this);
}

BOOL CALLBACK Dialog::dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		case WM_INITDIALOG :
		{
			Dialog *pDlg = (Dialog *)(lParam);
			pDlg->_hWnd = hDlg;
			::SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lParam);

            pDlg->runProc(uMsg, wParam, lParam);
			
			return TRUE;
		}

		default :
		{
			Dialog *pDlg = reinterpret_cast<Dialog *>(::GetWindowLongPtr(hDlg, GWL_USERDATA));

			if (!pDlg)	return FALSE;

			return pDlg->runProc(uMsg, wParam, lParam);
		}
	}
}

TString Dialog::GetItemText(int id)
{
	TCHAR *text = 0;

	HWND hwndItem = HwndFromId(id);

	if (!hwndItem)	return TEXT("");

	int len = ::GetWindowTextLength(hwndItem);

	text  = new TCHAR[len+1];

	::RtlSecureZeroMemory(text, len+1);

	::GetWindowText(hwndItem, text, len+1);

	return text;
}