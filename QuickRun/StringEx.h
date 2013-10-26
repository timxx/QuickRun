
#ifndef _STRING_EX_H_
#define _STRING_EX_H_

#include <string>
#include <tchar.h>
#include <strsafe.h>

class StringEx : public std::basic_string<TCHAR>
{
public:
	StringEx() : basic_string(){}
	StringEx(const TCHAR *str) : basic_string(str) {}
	StringEx(const StringEx & str) : basic_string(str/*.c_str()*/){}
 	StringEx(size_t cchDest, const TCHAR *fmt, ...);

	void format(const TCHAR *fmt, ...);	//do not exceed 1024 bytes!
	void format(size_t cchDest, const TCHAR *fmt, ...);

	void toUpper()	{	_tcsupr_s(this->_Bx._Buf);	}
	void toLower()	{	_tcslwr_s(this->_Bx._Buf);	}
};

#endif