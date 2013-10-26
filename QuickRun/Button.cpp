
#include "Button.h"
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")


void Button::init()
{
	fHovered = FALSE;

	lpfnOldProc = Subclass(ButtonProc);
}

void Button::DrawItem( LPDRAWITEMSTRUCT lpdis )
{
	RECT rect	=  lpdis->rcItem;
	HDC hDC		= lpdis->hDC;

	int nSaveDC = SaveDC(hDC);
	UINT state = lpdis->itemState;

	HPEN m_BoundryPen = CreatePen(PS_INSIDEFRAME | PS_SOLID, 1, RGB(10, 200, 100));
	TString text = getTitle();

	HPEN hOldPen = (HPEN)SelectObject(hDC, m_BoundryPen);

	RoundRect(hDC, rect.left, rect.top, rect.right, rect.bottom, 5, 5);

	SelectObject(hDC, hOldPen);
	
	if (fHovered)
		SetTextColor(hDC, RGB(255, 0, 0));
	else if (state & ODS_FOCUS)
	{
		SetTextColor(hDC, RGB(255, 144, 54));
	}
	else if(state & ODS_DISABLED)
	{
		SetTextColor(hDC, RGB(0, 0, 0));
	}
	else
	{
		SetTextColor(hDC, RGB(0, 0, 255));
	}

	if (!text.empty())
	{
		int nMode = SetBkMode(hDC, TRANSPARENT);

		DrawText(hDC, text.c_str(), text.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		SetBkMode(hDC, nMode);
	}

	RestoreDC(hDC, nSaveDC);
}

bool Button::setOwerDrawStyle()
{
	DWORD dwStyle = getStyle();
	dwStyle |= BS_OWNERDRAW;

	return ::SetWindowLongPtr(_hWnd, GWL_STYLE, dwStyle)==0 ? false:true;
}

LRESULT Button::doMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!fHovered)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = _hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		fHovered = _TrackMouseEvent(&tme);
	}
	return 0;
}

LRESULT Button::doMouseLeave(WPARAM wParam, LPARAM lParam)
{
	fHovered = FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT Button::doMouseHover(WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(NULL);
	return 0;
}

LRESULT CALLBACK Button::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DRAWITEM:	DrawItem((LPDRAWITEMSTRUCT)lParam);	return TRUE;

	case WM_MOUSEMOVE:	return doMouseMove(wParam, lParam);

	case WM_MOUSELEAVE:	return doMouseLeave(wParam, lParam);

	case WM_MOUSEHOVER:	return doMouseHover(wParam, lParam);
	}

	return ::CallWindowProc(lpfnOldProc, hWnd, uMsg, wParam, lParam);
}