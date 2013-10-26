//========================================================================================================

#ifndef CLASSEX_H
#define CLASSEX_H

#pragma once

#include <Windows.h>

class ClassEx : public WNDCLASSEX
{
public:
	ClassEx(HINSTANCE hInst, LPCTSTR clsName, UINT iconID, UINT menuID, WNDPROC proc)
	{
		::RtlSecureZeroMemory(&(*this), sizeof(WNDCLASSEX));

		hInstance = hInst;
		lpszClassName = clsName;
		hbrBackground = HBRUSH(COLOR_WINDOW+1);
		hCursor = LoadCursor(NULL, IDC_ARROW);
		cbSize = sizeof(WNDCLASSEX);
		hIcon = iconID==NULL?NULL:LoadIcon(hInst, MAKEINTRESOURCE(iconID));
		style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		lpszMenuName = menuID==NULL?NULL:MAKEINTRESOURCE(menuID);
		lpfnWndProc = proc;
	}
	~ClassEx(){}

	bool Register()	{	return RegisterClassEx(&*this)?true:false;	}
};

#endif