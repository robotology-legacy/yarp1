#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define NECC 152
#define NANG 252

void main()
{
	int XSize,YSize,Planes;
	char File_Name[256];
	char Working_Folder[256];
	unsigned char * Cartesian;
	unsigned char * LogPolar;
	Cart2LPInterp * Cart2LP_Map;
	Image_Data Param;
	
	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\TestImg.bmp");
	Cartesian = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);

	Param = Set_Param(	XSize,YSize,
						1090,1090,
						NECC,NANG,42,
						1090,
						CUSTOM,
						1);
	Param.padding = 1;
	Param.Fovea_Type = 0;
						
	sprintf(Working_Folder,"%s","C:\\Temp\\Tables\\1.00\\");
	Build_Cart2LP_Map(&Param,Working_Folder);
	Cart2LP_Map = Load_Cart2LP_Map(&Param,Working_Folder);



	LogPolar = (unsigned char *)malloc(Param.Size_LP * 3 * sizeof(unsigned char));

	Make_LP_Real(LogPolar,Cartesian,&Param,Cart2LP_Map);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\TestImgLP.bmp");
	Save_Bitmap(LogPolar,NANG,NECC,3,File_Name);

	free(Cartesian);
	free(LogPolar);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\Star8.bmp");
	Cartesian = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);
	LogPolar = (unsigned char *)malloc(Param.Size_LP * 3 * sizeof(unsigned char));

	Make_LP_Real(LogPolar,Cartesian,&Param,Cart2LP_Map);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\Star8LP.bmp");
	Save_Bitmap(LogPolar,NANG,NECC,3,File_Name);

	free(Cartesian);
	free(LogPolar);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\Star9.bmp");
	Cartesian = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);
	LogPolar = (unsigned char *)malloc(Param.Size_LP * 3 * sizeof(unsigned char));

	Make_LP_Real(LogPolar,Cartesian,&Param,Cart2LP_Map);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Images\\Star9LP.bmp");
	Save_Bitmap(LogPolar,NANG,NECC,3,File_Name);

	free(Cartesian);
	free(LogPolar);
}