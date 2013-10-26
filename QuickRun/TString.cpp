
#include <Windows.h>
#include "TString.h"

wchar_t *A2U(const char *a)
{
	int len = strlen(a);
	wchar_t *u = new wchar_t[len+1];

	//获取所需大小
	int len2 = MultiByteToWideChar( CP_ACP, 0, a, -1, u, 0 );

	if(len2 > 0)
	{
		if (len2 > len)
		{
			delete u;
			u = new wchar_t[len2];
		}

		MultiByteToWideChar( CP_ACP, 0, a, -1, u, len2 );
	}
	else	u[0] = '\0';

	return u;
}

char *U2A(const wchar_t *u)
{
	int len = wcslen(u);
	char *a = new char[len+1];

	int len2 = WideCharToMultiByte( CP_ACP, 0, u, -1, a, 0, NULL, NULL );

	if (len2 > 0)
	{
		if (len2 > len)
		{
			delete [] a;
			a = new char[len2];
		}
		WideCharToMultiByte(CP_ACP, 0, u, -1, a, len2, NULL, NULL);
	}
	else	a[0] = '\0';

	return a;
}