
#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#pragma once

#include "dialog.h"
#include "LinkCtrl.h"

class AboutDlg : public Dialog
{
public:
	AboutDlg(){}
	~AboutDlg(){}

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	LinkCtrl _qqLink;
	LinkCtrl _emailLink;
};

#endif
