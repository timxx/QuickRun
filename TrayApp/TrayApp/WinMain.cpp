//========================================================================================================
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//===============================================================================
#include "TrayApp.h"
//===============================================================================
int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
{
	TrayApp trayApp;
	MSG msg = {0};

	try
	{
		CreateMutex(NULL, FALSE, trayApp.getClassName().c_str());

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return 0;
		}

		trayApp.init(hInst, NULL);

		while(GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch(TCHAR *what){
		MessageBox(GetActiveWindow(), what, _T("Catch Errors"), MB_ICONERROR);
	}	
	catch(std::exception ex){
		MessageBoxA(GetActiveWindow(), ex.what(), "C++ Exception", MB_ICONERROR);
	}
	catch(...){
		MessageBox(GetActiveWindow(), _T("Something unknown errors happened"),
			_T("Error"), MB_ICONERROR);
	}

	return msg.wParam;
}
///////////////////////////////////////////////////////////////////////////////