
#include <Windows.h>
#include <tchar.h>

#pragma data_seg("Shared")
HWND ghWnd = NULL;
HINSTANCE ghDLLInst = NULL;
BOOL gfShow = FALSE;
#pragma data_seg()
#pragma comment(linker,"/section:Shared,rws")

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID)
{
	ghDLLInst = hInst;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		return TRUE;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		return TRUE;
	}
	return TRUE;
}