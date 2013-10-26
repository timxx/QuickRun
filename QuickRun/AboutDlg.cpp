
#include "AboutDlg.h"
#include "resource.h"

BOOL AboutDlg::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			TString buildTime(_T("Build time : "));
#ifdef UNICODE
			buildTime += A2U(__DATE__);
			buildTime += L" - ";
			buildTime += A2U(__TIME__);
#else
			buildTime += __DATE__;
			buildTime += " - ";
			buildTime += __TIME__;
#endif
			SetItemText(IDS_BUILD_TIME, buildTime.c_str());
			DisableCtrl(IDS_BUILD_TIME);

			_qqLink.init(_hinst, _hWnd);
			_qqLink.create(HwndFromId(IDS_QQ), _T("tencent://message/?uin=551907703"));
			_emailLink.init(_hinst, _hWnd);
			_emailLink.create(HwndFromId(IDS_EMAIL), _T("mailto:just_fancy@139.com"));

			SetItemText(IDC_EDIT1, _T("免费的，想咋用咋用吧，有问题或建议QQ或邮件联系"));
		}
		return TRUE;

	//case WM_PAINT:
	//	{
	//		PAINTSTRUCT ps;
	//		HDC hdc = BeginPaint(_hWnd, &ps);

	//		Rect rect;
	//		GetClientRect(&rect);

	//		FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 255, 255)));

	//		EndPaint(_hWnd, &ps);
	//	}
	//	return FALSE;

	//case WM_CTLCOLORSTATIC:

	//	SetBkMode((HDC)wParam, TRANSPARENT);
	//	return (BOOL) GetStockObject(NULL_BRUSH);

	case WM_CLOSE:
		Destroy();
		break;
	}

	return FALSE;
}