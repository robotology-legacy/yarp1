extern "C"
{
	#include "random.h"
	#include "Mm.h"
}
#include "mbp.h"
#include "math.h"
#include <stdio.h>

/****************************************************************************/
/*                         RandomWeights()                                  */
/****************************************************************************/

void RandomWeights (REAL *Matrix, int nRows, int nCols, REAL Range)
{
	int  i,j;
 
	for (i=0; i LT nCols; i++)
	{
		for (j=0; j LT nRows; j++)
		{
			Matrix[i+j*nCols] = Range*(REAL)(1.0-2.0*RandomNumber());
		}
	}
}

/****************************************************************************/
/*                           FeedForward()                                  */
/****************************************************************************/

void FeedForward (REAL *NewStatus, int nRowsNS, int nColsNS,
				  REAL *OldStatus, int nRowsOS, int nColsOS,
				  REAL *Weight,    int nRowsW,  int nColsW,
				  REAL *Bias,      int nElem)

{
	int i,j;

	/* Keep the compiler happy */

//	IGNORE(nRowsW);
//	IGNORE(nRowsOS);
//	IGNORE(nElem);

	/* Fast matrix multiplication routine to compute neurons' net input */

	MM2x2PB (NewStatus, OldStatus, Weight, nRowsNS, nColsNS, nColsOS,
			 nColsOS,   nColsW, BLOCK_DIMENSION);

	/* Compute neurons' output */
	
	for (i=0; i<nRowsNS; i++)
	{
		for (j=0; j<nColsNS; j++)
		{
			NewStatus[i*nColsNS+j] = f(NewStatus[i*nColsNS+j]+Bias[j]);
		}
	}
}

/****************************************************************************/
/*                              f()                                         */
/****************************************************************************/

REAL f (REAL x)
{
	return (REAL)tanh((double)x);     /* Tanh */
 
	/* IF YOU WANT TO USE THE APPROXIMATION USE THE FOLLOWING CODE INSTEAD */
	/*
	REAL Lambda = 0.26037;
	REAL Xs     = 1.92033;
	REAL Ys     = 0.96016;

	if ( x GT Xs )
		return Ys;
	else if ( x LT -Xs )
		return -Ys;
	else if ( x GE 0.0 )
		return Ys-Lambda*(x-Xs)*(x-Xs);
	else
		return Lambda*(x+Xs)*(x+Xs)-Ys;
	*/

}

/****************************************************************************/
/*                              df()                                        */
/****************************************************************************/

REAL df (REAL x)
{
	return (REAL)(1.0-x*x);      /* Tanh' */
}

/****************************************************************************/
/*                           EBPLastLayer()                                 */
/****************************************************************************/

void EBPLastLayer (REAL *Delta,      int nRowsD, int nColsD,
		   REAL *Target,     int nRowsT, int nColsT,
		   REAL *Status,     int nRowsS, int nColsS)
{
	int  i;

	/* Keep the compiler happy */

//	IGNORE(nRowsT);
//	IGNORE(nColsT);
//	IGNORE(nRowsS);
//	IGNORE(nColsS);

	/* Compute deltas */
	
	for (i=0; i LT nRowsD*nColsD; i++)
	{
		Delta[i] = (REAL)2.0/((REAL)(nRowsD*nColsD))*df(Status[i])*(Target[i]-Status[i]);
	}
}

/****************************************************************************/
/*                           EBP()                                          */
/****************************************************************************/

void EBP (REAL *NewDelta,   int nRowsND, int nColsND,
		  REAL *OldDelta,   int nRowsOD, int nColsOD,
		  REAL *Weight,     int nRowsW,  int nColsW,
		  REAL *Status,     int nRowsS,  int nColsS)
{
	int  i;

	/* Keep the compiler happy */

//	IGNORE(nRowsS);
//	IGNORE(nColsS);
//	IGNORE(nRowsW);
//	IGNORE(nRowsOD);

	/* Compute deltas of hidden neurons */

	MMT2x2P ( NewDelta, OldDelta, Weight, nRowsND, nColsND, nColsOD, nColsOD,
			  nColsW);

	for (i=0; i LT nRowsND*nColsND; i++)
	{
		NewDelta[i] *= df(Status[i]);
	}
}

/****************************************************************************/
/*                           Step()                                         */
/****************************************************************************/

void Step (REAL *DeltaWeight, int nRowsDW, int nColsDW,
		   REAL *Status,      int nRowsS,  int nColsS,
		   REAL *Delta,       int nRowsD,  int nColsD,
		   REAL *DeltaBias,   int nElemsDB)
{
	int i,j;

	/* Keep the compiler happy */

//	IGNORE(nElemsDB);

	/* Compute the gradient */

	MTM2x2PB(DeltaWeight, Status, Delta, nRowsDW, nColsDW, nRowsS, nColsS, 
			 nColsD, BLOCK_DIMENSION);

	for (i=0; i<nColsD; i++)
	{
		DeltaBias[i] = 0.0;
		for (j=0; j<nRowsD; j++)
		{
			DeltaBias[i] += Delta[j*nColsD+i];
		}
	}
}

/****************************************************************************/
/*                           ComputeDigitalCost()                           */
/****************************************************************************/

REAL ComputeDigitalCost (REAL *Target, int nRowsT, int nColsT,
						 REAL *Status, int nRowsS, int nColsS)
{
	int  i;
	REAL result = 0.0;

	/* Keep the compiler happy */
	
//	IGNORE(nRowsS);
//	IGNORE(nColsS);

	/* Compute the % of wrong answers of the net */

	for ( i=0; i LT nRowsT*nColsT; i++ )
	{
		if (Target[i]*Status[i] LE 0.0)
		{
			result += (REAL)100.0/(REAL)(nRowsT*nColsT);
		}
	}

	return result;
 
}

/****************************************************************************/
/*                           ComputeAnalogCost()                            */
/****************************************************************************/

REAL ComputeAnalogCost  (REAL *Target, int nRowsT, int nColsT,
						 REAL *Status, int nRowsS, int nColsS)
{
	int  i;
	REAL result = 0.0;

	/* Keep the compiler happy */

//	IGNORE(nRowsS);
//	IGNORE(nColsS);

	/* Compute the quadratic error of the net */

	for ( i=0; i LT nRowsT*nColsT; i++)
	{
		result += (Target[i]-Status[i])*(Target[i]-Status[i]);
	}
	result /= (REAL) (nRowsT*nColsT);

	return result;
}

/****************************************************************************/
/*                           ComputeMaximumCost()                           */
/****************************************************************************/

REAL ComputeMaximumCost (REAL *Target, int nRowsT, int nColsT,
						 REAL *Status, int nRowsS, int nColsS)
{
	int  i;
	REAL result = 0.0;
	REAL diff;

	/* Keep the compiler happy */

//	IGNORE(nRowsS);
//	IGNORE(nColsS);

	/* Compute the maximum abs error of the net */

	for ( i=0; i LT nRowsT*nColsT; i++)
	{
		diff = Target[i] - Status[i];
		if (diff LT 0.0)
		{
			diff = -diff;
		}
		result = (result GT diff) ? result : diff;
	}
	return result;
}

/****************************************************************************/
/*                           ComputeGradientNorm()                          */
/****************************************************************************/

REAL ComputeGradientNorm (REAL **DeltaWeight, REAL **DeltaBias,
						  int   *nUnit,       int    nLayer)
{
	int  i,j;
	REAL result = 0.0;
	REAL *pDeltaWeight;   /* Some pointers to speed things up */
	REAL *pDeltaBias;

	for ( i=1; i LE nLayer; i++ )
	{
		pDeltaWeight = DeltaWeight[i];
		pDeltaBias   = DeltaBias[i];
  
		for ( j=0; j LT nUnit[i]*nUnit[i-1]; j++)
		{
			result += (pDeltaWeight[j])*(pDeltaWeight[j]);
		}
		for ( j=0; j LT nUnit[i]; j++)
		{
			result += (pDeltaBias[j])*(pDeltaBias[j]);
		}
	}
	
	result = (REAL) sqrt((double)result);

	return result;

}
 
/****************************************************************************/
/*                             PrintStep()                                  */
/****************************************************************************/

void PrintStep (char *s, long nIter, REAL DigCost, 
				REAL AnaCost, REAL MaxCost, REAL GradientNorm)
{
	printf("%s %5ld Grad= %.4E AnaCost= %.6lf MaxCost= %.6lf DigCost= %6.2lf\n",
			s, nIter,
			(double) GradientNorm,
			(double) AnaCost,
			(double) MaxCost,
			(double) DigCost);
 
}

/****************************************************************************/
/*                             OutTime()                                    */
/****************************************************************************/

void OutTime (long nIter, int nLayer, int nPattern, int *nUnit, double Time)
{
	double    nC=0;
	int    i;

	for (i=1; i<= nLayer; i++)
	{
		nC += (double) (nUnit[i-1]+1) * nUnit[i];
	}

	nC *= (double) nIter*nPattern;

	if (Time GT 0.0)
	{
		printf ("\n\n* MCUPS = %6.4lf\n",nC/(1.0e6*Time));
	}
	else
	{
		printf ("\n\n* MCUPS = <not measurable>\n");
	}
}

/****************************************************************************/
/*                           BackStep()                                     */
/****************************************************************************/

void BackStep (REAL *DeltaWeight,    int nRowsDW,  int nColsDW,
			   REAL *DeltaBias,      int nElemsDB,
			   REAL *OldDeltaWeight, int nRowsODW, int nColsODW,
			   REAL *OldDeltaBias,   int nElemsODB,
			   REAL *Weight,         int nRowsW,   int nColsW,
			   REAL *Bias,           int nElemsB,
			   REAL *StepWeight,     int nRowsSW,  int nColsSW,
			   REAL *StepBias,       int nElemsSB)
{

	int i;

	/* Keep the compiler happy */

/*	IGNORE(nRowsODW);
	IGNORE(nColsODW);
	IGNORE(nElemsODB);
	IGNORE(nRowsW);
	IGNORE(nColsW);
	IGNORE(nElemsB);
	IGNORE(nRowsSW);
	IGNORE(nColsSW);
	IGNORE(nElemsSB);*/

	/* Makes a learning step back */

	for ( i=0; i LT nRowsDW*nColsDW; i++)
	{
		DeltaWeight[i] =  OldDeltaWeight[i];
		Weight[i]      -= StepWeight[i];
	}

	for ( i=0; i LT nElemsDB; i++)
	{
		DeltaBias[i] =  OldDeltaBias[i];
		Bias[i]      -= StepBias[i];
	}

}

/****************************************************************************/
/*                           UpdateWeights()                                */
/****************************************************************************/

void UpdateWeights (REAL *StepWeight,     int nRowsSW,  int nColsSW,
					REAL *StepBias,       int nElemsSB,
					REAL Eta, REAL Alpha,
					REAL *DeltaWeight,    int nRowsDW,  int nColsDW,
					REAL *DeltaBias,      int nRowsDB,
					REAL *OldDeltaWeight, int nRowsODW, int nColsODW,
					REAL *OldDeltaBias,   int nElemsODB,
					REAL *Weight,         int nRowsW,   int nColsW,
					REAL *Bias,           int nElemsW )
{

	int i;

	/* Keep the compiler happy */

/*	IGNORE(nRowsDW);
	IGNORE(nColsDW);
	IGNORE(nRowsDB);
	IGNORE(nRowsODW);
	IGNORE(nColsODW);
	IGNORE(nElemsODB);
	IGNORE(nRowsW);
	IGNORE(nColsW);
	IGNORE(nElemsW);*/

	/* A step in the right direction (hopefully) */
 
	for ( i=0; i LT nRowsSW*nColsSW; i++ )
	{
		OldDeltaWeight[i] = DeltaWeight[i];
		StepWeight[i]     = Eta*DeltaWeight[i] + Alpha*StepWeight[i];
		Weight[i]        += StepWeight[i];
	}
 
	for ( i=0; i LT nElemsSB; i++ )
	{
		OldDeltaBias[i] = DeltaBias[i];
		StepBias[i]     = Eta*DeltaBias[i] + Alpha*StepBias[i];
		Bias[i]        += StepBias[i];
	}
}

