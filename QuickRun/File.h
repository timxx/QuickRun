
#ifndef FILE_H
#define FILE_H

#include <Windows.h>
#include "TString.h"

class File
{
public:
	enum mode {	read, write, append	};
	File(const TString &filePath, mode m)
		:_filePath(filePath), _mode(m), hFile(INVALID_HANDLE_VALUE)
	{
		openFile(m);
	}

	~File();
	bool Write(LPVOID lpData, DWORD dwLen);
	bool Read(LPVOID lpData, DWORD dwLen);

	DWORD GetSize() const;
private:
	bool openFile(mode m = write);

private:
	TString _filePath;

	mode _mode;
	HANDLE hFile;
};

#endif