//========================================================================================================
#include "LinkCtrl.h"
//========================================================================================================

static COLORREF getParentDlgBkColor(HWND hWnd)
{
	COLORREF crRet = CLR_INVALID;

	if (!hWnd || !IsWindow(hWnd))
		return crRet;

	HWND hWndParent = GetParent(hWnd);
	if (!hWndParent)	return crRet;

	RECT rc;
	if (!GetClientRect(hWndParent, &rc))
		return crRet;

	HDC hDC = GetDC(hWndParent);
	if (!hDC)	return crRet;

	HDC hdcMem = CreateCompatibleDC(hDC);
	if (!hdcMem)
		return crRet;

	HBITMAP hBmp = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
	if (!hBmp)	return crRet;

	HGDIOBJ hOld = SelectObject(hdcMem, hBmp);
	if (!hOld)	return crRet;
	if (SendMessage(hWndParent,	WM_ERASEBKGND, (WPARAM)hdcMem, 0))
		crRet = GetPixel(hdcMem, 0, 0);

	SelectObject(hdcMem, hOld);
	DeleteObject(hBmp);
	DeleteDC(hdcMem);
	ReleaseDC(hWndParent, hDC);

	return crRet;
}

void LinkCtrl::create(HWND itemHandle, TCHAR * link, COLORREF linkColor)
{
	// turn on notify style
	::SetWindowLongPtr(itemHandle, GWL_STYLE, ::GetWindowLongPtr(itemHandle, GWL_STYLE) | SS_NOTIFY);

	// set the URL text (not the display text)
	if (link)	_URL = link;

	// set the hyperlink colour
	_linkColor = linkColor;

	// set the visited colour
	_visitedColor = RGB(128,0,128);

	// subclass the static control
	_oldproc = (WNDPROC)::SetWindowLongPtr(itemHandle, GWLP_WNDPROC, (LONG_PTR)LinkCtrlProc);

	// associate the URL structure with the static control
	::SetWindowLongPtr(itemHandle, GWLP_USERDATA, (LONG_PTR)this);

}
void LinkCtrl::create(HWND itemHandle, int cmd, HWND msgDest)
{
	// turn on notify style
	::SetWindowLongPtr(itemHandle, GWL_STYLE, ::GetWindowLongPtr(itemHandle, GWL_STYLE) | SS_NOTIFY);

	_cmdID = cmd;
	_msgDest = msgDest;

	// set the hyperlink colour
	_linkColor = RGB(0,0,255);

	// subclass the static control
	_oldproc = (WNDPROC)::SetWindowLongPtr(itemHandle, GWLP_WNDPROC, (LONG_PTR)LinkCtrlProc);

	// associate the URL structure with the static control
	::SetWindowLongPtr(itemHandle, GWLP_USERDATA, (LONG_PTR)this);
}

void LinkCtrl::destroy()
{
	if(_hfUnderlined)
		::DeleteObject(_hfUnderlined);
	//if(_hCursor)
	//	::DestroyCursor(_hCursor);
}

LRESULT LinkCtrl::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg)
	{
		// Free up the structure we allocated
	case WM_NCDESTROY:
		//HeapFree(GetProcessHeap(), 0, url);
		break;

		// Paint the static control using our custom
		// colours, and with an underline text style
	case WM_PAINT:
		{
			DWORD dwStyle = ::GetWindowLongPtr(hWnd, GWL_STYLE);
			DWORD dwDTStyle = DT_SINGLELINE;

			//Test if centered horizontally or vertically
			if(dwStyle & SS_CENTER)	     dwDTStyle |= DT_CENTER;
			if(dwStyle & SS_RIGHT)		 dwDTStyle |= DT_RIGHT;
			if(dwStyle & SS_CENTERIMAGE) dwDTStyle |= DT_VCENTER;

			RECT		rect;
			::GetClientRect(hWnd, &rect);

			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hWnd, &ps);

			::SetTextColor(hdc, _linkColor);

			::SetBkColor(hdc, getParentDlgBkColor(hWnd)); ///*::GetSysColor(COLOR_3DFACE)*/);

			// Create an underline font 
			if(_hfUnderlined == 0)
			{
				// Get the default GUI font
				LOGFONT lf;
				HFONT hf = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

				// Add UNDERLINE attribute
				GetObject(hf, sizeof lf, &lf);
				lf.lfUnderline = TRUE;

				// Create a new font
				_hfUnderlined = ::CreateFontIndirect(&lf);
			}

			HANDLE hOld = SelectObject(hdc, _hfUnderlined);

			// Draw the text!
			TCHAR szWinText[MAX_PATH];
			::GetWindowText(hWnd, szWinText, MAX_PATH);
			::DrawText(hdc, szWinText, -1, &rect, dwDTStyle);

			::SelectObject(hdc, hOld);

			::EndPaint(hWnd, &ps);

			return 0;
		}

	case WM_SETTEXT:
		{
			LRESULT ret = ::CallWindowProc(_oldproc, hWnd, uMsg, wParam, lParam);
			::InvalidateRect(hWnd, 0, 0);
			return ret;
		}
		// Provide a hand cursor when the mouse moves over us
		//case WM_SETCURSOR:
	case WM_MOUSEMOVE:
		{
			if (_hCursor == 0)
				_hCursor = LoadCursor(NULL, IDC_HAND);

			SetCursor(_hCursor);
			return TRUE;
		}

	case WM_LBUTTONDOWN:
		_clicking = true;
		break;

	case WM_LBUTTONUP:
		if(_clicking)
		{
			_clicking = false;
			if (_cmdID)
			{
				::SendMessage(_msgDest?_msgDest:_parentWnd, WM_COMMAND, _cmdID, 0);
			}
			else
			{
				_linkColor = _visitedColor;

				::InvalidateRect(hWnd, 0, 0);
				::UpdateWindow(hWnd);

				// Open a browser
				if(_URL != _T(""))
				{
					::ShellExecute(NULL, _T("open"), _URL.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
				else
				{
					TCHAR szWinText[MAX_PATH];
					::GetWindowText(hWnd, szWinText, MAX_PATH);
					::ShellExecute(NULL, _T("open"), szWinText, NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}

		break;

		// A standard static control returns HTTRANSPARENT here, which
		// prevents us from receiving any mouse messages. So, return
		// HTCLIENT instead.
	case WM_NCHITTEST:
		return HTCLIENT;
	}
	return ::CallWindowProc(_oldproc, hWnd, uMsg, wParam, lParam);
}
