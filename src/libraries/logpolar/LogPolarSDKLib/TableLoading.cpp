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

unsigned char Load_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned char List)
{
	char File_Name [256];
	unsigned char retval = 0;
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

	if ((List&4)==4)
	{
		sprintf(File_Name,"%s%s",Path,"DownSampleMap.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables->DownSampleMap = (unsigned short *) malloc (Param->Size_LP * 4 * sizeof(unsigned short));
			fread(Tables->DownSampleMap,sizeof(unsigned short),Param->Size_LP * 4,fin);
			fclose (fin);
			retval = retval | 4;
		}
		else
			Tables->DownSampleMap = NULL;
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
			Tables->PadMap = (short *) malloc (Param->Size_Theta * Param->Size_Fovea * sizeof(short));
			fread(Tables->PadMap,sizeof(short),Param->Size_Theta * Param->Size_Fovea,fin);
			fclose (fin);
			retval = retval | 16;
		}
		else
			Tables->PadMap = NULL;
	}

	if ((List&32)==32)
	{
		sprintf(File_Name,"%s%s_%2.2f_%dx%d%s",Path,"RemapMap",Param->Zoom_Level,Param->Size_X_Remap,Param->Size_Y_Remap,".gio");
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
			fread(Tables->XYMap,sizeof(double),2 * Param->Size_LP * 3,fin);
			fclose (fin);
			retval = retval | 128;
		}
		else
			Tables->XYMap = NULL;
	}

	return retval;
}

Cart2LPInterp * Load_Cart2LP_Map(Image_Data * Param, char * Path)
{
	char File_Name [256];
	int j;
	FILE * fin;
	Cart2LPInterp * Cart2LP_Map;
	
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
