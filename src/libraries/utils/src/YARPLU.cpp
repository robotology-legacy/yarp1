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
/// $Id: YARPLU.cpp,v 1.1 2003-11-10 19:08:57 babybot Exp $
///
///

////////////////////////////////////////////////////////////////////////////
//
// NAME
//  PasaLU.cpp -- double precision matrix/vector operations
//
// DESCRIPTION
//	Modified and imported from numerical recipes. This is the LU
//	decomposition and LS solution.
//
///////////////////////////////////////////////////////////////////////////

#include "YARPMatrix.h"

#ifdef WIN32

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif	// WIN32

#ifdef __QNX4__
// QNX specific stuff.
#endif

//
//
//
#define TINY 1.0e-20;

void LU (YMatrix& a, YVector& indx, double& d)
{
    int n = a.NRows ();
        
	int i, imax, j, k;
	double big, dum, sum, temp;

	YVector vv(n);
	d = 1.0;
	
    for (i = 1; i <= n; i++) 
    {
	    big = 0.0;
	    for (j = 1; j <= n; j++)
		if ((temp = fabs (a (i, j))) > big) big = temp;

		assert(big != 0.0);

		vv (i) = 1.0 / big;
	}
        
	for (j = 1; j <= n; j++) 
    {
	    for (i = 1; i < j; i++) 
	    {
			sum = a (i, j);
			for (k = 1; k < i; k++) sum -= a (i, k) * a (k, j);
			a (i, j) = sum;
		}
	    big = 0.0;
	    for (i = j; i <= n; i++) 
        {
			sum = a (i, j);
			for (k = 1; k < j; k++)
				sum -= a (i, k) * a (k, j);
			a (i, j) = sum;
			if ((dum = vv (i) * (fabs (sum))) >= big) 
            {
				big = dum;
				imax=i;
			}
	    }
	    if (j != imax) 
        {
			for (k = 1; k <= n; k++) 
            {
				dum = a (imax, k);
				a (imax, k) = a (j, k);
				a (j, k) = dum;
			}
			d = -d;
			vv (imax) = vv (j);
	    }
	    indx (j) = double (imax);
	    if (a (j, j) == 0.0) a (j, j) = TINY;
	    if (j != n) 
        {
			dum = 1.0 / (a (j, j));
			for (i = j + 1; i <= n; i++) a (i, j) *= dum;
	    }
	}
}

#undef TINY

//
// backsubstitute.
//
void LuSolve(YMatrix& a, YVector& indx, YVector& b)
{
    int n = a.NRows ();
         
	int i, ii = 0, ip, j;
	double sum;

	for (i = 1; i <= n; i++) 
    {
	    ip = int (indx (i));
	    sum = b (ip);
	    b (ip) = b (i);
	    if (ii)
			for (j = ii; j <= i - 1; j++) sum -= a (i, j) * b(j);
	    else 
            if (sum) ii = i;
	     
        b (i) = sum;
	}
        
	for (i = n; i >= 1; i--) 
    {
	    sum = b (i);
	    for (j = i + 1; j <= n; j++) sum -= a (i, j) * b (j);
	     
        b (i) = sum / a (i, i);
	}
}
