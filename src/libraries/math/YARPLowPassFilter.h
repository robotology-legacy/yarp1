/////////////////////////////////////////////////////////////////////
//
// LowPassFilter.h
//	Interface of the CLowPassFilter class
//
// Created 26/1/1998  by <Eman>
// Revised 1999 by Pasa & Panerai
//

#ifndef __CLowPassFilterh__
#define __CLowPassFilterh__

#include <string.h>

class CLowPassFilter
{
private:
	CLowPassFilter(const CLowPassFilter&);
	void operator=(const CLowPassFilter&);

    double a[4];   
	double b[3];
    double c[4];
	double d[3];
  
	double in[4];
	double out[4];

	double out3;

public:
	CLowPassFilter(void);
	virtual ~CLowPassFilter(void);

	double filter(double value);
};

#endif