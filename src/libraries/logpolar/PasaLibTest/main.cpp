#include <stdio.h>
#include <stdlib.h>
#include "LogPolarSDK.h"


void main ()
{
	char File_Name [256]; 
	char Path [256]; 

	Image_Data Param;

	int i,j;

	unsigned char * Orig_Image;
	unsigned char * LP_Image;
	unsigned char * Rem_Image;
	unsigned char * Rem_Fovea;
	int XSize,YSize,Planes;
	int RemXSize = 128;
	int RemYSize = 128;
	int SRho = 152;
	int STheta = 252;
	int SFovea = 42;
	Cart2LPInterp * Cart2LP_Map;
	int * RemapMap;

	FILE * fin;

	sprintf(File_Name,"%s","C:\\Temp\\Test.bmp");
	sprintf(Path,"%s","C:\\Temp\\Tables\\");
	Orig_Image = Read_Bitmap(&XSize,&YSize,&Planes,File_Name);

	Param = Set_Param(XSize,YSize,
						128,128,
						SRho,STheta,SFovea,
						1090,
						CUSTOM,256.0/1090.0);

	Param.padding = 8;

	Rem_Fovea = (unsigned char *) malloc (3*Param.Size_Img_Remap * sizeof(unsigned char));	
	
	int PadSizeLP;

	PadSizeLP = (((Param.Size_Theta * Param.LP_Planes) % Param.padding) + (Param.Size_Theta * Param.LP_Planes))*Param.Size_Rho;
	LP_Image = (unsigned char *) malloc (PadSizeLP * sizeof (unsigned char));
	int PadSizeLPBW = (((Param.Size_Theta * 1) % Param.padding) + (Param.Size_Theta * 1))*Param.Size_Rho;
	unsigned char * LP_Image_BW = (unsigned char *) malloc (PadSizeLPBW * sizeof (unsigned char));

	Build_Pad_Map(&Param,Path );

	
	Build_Cart2LP_Map(&Param,Path);
	
	Cart2LP_Map = Load_Cart2LP_Map(&Param,Path);
	int time = Get_Time();
//	for (int y=0; y<1000; y++)
//	{
		Make_LP_Real(LP_Image,Orig_Image,&Param,Cart2LP_Map);
//		free(LP_Image);
//	}
	time = Get_Time()-time;

//	sprintf(File_Name,"%s","C:\\Temp\\Test2.bmp");
//	Save_Bitmap(LP_Image+252*42*3,252,110,3,File_Name);
	sprintf(File_Name,"%s","C:\\Temp\\TestPad2.bmp");
	Save_Bitmap(LP_Image,760,152,1,File_Name);


//	Param.Zoom_Level *= 2.0; 
	Build_Remap_Map(&Param,Path);
	Crop_Remap_Map(&Param,Path);

	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;

	Remap(Rem_Fovea,LP_Image,&Param,RemapMap);
	sprintf(File_Name,"%s","C:\\Temp\\Test3.bmp");
	Save_Bitmap(Rem_Fovea,128,128,3,File_Name);

	free(RemapMap);

	Param.Size_X_Remap = 256;
	Param.Size_Y_Remap = 256;
	Param.Size_Img_Remap = 256*256;

	Rem_Image = (unsigned char *) malloc (3*Param.Size_Img_Remap * sizeof(unsigned char));	

	Build_Remap_Map_No_Fov(&Param,Path);

	sprintf(File_Name,"%s%s_%2.3f%s",Path,"RemapMapNoFov",Param.Zoom_Level,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;

	Remap(Rem_Image,LP_Image +760*42,&Param,RemapMap);
	sprintf(File_Name,"%s","C:\\Temp\\Test4.bmp");
	Save_Bitmap(Rem_Image,256,256,3,File_Name);

	char * ColorMap;

	sprintf(File_Name,"%s%s",Path,"ColorMap.gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		ColorMap = (char *) malloc (Param.Size_LP * sizeof(char));
		fread(ColorMap,sizeof(char),Param.Size_LP,fin);
		fclose (fin);
	}

	unsigned char * LP_Ptr = LP_Image;
	unsigned char * LP_Ptr_BW = LP_Image_BW;
	char * ColorPtr = ColorMap;

	for (j=0; j<Param.Size_Rho; j++)
	{
		for (i=0; i<Param.Size_Theta; i++)
		{
			*LP_Ptr_BW++ = *(LP_Ptr+*ColorPtr);
			LP_Ptr +=3;
			ColorPtr++;
		}
		LP_Ptr_BW+=(Param.Size_Theta) % Param.padding;
		LP_Ptr+=(Param.Size_Theta * 3) % Param.padding;
	}


	sprintf(File_Name,"%s","C:\\Temp\\TestBW1.bmp");
	Save_Bitmap(LP_Image_BW,256,152,1,File_Name);

	Neighborhood * WeightsMap;
	sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Param.Pix_Numb,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		WeightsMap = (Neighborhood *) malloc (Param.Size_LP * Param.Pix_Numb * 3 * sizeof(Neighborhood));
		fread(WeightsMap,sizeof(Neighborhood),Param.Size_LP * Param.Pix_Numb * 3,fin);
		fclose (fin);
	}

//	for (j=0; j<Param.Size_LP * Param.Pix_Numb * 3; j++)
//		if (WeightsMap[j].weight>0.1)
//			i=i;

	Reconstruct_Color(LP_Image,LP_Image_BW,Param.Size_LP,WeightsMap,4);

	sprintf(File_Name,"%s","C:\\Temp\\TestBW2.bmp");
	Save_Bitmap(LP_Image,760,152,1,File_Name);

}