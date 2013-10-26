
#ifndef BUTTON_H_
#define BUTTON_H_

#include "Window.h"

class Button : public Window
{
public:
	Button(HWND hWnd){	Assign(hWnd);	}
	Button(){	_hWnd = NULL;	}
	Button(int id, HWND hWndParent){
		Assign(id, hWndParent);
	}
	
	bool setOwerDrawStyle();

	void Assign(HWND hWnd)	{	
		_hWnd = hWnd;
		init();
	}
	void Assign(int id, HWND hWndParent){	
		_parentWnd = hWndParent;
		_hWnd = ::GetDlgItem(hWndParent, id);
		init();
	}
	
private:
	void init();

	void DrawItem( LPDRAWITEMSTRUCT lpdis );

	LRESULT doMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT doMouseLeave(WPARAM wParam, LPARAM lParam);
	LRESULT doMouseHover(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return ((Button* )::GetWindowLongPtr(hWnd, GWLP_USERDATA))->runProc( hWnd, uMsg, wParam, lParam);
	}
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
protected:

	BOOL fHovered;

private:
	WNDPROC lpfnOldProc;
};

#endif