
//========================================================================================================
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//========================================================================================================

// #include <Windows.h>
// #include <tchar.h>

#include "QuickRunApp.h"
#include "resource.h"

int __stdcall _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int nCmdShow)
{
	QuickRunApp quickRunApp;
	MSG msg = {0};
	
	try
	{
		CreateMutex(NULL, FALSE, quickRunApp.getClassName().c_str());

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND hWnd = FindWindow(quickRunApp.getClassName().c_str(), NULL);
			if (hWnd)
			{
				if (IsIconic(hWnd))
					SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				else
					SetForegroundWindow(hWnd);
			}
			return 0;
		}

		quickRunApp.init(hInst, NULL);

		HACCEL hAcc = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));

		while(GetMessage(&msg, NULL, 0, 0))
		{
			if (quickRunApp.IsDlgMsg(&msg))
				continue;

			if (!TranslateAccelerator(quickRunApp.getSelf(), hAcc, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}		
		}
	}
	catch(const TCHAR *what){
		MessageBox(GetActiveWindow(), what, _T("Catch Errors"), MB_ICONERROR);

	}catch(std::exception ex){
		MessageBoxA(GetActiveWindow(), ex.what(), "C++ Exception", MB_ICONERROR);

	}catch(...){
		MessageBox(GetActiveWindow(), _T("Something unknown errors happened"), _T("Error"), MB_ICONERROR);
	}

	return msg.wParam;
}