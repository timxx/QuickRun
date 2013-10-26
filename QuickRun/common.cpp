
#include "common.h"

void ShowLastErr(const TCHAR *title, DWORD dwErrCode)
{
	if (dwErrCode == 0)
		return ;

	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErrCode,
		MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
		(LPTSTR)&lpMsgBuf, 0, NULL );

	MessageBox(GetActiveWindow(), (TCHAR*)lpMsgBuf, title, MB_ICONERROR);

	LocalFree(lpMsgBuf);
}