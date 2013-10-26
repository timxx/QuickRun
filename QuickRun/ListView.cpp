
#include "ListView.h"

void ListView::init(HINSTANCE hInst, HWND hwndParent)
{
	Window::init(hInst, hwndParent);

	InitCommonControls();
	//_hWnd = Create(WC_LISTVIEW, _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 0, 200, 100);

	//if (!_hWnd)	throw _T("Create ListView failed");
}