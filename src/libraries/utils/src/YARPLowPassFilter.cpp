
// LowPassFilter.cpp
//
// Digital low-pass filter
// 26/1/1998  by <Paolo & Emanuele>
// 1999 Pasa & Panerai

#include "YARPLowPassFilter.h"
#include <memory.h>

YARPLowPassFilter::YARPLowPassFilter(void)
{
	a[3]=0.0465;
	a[2]=0.1397;
	a[1]=0.1397;
	a[0]=0.0465;
	
	b[2]=0.1457;
	b[1]=-0.7245;
	b[0]=1.2045;
	
	c[3]=0.0013; // i coeff. c[] d[] ,usati per altra versione
	c[2]=0.0039; // del filtro
	c[1]=0.0039;
	c[0]=0.0013;
	
	d[2]=0.6241;
	d[1]=-2.1653;
	d[0]=2.5308;
	
	out3=0;

	memset (in, 0, sizeof(double) * 4);
	memset (out, 0, sizeof(double) * 4);
}

YARPLowPassFilter::~YARPLowPassFilter(void)
{
}

double YARPLowPassFilter::filter(double value)
{
	const int n=3;
	
	in[n-3]=in[n-2];
	in[n-2]=in[n-1];
	in[n-1]=in[n];
	in[n]=value;
	
	for(int h = 0; h < 3; h++)
	{
		out[h]=out[h+1];
	}

	out[n]=a[0]*in[n]+a[1]*in[n-1]+a[2]*in[n-2]+a[3]*in[n-3]+b[0]*out[n-1]+b[1]*out[n-2]+b[2]*out[n-3];

    return out[n];
}


