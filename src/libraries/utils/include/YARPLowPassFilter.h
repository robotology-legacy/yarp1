/////////////////////////////////////////////////////////////////////
//
// LowPassFilter.h
//

#ifndef __YARPLOWPASSFILTER__
#define __YARPLOWPASSFILTER__

class YARPLowPassFilter
{
private:
	YARPLowPassFilter(const YARPLowPassFilter&);
	void operator=(const YARPLowPassFilter&);

    double a[4];   
	double b[3];
    double c[4];
	double d[3];
  
	double in[4];
	double out[4];

	double out3;

public:
	YARPLowPassFilter(void);
	virtual ~YARPLowPassFilter(void);

	double filter(double value);
};

#endif