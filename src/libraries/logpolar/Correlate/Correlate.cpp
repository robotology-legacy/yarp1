#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define NECC 152
#define NANG 252

void main ()
{
	int XSize1,YSize1,Planes1;
	int XSize2,YSize2,Planes2;
	Image_Data Param;
	char File_Name [256]; 
//	char Working_Folder[256];

	int OrigXSize = 1090;
	int OrigYSize = 1090;
	int RemXSize  = 1090;
	int RemYSize  = 1090;
	int RhoSize   =  152;
	int ThetaSize =  252;
	int DestSize  = 1090;

	int i,j, min_i;
	int CenterX=0, CenterY=0;

	unsigned char * Remapped_Image [2]; 
	double * CorrVal;
	unsigned char * CorrelMap;

	double MAX = 0.0;
	double MIN = 0.0;

	Param = Set_Param(OrigXSize,OrigYSize,
					  RemXSize,RemYSize,
					  RhoSize,ThetaSize,42,
					  1090,
					  CUSTOM,
					  FITIN,0,0);


	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Cart1.bmp");
	Remapped_Image [0] = Load_Bitmap(&XSize1,&YSize1,&Planes1,File_Name);
	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Cart2.bmp");
	Remapped_Image [1] = Load_Bitmap(&XSize2,&YSize2,&Planes2,File_Name);

	CorrVal = Find_Max_Corr(XSize1,YSize1,
							XSize2,YSize2,
							Remapped_Image[0],
							Remapped_Image[1]);


	CorrelMap = (unsigned char *) malloc ((XSize1-XSize2) * (YSize1-YSize2) * sizeof(unsigned char));
	MIN = XSize1 * YSize1 * XSize2 * YSize2;
	MAX = 0.0;

	for (i=0; i<(XSize1-XSize2) * (YSize1-YSize2); i++)
		if (CorrVal[i]>MAX)
			MAX = CorrVal[i];

	for (i=0; i<(XSize1-XSize2) * (YSize1-YSize2); i++)
		if (CorrVal[i]<MIN)
		{
			MIN = CorrVal[i];
			min_i = i;
		}

	for (i=0; i<(XSize1-XSize2) * (YSize1-YSize2); i++)
		CorrelMap[i] = (unsigned char)(255*(CorrVal[i]/MAX));

	sprintf(File_Name,"%s_%d_%s","C:\\Temp\\From Talia\\Test Mosaic\\FullCorrMap",0,".bmp");
	Save_Bitmap(CorrelMap,(XSize1-XSize2),(YSize1-YSize2),1,File_Name);
	printf("%d\n",min_i);

	free(CorrVal);

//	CenterX += (int)(pow(2,j)*(min_i%(XSize1-XSize2)-(XSize1-XSize2)/2));
//	CenterY += (int)(pow(2,j)*(min_i/(XSize1-XSize2)-(XSize1-XSize2)/2));
}
