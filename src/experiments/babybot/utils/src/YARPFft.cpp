/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPFft.cpp,v 1.1 2004-07-29 13:09:14 babybot Exp $
///
///

/*--------------------------------*-C-*---------------------------------*
 *
 * Descript:
 *	multivariate complex Fourier transform, computed in place
 *	using mixed-radix Fast Fourier Transform algorithm.
 *
 *	Fortran code by:
 *	RC Singleton, Stanford Research Institute, Sept. 1968
 *
 *	translated by f2c (version 19950721).
 *
 * Revisions:
 * 26 July 95	John Beale
 *	- added maxf and maxp as parameters to fftradix()
 *
 * 28 July 95	Mark Olesen <olesen@me.queensu.ca>
 *	- cleaned-up the Fortran 66 goto spaghetti, only 3 labels remain.
 *
 *	- added fft_free() to provide some measure of control over
 *	  allocation/deallocation.
 *
 *	- added fftn() wrapper for multidimensional FFTs
 *
 *	- use -DFFT_NOFLOAT or -DFFT_NODOUBLE to avoid compiling that
 *	  precision. Note suffix `f' on the function names indicates
 *	  float precision.
 *
 *	- revised documentation
 *
 * 31 July 95	Mark Olesen <olesen@me.queensu.ca>
 *	- added GNU Public License
 *	- more cleanup
 *	- define SUN_BROKEN_REALLOC to use malloc() instead of realloc()
 *	  on the first pass through, apparently needed for old libc
 *	- removed #error directive in favour of some code that simply
 *	  won't compile (generate an error that way)
 *
 * 1 Aug 95	Mark Olesen <olesen@me.queensu.ca>
 *	- define FFT_RADIX4 to only have radix 2, radix 4 transforms
 *	- made fftradix /fftradixf () static scope, just use fftn()
 *	  instead.  If you have good ideas about fixing the factors
 *	  in fftn() please do so.
 *
 * ======================================================================*
 * NIST Guide to Available Math Software.
 * Source for module FFT from package GO.
 * Retrieved from NETLIB on Wed Jul  5 11:50:07 1995.
 * ======================================================================*
 *
 *-----------------------------------------------------------------------*
 *
 * int fftn (int ndim, const int dims[], REAL Re[], REAL Im[],
 *	    int iSign, double scaling);
 *
 * NDIM = the total number dimensions
 * DIMS = a vector of array sizes
 *	if NDIM is zero then DIMS must be zero-terminated
 *
 * RE and IM hold the real and imaginary components of the data, and return
 * the resulting real and imaginary Fourier coefficients.  Multidimensional
 * data *must* be allocated contiguously.  There is no limit on the number
 * of dimensions.
 *
 * ISIGN = the sign of the complex exponential (ie, forward or inverse FFT)
 *	the magnitude of ISIGN (normally 1) is used to determine the
 *	correct indexing increment (see below).
 *
 * SCALING = normalizing constant by which the final result is *divided*
 *	if SCALING == -1, normalize by total dimension of the transform
 *	if SCALING <  -1, normalize by the square-root of the total dimension
 *
 * example:
 * tri-variate transform with Re[n1][n2][n3], Im[n1][n2][n3]
 *
 *	int dims[3] = {n1,n2,n3}
 *	fftn (3, dims, Re, Im, 1, scaling);
 *
 *-----------------------------------------------------------------------*
 * int fftradix (REAL Re[], REAL Im[], size_t nTotal, size_t nPass,
 *		 size_t nSpan, int iSign, size_t max_factors,
 *		 size_t max_perm);
 *
 * RE, IM - see above documentation
 *
 * Although there is no limit on the number of dimensions, fftradix() must
 * be called once for each dimension, but the calls may be in any order.
 *
 * NTOTAL = the total number of complex data values
 * NPASS  = the dimension of the current variable
 * NSPAN/NPASS = the spacing of consecutive data values while indexing the
 *	current variable
 * ISIGN - see above documentation
 *
 * example:
 * tri-variate transform with Re[n1][n2][n3], Im[n1][n2][n3]
 *
 *	fftradix (Re, Im, n1*n2*n3, n1,       n1, 1, maxf, maxp);
 *	fftradix (Re, Im, n1*n2*n3, n2,    n1*n2, 1, maxf, maxp);
 *	fftradix (Re, Im, n1*n2*n3, n3, n1*n2*n3, 1, maxf, maxp);
 *
 * single-variate transform,
 *    NTOTAL = N = NSPAN = (number of complex data values),
 *
 *	fftradix (Re, Im, n, n, n, 1, maxf, maxp);
 *
 * The data can also be stored in a single array with alternating real and
 * imaginary parts, the magnitude of ISIGN is changed to 2 to give correct
 * indexing increment, and data [0] and data [1] used to pass the initial
 * addresses for the sequences of real and imaginary values,
 *
 * example:
 *	REAL data [2*NTOTAL];
 *	fftradix ( &data[0], &data[1], NTOTAL, nPass, nSpan, 2, maxf, maxp);
 *
 * for temporary allocation:
 *
 * MAX_FACTORS	>= the maximum prime factor of NPASS
 * MAX_PERM	>= the number of prime factors of NPASS.  In addition,
 *	if the square-free portion K of NPASS has two or more prime
 *	factors, then MAX_PERM >= (K-1)
 *
 * storage in FACTOR for a maximum of 15 prime factors of NPASS. if NPASS
 * has more than one square-free factor, the product of the square-free
 * factors must be <= 210 array storage for maximum prime factor of 23 the
 * following two constants should agree with the array dimensions.
 *
 *-----------------------------------------------------------------------*
 *
 * void fft_free (void);
 *
 * free-up allocated temporary storage after finished all the Fourier
 * transforms.
 *
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include <yarp/YARPFft.h>

// helper functions.
// this is used to convert from std image type to double precision.
void YARPFft::Convert2DoubleScaled (const unsigned char *in, double *out, int w, int h)
{
	const int size = w * h;
	for (int i = 0; i < size; i++)
	{
		*out++ = double (*in) / 255.0;
		in++;
	}
}

void YARPFft::Convert2Double (const unsigned char *in, double *out, int w,int h)
{
	const int size = w * h;
	for (int i = 0; i < size; i++)
	{
		*out++ = double (*in);
		in++;
	}
}

// 
// Swap result quadrants (low freq around the origin).
void YARPFft::Fft2DShift (const double *in, double *out, int w, int h)
{
	assert (in != out);

	double *q1 = (double *)in;
	double *q2 = (double *)in + w/2;
	double *q3 = out + h/2 * w + w/2;
	double *q4 = out + h/2 * w;
	int i = 0;

	for (i = 0; i < h/2; i++)
	{
		memcpy (q3, q1, sizeof(double) * w/2);
		memcpy (q4, q2, sizeof(double) * w/2);
		q1 += w;
		q2 += w;
		q3 += w;
		q4 += w;
	}

	q1 = out;
	q2 = out + w/2;
	q3 = (double *)in + h/2 * w + w/2;
	q4 = (double *)in + h/2 * w;
	
	
	for (i = 0; i < h/2; i++)
	{
		memcpy (q1, q3, sizeof(double) * w/2);
		memcpy (q2, q4, sizeof(double) * w/2);
		q1 += w;
		q2 += w;
		q3 += w;
		q4 += w;
	}
}

/* double precision routine */
static int
fftradix (double Re[], double Im[],
	  size_t nTotal, size_t nPass, size_t nSpan, int isign,
	  int max_factors, int max_perm);

/* parameters for memory management */

static size_t SpaceAlloced = 0;
static size_t MaxPermAlloced = 0;

/* temp space, (void *) since both float and double routines use it */
static double *Tmp0 = NULL;	/* temp space for real part */
static double *Tmp1 = NULL;	/* temp space for imaginary part */
static double *Tmp2 = NULL;	/* temp space for Cosine values */
static double *Tmp3 = NULL;	/* temp space for Sine values */
static int  *Perm = NULL;	/* Permutation vector */

#define NFACTOR	11
static int factor [NFACTOR];

void
fft_free (void)
{
   SpaceAlloced = MaxPermAlloced = 0;
   if (Tmp0 != NULL)	{ delete[] (Tmp0);	Tmp0 = NULL; }
   if (Tmp1 != NULL)	{ delete[] (Tmp1);	Tmp1 = NULL; }
   if (Tmp2 != NULL)	{ delete[] (Tmp2);	Tmp2 = NULL; }
   if (Tmp3 != NULL)	{ delete[] (Tmp3);	Tmp3 = NULL; }
   if (Perm != NULL)	{ delete[] (Perm);	Perm = NULL; }
}

void
fft_alloc (int max_factors, int max_perm)
{
   /*  allocate storage */
   if (!SpaceAlloced)
   {
		SpaceAlloced = max_factors;
		Tmp0 = new double[SpaceAlloced];
		Tmp1 = new double[SpaceAlloced];
		Tmp2 = new double[SpaceAlloced];
		Tmp3 = new double[SpaceAlloced];
   }

   if (!MaxPermAlloced)
   {
	   MaxPermAlloced = max_perm;
	   Perm = new int[max_perm];
   }
   
   assert (!(Tmp0 == NULL || 
			 Tmp1 == NULL || 
			 Tmp2 == NULL || 
			 Tmp3 == NULL || 
			 Perm == NULL));
}

#ifndef M_PI
# define M_PI	3.14159265358979323846264338327950288
#endif

#ifndef SIN60
# define SIN60	0.86602540378443865	/* sin(60 deg) */
# define COS72	0.30901699437494742	/* cos(72 deg) */
# define SIN72	0.95105651629515357	/* sin(72 deg) */
#endif

# define REAL		double
# define FFTN		fftn
# define FFTNS		"fftn"
# define FFTRADIX	fftradix
# define FFTRADIXS	"fftradix"

/*
 *
 */
int
FFTN (int ndim, const int dims[],
      REAL Re [],
      REAL Im [],
      int iSign,
      double scaling)
{
   size_t nSpan, nPass, nTotal;
   int ret, max_factors, max_perm;
   int i;

   /*
    * tally the number of elements in the data array
    * and determine the number of dimensions
    */
   nTotal = 1;
   if (ndim && dims [0])
     {
	for (i = 0; i < ndim; i++)
	  {
	     if (dims [i] <= 0)
	       {
		  fputs ("Error: " FFTNS "() - dimension error\n", stderr);
		  fft_free ();	/* free-up memory */
		  return -1;
	       }
	     nTotal *= dims [i];
	  }
     }
   else
     {
	ndim = 0;
	for (i = 0; dims [i]; i++)
	  {
	     if (dims [i] <= 0)
	       {
		  fputs ("Error: " FFTNS "() - dimension error\n", stderr);
		  fft_free ();	/* free-up memory */
		  return -1;
	       }
	     nTotal *= dims [i];
	     ndim++;
	  }
     }

   /* determine maximum number of factors and permuations */
#if 1
   /*
    * follow John Beale's example, just use the largest dimension and don't
    * worry about excess allocation.  May be someone else will do it?
    */
   max_factors = max_perm = 1;
   for (i = 0; i < ndim; i++)
     {
	nSpan = dims [i];
	if (nSpan > (unsigned int)max_factors) max_factors = nSpan;
	if (nSpan > (unsigned int)max_perm) max_perm = nSpan;
     }
#else
   /* use the constants used in the original Fortran code */
   max_factors = 23;
   max_perm = 209;
#endif

   /* loop over the dimensions: */
   nPass = 1;
   for (i = 0; i < ndim; i++)
     {
	nSpan = dims [i];
	nPass *= nSpan;
	ret = FFTRADIX (Re, Im, nTotal, nSpan, nPass, iSign,
			max_factors, max_perm);
	/* exit, clean-up already done */
	if (ret)
	  return ret;
     }

   /* Divide through by the normalizing constant: */
   if (scaling && scaling != 1.0)
     {
	if (iSign < 0) iSign = -iSign;
	if (scaling < 0.0)
	  {
	     scaling = nTotal;
	     if (scaling < -1.0)
	       scaling = sqrt (scaling);
	  }
	scaling = 1.0 / scaling;	/* multiply is often faster */
	for (i = 0; i < (int)nTotal; i += iSign)
	  {
	     Re [i] *= scaling;
	     Im [i] *= scaling;
	  }
     }
   return 0;
}

/*
 * singleton's mixed radix routine
 *
 * could move allocation out to fftn(), but leave it here so that it's
 * possible to make this a standalone function
 */
static int
FFTRADIX (REAL Re[],
	  REAL Im[],
	  size_t nTotal,
	  size_t nPass,
	  size_t nSpan,
	  int iSign,
	  int max_factors,
	  int max_perm)
{
   int ii, mfactor, kspan, ispan, inc;
   int j, jc, jf, jj, k, k1, k2, k3, k4, kk, kt, nn, ns, nt;

   REAL radf;
   REAL c1, c2, c3, cd, aa, aj, ak, ajm, ajp, akm, akp;
   REAL s1, s2, s3, sd, bb, bj, bk, bjm, bjp, bkm, bkp;

   REAL *Rtmp = NULL;	/* temp space for real part*/
   REAL *Itmp = NULL;	/* temp space for imaginary part */
   REAL *Cos = NULL;	/* Cosine values */
   REAL *Sin = NULL;	/* Sine values */

   REAL s60 = SIN60;		/* sin(60 deg) */
   REAL c72 = COS72;		/* cos(72 deg) */
   REAL s72 = SIN72;		/* sin(72 deg) */
   REAL pi2 = M_PI;		/* use PI first, 2 PI later */

   /* gcc complains about k3 being uninitialized, but I can't find out where
    * or why ... it looks okay to me.
    *
    * initialize to make gcc happy
    */
   k3 = 0;

   /* gcc complains about c2, c3, s2,s3 being uninitialized, but they're
    * only used for the radix 4 case and only AFTER the (s1 == 0.0) pass
    * through the loop at which point they will have been calculated.
    *
    * initialize to make gcc happy
    */
   c2 = c3 = s2 = s3 = 0.0;

   /* Parameter adjustments, was fortran so fix zero-offset */
   Re--;
   Im--;

   if (nPass < 2)
     return 0;

   /* assign pointers */
   Rtmp = (REAL *) Tmp0;
   Itmp = (REAL *) Tmp1;
   Cos  = (REAL *) Tmp2;
   Sin  = (REAL *) Tmp3;

   /*
    * Function Body
    */
   inc = iSign;
   if (iSign < 0) {
      s72 = -s72;
      s60 = -s60;
      pi2 = -pi2;
      inc = -inc;		/* absolute value */
   }

   /* adjust for strange increments */
   nt = inc * nTotal;
   ns = inc * nSpan;
   kspan = ns;

   nn = nt - inc;
   jc = ns / nPass;
   radf = pi2 * (double) jc;
   pi2 *= 2.0;			/* use 2 PI from here on */

   ii = 0;
   jf = 0;
   /*  determine the factors of n */
   mfactor = 0;
   k = nPass;
   while (k % 16 == 0) {
      mfactor++;
      factor [mfactor - 1] = 4;
      k /= 16;
   }
   j = 3;
   jj = 9;
   do {
      while (k % jj == 0) {
	 mfactor++;
	 factor [mfactor - 1] = j;
	 k /= jj;
      }
      j += 2;
      jj = j * j;
   } while (jj <= k);
   if (k <= 4) {
      kt = mfactor;
      factor [mfactor] = k;
      if (k != 1)
	mfactor++;
   } else {
      if (k - (k / 4 << 2) == 0) {
	 mfactor++;
	 factor [mfactor - 1] = 2;
	 k /= 4;
      }
      kt = mfactor;
      j = 2;
      do {
	 if (k % j == 0) {
	    mfactor++;
	    factor [mfactor - 1] = j;
	    k /= j;
	 }
	 j = ((j + 1) / 2 << 1) + 1;
      } while (j <= k);
   }
   if (kt) {
      j = kt;
      do {
	 mfactor++;
	 factor [mfactor - 1] = factor [j - 1];
	 j--;
      } while (j);
   }

   /* test that mfactors is in range */
   if (mfactor > NFACTOR)
     {
	fputs ("Error: " FFTRADIXS "() - exceeded number of factors\n", stderr);
	goto Memory_Error_Label;
      }

   /* compute fourier transform */
   for (;;) {
      sd = radf / (double) kspan;
      cd = sin(sd);
      cd = 2.0 * cd * cd;
      sd = sin(sd + sd);
      kk = 1;
      ii++;

      switch (factor [ii - 1]) {
       case 2:
	 /* transform for factor of 2 (including rotation factor) */
	 kspan /= 2;
	 k1 = kspan + 2;
	 do {
	    do {
	       k2 = kk + kspan;
	       ak = Re [k2];
	       bk = Im [k2];
	       Re [k2] = Re [kk] - ak;
	       Im [k2] = Im [kk] - bk;
	       Re [kk] += ak;
	       Im [kk] += bk;
	       kk = k2 + kspan;
	    } while (kk <= nn);
	    kk -= nn;
	 } while (kk <= jc);
	 if (kk > kspan)
	   goto Permute_Results_Label;		/* exit infinite loop */
	 do {
	    c1 = 1.0 - cd;
	    s1 = sd;
	    do {
	       do {
		  do {
		     k2 = kk + kspan;
		     ak = Re [kk] - Re [k2];
		     bk = Im [kk] - Im [k2];
		     Re [kk] += Re [k2];
		     Im [kk] += Im [k2];
		     Re [k2] = c1 * ak - s1 * bk;
		     Im [k2] = s1 * ak + c1 * bk;
		     kk = k2 + kspan;
		  } while (kk < nt);
		  k2 = kk - nt;
		  c1 = -c1;
		  kk = k1 - k2;
	       } while (kk > k2);
	       ak = c1 - (cd * c1 + sd * s1);
	       s1 = sd * c1 - cd * s1 + s1;
	       c1 = 2.0 - (ak * ak + s1 * s1);
	       s1 *= c1;
	       c1 *= ak;
	       kk += jc;
	    } while (kk < k2);
	    k1 += inc + inc;
	    kk = (k1 - kspan) / 2 + jc;
	 } while (kk <= jc + jc);
	 break;

       case 4:			/* transform for factor of 4 */
	 ispan = kspan;
	 kspan /= 4;

	 do {
	    c1 = 1.0;
	    s1 = 0.0;
	    do {
	       do {
		  k1 = kk + kspan;
		  k2 = k1 + kspan;
		  k3 = k2 + kspan;
		  akp = Re [kk] + Re [k2];
		  akm = Re [kk] - Re [k2];
		  ajp = Re [k1] + Re [k3];
		  ajm = Re [k1] - Re [k3];
		  bkp = Im [kk] + Im [k2];
		  bkm = Im [kk] - Im [k2];
		  bjp = Im [k1] + Im [k3];
		  bjm = Im [k1] - Im [k3];
		  Re [kk] = akp + ajp;
		  Im [kk] = bkp + bjp;
		  ajp = akp - ajp;
		  bjp = bkp - bjp;
		  if (iSign < 0) {
		     akp = akm + bjm;
		     bkp = bkm - ajm;
		     akm -= bjm;
		     bkm += ajm;
		  } else {
		     akp = akm - bjm;
		     bkp = bkm + ajm;
		     akm += bjm;
		     bkm -= ajm;
		  }
		  /* avoid useless multiplies */
		  if (s1 == 0.0) {
		     Re [k1] = akp;
		     Re [k2] = ajp;
		     Re [k3] = akm;
		     Im [k1] = bkp;
		     Im [k2] = bjp;
		     Im [k3] = bkm;
		  } else {
		     Re [k1] = akp * c1 - bkp * s1;
		     Re [k2] = ajp * c2 - bjp * s2;
		     Re [k3] = akm * c3 - bkm * s3;
		     Im [k1] = akp * s1 + bkp * c1;
		     Im [k2] = ajp * s2 + bjp * c2;
		     Im [k3] = akm * s3 + bkm * c3;
		  }
		  kk = k3 + kspan;
	       } while (kk <= nt);

	       c2 = c1 - (cd * c1 + sd * s1);
	       s1 = sd * c1 - cd * s1 + s1;
	       c1 = 2.0 - (c2 * c2 + s1 * s1);
	       s1 *= c1;
	       c1 *= c2;
	       /* values of c2, c3, s2, s3 that will get used next time */
	       c2 = c1 * c1 - s1 * s1;
	       s2 = 2.0 * c1 * s1;
	       c3 = c2 * c1 - s2 * s1;
	       s3 = c2 * s1 + s2 * c1;
	       kk = kk - nt + jc;
	    } while (kk <= kspan);
	    kk = kk - kspan + inc;
	 } while (kk <= jc);
	 if (kspan == jc)
	   goto Permute_Results_Label;		/* exit infinite loop */
	 break;

       default:
	 /*  transform for odd factors */
#ifdef FFT_RADIX4
	 fputs ("Error: " FFTRADIXS "(): compiled for radix 2/4 only\n", stderr);
	 fft_free ();		/* free-up memory */
	 return -1;
	 break;
#else	/* FFT_RADIX4 */
	 k = factor [ii - 1];
	 ispan = kspan;
	 kspan /= k;

	 switch (k) {
	  case 3:	/* transform for factor of 3 (optional code) */
	    do {
	       do {
		  k1 = kk + kspan;
		  k2 = k1 + kspan;
		  ak = Re [kk];
		  bk = Im [kk];
		  aj = Re [k1] + Re [k2];
		  bj = Im [k1] + Im [k2];
		  Re [kk] = ak + aj;
		  Im [kk] = bk + bj;
		  ak -= 0.5 * aj;
		  bk -= 0.5 * bj;
		  aj = (Re [k1] - Re [k2]) * s60;
		  bj = (Im [k1] - Im [k2]) * s60;
		  Re [k1] = ak - bj;
		  Re [k2] = ak + bj;
		  Im [k1] = bk + aj;
		  Im [k2] = bk - aj;
		  kk = k2 + kspan;
	       } while (kk < nn);
	       kk -= nn;
	    } while (kk <= kspan);
	    break;

	  case 5:	/*  transform for factor of 5 (optional code) */
	    c2 = c72 * c72 - s72 * s72;
	    s2 = 2.0 * c72 * s72;
	    do {
	       do {
		  k1 = kk + kspan;
		  k2 = k1 + kspan;
		  k3 = k2 + kspan;
		  k4 = k3 + kspan;
		  akp = Re [k1] + Re [k4];
		  akm = Re [k1] - Re [k4];
		  bkp = Im [k1] + Im [k4];
		  bkm = Im [k1] - Im [k4];
		  ajp = Re [k2] + Re [k3];
		  ajm = Re [k2] - Re [k3];
		  bjp = Im [k2] + Im [k3];
		  bjm = Im [k2] - Im [k3];
		  aa = Re [kk];
		  bb = Im [kk];
		  Re [kk] = aa + akp + ajp;
		  Im [kk] = bb + bkp + bjp;
		  ak = akp * c72 + ajp * c2 + aa;
		  bk = bkp * c72 + bjp * c2 + bb;
		  aj = akm * s72 + ajm * s2;
		  bj = bkm * s72 + bjm * s2;
		  Re [k1] = ak - bj;
		  Re [k4] = ak + bj;
		  Im [k1] = bk + aj;
		  Im [k4] = bk - aj;
		  ak = akp * c2 + ajp * c72 + aa;
		  bk = bkp * c2 + bjp * c72 + bb;
		  aj = akm * s2 - ajm * s72;
		  bj = bkm * s2 - bjm * s72;
		  Re [k2] = ak - bj;
		  Re [k3] = ak + bj;
		  Im [k2] = bk + aj;
		  Im [k3] = bk - aj;
		  kk = k4 + kspan;
	       } while (kk < nn);
	       kk -= nn;
	    } while (kk <= kspan);
	    break;

	  default:
	    if (k != jf) {
	       jf = k;
	       s1 = pi2 / (double) k;
	       c1 = cos(s1);
	       s1 = sin(s1);
	       if (jf > max_factors)
		 goto Memory_Error_Label;
	       Cos [jf - 1] = 1.0;
	       Sin [jf - 1] = 0.0;
	       j = 1;
	       do {
		  Cos [j - 1] = Cos [k - 1] * c1 + Sin [k - 1] * s1;
		  Sin [j - 1] = Cos [k - 1] * s1 - Sin [k - 1] * c1;
		  k--;
		  Cos [k - 1] = Cos [j - 1];
		  Sin [k - 1] = -Sin [j - 1];
		  j++;
	       } while (j < k);
	    }
	    do {
	       do {
		  k1 = kk;
		  k2 = kk + ispan;
		  ak = aa = Re [kk];
		  bk = bb = Im [kk];
		  j = 1;
		  k1 += kspan;
		  do {
		     k2 -= kspan;
		     j++;
		     Rtmp [j - 1] = Re [k1] + Re [k2];
		     ak += Rtmp [j - 1];
		     Itmp [j - 1] = Im [k1] + Im [k2];
		     bk += Itmp [j - 1];
		     j++;
		     Rtmp [j - 1] = Re [k1] - Re [k2];
		     Itmp [j - 1] = Im [k1] - Im [k2];
		     k1 += kspan;
		  } while (k1 < k2);
		  Re [kk] = ak;
		  Im [kk] = bk;
		  k1 = kk;
		  k2 = kk + ispan;
		  j = 1;
		  do {
		     k1 += kspan;
		     k2 -= kspan;
		     jj = j;
		     ak = aa;
		     bk = bb;
		     aj = 0.0;
		     bj = 0.0;
		     k = 1;
		     do {
			k++;
			ak += Rtmp [k - 1] * Cos [jj - 1];
			bk += Itmp [k - 1] * Cos [jj - 1];
			k++;
			aj += Rtmp [k - 1] * Sin [jj - 1];
			bj += Itmp [k - 1] * Sin [jj - 1];
			jj += j;
			if (jj > jf) {
			   jj -= jf;
			}
		     } while (k < jf);
		     k = jf - j;
		     Re [k1] = ak - bj;
		     Im [k1] = bk + aj;
		     Re [k2] = ak + bj;
		     Im [k2] = bk - aj;
		     j++;
		  } while (j < k);
		  kk += ispan;
	       } while (kk <= nn);
	       kk -= nn;
	    } while (kk <= kspan);
	    break;
	 }
	 /*  multiply by rotation factor (except for factors of 2 and 4) */
	 if (ii == mfactor)
	   goto Permute_Results_Label;		/* exit infinite loop */
	 kk = jc + 1;
	 do {
	    c2 = 1.0 - cd;
	    s1 = sd;
	    do {
	       c1 = c2;
	       s2 = s1;
	       kk += kspan;
	       do {
		  do {
		     ak = Re [kk];
		     Re [kk] = c2 * ak - s2 * Im [kk];
		     Im [kk] = s2 * ak + c2 * Im [kk];
		     kk += ispan;
		  } while (kk <= nt);
		  ak = s1 * s2;
		  s2 = s1 * c2 + c1 * s2;
		  c2 = c1 * c2 - ak;
		  kk = kk - nt + kspan;
	       } while (kk <= ispan);
	       c2 = c1 - (cd * c1 + sd * s1);
	       s1 += sd * c1 - cd * s1;
	       c1 = 2.0 - (c2 * c2 + s1 * s1);
	       s1 *= c1;
	       c2 *= c1;
	       kk = kk - ispan + jc;
	    } while (kk <= kspan);
	    kk = kk - kspan + jc + inc;
	 } while (kk <= jc + jc);
	 break;
#endif	/* FFT_RADIX4 */
      }
   }

/*  permute the results to normal order---done in two stages */
/*  permutation for square factors of n */
Permute_Results_Label:
   Perm [0] = ns;
   if (kt) {
      k = kt + kt + 1;
      if (mfactor < k)
	k--;
      j = 1;
      Perm [k] = jc;
      do {
	 Perm [j] = Perm [j - 1] / factor [j - 1];
	 Perm [k - 1] = Perm [k] * factor [j - 1];
	 j++;
	 k--;
      } while (j < k);
      k3 = Perm [k];
      kspan = Perm [1];
      kk = jc + 1;
      k2 = kspan + 1;
      j = 1;
      if (nPass != nTotal) {
/*  permutation for multivariate transform */
Permute_Multi_Label:
	 do {
	    do {
	       k = kk + jc;
	       do {
		  /* swap Re [kk] <> Re [k2], Im [kk] <> Im [k2] */
		  ak = Re [kk]; Re [kk] = Re [k2]; Re [k2] = ak;
		  bk = Im [kk]; Im [kk] = Im [k2]; Im [k2] = bk;
		  kk += inc;
		  k2 += inc;
	       } while (kk < k);
	       kk += ns - jc;
	       k2 += ns - jc;
	    } while (kk < nt);
	    k2 = k2 - nt + kspan;
	    kk = kk - nt + jc;
	 } while (k2 < ns);
	 do {
	    do {
	       k2 -= Perm [j - 1];
	       j++;
	       k2 = Perm [j] + k2;
	    } while (k2 > Perm [j - 1]);
	    j = 1;
	    do {
	       if (kk < k2)
		 goto Permute_Multi_Label;
	       kk += jc;
	       k2 += kspan;
	    } while (k2 < ns);
	 } while (kk < ns);
      } else {
/*  permutation for single-variate transform (optional code) */
Permute_Single_Label:
	 do {
	    /* swap Re [kk] <> Re [k2], Im [kk] <> Im [k2] */
	    ak = Re [kk]; Re [kk] = Re [k2]; Re [k2] = ak;
	    bk = Im [kk]; Im [kk] = Im [k2]; Im [k2] = bk;
	    kk += inc;
	    k2 += kspan;
	 } while (k2 < ns);
	 do {
	    do {
	       k2 -= Perm [j - 1];
	       j++;
	       k2 = Perm [j] + k2;
	    } while (k2 > Perm [j - 1]);
	    j = 1;
	    do {
	       if (kk < k2)
		 goto Permute_Single_Label;
	       kk += inc;
	       k2 += kspan;
	    } while (k2 < ns);
	 } while (kk < ns);
      }
      jc = k3;
   }

   if ((kt << 1) + 1 >= mfactor)
     return 0;
   ispan = Perm [kt];
   /* permutation for square-free factors of n */
   j = mfactor - kt;
   factor [j] = 1;
   do {
      factor [j - 1] *= factor [j];
      j--;
   } while (j != kt);
   kt++;
   nn = factor [kt - 1] - 1;
   if (nn > max_perm)
     goto Memory_Error_Label;
   j = jj = 0;
   for (;;) {
      k = kt + 1;
      k2 = factor [kt - 1];
      kk = factor [k - 1];
      j++;
      if (j > nn)
	break;				/* exit infinite loop */
      jj += kk;
      while (jj >= k2) {
	 jj -= k2;
	 k2 = kk;
	 k++;
	 kk = factor [k - 1];
	 jj += kk;
      }
      Perm [j - 1] = jj;
   }
   /*  determine the permutation cycles of length greater than 1 */
   j = 0;
   for (;;) {
      do {
	 j++;
	 kk = Perm [j - 1];
      } while (kk < 0);
      if (kk != j) {
	 do {
	    k = kk;
	    kk = Perm [k - 1];
	    Perm [k - 1] = -kk;
	 } while (kk != j);
	 k3 = kk;
      } else {
	 Perm [j - 1] = -j;
	 if (j == nn)
	   break;		/* exit infinite loop */
      }
   }
   max_factors *= inc;
   /*  reorder a and b, following the permutation cycles */
   for (;;) {
      j = k3 + 1;
      nt -= ispan;
      ii = nt - inc + 1;
      if (nt < 0)
	break;			/* exit infinite loop */
      do {
	 do {
	    j--;
	 } while (Perm [j - 1] < 0);
	 jj = jc;
	 do {
	    kspan = jj;
	    if (jj > max_factors) {
	       kspan = max_factors;
	    }
	    jj -= kspan;
	    k = Perm [j - 1];
	    kk = jc * k + ii + jj;
	    k1 = kk + kspan;
	    k2 = 0;
	    do {
	       k2++;
	       Rtmp [k2 - 1] = Re [k1];
	       Itmp [k2 - 1] = Im [k1];
	       k1 -= inc;
	    } while (k1 != kk);
	    do {
	       k1 = kk + kspan;
	       k2 = k1 - jc * (k + Perm [k - 1]);
	       k = -Perm [k - 1];
	       do {
		  Re [k1] = Re [k2];
		  Im [k1] = Im [k2];
		  k1 -= inc;
		  k2 -= inc;
	       } while (k1 != kk);
	       kk = k2;
	    } while (k != j);
	    k1 = kk + kspan;
	    k2 = 0;
	    do {
	       k2++;
	       Re [k1] = Rtmp [k2 - 1];
	       Im [k1] = Itmp [k2 - 1];
	       k1 -= inc;
	    } while (k1 != kk);
	 } while (jj);
      } while (j != 1);
   }
   return 0;			/* exit point here */

   /* alloc or other problem, do some clean-up */
Memory_Error_Label:
   fputs ("Error: " FFTRADIXS "() - insufficient memory.\n", stderr);
   fft_free ();			/* free-up memory */
   return -1;
}
