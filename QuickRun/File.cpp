
#include "File.h"

File::~File()
{
	if (hFile)
		::CloseHandle(hFile);
}

bool File::openFile(mode m/* = write*/)
{
	if (_filePath.empty())	return false;

	DWORD access = 0;
	DWORD dwCreation = 0;

	if ( m == write || m == append)
	{
		access =  GENERIC_WRITE;
		dwCreation = CREATE_ALWAYS;
	}
	else
	{
		access = GENERIC_READ;
		dwCreation = OPEN_EXISTING;
	}

	hFile = CreateFile(_filePath.c_str(), access, FILE_SHARE_READ, NULL,\
		dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hFile)	return false;
	if (m == append)
	{
		::SetFilePointer(hFile, 0, 0, FILE_END);
	}

	return true;
}

bool File::Write(LPVOID lpData, DWORD dwLen)
{
	if (!hFile)	return false;

	DWORD dwWritten;

	if (!::WriteFile(hFile, lpData, dwLen, &dwWritten, 0))
		return false;

	return true;
}

bool File::Read(LPVOID lpData, DWORD dwLen)
{
	if (!hFile)	return false;

	DWORD dwRead;

	if (!::ReadFile(hFile, lpData, dwLen, &dwRead, NULL))
		return false;

	return true;
}

DWORD File::GetSize() const
{
	if (!hFile)	return -1;
	return ::GetFileSize(hFile, NULL);
}