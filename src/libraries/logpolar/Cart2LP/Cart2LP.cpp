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
	
	Param = Set_Param(	1090,1090,
						1090,1090,
						NECC,NANG,42,
						1090,
						CUSTOM,
						1,
						0,0);
						
	sprintf(Working_Folder,"%s","C:\\Temp\\From Talia\\Tables\\Test\\Giotto2.0\\");
	Cart2LP_Map = Load_Cart2LP_Map(&Param,Working_Folder);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Square1.bmp");
	Cartesian = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);

	LogPolar = Make_LP_Real(Cartesian,&Param,Cart2LP_Map);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\SquareLP1.bmp");
	Save_Bitmap(LogPolar,NANG,NECC,3,File_Name);

	free(Cartesian);
	free(LogPolar);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Square2.bmp");
	Cartesian = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);

	LogPolar = Make_LP_Real(Cartesian,&Param,Cart2LP_Map);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\SquareLP2.bmp");
	Save_Bitmap(LogPolar,NANG,NECC,3,File_Name);

	free(Cartesian);
	free(LogPolar);
}