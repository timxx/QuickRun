
#ifndef MY_MESSAGE_H
#define MY_MESSAGE_H

//add a new key
//SendMessage(hWnd, MM_ADDKEY, 0, (LPARAM)&keydata);
#define	MM_ADDKEY	WM_USER + 5

//tray message
#define MM_TRAY		WM_USER + 6

//update record
#define MM_UPDATE	WM_USER + 7

//new record
#define MM_NEW		WM_USER + 8

//show pressed key info
#define MM_SHOWINFO	WM_USER + 9

//tray app quit
#define MM_TRAYQUIT	WM_USER + 10

#define MM_PASTEPATH WM_USER + 11

#endif