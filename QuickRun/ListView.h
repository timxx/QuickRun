//========================================================================================================

#ifndef LISTVIEW_H
#define LISTVIEW_H

#pragma once

#include "Window.h"
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

class ListView : public Window
{
public:
	ListView(){}

	void init(HINSTANCE hInst, HWND hwndParent);

	virtual HWND Create(DWORD dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS,\
		int x = 0, int y = 0, int nWidth = 0, int nHeight = 0,
		UINT uMenuID = 0, LPVOID lpParam = 0)
	{
		return Window::Create(WC_LISTVIEW, _T(""), dwStyle, x, y, nWidth, nHeight, uMenuID, lpParam);
	}

	BOOL DeleteAllItems()		{	return ListView_DeleteAllItems(_hWnd);		}
	BOOL DeleteItem(int iItem)	{	return ListView_DeleteItem(_hWnd, iItem);	}

	int  GetHotItem() const			{	return ListView_GetHotItem(_hWnd);		}
	BOOL GetItem(LPLVITEM pitem)	{	return ListView_GetItem(_hWnd, pitem);	}
	int  GetItemCount() const		{	return ListView_GetItemCount(_hWnd);	}

	void GetItemText(int iItem, int iSubItem, LPTSTR pszText, int cchTextMax)
		{	ListView_GetItemText(_hWnd, iItem, iSubItem, pszText, cchTextMax);	}

	int GetSelectedColumn() const	{	return ListView_GetSelectedColumn(_hWnd);	}
	int GetSelectionMark() const	{	return ListView_GetSelectionMark(_hWnd);	}
	int GetSelectedCount() const	{ return ListView_GetSelectedCount(_hWnd);	}

	int InsertItem(const LPLVITEM pitem)	{	return ListView_InsertItem(_hWnd, pitem);	}
	BOOL SetItem(const LPLVITEM pitem)		{	return ListView_SetItem(_hWnd, pitem);		}

	void SetItemText(int iItem, int iSubItem, LPTSTR pszText)
	{	ListView_SetItemText(_hWnd, iItem, iSubItem, pszText);	}

	BOOL SetBkColor(COLORREF clrBk)	{	return ListView_SetBkColor(_hWnd, clrBk);	}

	BOOL SetTextColor(COLORREF clrText)		{	return ListView_SetTextColor(_hWnd, clrText);	}
	BOOL SetTextBkColor(COLORREF clrTextBk)	{	return ListView_SetTextBkColor(_hWnd, clrTextBk);	}

	int InsertColumn(int iCol, const LPLVCOLUMN pcol)	{	return ListView_InsertColumn(_hWnd, iCol, pcol);	}
	void SetExtendedStyle(DWORD dwExStyle)	{	ListView_SetExtendedListViewStyle(_hWnd, dwExStyle);	}

	BOOL SetColumnWidth(int iCol, int iWidth = LVSCW_AUTOSIZE_USEHEADER)
	{	return ListView_SetColumnWidth(_hWnd, iCol, iWidth);	}

	HIMAGELIST SetImageList(HIMAGELIST  himl, int iImageList) const{
		return ListView_SetImageList(_hWnd, himl, iImageList);
	}

	HWND GetHeader() const {	return ListView_GetHeader(_hWnd);	}
};

#endif