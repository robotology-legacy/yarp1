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

#include "VisDMatrix.h"

#ifdef WIN32

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif	// WIN32

#ifdef __QNX__
// QNX specific stuff.
#endif

//
//
//
#define TINY 1.0e-20;

void LU (CVisDMatrix& a, CVisDVector& indx, double& d)
{
    int n = a.NRows ();
        
	int i, imax, j, k;
	double big, dum, sum, temp;

	CVisDVector vv(n);
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
void LuSolve(CVisDMatrix& a, CVisDVector& indx, CVisDVector& b)
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
