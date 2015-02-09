#include "stdafx.h"
#include "helper.h"

int Helper::U_Randint(int a, int b)
{
	double x;
	x = (double)rand()/RAND_MAX;

	return (int)(a + (b - a) * x);
}

double Helper::P_Rand(double Lamda)
{
	double x = 0, b = 1, c = exp(-Lamda), u;
	do
	{
		u = (double)rand()/RAND_MAX;
		b *= u;
		if (b >= c)
			x++;
	} while (b >= c);

	return x;
}

double Helper::E_Rand(double Lamda)
{
    double e, r;
	r = (double)rand()/RAND_MAX;
    e = -log(r) / Lamda;
    if(e == 0)
    e = 1 / Lamda;

    return e;
}
