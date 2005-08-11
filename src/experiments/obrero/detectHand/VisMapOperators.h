

#ifndef __MVisMapOperators
#define __MVisMapOperators

//#include <assert.h>

#include "fakevis.h"
//#include "ScaleSpaceMath.h"
//#include "Histogram.h"
//#include "time.h"

#include "GlobalMathHelper.h"
#include "float.h"

//cck 03-12-02
//location of rand() function and RAND_MAX
#include <stdlib.h>


//////////////////////////////////////////////
//		Create Test Images
//

struct MakeRandomRGB
{
	inline void operator() (MVisRGBBytePixel &in)
	{
		in.Set(
			((float) rand()/(float) RAND_MAX) * 255.0,
			((float) rand()/(float) RAND_MAX) * 255.0,
			((float) rand()/(float) RAND_MAX) * 255.0
			);
	};
};


struct MakeRandomFloat
{
	inline void operator() (MVisFloatPixel &in)
	{
		in = ((float) rand()/(float) RAND_MAX); //[0.0, 1.0]
	};
};


struct MakeRandomUSInt
{
	inline void operator() (MVisUSIntPixel &in)
	{
		in = (MVisUSIntPixel)(((float) rand()/(float) RAND_MAX) * 64000.0); 
	};
};

//////////////////////////////////////////////////////////////////////
//  Operators to be used with Vectorfields
//////////////////////////////////////////////////////////////////////

struct CartesianToPolar
{
	inline void operator()(
		MVisFloatPixel &x, MVisFloatPixel &y, 
		MVisFloatPixel &mag, MVisFloatPixel &ang)
	{
		mag  = sqrt(sqr(x ) + sqr(y ));
		ang  = atan2(y ,x );
		//[-pi pi] -> [0 2*pi]
		if( ang < 0.0f) ang = ang + (2.0f*PI);
	}
};

struct CartesianToMag
{
	inline void operator()(
		MVisFloatPixel &x, MVisFloatPixel &y, 
		MVisFloatPixel &mag)
	{
		mag  = sqrt(sqr(x ) + sqr(y ));
	}
};

struct PolarToCartesian
{
	inline void operator()(
		MVisFloatPixel &mag, MVisFloatPixel &ang, 
		MVisFloatPixel &x, MVisFloatPixel &y)
	{
		x  = mag *cos(ang );
		y  = mag *sin(ang );
	}
};

struct PolarThresh
{
	inline void operator()(
		MVisFloatPixel &mag, MVisFloatPixel &ang)
	{
	}
};

struct CartesianThresh
{
	inline void operator()(
		MVisFloatPixel &mag, MVisFloatPixel &ang)
	{
	}
};
//////////////////////////////////////////////////////////////////////

struct ClearFloats
{
	inline void operator()(MVisFloatPixel &p)
	{
		p =0.0;
	}
};

class SetRGBToWhite
{
 public:
	inline void operator()(MVisRGBBytePixel &p)
	{
		p.Set(255,255,255);
	}
};

struct AccumFloatDifference
{
	float total;
	inline AccumFloatDifference() : total(0.0) {};
	inline void operator() (MVisFloatPixel &in1, MVisFloatPixel &in2)
	{
		total = total + fabs(in1  - in2 );
	};
};


struct DifferenceRGB
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisRGBBytePixel &out)
	{
		int rd,gd,bd;
		
		rd = in1.R() - in2.R();
		gd = in1.G() - in2.G();
		bd = in1.B() - in2.B();
		
		if(rd < 0) rd = 0;
		if(gd < 0) gd = 0;
		if(bd < 0) bd = 0;
		
		out.Set(rd,gd,bd);
	}
};


struct SumRGB
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisRGBBytePixel &out)
	{
		int rd,gd,bd;
		
		rd = myround((float)(in1.R() + in2.R())/2.0f);
		gd = myround((float)(in1.G() + in2.G())/2.0f);
		bd = myround((float)(in1.B() + in2.B())/2.0f);
		
		if(rd > 255) rd = 255;
		if(gd > 255) gd = 255;
		if(bd > 255) bd = 255;
		
		out.Set(rd,gd,bd);
	}
};


struct Difference3
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisRGBBytePixel &out)
	{
		int rd,gd,bd;
		
		rd = in1.R() - in2.R();
		gd = in1.G() - in2.G();
		bd = in1.B() - in2.B();
		
		if(rd < 0) rd = -rd;
		if(gd < 0) gd = -gd;
		if(bd < 0) bd = -bd;
		
		out.SetR(rd);
		out.SetG(gd);
		out.SetB(bd);
		//= (rd + gd + bd)/3;	
		//out = Minus(in1, in2);
	}
};



struct Difference1
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisRGBBytePixel &out)
	{
		int rd,gd,bd;
		
		rd = in1.R() - in2.R();
		gd = in1.G() - in2.G();
		bd = in1.B() - in2.B();
		
		if(rd < 0) rd = -rd;
		if(gd < 0) gd = -gd;
		if(bd < 0) bd = -bd;
		
		out.SetB((rd + gd + bd)/3);	
	}
};

struct DifferenceToFloat
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisFloatPixel &out)
	{
		int rd,gd,bd;
		
		rd = in1.R() - in2.R();
		gd = in1.G() - in2.G();
		bd = in1.B() - in2.B();
		
		if(rd < 0) rd = -rd;
		if(gd < 0) gd = -gd;
		if(bd < 0) bd = -bd;
		
		out  = ((float)(rd + gd + bd))/3.0;	
	}
};


struct AccumDifferenceToFloat
{
	inline void operator() (MVisRGBBytePixel &in1, MVisRGBBytePixel &in2, MVisFloatPixel &out)
	{
		int rd,gd,bd;
		
		rd = in1.R() - in2.R();
		gd = in1.G() - in2.G();
		bd = in1.B() - in2.B();
		
		if(rd < 0) rd = -rd;
		if(gd < 0) gd = -gd;
		if(bd < 0) bd = -bd;
		
		out  = out  + ((float)(rd+gd+bd))/3.0;
	}
};

struct NormRGBVectors
{
	//normalizes all of the rgb vectors to have unit length
	
	float norm;
	float thresh;
	
	inline NormRGBVectors(float threshi) : thresh(threshi) {};
	
	inline void operator()(MVisRGBBytePixel &in)
	{
		norm =  sqrt( sqr(in.R()) + sqr(in.G()) + sqr(in.B()) );
		if(norm <= thresh)
		{
			in.SetR(0);
			in.SetG(0);
			in.SetB(0);
		} else
		{
			in.SetR( myround(255.0 * (float) in.R()/norm) );
			in.SetG( myround(255.0 * (float) in.G()/norm) );
			in.SetB( myround(255.0 * (float) in.B()/norm) );
		}
	}
};

struct GetRGBNorm
{
	//outputs the length of the RGB vectors
	
	inline void operator()(MVisRGBBytePixel &in, MVisFloatPixel &out)
	{
		out  =  sqrt( sqr(in.R()) + sqr(in.G()) + sqr(in.B()) );
	}
};


struct SphericalColorModel
{
	float cmr;
	float cmg;
	float cmb;
	
	inline SphericalColorModel(float ri, float gi, float bi) : cmr(ri), cmg(gi), cmb(bi) {};
	
	//computes the distance map using simple euclidean distance
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &distanceout)
	{
		distanceout  = sqrt(sqr(in.R() - cmr) + sqr(in.G() - cmg) + sqr(in.B() - cmb));
	}
};


struct VectorAngleColorModel
{
	float cmr;
	float cmg;
	float cmb;
	float magin;
	float cutoff;
	float maxdist;
	float mindist;
	float dotprod;
	
	inline VectorAngleColorModel(float ri, float gi, float bi) 
	{
		magin = sqrt(sqr(ri) + sqr(gi) + sqr(bi));
		cmr = ri/magin;
		cmg = gi/magin;
		cmb = bi/magin;
		cutoff = 1.0f;
		mindist = 0.0f;
		maxdist = PI;
	};
	
	//computes the distance map using the angle between two vectors going through the 
	//origin, this essentially allows for larger color variance with increasing 
	//brightness, which may not be what we want
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &distanceout)
	{
		magin = sqrt(sqr(in.R()) + sqr(in.G()) + sqr(in.B()));
		if(magin < cutoff) 
		{
			distanceout  = maxdist/2.0f;
		} else
		{
			dotprod = ((in.R() * cmr) + (in.G() * cmg) + (in.B() * cmb)) / magin;
			//dotprod must be [-1 1]
			if(dotprod < -1.0f) dotprod = -1.0f;
			else if(dotprod > 1.0f) dotprod = 1.0f;

			//this gives an output in the range of [0 pi] radians
			//invert the distance so pi is the best match
			distanceout  = maxdist -  acos( dotprod );	
		}
		
		//normalize the distance to be [0,1]
		distanceout  = distanceout /maxdist;

		if(distanceout <0.0f) distanceout  = 0.0f;
		else if(distanceout >1.0f) distanceout  = 1.0f;

	}
};

struct PlanarColorModel
{
	float v1r;
	float v1g;
	float v1b;
	
	float v2r;
	float v2g;
	float v2b;
	
	float v1comp;
	float v2comp;
	
	float nr;
	float ng;
	float nb;
	
	inline PlanarColorModel(
		float p1r, float p1g, float p1b,
		float p2r, float p2g, float p2b)
	{
		//creates a plane out of the two input points and the origin
		//
		//take the two input points and create an orthonormal basis 
		//with orthogonal unit length components
		//
		//since it's only executed once at initialization, this does 
		//not need to be too efficient
		
		float p1mag;
		float p2projonv1;
		float v2mag;
		
		p1mag = sqrt(sqr(p1r) + sqr(p1g) + sqr(p1b));
		
		//use the first input point as one of the vectors
		v1r = p1r/p1mag;
		v1g = p1g/p1mag;
		v1b = p1b/p1mag;
		
		p2projonv1 = p2r*v1r + p2g*v1g + p2b*v1b;
		
		//subtract the non-orthogonal component from p2
		v2r = p2r - (p2projonv1 * v1r);
		v2g = p2g - (p2projonv1 * v1g);
		v2b = p2b - (p2projonv1 * v1b);
		
		//normalize the second basis vector, v2
		v2mag = sqrt(sqr(v2r) + sqr(v2g) + sqr(v2b));
		v2r = v2r/v2mag;
		v2g = v2g/v2mag;
		v2b = v2b/v2mag;
	};
	
	//computes the distance map using simple euclidean distance
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &distanceout)
	{
		//it appears as though a large portion of this could be precomputed
		//to facilitate fast multiple executions, as an expanded equation
		//with precomputed terms.
		//I won't make the conversion unless this appears to be very useful
		
		//first determine the component of the input vector that sits on the plane
		//by projecting the input vector onto the orthonormal basis
		v1comp = in.R()*v1r + in.G()*v1g + in.B()*v1b;
		v2comp = in.R()*v2r + in.G()*v2g + in.B()*v2b;
		
		//now subtract the planar component from the input vector
		//to get the component vector normal to the plane
		nr = in.R() - ((v1comp * v1r) + (v2comp*v2r));
		ng = in.G() - ((v1comp * v1g) + (v2comp*v2g));
		nb = in.B() - ((v1comp * v1b) + (v2comp*v2b));
		
		//return the normal vector's length
		distanceout  = sqrt(sqr(nr) + sqr(ng) + sqr(nb));
	}
};



struct RGPlanarColorModel
{
	float v1r;
	float v1g;
	
	float v1comp;
	
	float nr;
	float ng;
	
	float outbound;
	
	inline RGPlanarColorModel(float p1r, float p1g)
	{
		//this is a simplified planar model for use in skin color
		//detection. It simply makes a plane coming out normal to 
		//the RG plane and intersecting (0,0)
		//might want to add a constant color offset
		
		float p1mag;
		
		p1mag = sqrt(sqr(p1r) + sqr(p1g));
		
		//use the first input point as one of the vectors
		v1r = p1r/p1mag;
		v1g = p1g/p1mag;
		
		outbound = sqrt(sqr(255) + sqr(255));
	};
	
	//computes the distance map using simple euclidean distance
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &distanceout)
	{
		
		//for this simple plane all we need to do is determine the 
		//2-d problem of the projection of the RG plane onto the (r,g,0) plane
		//which will be a line, and the projection of the input vector onto
		//(r,g,0) plane, which is also a line.
		
		//first determine the component of the input vector that sits on the plane
		//by projecting the input vector onto the orthonormal basis
		v1comp = in.R() *v1r + in.G()*v1g;
		//v2comp = in.B();
		
		//now subtract the planar component from the input vector
		//to get the component vector normal to the plane
		nr = in.R() - (v1comp * v1r);
		ng = in.G() - (v1comp * v1g);
		//as we would suspect  in.B() does not have any influence
		//nb = in.B() - (v2comp * v2b);
		//notice that this is equivalent to: 
		//nb = in.B()*(1-v2b)
		//nb = in.B()*(0) = 0
		
		//return the normal vector's length
		//	distanceout = sqrt(sqr(nr) + sqr(ng) + sqr(nb));
		//an upper bound on the maximum distance possible is 
		// sqrt(sqr(255) + sqr(255));
		//we can use this to invert the measurement in a 
		//consistent way and normalize it to be between [0,1]
		distanceout  = 1.0 - ( sqrt(sqr(nr) + sqr(ng)) / outbound );
	}
};


struct SkinColorModel
{
	float v1r;
	float v1g;
	
	float v1comp;
	
	float nr;
	float ng;
	
	float outbound;
	
	inline SkinColorModel(float p1r, float p1g)
	{
		//this is a simplified planar model for use in skin color
		//detection. It simply makes a plane coming out normal to 
		//the RG plane and intersecting (0,0)
		//might want to add a constant color offset
		
		float p1mag;
		
		p1mag = sqrt(sqr(p1r) + sqr(p1g));
		
		//use the first input point as one of the vectors
		v1r = p1r/p1mag;
		v1g = p1g/p1mag;
		
		outbound = sqrt(sqr(255) + sqr(255));
	};
	
	//computes the distance map using simple euclidean distance
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &distanceout)
	{
		if(in.R() < 10) 
		{
			distanceout  = 0.0;
			return;
		}
		else
		{
			if( (in.G() < 10) || (in.B() < 10) ) 
			{
				distanceout  = 0.0;
				return;
			}
			else
			{
				
				float rg, rb, bg;
				rg = (float) in.R() / (float) in.G();
				rb = (float) in.R() / (float) in.B();
				bg = (float) in.B() / (float) in.G();
				
				if( rg < 1.0 ) 
				{
					distanceout  = 0.0;
					return;
				}
				if( rb < 1.0 ) 
				{
					distanceout  = 0.0;
					return;
				}
				
				distanceout  = (5.0 - (sqr(rg - 1.0) + sqr(rb - 1.0))) / 5.0; 
				if(distanceout  < 0.0) distanceout  = 0.0;
				
				//each ratio can vary from 1 to about 30
				//30^2 = 900
				//9000 is approx the max value 
				/*	distanceout  = 
				(50 -
				(2.5 * sqr( ((float) in.R()/ (float) in.G()) - 1.5 ) ) + 
				(5.0 * sqr( ((float) in.R()/ (float) in.B()) - 1.2 ) ) + 
				(2.5 * sqr( ((float) in.B()/ (float) in.G()) - 1.0 ) )
				)/ 50.0;
				if(distanceout  < 0.25) distanceout  = 0.0;
				else distanceout  = distanceout  - 0.25;
				*/
			}
		}
	}
};

//OTHER DESIRABLE COLOR MODELS 
//line, simply the distance between the input point and the model line
//gaussian blob
//rectangular region
//HSV something

struct MakeGreyFloat
{
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &out)
	{
		out  = (float) in.R() + (float) in.G() + (float) in.B();
	}
};

struct Byte_to_Float
{
	inline void operator() (MVisBytePixel &in, MVisFloatPixel &out)
	{
		out  = (float) in ;
	}
};


struct Byte_to_RGB
{
	inline void operator() (MVisBytePixel &in, MVisRGBBytePixel &out)
	{
		out.Set(in , in , in );
	}
};


struct RGB_to_Byte
{
	inline void operator() (MVisRGBBytePixel &in, MVisBytePixel &out)
	{
		out  = (unsigned char) ( ((float) in.R() + (float) in.G() + (float) in.B()) / 3.0f );
	}
};

struct MakeGreyFloatTest
{
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &out)
	{
		out  = (float) in.G();
	}
};

struct RGBToFloat
{
	inline void operator() (MVisRGBBytePixel &in, MVisFloatPixel &out)
	{
		out  = (float) in.R() + (float) in.G() + (float) in.B();
	}
};

struct RGBToByte
{
	inline void operator() (MVisRGBBytePixel &in, MVisBytePixel &out)
	{
		out  = MVisBytePixel(myround( ((float) in.R() + (float) in.G() + (float) in.B()) / 3.0 ));
	}
};

struct MakeFloat
{
	inline void operator() (const MVisRGBBytePixel &in1, MVisFloatPixel &out)
	{
		out  = (float) in1.B();
	}
};


struct ComputeAverageVector
{
	float av_x;
	float av_y;
	inline ComputeAverageVector() : av_x(0.0f), av_y(0.0f){};
	inline void operator() (const MVisFloatPixel &dx_in, const MVisFloatPixel &dy_in)
	{
		av_x = av_x + dx_in ;
		av_y = av_y + dy_in ;
	};
};


struct Mag_Byte
{
	float mult;
	inline Mag_Byte()
	{
		mult = 255.0/sqrt(sqr(255.0) + sqr(255.0));
	}
	
	inline void operator() (
		const MVisBytePixel &dx_in, 
		const MVisBytePixel &dy_in,
		MVisBytePixel &mag
		)
	{
		mag  = (char) (sqrt((float) (sqr(dx_in ) + sqr(dy_in )) ) * mult);
	};
};


struct Ang_Byte
{
	float mult;
	float add;
	unsigned char thresh;
	
	inline Ang_Byte(unsigned char threshi)
	{
		thresh = threshi;
	}
	
	inline void operator() (
		const MVisBytePixel &dx_in, 
		const MVisBytePixel &dy_in,
		const MVisBytePixel &mag_in,
		MVisBytePixel &ang
		)
	{
		//atan2 returns -pi to pi radians
		if(mag_in  > thresh)
		{
			float fang;
			fang = atan2( (float) dy_in , (float) dx_in );
		//[-pi pi] -> [0 2*pi]
		if( fang < 0.0f) fang = fang + (2.0f*PI);
		ang = (char)  ( ( ((float) fang) / (2.0f*PI) ) * 255.0f );
		//output is 0 = 0, 255 = 2*PI
		} else ang = 0;
	};
};

struct RGB_Deriv_to_5d
{
	float thresh;
	float ang;
	
	inline void SetToZero(float &dr, float &dg, float &db, float &mag, float &ang)
	{
		mag = 0.0f;
		ang = -10.0f;  //this represents an invalide angle
		dr = 0.0f;
		dg = 0.0f;
		db = 0.0f;
	}
	
	inline float Mag(int x, int y)
	{
		return( sqrt((double)(sqr(x) + sqr(y))) );
	}
	
	inline float Ang(int x, int y)
	{
		//ang is between -pi and pi
		ang = atan2((float) y, (float) x);
		//[-pi pi] -> [0 2*pi]
		if( ang < 0.0f) return(ang + (2.0f*PI));
		else return(ang);
	}
	
	inline RGB_Deriv_to_5d(float threshi)
	{
		thresh = threshi;
	}
	
	inline void operator() (
		const MVisRGBBytePixel &dx_pos_in, 
		const MVisRGBBytePixel &dy_pos_in,
		const MVisRGBBytePixel &dx_neg_in, 
		const MVisRGBBytePixel &dy_neg_in,
		MVisFloatPixel  &dr,
		MVisFloatPixel	&dg,
		MVisFloatPixel	&db,
		MVisFloatPixel	&mag,
		MVisFloatPixel	&ang)
	{
		int rx,ry;
		int gx,gy;
		int bx,by;
		
		float rmag;
		float gmag;
		float bmag;
		
		int dx,dy;
		//////////////////////////////////////////////////
		//alternatively, the magnitude could be
		//the euclidean distance in the color space
		
		//////////////////////////////////////////////////
		//find the color dimension with the maximum change 
		//and it's associated angle, mag and vector
		
		rx = dx_pos_in.R() - dx_neg_in.R();
		ry = dy_pos_in.R() - dy_neg_in.R();
		gx = dx_pos_in.G() - dx_neg_in.G();
		gy = dy_pos_in.G() - dy_neg_in.G();
		bx = dx_pos_in.B() - dx_neg_in.B();
		by = dy_pos_in.B() - dy_neg_in.B();
		
		rmag = Mag( rx, ry );
		gmag = Mag( gx, gy );
		bmag = Mag( bx, by );
		
		////////////////////////////////////////////
		//also calculate the change vector in color space
		//by projecting the derivative vector onto
		//a normalized version of the winning direction
		
		if( (rmag > gmag) && (rmag > bmag) )
		{ 
			//red wins
			if(rmag < thresh) SetToZero(dr , dg , db , mag , ang );
			else
			{
				ang  = Ang(rx, ry);
				dx = rx;
				dy = ry;
				
				dr  = rmag;
				if(rmag > 0.0001)
				{
					dg  = (gx*dx + gy*dy)/rmag;
					db  = (bx*dx + by*dy)/rmag;
				} else
				{
					dg  = 0.0f;
					db  = 0.0f;
				}
			}
		} else
			if( gmag > bmag )
			{					
				//the green change wins
				if(gmag < thresh) SetToZero(dr , dg , db , mag , ang );
				else
				{
					ang = Ang(gx, gy);
					dx = gx;
					dy = gy;
					
					dg  = gmag;
					if(gmag > 0.0001)
					{
						dr  = (rx*dx + ry*dy)/gmag;
						db  = (bx*dx + by*dy)/gmag;
					} else
					{
						dr  = 0.0f;
						db  = 0.0f;
					}
				}
			} else
			{	
				//the blue change is the biggest
				if(bmag < thresh) SetToZero(dr , dg , db , mag , ang );
				else
				{
					ang  = Ang(bx, by);
					dx = bx;
					dy = by;
					
					db  = bmag;
					if(bmag > 0.0001)
					{
						dr  = (rx*dx + ry*dy)/bmag;
						dg  = (gx*dx + gy*dy)/bmag;
					} else
					{
						dr  = 0.0f;
						dg  = 0.0f;
					}
				}
			}
			
			mag  = sqrt(sqr(dr ) + sqr(dg ) + sqr(db ));
		};
};



struct QuantizeAngles
{
	inline void operator() (const MVisFloatPixel &in, MVisBytePixel &out)
	{
		
		//this now only works for angles in the range [0, 2*PI]
		if(in  == -10) out  = 5;
	//	else if(in  < (-7.0f/8.0f)*PI) out  = 1;
	//	else if(in  < (-5.0f/8.0f)*PI) out  = 2;
	//	else if(in  < (-3.0f/8.0f)*PI) out  = 3;
	//	else if(in  < (-1.0f/8.0f)*PI) out  = 4;
		else if(in  < (1.0f/8.0f)*PI)  out  = 1;
		else if(in  < (3.0f/8.0f)*PI)  out  = 2;
		else if(in  < (5.0f/8.0f)*PI)  out  = 3;
		else if(in  < (7.0f/8.0f)*PI)  out  = 4;
		else if(in  < (9.0f/8.0f)*PI)  out  = 1;
		else if(in  < (11.0f/8.0f)*PI)  out  = 2;
		else if(in  < (13.0f/8.0f)*PI)  out  = 3;
		else if(in  < (15.0f/8.0f)*PI)  out  = 4;
		else out  = 1;
		
		//out = 1 : from (-pi/8 to pi/8)   or (7pi/8 to pi)      or (-7pi/8 to -pi)
		//out = 2 : from (pi/8 to 3pi/8)   or (-5pi/8 to -7pi/8)
		//out = 3 : from (3pi/8 to 5pi/8)  or (-3pi/8 to -5pi/8)
		//out = 4 : from (5pi/8 to 7pi/8)  or (-3pi/8 to -pi/8)
		
	};
};



struct X_Deriv_RGB
{
	MVisRGBBytePixel prev_in;
	float mult;
	
	inline X_Deriv_RGB()
	{
		prev_in.SetR(0);
		prev_in.SetG(0); 
		prev_in.SetB(0);
		mult = 255.0/sqrt(sqr(255.0) + sqr(255.0) + sqr(255.0));
	};
	
	inline void operator() (const MVisRGBBytePixel &in, MVisBytePixel &out)
	{
		out  = (char)
			(sqrt( (float)
			(sqr(in.R() - prev_in.R()) + 
			sqr(in.G() - prev_in.G()) + 
			sqr(in.B() - prev_in.B())) * mult)
			) ;
		
		prev_in.SetR(in.R());
		prev_in.SetG(in.G());
		prev_in.SetB(in.B());
	};
};


class MaxMin
{
public:

	float maxpix;
	float minpix;
	
	//FLT_MIN = the smallest positive float
	//	MaxMin() : maxpix(FLT_MIN), minpix(FLT_MAX){};
	//not sure if this will work
	MaxMin() : maxpix(-FLT_MAX), minpix(FLT_MAX){};
	
	inline void operator() (const MVisFloatPixel &in)
	{
		if(in  > maxpix)
		{
			maxpix = in ;
		}
		if(in  < minpix)
		{
			minpix = in ;
		}
	};
};


class Float2RGB
{
public:
	float minpix;
	float maxpix;
	float divisor;
	inline Float2RGB(FIm & f_image) 
	{
		MaxMin mm_op;
		VisMap1(mm_op, f_image);

		minpix = mm_op.minpix;
		maxpix = mm_op.maxpix;
		divisor = (maxpix - minpix)/255.0;
		//this avoids divide by zero and sets each pixel to have a value of zero
		//assuming the min and max are accurate
		if(divisor == 0.0) divisor = 1.0;
	};
	
	inline void operator() (const MVisFloatPixel &in1, MVisRGBBytePixel &out)
	{
		//comment out commands for efficiency
		out.SetR( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetG( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetB( (unsigned char) ((in1 -minpix)/divisor) );
	};
};


/*
struct HistOp
{
	CHistogram * hist;
	float thresh;
	inline HistOp(CHistogram * histi, float thresh) 
	{
		hist = histi;
	};
	inline void operator() (MVisFloatPixel &in)
	{
		if(in >thresh) hist->AddValue(in );	
	};
};
*/

struct SetToZeroFloat
{
	inline void operator() (MVisFloatPixel &in1)
	{
		in1 =0.0;
	};	
};

struct SetToZeroRGB
{
	inline void operator() (MVisRGBBytePixel &in1)
	{
		in1.Set(0,0,0);
	};
};

struct SetToRand
{
	SetToRand() {	
		
	   /* Seed the random-number generator with current time so that
	   * the numbers will be different every time we run.
		*/
		//srand( (unsigned)time( NULL ) );	
	}
	
	inline void operator() (MVisFloatPixel &in1)
	{
#ifndef UNIX_BUILD
		//set the pixels to have random float values between 0.0 and 1.0 
		in1  = (float)rand()/(float)RAND_MAX;
#endif
	};
	
};


struct ByteToRGB
{	
	inline void operator() (const MVisBytePixel &in1, MVisRGBBytePixel &out)
	{
		//comment out commands for efficiency
		out.SetR( in1  );
		out.SetG( in1  );
		out.SetB( in1  );
	};
};

struct USIntToFloat
{
	//from [0 max] -> [0 1]  with >max = 1
	//assumes that the float output should range from [0 1]

	float max;
	inline USIntToFloat(float maxi)
	{
		max = maxi;
	};

	inline void operator() (const MVisUSIntPixel &in , MVisFloatPixel &out)
	{
		if(in  >= max) 
		{
			out  = 1.0;
		}
		else 
		{
			out  =  ((float) in )/max;
		}
	};
};

struct FloatToRGB
{
	float minpix;
	float maxpix;
	float divisor;
	inline FloatToRGB(float minpixi, float maxpixi): minpix(minpixi), maxpix(maxpixi) 
	{
		divisor = (maxpix - minpix)/255.0;
		//this avoids divide by zero and sets each pixel to have a value of zero
		//assuming the min and max are accurate
		if(divisor == 0.0) divisor = 1.0;
	};
	
	inline void operator() (const MVisFloatPixel &in1, MVisRGBBytePixel &out)
	{
		//comment out commands for efficiency
		out.SetR( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetG( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetB( (unsigned char) ((in1 -minpix)/divisor) );
	};
};

struct NormFloat
{
	float minpix;
	float maxpix;
	float divisor;
	inline NormFloat(float minpixi, float maxpixi): minpix(minpixi), maxpix(maxpixi) 
	{
//		assert(minpixi<=maxpixi);
		divisor = (maxpix - minpix)/1.0;
		//this avoids divide by zero and sets each pixel to have a value of zero
		//assuming the min and max are accurate
		if(divisor == 0.0) divisor = 1.0;
	};
	
	inline void operator() (MVisFloatPixel &in1)
	{
		//comment out commands for efficiency
		in1  = (in1 -minpix)/divisor;
	};
};

struct FloatToDisplay
{
	float minpix;
	float maxpix;
	float divisor;
	inline FloatToDisplay(float minpixi, float maxpixi): minpix(minpixi), maxpix(maxpixi) 
	{
		//assert(minpixi<=maxpixi);
		divisor = (maxpix - minpix)/254;
		//254 is safe (maybe too safe) to avoid overflow on byte
		//this avoids divide by zero and sets each pixel to have a value of zero
		//assuming the min and max are accurate
		if(divisor == 0.0) divisor = 1.0;
	};
	
	inline void operator() (const MVisFloatPixel &in1, MVisRGBBytePixel &out)
	{
		//comment out commands for efficiency
		out.SetR( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetG( (unsigned char) ((in1 -minpix)/divisor) );
		out.SetB( (unsigned char) ((in1 -minpix)/divisor) );
	};
};


struct FloatToDisplayR
{
	float minpix;
	float maxpix;
	float divisor;
	inline FloatToDisplayR(float minpixi, float maxpixi): minpix(minpixi), maxpix(maxpixi) 
	{
		divisor = (maxpix - minpix)/255.0;
		//this avoids divide by zero and sets each pixel to have a value of zero
		//assuming the min and max are accurate
		if(divisor == 0.0) divisor = 1.0;
	};
	
	inline void operator() (const MVisFloatPixel &in1, MVisRGBBytePixel &out)
	{
		out.SetR( (unsigned char) ((in1 -minpix)/divisor) );
	};
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////

struct  Clip
{
	float top;
	inline Clip(float topi):top(topi){};
	inline void operator() (MVisFloatPixel & in)
	{
		if(in  > top) in  = top;
	} 
};

struct  Thresh
{
	float thresh;
	inline Thresh(float threshi):thresh(threshi){};
	inline void operator() (MVisFloatPixel & in)
	{
		if(fabs(in ) > thresh) in  = 1.0;
		else in  = 0.0;
	} 
};

struct  ThreshVect
{
	float thresh;
	inline ThreshVect(float threshi):thresh(threshi){};
	inline void operator() (MVisFloatPixel & in1, MVisFloatPixel & in2)
	{
		if(fabs(in1 ) > thresh) in1  = 1.0;
		else
		{
			in1  = 0.0;
			in2  = 0.0;
		}
	} 
};

struct  CopyPixels
{
	inline void operator() (MVisFloatPixel & from, MVisFloatPixel & to )
	{
		to  = from ;
	} 
};

struct CopyRGBByte
{	
	inline void operator() (MVisRGBBytePixel & from, MVisRGBBytePixel & to )
	{
		to = from;
	} 
};

struct CopyByte
{	
	inline void operator() (MVisBytePixel & from, MVisBytePixel & to )
	{
		to  = from ;
	} 
};

struct  CopyFloat
{
	inline void operator() (MVisFloatPixel & from, MVisFloatPixel & to )
	{
		to  = from ;
	} 
};

struct  AddFloats
{
	inline void operator() (
		MVisFloatPixel &in1,
		MVisFloatPixel &in2
		)
	{
		in1  = in1  + in2 ;
	};
};


struct  AddInts
{
	inline void operator() (
		MVisIntPixel &in1,
		MVisIntPixel &in2
		)
	{
		in1  = in1  + in2 ;
	};
};

struct  AddVectors
{
	inline void operator() (
		MVisFloatPixel &in1c1,
		MVisFloatPixel &in1c2,
		const MVisFloatPixel &in2c1,
		const MVisFloatPixel &in2c2
		)
	{
		in1c1  = in1c1  + in2c1 ;
		in1c2  = in1c2  + in2c2 ;
	};
};


//////////////////////////////////////////////////


struct MaxCurv
{
	inline void operator() (MVisFloatPixel & out, MVisFloatPixel & dxx, MVisFloatPixel & dyy, MVisFloatPixel & dxy)
	{
		float first_term, root_term;
		float test1, test2, sign1, sign2;
		float lambda1, lambda2, angle1, angle2;
		float max_response, max_angle;
		
		
		//compute the eigenvalues and eigenvector angles (
		//principal axes of curvature)
		
		first_term= dyy +dxx ;
		root_term=sqrt( pow(first_term, 2) + 4.0*(dxx *dyy  - pow(dxy , 2)));
		
		lambda1=0.5*(first_term+root_term);
		lambda2=0.5*(first_term-root_term);
		
		if((dyy -lambda1)!=0.0) {
			
			if((dxy /(dyy -lambda1))<0) sign1=-1.0; else sign1=1.0;
			
			if((dxx -lambda1)!=0)
			{
				test1=sqrt((dyy -lambda1)/(dxx -lambda1));
				angle1=atan(sign1*test1);
			}else angle1=PI/2.0f;
			
		}else angle1=2.0*PI;
		
		
		if((dyy -lambda2)!=0.0) {
			
			if((dxy /(dyy -lambda2))<0) sign2=-1.0; else sign2=1.0;
			
			if((dxx -lambda2)!=0)
			{
				test2=sqrt((dyy -lambda2)/(dxx -lambda2));
				angle2=atan(sign2*test2);
			}else angle2=PI/2.0f;
			
		} else angle2=2.0f*PI;
		
		
		
		
		//determine the maximum angle and the corresponding 
		//response (curvature = 2nd derivative)
		
		angle1=angle1+(0.5*PI);
		angle2=angle2+(0.5*PI);
		
		if(lambda1>lambda2)
		{
			if(lambda1>0)
			{
				max_response=lambda1;
				max_angle=angle1;
			} else 
			{
				max_response=0.0;
				max_angle=0.0;
			}
		} else
		{
			if(lambda2>0)
			{
				max_response=lambda2;
				max_angle=angle2;
			} else
			{
				max_response=0.0;
				max_angle=0.0;
			} 
		}
		
		out  = max_response;
	} 
};


struct MaxCurvFull
{
	inline void operator() (MVisFloatPixel & outmag, MVisFloatPixel & outang, MVisFloatPixel & dxx, MVisFloatPixel & dyy, MVisFloatPixel & dxy)
	{
		float first_term, root_term;
		float test1, test2, sign1, sign2;
		float lambda1, lambda2, angle1, angle2;
		float max_response, max_angle;
		
		
		//compute the eigenvalues and eigenvector angles (
		//principal axes of curvature)
		
		first_term= dyy +dxx ;
		root_term=sqrt( pow(first_term, 2) + 4.0*(dxx *dyy  - pow(dxy , 2)));
		
		lambda1=0.5*(first_term+root_term);
		lambda2=0.5*(first_term-root_term);
		
		if((dyy -lambda1)!=0.0) {
			
			if((dxy /(dyy -lambda1))<0) sign1=-1.0; else sign1=1.0;
			
			if((dxx -lambda1)!=0)
			{
				test1=sqrt((dyy -lambda1)/(dxx -lambda1));
				angle1=atan(sign1*test1);
			}else angle1=PI/2.0f;
			
		}else angle1=2.0f*PI;
		
		
		if((dyy -lambda2)!=0.0) {
			
			if((dxy /(dyy -lambda2))<0) sign2=-1.0; else sign2=1.0;
			
			if((dxx -lambda2)!=0)
			{
				test2=sqrt((dyy -lambda2)/(dxx -lambda2));
				angle2=atan(sign2*test2);
			}else angle2=PI/2.0f;
			
		} else angle2=2.0f*PI;
		
		
		
		
		//determine the maximum angle and the corresponding 
		//response (curvature = 2nd derivative)
		
		angle1=angle1+(0.5*PI);
		angle2=angle2+(0.5*PI);
		
		if(lambda1>lambda2)
		{
			if(lambda1>0)
			{
				max_response=lambda1;
				max_angle=angle1;
			} else 
			{
				max_response=0.0;
				max_angle=0.0;
			}
		} else
		{
			if(lambda2>0)
			{
				max_response=lambda2;
				max_angle=angle2;
			} else
			{
				max_response=0.0;
				max_angle=0.0;
			} 
		}
		
		outmag  = max_response;
		outang  = max_angle;
	} 
};

#endif

//////////////////////////////////////////////////

