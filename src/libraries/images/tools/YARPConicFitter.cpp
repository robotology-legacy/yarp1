#include "YARPConicFitter.h"

using namespace _logpolarParams;

void YARPLpConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *Rmin, int *Rmax, int *Rav)
{
	int x,y;
	_moments.centerOfMass(in, &x, &y);
	_moments.Cartesian2Logpolar(x, y, *r0, *t0);
	
	_radius(in, *t0, *r0, Rmin, Rmax, Rav);
}

void YARPLpConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *R)
{
	int x,y;
	_moments.centerOfMass(in, &x, &y);
	_moments.Cartesian2Logpolar(x, y, *r0, *t0);
	
	*R = _radiusOfGyration(in, x, y);
}

void YARPLpConicFitter::fitEllipse(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, double *a11, double *a12, double *a22)
{
	int x,y;
	_moments.centerOfMass(in, &x, &y);
	_moments.Cartesian2Logpolar(x, y, *r0, *t0);
	
	double u20 = _moments.centralMoments(in, x, y, 2, 0);
	double u11 = _moments.centralMoments(in, x, y, 1, 1);
	double u02 = _moments.centralMoments(in, x, y, 0, 2);
	double u00 = _moments.centralMoments(in, x, y, 0, 0);

	double aSq;
	double bSq;
	double theta;

	double tmp = u20-u02;
	if (tmp != 0)
		theta = 0.5*atan(2*u11/tmp);
	else 
		theta = 3.14/4;

	aSq = (2/u00)*(u20+u02 + sqrt((u20-u02)*(u20-u02) + 4*u11*u11));
	bSq = (2/u00)*(u20+u02 - sqrt((u20-u02)*(u20-u02) + 4*u11*u11));
	
	double costh = cos(theta);
	double sinth = sin(theta);
	*a11 = (costh*costh)/(aSq) + (sinth*sinth)/(bSq);
	*a22 = (sinth*sinth)/(aSq) + (costh*costh)/(bSq);
	*a12 = (1/(aSq) - 1/(bSq)) * sinth*costh;
}

void YARPLpConicFitter::_radius(YARPImageOf<YarpPixelMono> &in, int theta,  int rho, int *Rmin, int *Rmax, int *Rav)
{
	int t,c;
	double avR = 0.0;
	double maxR = 0.0;
	double minR = 255*255;
	int n = 0;
	int x0;
	int y0;
	unsigned char *src;
	_moments.Logpolar2Cartesian(rho, theta, x0, y0);
	for(c = 0; c < _srho; c++)
	{
		src = (unsigned char *) in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			if ( in(t,c) == 255 )
			{
				int x;
				int y;
				_moments.Logpolar2Cartesian(c, t, x, y);
				double R;
				R = (x-x0)*(x-x0)+(y-y0)*(y-y0);
				avR += R;
				if (R>maxR)
					maxR = R;
			}
			else
			{
				int x;
				int y;
				_moments.Logpolar2Cartesian(c, t, x, y);
				double R;
				R = (x-x0)*(x-x0)+(y-y0)*(y-y0);
				avR += R;
				if (R<minR)
					minR = R;
			}
			src++;
		}
	}
	
	*Rmax = (int) sqrt(maxR);
	*Rmin = (int) sqrt(minR);
	*Rav = (int) (*Rmax+*Rmin)/2;
}

double YARPLpConicFitter::_radiusOfGyration(YARPImageOf<YarpPixelMono> &in, int x, int y)
{
	double u20 = _moments.centralMoments(in, x, y, 2, 0);
	double u02 = _moments.centralMoments(in, x, y, 0, 2);
	double u00 = _moments.centralMoments(in, x, y, 0, 0);

	return sqrt((u20+u02)/u00);
}

void YARPLpConicFitter::plotEllipse(int T0, int R0, double a11, double a12, double a22, YARPImageOf<YarpPixelMono> &output)
{
	int theta;
	int rho;
	int r0;

	if ( !_checkDet(a11, a12, a22) )
	{
		// sorry, not an ellipse...
		return;
	}
	
	r0 = _moments.CsiToRo(R0);
	double c0 = cos((T0)/_q);
	double s0 = sin((T0)/_q);

	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta)/_q);
		double s = sin((theta)/_q);

		double A;
		double B;
		double C;
	
		A = a11*c*c + 2*a12*c*s + a22*s*s;
		B = r0*(a11*c*c0 + a12*c*s0 + a12*c0*s + a22*s*s0);
		C = r0*r0*(2*a12*c0*s0 + a22*s0*s0 + a11*c0*c0)-1;

		double DELTA = B*B-A*C;
		if (DELTA >= 0)
		{
			int r = (B+sqrt(DELTA))/A + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
					output(theta,rho) = 255;
			}
			
			r = (B-sqrt(DELTA))/A + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
						output(theta,rho) = 255;
			}
		}

	}
	// plot center
	output.SafePixel(T0, R0) = 255;
}

void YARPLpConicFitter::plotEllipse(int T0, int R0, double a11, double a12, double a22, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	int theta;
	int rho;
	int r0;

	if ( !_checkDet(a11, a12, a22) )
	{
		// sorry, not an ellipse...
		return;
	}

	r0 = _moments.CsiToRo(R0);
	double c0 = cos((T0)/_q);
	double s0 = sin((T0)/_q);

	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta)/_q);
		double s = sin((theta)/_q);

		double A;
		double B;
		double C;
	
		A = a11*c*c + 2*a12*c*s + a22*s*s;
		B = r0*(a11*c*c0 + a12*c*s0 + a12*c0*s + a22*s*s0);
		C = r0*r0*(2*a12*c0*s0 + a22*s0*s0 + a11*c0*c0)-1;

		double DELTA = B*B-A*C;
		if (DELTA >= 0)
		{
			int r = (B+sqrt(DELTA))/A + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
					output(theta,rho) = v;
			}
			
			r = (B-sqrt(DELTA))/A + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
						output(theta,rho) = v;
			}
		}

	}
	// plot center
	output.SafePixel(T0, R0) = v;
}

void YARPLpConicFitter::plotCircle(int T0, int R0, double R, YARPImageOf<YarpPixelMono> &output)
{
	int theta;
	int rho;
	int r0;
	r0 = _moments.CsiToRo(R0);
	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta-T0)/_q);
		double DELTA = (r0*r0*(c*c-1) + R*R);
		if (DELTA>=0)
		{
			int r = (r0*c+sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
					output(theta,rho) = 255;
			}
			
			r = (r0*c-sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
						output(theta,rho) = 255;
			}
		}
	}
	// plot center
	output.SafePixel(T0, R0) = 255;
}

void YARPLpConicFitter::plotCircle(int T0, int R0, double R, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	int theta;
	int rho;
	int r0;
	r0 = _moments.CsiToRo(R0);
	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta-T0)/_q);
		double DELTA = (r0*r0*(c*c-1) + R*R);
		if (DELTA>=0)
		{
			int r = (r0*c+sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
					output(theta,rho) = v;
			}
			
			r = (r0*c-sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho = _moments.RoToCsi(r);
				if ( (rho<=(_logpolarParams::_srho-1)) && (rho>0) )
						output(theta,rho) = v;
			}
		}
	}
	// plot center
	output.SafePixel(T0, R0) = v;
}

void YARPLpConicFitter::findCircle(int T0, int R0, double R, YARPLpShapeRegion &out)
{
	out.reset();
	int theta;
	int rho2;
	int rho1;
	int r0;
	r0 = _moments.CsiToRo(R0);
	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta-T0)/_q);
		double DELTA = (r0*r0*(c*c-1) + R*R);
		if (DELTA>=0)
		{
			int r1 = (r0*c+sqrt(DELTA)) + 0.5;
			int r2 = (r0*c-sqrt(DELTA)) + 0.5;
			rho1 = _moments.RoToCsi(r1);
			rho2 = _moments.RoToCsi(r2);

			if (rho1<0)
				rho1=0;
			if ((rho1>(_logpolarParams::_srho-1)))
			{
				rho1 = _logpolarParams::_srho-1;
			}

			if (rho2<0)
				rho2=0;
			if ((rho2>(_logpolarParams::_srho-1)))
			{
				rho2 = _logpolarParams::_srho-1;
			}
			
			int p;
			for(p = rho2; p<=rho1; p++)
				out.add(theta, p);
		}

	}
}

void YARPLpConicFitter::findEllipse(int T0, int R0, double a11, double a12, double a22, YARPLpShapeRegion &out)
{
	out.reset();
	int theta;
	int rho2;
	int rho1;
	int r0;

	if ( !_checkDet(a11, a12, a22) )
	{
		// sorry, not an ellipse...
		return;
	}

	r0 = _moments.CsiToRo(R0);
	double c0 = cos((T0)/_q);
	double s0 = sin((T0)/_q);

	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta)/_q);
		double s = sin((theta)/_q);

		double A;
		double B;
		double C;
	
		A = a11*c*c + 2*a12*c*s + a22*s*s;
		B = r0*(a11*c*c0 + a12*c*s0 + a12*c0*s + a22*s*s0);
		C = r0*r0*(2*a12*c0*s0 + a22*s0*s0 + a11*c0*c0)-1;

		double DELTA = B*B-A*C;

		if (DELTA>=0)
		{
			int r1 = (B+sqrt(DELTA))/A + 0.5;
			int r2 = (B-sqrt(DELTA))/A + 0.5;
			rho1 = _moments.RoToCsi(r1);
			rho2 = _moments.RoToCsi(r2);

			if (rho1<0)
				rho1=0;
			if ((rho1>(_logpolarParams::_srho-1)))
			{
				rho1 = _logpolarParams::_srho-1;
			}

			if (rho2<0)
				rho2=0;
			if ((rho2>(_logpolarParams::_srho-1)))
			{
				rho2 = _logpolarParams::_srho-1;
			}
			
			int p;
			for(p = rho2; p<=rho1; p++)
				out.add(theta, p);
		}
	}
}

////////// CARTESIAN
void YARPConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, int *x0,  int *y0, int *Rmin, int *Rmax, int *Rav)
{
	_moments.centerOfMass(in, x0, y0);
	_radius(in, *x0, *y0, Rmin, Rmax, Rav);
}

void YARPConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, int *x0,  int *y0, int *R)
{
	_moments.centerOfMass(in, x0, y0);
	*R = _radiusOfGyration(in, *x0, *y0);
}

void YARPConicFitter::fitEllipse(YARPImageOf<YarpPixelMono> &in, int *x0,  int *y0, double *a11, double *a12, double *a22)
{
	_moments.centerOfMass(in, x0, y0);
		
	double u20 = _moments.centralMoments(in, *x0, *y0, 2, 0);
	double u11 = _moments.centralMoments(in, *x0, *y0, 1, 1);
	double u02 = _moments.centralMoments(in, *x0, *y0, 0, 2);
	double u00 = _moments.centralMoments(in, *x0, *y0, 0, 0);

	double aSq;
	double bSq;
	double theta;

	double tmp = u20-u02;
	if (tmp != 0)
		theta = 0.5*atan(2*u11/tmp);
	else 
		theta = 3.14/4;

	aSq = (2/u00)*(u20+u02 + sqrt((u20-u02)*(u20-u02) + 4*u11*u11));
	bSq = (2/u00)*(u20+u02 - sqrt((u20-u02)*(u20-u02) + 4*u11*u11));
	
	double costh = cos(theta);
	double sinth = sin(theta);
	*a11 = (costh*costh)/(aSq) + (sinth*sinth)/(bSq);
	*a22 = (sinth*sinth)/(aSq) + (costh*costh)/(bSq);
	*a12 = (1/(aSq) - 1/(bSq)) * sinth*costh;
}

void YARPConicFitter::_radius(YARPImageOf<YarpPixelMono> &in, int x,  int y, int *Rmin, int *Rmax, int *Rav)
{
	ACE_ASSERT(0);	// not implemented yet
}

double YARPConicFitter::_radiusOfGyration(YARPImageOf<YarpPixelMono> &in, int x, int y)
{
	double u20 = _moments.centralMoments(in, x, y, 2, 0);
	double u02 = _moments.centralMoments(in, x, y, 0, 2);
	double u00 = _moments.centralMoments(in, x, y, 0, 0);

	return sqrt((u20+u02)/u00);
}

void YARPConicFitter::plotEllipse(int X0, int Y0, double a11, double a12, double a22, YARPImageOf<YarpPixelMono> &output)
{
	ACE_ASSERT(0);	// not impl yet
}

void YARPConicFitter::plotEllipse(int X0, int Y0, double a11, double a12, double a22, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	ACE_ASSERT(0);	// not impl yet
}

void YARPConicFitter::plotCircle(int X0, int Y0, double R, YARPImageOf<YarpPixelMono> &output)
{
	ACE_ASSERT(0);	// not impl yet
}

void YARPConicFitter::plotCircle(int X0, int Y0, double R, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	ACE_ASSERT(0);	// not impl yet
}

void YARPConicFitter::findCircle(int X0, int Y0, double R, YARPShapeRegion &out)
{
	ACE_ASSERT(0);	// not impl yet
}

void YARPConicFitter::findEllipse(int X0, int Y0, double a11, double a12, double a22, YARPShapeRegion &out)
{
	ACE_ASSERT(0);	// not impl yet
}