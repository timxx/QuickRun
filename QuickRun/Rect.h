//=================================================================

#ifndef _RECT_H_
#define _RECT_H_

#pragma once

#include <Windows.h>
//=================================================================
class Rect : public RECT
{
public:
	Rect(){}
	Rect(const Rect &rect){	CopyRect(rect);	}
	Rect(int iLeft, int iTop, int iRight, int iBottom)	{	SetRect(iLeft, iTop, iRight, iBottom);	}
	~Rect(){}

	BOOL CopyRect(const RECT &rect)	{	return ::CopyRect(this, &rect);	}
	void Clear()	{	left = top = right = bottom = 0;	}
	void SetRect(int l, int t, int r, int b){
		left = l; top = t; right = r; bottom = b;
	}

	int Width()	const {	return right - left;	}
	int Height()	const	{	return bottom - top;	}

	bool IsRectEmpty() const {	return (Width() == 0 || Height() == 0);	}
	BOOL PtInRect(const POINT &pt) const {	return ::PtInRect(this, pt);	}
};

#endif