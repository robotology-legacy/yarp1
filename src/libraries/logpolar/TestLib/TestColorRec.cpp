#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define INTEGERMAP

void main()
{
	int BWRho,BWTheta,BWPlanes;
	int i,j,k,l;

	Image_Data Param;

	Neighborhood * WeightsMap;
	IntNeighborhood * FWeightsMap;

	int TimeF;
	int TimeI;


//	float * ArrayF;
//	int   * ArrayI;

//	float * ArrayFO;
//	int   * ArrayIO;


/*
	ArrayF = (float *)malloc(100000 * sizeof(float));
	ArrayI = (int *)malloc(100000 * sizeof(int));
	ArrayFO = (float *)malloc(100000 * sizeof(float));
	ArrayIO = (int *)malloc(100000 * sizeof(int));

	srand( (unsigned)time( NULL ) );

	for( i = 0;   i < 100000;i++ )
	{
		ArrayF[i] = (float)rand()/RAND_MAX;
		ArrayI[i] = rand();
	}

	TimeI = Get_Time();
	for (j=0; j<10000; j++)
		for( i = 0;   i < 99999;i++ )
			ArrayFO[i] = ArrayFO[i]+ArrayFO[i+1];

	TimeI = Get_Time()-TimeI;
	printf("Time for 1,000,000 sums (float): %f ms\n", TimeI/100.0);

	TimeI = Get_Time();

	for (j=0; j<10000; j++)
		for( i = 0;   i < 99999;i++ )
			ArrayIO[i] = ArrayIO[i]+ArrayIO[i+1];

	TimeI = Get_Time()-TimeI;
	printf("Time for 1,000,000 sums (integer): %f ms\n", TimeI/100.0);
*/
	char Path [256]; 
	char File_Name [256]; 
	FILE* fin;

	sprintf(Path,"%s%2.2f\\","C:\\Temp\\Tables\\",1.0);
	
	unsigned char * BWLP =  Load_Bitmap(&BWTheta,&BWRho,&BWPlanes,"c:\\Temp\\TestLP_BW.bmp");
	BWRho   = 152;
	BWTheta = 252;
	BWPlanes = 1;

	Param = Set_Param(	1090,1090,
						256,256,
						BWRho,BWTheta,42,
						1090,
						CUSTOM,
						1);

	Param.padding = 1;
	Param.Fovea_Type = 0;

//	unsigned char * BWLP =  (unsigned char *)malloc(Param.Size_LP * 3 * 100 * sizeof(unsigned char));
	srand( (unsigned)time( NULL ) );
//	for (j=0; j<Param.Size_LP * 3 * 100; j++)
//		BWLP[j] = 256*rand()/RAND_MAX;

	unsigned char * ColLP =   (unsigned char *)malloc(Param.Size_LP * 3 * sizeof(unsigned char));
	unsigned char * ColLPI =  (unsigned char *)malloc(Param.Size_LP * 3 * sizeof(unsigned char));

//	Build_Neighborhood_Map(&Param, Path);

	Param.Pix_Numb = 4;
	Build_Weights_Map(&Param, Path);
	FWeightsMap = Build_Fast_Weights_Map(&Param, Path);

	sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Param.Pix_Numb,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		WeightsMap = (Neighborhood *) malloc (Param.Size_LP * Param.Pix_Numb * 3 * sizeof(Neighborhood));
		fread(WeightsMap,sizeof(Neighborhood),Param.Size_LP * Param.Pix_Numb * 3,fin);
		fclose (fin);
	}
	else
		WeightsMap = NULL;

//	sprintf(File_Name,"%s%s%02d%s",Path,"FastWeightsMap",Param.Pix_Numb,".gio");
//	if ((fin = fopen(File_Name,"rb")) != NULL)
//	{
//		FWeightsMap = (Neighborhood *) malloc (Param.Size_LP * Param.Pix_Numb * 3 * sizeof(Neighborhood));
//		fread(FWeightsMap,sizeof(Neighborhood),Param.Size_LP * Param.Pix_Numb * 3,fin);
//		fclose (fin);
//	}
//	else
//		FWeightsMap = NULL;


	
	int i_SumR,i_SumG,i_SumB;                                                                                                                                    
	unsigned char shift; 

	TimeI = Get_Time();

 
	for (i=0; i<100; i++)//tornare a planar?
//		Fast_Reconstruct_Color(ColLPI,&BWLP[i * Param.Size_LP * 3],BWRho,BWTheta,Param.padding,FWeightsMap,Param.Pix_Numb);
		Fast_Reconstruct_Color(ColLPI,BWLP,BWRho,BWTheta,Param.padding,FWeightsMap,Param.Pix_Numb);

//		Reconstruct_Color(ColLPI,BWLP,BWRho,BWTheta,Param.padding,FWeightsMap,Param.Pix_Numb);
	
	TimeI = Get_Time()-TimeI;
	printf("Time to Reconstruct Colors (integer): %f ms\n", TimeI/100.0);
	srand( (unsigned)time( NULL ) );
//	for (j=0; j<Param.Size_LP * 3 * 100; j++)
//		BWLP[j] = 256*rand()/RAND_MAX;

	TimeF = Get_Time();
	
	for (i=0; i<100; i++)
//		Reconstruct_Color(ColLP,&BWLP[i * Param.Size_LP * 3],BWRho,BWTheta,Param.padding,WeightsMap,Param.Pix_Numb);
		Reconstruct_Color(ColLP,BWLP,BWRho,BWTheta,Param.padding,WeightsMap,Param.Pix_Numb);
	
	TimeF = Get_Time()-TimeF;
	printf("Time to Reconstruct Colors (float): %f ms\n", TimeF/100.0);

	Save_Bitmap(ColLPI,BWTheta,BWRho,3,"C:\\Temp\\TestColorRecColInt.bmp");
	Save_Bitmap(ColLP,BWTheta,BWRho,3,"C:\\Temp\\TestColorRecCol.bmp");

	i_SumR = 0;
	for (j=0; j<3*Param.Size_LP; j++)
		i_SumR += FWeightsMap[j].NofPixels;

	int * RemapMap;
	Build_Remap_Map(&Param,Path);
	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;

	unsigned char * Remapped = (unsigned char *)malloc(Param.Size_Img_Remap * 3 * sizeof(unsigned char));;

	TimeI = Get_Time();
	for (i=0; i<100; i++)//tornare a planar?
		Remap(Remapped,ColLP,&Param,RemapMap);
	TimeI = Get_Time()-TimeI;
	printf("Time to Remap: %f ms\n", TimeI/100.0);

	
	free(WeightsMap);
	free(FWeightsMap);
	free(BWLP);
	free(ColLP);
	free(ColLPI);
	free(RemapMap);
}