
#ifndef _DATA_H_
#define _DATA_H_

enum RunType{	NORMAL = 0, MAX, MIN, HIDE	};

struct KEYDATA
{
	bool fCtrl;
	bool fShift;
	bool fAlt;
	TCHAR key[4];
	TCHAR exePath[MAX_PATH];
	TCHAR szName[MAX_PATH];
	TCHAR szParam[MAX_PATH];
	RunType showType;

	KEYDATA()
	{
		fCtrl = fShift = fAlt = false;
		key[0] = key[1] = key[2] = key[3] = 0;
		SecureZeroMemory(exePath, MAX_PATH);
		SecureZeroMemory(szName, MAX_PATH);
		SecureZeroMemory(szParam, MAX_PATH);
		showType = NORMAL;
	}
};

struct LVCOLUMNHDRINFO
{
	unsigned short colIndex[3];
	unsigned short sortIndex;
	unsigned short sortType;	//0 - up; 1 - down
};

#endif