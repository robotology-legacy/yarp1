#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"


void main ()
{
	Image_Data FovParam;
	Image_Data Param;

	int FTime, PTime;
	int j;

	int BWRho, BWTheta, BWPlanes;

	Neighborhood * WeightsMap;
	int  * RemapMapF;
	int  * RemapMap;

	char Path [256]; 
	char File_Name [256]; 
	FILE* fin;

	sprintf(Path,"%s%2.2f\\","C:\\Temp\\Tables\\",1.0);
//	sprintf(Path,"%s","C:\\Temp2\\");

	unsigned char * ColLP;
	unsigned char * RemFovea;
	unsigned char * Rem;

	unsigned char * BWLP =  Load_Bitmap(&BWTheta,&BWRho,&BWPlanes,"c:\\Temp\\TestLP_BW_Pad.bmp");
//	free(BWLP);

	FovParam = Set_Param(	1090,1090,
							128,128,
							152,252,42,
							1090,
							CUSTOM,
							512.0/1090.0);

	Param = Set_Param(		1090,1090,
							256,256,
							152,252,42,
							1090,
							CUSTOM,
							256.0/1090.0);

	FovParam.padding = 8;
	FovParam.Fovea_Type = 0;

	Param.padding = 8;
	Param.Fovea_Type = 0;

	Param.Pix_Numb = 2;

	float steps = 100.0;
	
//	BWLP =  (unsigned char *) malloc (Param.Size_LP * (int)steps * sizeof(unsigned char));
//	srand( (unsigned)time( NULL ) );
//
//	for (j=0; j<Param.Size_LP * steps; j++)
//		BWLP[3*(j/3)+2] = (256*rand()/RAND_MAX);
	
	Build_XY_Map(&Param,Path);
	Build_Color_Map(Path);
	Build_Neighborhood_Map(&Param, Path);
	Build_Weights_Map(&Param, Path);

	sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Param.Pix_Numb,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		WeightsMap = (Neighborhood *) malloc (Param.Size_LP * Param.Pix_Numb * 3 * sizeof(Neighborhood));
		fread(WeightsMap,sizeof(Neighborhood),Param.Size_LP * Param.Pix_Numb * 3,fin);
		fclose (fin);
	}
	else
		WeightsMap = NULL;

	int padsize = 3*Param.Size_Theta+((3*Param.Size_Theta)%Param.padding);

	ColLP = (unsigned char *) malloc (Param.Size_Rho * padsize * sizeof(unsigned char));

//	Build_Ang_Shift_Map(&Param,Path);
//	Build_Pad_Map(&Param,Path);
//	Build_Remap_Map(&FovParam,Path);
//	Build_Remap_Map(&Param,Path);

	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;

	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",FovParam.Zoom_Level,FovParam.Size_X_Remap,FovParam.Size_Y_Remap,".gio");
	
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMapF = (int *) malloc (FovParam.Size_Img_Remap * sizeof(int));
		fread(RemapMapF,sizeof(int),FovParam.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMapF = NULL;

	Rem = (unsigned char *) malloc (Param.Size_Img_Remap * 3 * sizeof(unsigned char));
	RemFovea = (unsigned char *) malloc (Param.Size_Img_Remap * 3 * sizeof(unsigned char));

	Reconstruct_Color(ColLP,BWLP,BWRho,Param.Size_Theta,8,WeightsMap,Param.Pix_Numb);

	sprintf(File_Name,"C:\\Temp\\FullTestLP.bmp");
	Save_Bitmap(ColLP,padsize,Param.Size_Rho,1,File_Name);

	Remap(Rem,ColLP,&Param,RemapMap);

	Remap(RemFovea,ColLP,&FovParam,RemapMapF);

	sprintf(File_Name,"C:\\Temp\\FullTest.bmp");
	Save_Bitmap(Rem,Param.Size_X_Remap,Param.Size_Y_Remap,3,File_Name);
	sprintf(File_Name,"C:\\Temp\\FullTestFov.bmp");
	Save_Bitmap(RemFovea,FovParam.Size_X_Remap,FovParam.Size_Y_Remap,3,File_Name);

	free (BWLP);
	free (ColLP);
	free (WeightsMap);
	free (RemapMap);
	free (RemapMapF);
	free (Rem);
	free (RemFovea);

}