#ifndef CONFIG_H
#define CONFIG_H


#include <math.h>

const double BIGVALUE = 99999999.0;



int getMin(const int x, const int y)
{
	if (x < y)
	{
		return x;
	}
	
	return y;
}

#endif