/*--------------------------------*-C-*---------------------------------*
 * File:
 *	fftn.h
 * ---------------------------------------------------------------------*
 * Re[]:	real value array
 * Im[]:	imaginary value array
 * nTotal:	total number of complex values
 * nPass:	number of elements involved in this pass of transform
 * nSpan:	nspan/nPass = number of bytes to increment pointer
 *		in Re[] and Im[]
 * isign:	exponent: +1 = forward  -1 = reverse
 * scaling:	normalizing constant by which the final result is *divided*
 *	scaling == -1, normalize by total dimension of the transform
 *	scaling <  -1, normalize by the square-root of the total dimension
 *
 * ----------------------------------------------------------------------
 * See the comments in the code for correct usage!
 */

#ifndef __YARPFfth__
#define __YARPFfth__

// Usual QNX stuff.
#ifdef __QNX__
#include "YARPSafeNew.h"
#endif

/* double precision routine */
extern int fftn (int ndim, const int dims[], double Re[], double Im[],
		 int isign, double scaling);

extern void fft_free (void);
extern void fft_alloc (int max_factors, int max_perm);

//
// FAST PORTING July 2001: so far this class can be only instantiated once...
// because it uses static memory allocation.
//
// LATER: complete porting to C++.
//
class YARPFft
{
public:
	YARPFft (int max_factors, int max_perm) 
	{ 
		fft_alloc(max_factors, max_perm);
	}

	~YARPFft() 
	{ 
		fft_free(); 
	}

	void Fft (int ndim, const int dims[], double Re[], double Im[], int isign, double scaling)
	{
		fftn (ndim, dims, Re, Im, isign, scaling);
	}


	// helper functions.
	static void Convert2DoubleScaled (const unsigned char *in, double *out, int w, int h);
	static void Convert2Double (const unsigned char *in, double *out, int w, int h);
	static void Fft2DShift (const double *in, double *out, int w, int h);
};

//
// See the YARPFft.cpp for implementation.
//	- the whole class is really weird as it is now but with little work
//		I imagine I could clean it.
//
#endif
