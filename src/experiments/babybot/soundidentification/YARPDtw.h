// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2004 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPDtw.h
// 
//     Description:  
// 
//         Version:  $Id: YARPDtw.h,v 1.1 2004-07-30 17:11:19 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================
#ifndef __YARPDtwh__
#define __YARPDtwh__

#ifndef	HUGE
	#define	HUGE	1e30
#endif

#include <conf/YARPConfig.h>
#include <YARPConfigFile.h>
#include <YARPMatrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

// =====================================================================================
//        Class:  YARPDtw
// 
//  Description:  Dynamic Time Warping class. This file implements
//	   			  the class definition for the YARPDtw.
//	   			  It calculates the optimal minimal distance between two sets of data
//	   			  Adapted from the code made public by Tony Robinson in comp.speech
//	   			  NOTE: This code should be moved later to an adequante position.
//				  ***ASK PASA WHERE TO PUT IT*******
// 
//       Author:  Carlos Beltran
//      Created:  30/07/2004 16:36:11 W. Europe Daylight Time
//     Revision:  none
// =====================================================================================
class YARPDtw
{
  public:
    YARPDtw(){}
	~YARPDtw(){}

	//--------------------------------------------------------------------------------------
	//       Class: YARPDtw  
	//      Method: Dtw 
	// Description: Dynamic Time Warping algorithm. For a detailed description have a look
    // at the next reference:
	// 		"Fundamentals of Speech Recognition"
    //		Lawrence Rabiner, Biing-Hwang Juang
	//		Prentice Hall Signal Precessing Series	
	// Parameters:
	// It is necessary to enter two templates, and unknown one and a reference one. Each
	// template is suposed to be a set of vector (i.e. a Matrix) with the data to be compared.
    // The algorithm returns the optimal minimum distance between the two templates.	
	//--------------------------------------------------------------------------------------
	inline double 
	Dtw(double **unkTemp, int nunkTemp, double **refTemp, int nrefTemp, int tempSize) 
	{
		int i, j, k;
		double bestDist;
		double diff;
		double sum;
		double * p_unkTemp;
		double * p_refTemp;
		YMatrix localDist;
		YMatrix globalDist;

		localDist.Resize  ( nunkTemp,nrefTemp);
		globalDist.Resize ( nunkTemp,nrefTemp);

		//----------------------------------------------------------------------
		//  Compute all the local distances
		//----------------------------------------------------------------------
		for(i = 1; i <= nunkTemp; i++)
			for(j = 1; j <= nrefTemp; j++)
			{
				sum = 0.0;
				//Get the pointers to the vectors
				p_unkTemp = unkTemp[i-1];
				p_refTemp = refTemp[j-1];

				//Just calculate the distance between the vectors
				for ( k = 0; k < tempSize; k++)
				{
					diff = p_unkTemp[k] - p_refTemp[k];
					sum += (diff*diff);
				}
				localDist(i,j) = sum;
			}

		//----------------------------------------------------------------------
		// For the first frame the only possible mathc is at (1,1) 
		//----------------------------------------------------------------------
		globalDist(1,1) = localDist(1,1);
		for(j = 2; j <= nrefTemp; j++)
			globalDist(1,j) = HUGE;

		//----------------------------------------------------------------------
		//  In the second frame the only valid state is (2,2)
		//----------------------------------------------------------------------
		globalDist(2,1) = HUGE;
		globalDist(2,2) = globalDist(1,1) + localDist(2,2);
		for(j = 3; j <= nrefTemp; j++)
			globalDist(2,j) = HUGE;

		//----------------------------------------------------------------------
		// Do the general case for the rest of the frames 
		//----------------------------------------------------------------------
		for(i = 3; i <= nunkTemp; i++) 
		{
			globalDist(i,1) = HUGE;
			globalDist(i,2) = globalDist(i - 1,1) + localDist(i,2);

			for(j = 3; j <= nrefTemp; j++) 
			{
				double topPath;
				double midPath;
				double botPath;

				if( globalDist(i-2,j-1) < 0.0)
					printf("DTWMessage: globalDist(i-2,j-1) < 0.0\n: %d\t%d\t%f\n", i, j, globalDist(i-2,j-1));

				topPath = globalDist(i-2,j-1) + localDist(i-1,j) + localDist(i,j);
				midPath = globalDist(i-1,j-1) + localDist(i,j);
				botPath = globalDist(i-1,j-2) + localDist(i,j-1) + localDist(i,j);

				//----------------------------------------------------------------------
				// Find and store the smallest global distance 
				//----------------------------------------------------------------------
				if(topPath < midPath && topPath < botPath)
					globalDist(i,j) = topPath;
				else if (midPath < botPath)
					globalDist(i,j) = midPath;
				else
					globalDist(i,j) = botPath;
			}    
		}

		bestDist = globalDist(nunkTemp,nrefTemp);

		return(bestDist);
	}
};
#endif
