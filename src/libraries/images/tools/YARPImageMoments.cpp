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
			int tmpX = 0;
			int tmpY = 0;
			double J = 0.0;
			if ( *src == 255 )
			{
				Logpolar2Cartesian(c, t, tmpX, tmpY);

				J = Jacobian(c, t);

				sumX += tmpX*J;
				sumY += tmpY*J;
				area += J;
			}
			src++;
		}
	}
	
	if (area != 0)
	{
		*x = (int)(sumX/area);
		*y = (int)(sumY/area);
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

// CARTESIAN
void YARPImageMoments::centerOfMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y)
{
	int i,j;
	double area = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	unsigned char *src;
	for(j = 0; j < in.GetHeight() ; i++)
	{
		src = (unsigned char *)in.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			if ( *src == 255 )
			{
				sumX += i;
				sumY += j;
				area ++;
			}
			src++;
		}
	}
	
	if (area != 0)
	{
		*x = (int)(sumX/area);
		*y = (int)(sumY/area);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

double YARPImageMoments::centralMoments(YARPImageOf<YarpPixelMono> &in, int xm, int ym, int p, int q)
{
	int i,j;
	double area = 0.0;
	double res = 0.0;
	unsigned char *src;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (unsigned char *)in.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			if ( *src == 255 )
				res += pow((double)(i-xm),(double)p)*pow((double)(j-ym),(double)q);
			
			src++;
		}
	}
	
	return res;
}
