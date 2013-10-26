
#include "StringEx.h"

#pragma comment(lib, "strsafe.lib")

StringEx::StringEx(size_t cchDest, const TCHAR *fmt, ...)
{
	TCHAR *value = new TCHAR[cchDest];

	va_list args;
	va_start(args, fmt);

	StringCchVPrintf(value, cchDest, fmt, args);

	va_end(args);

	assign(value);
}

void StringEx::format(const TCHAR *fmt, ...)
{
	TCHAR value[1024];

	va_list args;
	va_start(args, fmt);

	StringCchVPrintf(value, 1024, fmt, args);

	va_end(args);

	assign(value);
}

void StringEx::format(size_t cchDest, const TCHAR *fmt, ...)
{
	TCHAR *value = new TCHAR[cchDest];

	va_list args;
	va_start(args, fmt);

	StringCchVPrintf(value, cchDest, fmt, args);

	va_end(args);

	assign(value);
}