#include "YARPImageMoments.h"
#include "YARPLogpolar.h"

using namespace _logpolarParams;

void YARPLpImageMoments::centerOfMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y)
{
	int t,c;
	double area = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	unsigned char *src;
	for(c = 0; c < _srho; c++)
	{
		src = (unsigned char *)in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			int x = 0;
			int y = 0;
			double J = 0.0;
			if ( *src == 255 )
			{
				Logpolar2Cartesian(c, t, x, y);
				J = Jacobian(c, t);

				sumX += x*J;
				sumY += y*J;
				area += J;
			}
			src++;
		}
	}
	
	if (area != 0)
	{
		*x = (sumX/area);
		*y = (sumY/area);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

double YARPLpImageMoments::centralMoments(YARPImageOf<YarpPixelMono> &in, int xm, int ym, int p, int q)
{
	int t,c;
	double area = 0.0;
	double res = 0.0;
	unsigned char *src;
	for(c = 0; c < _srho; c++)
	{
		src = (unsigned char *)in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			int x = 0;
			int y = 0;
			double J = 0.0;
			if ( *src == 255 )
			{
				Logpolar2Cartesian(c, t, x, y);
				J = Jacobian(c, t);

				res += pow((double)(x-xm),(double)p)*pow((double)(y-ym),(double)q)*J;
				area += J;
			}
			src++;
		}
	}
	
	return res;
}

