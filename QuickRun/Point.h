
#ifndef _POINT_H_
#define _POINT_H_

#include <Windows.h>

class Point : public POINT
{
public:
	Point()	{	x = y = 0;	}
	Point(int iX, int iY) { x = iX; y = iY;	}

};

#endif