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
///                    #fberton#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: TableLoading.cpp,v 1.7 2003-09-30 17:21:44 fberton Exp $
///
///

#include <stdio.h>
#include <stdlib.h> 

#include "LogPolarSDK.h"

/****************************************************************************
*																			*
* Load_Tables																*
* 																			*
* Loads all the needed LUT's from disk.										*
*																			*
* Input: Param																*
* 			Pointer to a struct containing all the needed parameters		*
* 		 Tables																*
* 			Pointer to a struct containing all the pointers					*
*        Path																*
* 			Working folder													*
*        List																*
* 			Control value used to determine which tables will be loaded:	*
*																			*
* 				1	AngShiftMap.gio											*
*				2	ColorMap.gio 											*
*				4	DownsampleMap.gio 										*
*				8	NeighborhoodMap.gio 									*
*			   16	PadMap.gio 												*
*			   32	RemMap_X.XX_YYxYY.gio 									*
*			   64	WeightsMap.gio 											*
*			  128	XYMap.gio												*
*																			*
*		The values have to be summed when loading more than one table.		*
*		Use the ALLMAPS value to load all the LUT's.						*
*																			*
****************************************************************************/	

unsigned short Load_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned short List)
{
	char File_Name [256];
	unsigned short retval = 0;
	int j;
	FILE * fin;
	
	if (List&1==1)
	{
		sprintf(File_Name,"%s%s",Path,"AngularShiftMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->AngShiftMap = (double *) malloc (Param->Size_Rho * sizeof(double));
			fread(Tables->AngShiftMap,sizeof(double),Param->Size_Rho,fin);
			fclose (fin);
			retval = retval | 1;
		}
		else
			Tables->AngShiftMap = NULL;
	}

	if ((List&2)==2)
	{
		sprintf(File_Name,"%s%s",Path,"ColorMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->ColorMap = (char *) malloc (Param->Size_LP * sizeof(char));
			fread(Tables->ColorMap,sizeof(char),Param->Size_LP,fin);
			fclose (fin);
			retval = retval | 2;
		}
		else
			Tables->ColorMap = NULL;
	}

	if ((List&8)==8)
	{
		sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->NeighborMap = (Neighborhood *) malloc (Param->Size_LP * MAX_PIX * 3 * sizeof(Neighborhood));
			fread(Tables->NeighborMap,sizeof(Neighborhood),Param->Size_LP * MAX_PIX * 3,fin);
			fclose (fin);
			retval = retval | 8;
		}
		else
			Tables->NeighborMap = NULL;
	}

	if ((List&16)==16)
	{
		sprintf(File_Name,"%s%s",Path,"PadMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->PadMap = (unsigned short *) malloc (Param->Size_Theta * Param->Size_Fovea * sizeof(unsigned short));
			fread(Tables->PadMap,sizeof(unsigned short),Param->Size_Theta * Param->Size_Fovea,fin);
			fclose (fin);
			retval = retval | 16;
		}
		else
			Tables->PadMap = NULL;
	}

	if ((List&32)==32)
	{
		sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param->Zoom_Level,Param->Size_X_Remap,Param->Size_Y_Remap,".gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->RemapMap = (int *) malloc (Param->Size_Img_Remap * sizeof(int));
			fread(Tables->RemapMap,sizeof(int),Param->Size_Img_Remap,fin);
			fclose (fin);
			retval = retval | 32;
		}
		else
			Tables->RemapMap = NULL;
	}

	if ((List&64)==64)
	{
		sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Param->Pix_Numb,".gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->WeightsMap = (Neighborhood *) malloc (Param->Size_LP * Param->Pix_Numb * 3 * sizeof(Neighborhood));
			fread(Tables->WeightsMap,sizeof(Neighborhood),Param->Size_LP * Param->Pix_Numb * 3,fin);
			fclose (fin);
			retval = retval | 64;
		}
		else
			Tables->WeightsMap = NULL;
	}

	if ((List&128)==128)
	{
		sprintf(File_Name,"%s%s",Path,"XYMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->XYMap = (double *) malloc (2 * Param->Size_LP * sizeof(double));
			fread(Tables->XYMap,sizeof(double),2 * Param->Size_LP * 1,fin);
			fclose (fin);
			retval = retval | 128;
		}
		else
			Tables->XYMap = NULL;
	}

	if ((List&4)==4)
	{
		sprintf(File_Name,"%s%s%1.2f%s",Path,"DSMap_",4.00,".gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->DownSampleMap = (IntNeighborhood *) malloc ((Param->Size_LP / 16) * sizeof(unsigned short));
			for (j=0; j<Param->Size_LP/16; j++)
			{
				fread(&(Tables->DownSampleMap[j].NofPixels) ,sizeof(unsigned short),1,fin);
				Tables->DownSampleMap[j].position = (unsigned short *) malloc (Tables->DownSampleMap[j].NofPixels*sizeof(unsigned short));
				Tables->DownSampleMap[j].weight = (unsigned char *) malloc (Tables->DownSampleMap[j].NofPixels*sizeof(unsigned char));
				fread(&(Tables->DownSampleMap[j].position) ,sizeof(unsigned short),Tables->DownSampleMap[j].NofPixels,fin);
				fread(&(Tables->DownSampleMap[j].weight) ,sizeof(unsigned char),Tables->DownSampleMap[j].NofPixels,fin);
			}
			
			fclose (fin);
			retval = retval | 4;
		}
		else
			Tables->DownSampleMap = NULL;
	}

	if ((List&256)==256)
	{
		sprintf(File_Name,"%s%s%1.2f%s",Path,"DSMap_",2.00,".gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->DownSampleMap = (IntNeighborhood *) malloc ((Param->Size_LP / 4) * sizeof(unsigned short));
			for (j=0; j<Param->Size_LP/4; j++)
			{
				fread(&(Tables->DownSampleMap[j].NofPixels) ,sizeof(unsigned short),1,fin);
				Tables->DownSampleMap[j].position = (unsigned short *) malloc (Tables->DownSampleMap[j].NofPixels*sizeof(unsigned short));
				Tables->DownSampleMap[j].weight = (unsigned char *) malloc (Tables->DownSampleMap[j].NofPixels*sizeof(unsigned char));
				fread(&(Tables->DownSampleMap[j].position) ,sizeof(unsigned short),Tables->DownSampleMap[j].NofPixels,fin);
				fread(&(Tables->DownSampleMap[j].weight) ,sizeof(unsigned char),Tables->DownSampleMap[j].NofPixels,fin);
			}
			
			fclose (fin);
			retval = retval | 256;
		}
		else
			Tables->DownSampleMap = NULL;
	}


	return retval;
}

Cart2LPInterp * Load_Cart2LP_Map(Image_Data * Param, char * Path)
{
	char File_Name [256];
	int j;
	FILE * fin;
	Cart2LPInterp * Cart2LP_Map;
		
//	int PadSizeTheta = (((Param->Size_Theta * Param->LP_Planes) % Param->padding) + (Param->Size_Theta * Param->LP_Planes));
	
	sprintf(File_Name,"%s%s",Path,"Cart2LPMap.gio");

	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		Cart2LP_Map = (Cart2LPInterp *) malloc (Param->Size_LP * sizeof(Cart2LPInterp));
		for (j=0; j<Param->Size_LP; j++)
		{
			fread(&Cart2LP_Map[j].NofPixels,sizeof(unsigned char),1,fin);
			Cart2LP_Map[j].position = (unsigned int *) malloc (Cart2LP_Map[j].NofPixels * sizeof(unsigned int));
			fread(Cart2LP_Map[j].position,sizeof(unsigned int),Cart2LP_Map[j].NofPixels,fin);
		}
		fclose (fin);
	}
	else
		Cart2LP_Map = NULL;

	return Cart2LP_Map;
}

void Free_Cart2LP_Map (Cart2LPInterp *map)
{
	if (map != NULL) free (map);
}
