#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define NECC 152
#define NANG 252
#define CAMERA 0

void main ()
{
	int XSize,YSize,Planes;
	Image_Data Param;
	char File_Name [256]; 
	char Working_Folder[256];

	int i,j,frames;
	int start = 0;
	int end   = 100 ;
	int step = 25;
	int a,b,c;
	int min_i;
	int MapSize;

	int OrigXSize = 1090;
	int OrigYSize = 1090;
	int RemXSize  = 1090;
	int RemYSize  = 1090;
	int RhoSize   =  152;
	int ThetaSize =  252;
	int DestSize  = 1090;

	Cart2LPInterp * Cart2LP_Map;

	FILE *fin;

	srand( (unsigned)time( NULL ) );

	unsigned char * Full_Image; 
	unsigned char * Remapped_Image [2]; 
	unsigned char * LP_Image [2]; 
	unsigned char * LPImg; 
	unsigned char *Total_Cart_Image;
	unsigned char *Temp_Cart_Image;
	unsigned char *Total_Map;
	unsigned char *RemLP_Map;
	int * Coordinates;
	int * RemapMapFull;
	int * RemapMap[4]; 

	int ret_addr;
	int ret_size;

	int CenterX =0, CenterY = 0;
	int CenterX2, CenterY2;

	double * CorrVal;
	unsigned char * CorrelMap;
	int Size1, Size2;

	int TESTIMGSIZE = 96;
	int IMAGE_RATIO =  4;
	double MAX = 0.0;
	double MIN = 0.0;
	int StartPosX, StartPosY;

	Size1 = TESTIMGSIZE;
	Size2 = TESTIMGSIZE/IMAGE_RATIO;

	Param = Set_Param(OrigXSize,OrigYSize,
					  RemXSize,RemYSize,
					  RhoSize,ThetaSize,42,
					  1090,
					  CUSTOM,
					  FITIN,0,0);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\RemLPMap.bmp");
	RemLP_Map = Load_Bitmap(&RemXSize,&RemYSize,&Planes,File_Name);
	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\2428x2428B.bmp");
	sprintf(Working_Folder,"%s","C:\\Temp\\From Talia\\Tables\\Test\\Giotto2.0\\");
	Full_Image = Load_Bitmap(&XSize,&YSize,&Planes,File_Name);
	Cart2LP_Map = Load_Cart2LP_Map(&Param,Working_Folder);
	Total_Cart_Image = (unsigned char *)calloc(3 * XSize * YSize , sizeof(unsigned char));
	Total_Map = (unsigned char *)malloc(XSize * YSize * sizeof(unsigned char));
	Temp_Cart_Image = (unsigned char *) malloc (3 * Param.Size_Img_Remap * sizeof(unsigned char));
	CorrelMap = (unsigned char *) malloc ((Size1-Size2) * (Size1-Size2) * sizeof(unsigned char));
	Coordinates = (int*)malloc((end)*sizeof(int));

	LPImg = (unsigned char *)malloc(Param.Size_LP * sizeof(unsigned char));
	LP_Image[0] = (unsigned char *)malloc(3 * Param.Size_LP * sizeof(unsigned char));
	LP_Image[1] = (unsigned char *)malloc(3 * Param.Size_LP * sizeof(unsigned char));

	for (j=3; j>=0; j--)//Crea e Legge le Remap LUT, a tutti i livelli di zoom.
	{
		Param.Zoom_Level = 1.0/pow(2,j);
		Build_Remap_Map(&Param,Working_Folder);

		sprintf(File_Name,"%s%s_%2.3f%s",Working_Folder,"RemapMap",Param.Zoom_Level,".gio");
		MapSize = (int)(Param.Resolution * Param.Zoom_Level + 0.5);
		RemapMap[j] = (int *) malloc (MapSize * MapSize * sizeof(int));

		fin = fopen(File_Name,"rb");	
		fread(RemapMap[j],sizeof(int),MapSize * MapSize,fin);
		fclose (fin);

	}

	Param.Center_X_Remap = (XSize)*rand()/RAND_MAX;	
	Param.Center_Y_Remap = (YSize)*rand()/RAND_MAX;
	Param.Center_X_Remap = 0;	
	Param.Center_Y_Remap = 0;
	StartPosX = Param.Center_X_Remap;
	StartPosY = Param.Center_Y_Remap;
	if (CAMERA)
	{
		Param.Center_X_Remap = XSize/2;	
		Param.Center_Y_Remap = YSize/2;
		StartPosX = Param.Center_X_Remap;
		StartPosY = Param.Center_Y_Remap;
		ret_addr = Giotto_Init(0x378);	
	
		ret_size = Giotto_Acq(LPImg,NANG,NECC,ret_addr);
		LPImg[0]=0;
		for (j=0; j<Param.Size_LP; j++)
		{
			LP_Image[0][3*j+0] = LPImg[j];
			LP_Image[0][3*j+1] = LPImg[j];
			LP_Image[0][3*j+2] = LPImg[j];
		}
	}
	else
	{
//		LP_Image [0] = Get_LP_from_Large_Image(Full_Image,&Param,XSize,Cart2LP_Map);
		Param.Center_X_Remap = XSize/2;	
		Param.Center_Y_Remap = YSize/2;
		StartPosX = Param.Center_X_Remap;
		StartPosY = Param.Center_Y_Remap;
		sprintf(File_Name,"%s%03d%s","C:\\Temp\\From Talia\\Frames\\Frame_",start,".bmp");
		LP_Image[0] = Load_Bitmap(&a,&b,&c,File_Name);
	}
	
	Coordinates [start] = Param.Center_Y_Remap * XSize + Param.Center_X_Remap;

	MapSize = (int)(Param.Resolution * Param.Zoom_Level + 0.5);

	sprintf(File_Name,"%s%s_%2.3f%s",Working_Folder,"RemapMap",Param.Zoom_Level,".gio");

	RemapMapFull = (int *) malloc (MapSize * MapSize * sizeof(int));

	fin = fopen(File_Name,"rb");	
	fread(RemapMapFull,sizeof(int),MapSize * MapSize,fin);
	fclose (fin);

	Remap(Temp_Cart_Image,LP_Image[0],&Param,RemapMapFull);	
	Add_to_Full_Image(Temp_Cart_Image,Total_Cart_Image,Total_Map,RemLP_Map,XSize,&Param);

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Cart1.bmp");
	Save_Bitmap(Temp_Cart_Image,1090,1090,3,File_Name);
//	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\TotalImg.bmp");
//	Save_Bitmap(Total_Cart_Image,XSize,YSize,3,File_Name);
//	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\LP Image.bmp");
//	Save_Bitmap(LP_Image[j],ThetaSize,RhoSize,Planes,File_Name);

	for (frames = start+step; frames<=end; frames+=step)
	{
		printf("Frame: %d (%d,%d)\n",frames-step,Param.Center_X_Remap,Param.Center_Y_Remap);
		Param = Set_Param(OrigXSize,OrigYSize,
						  RemXSize,RemYSize,
						  RhoSize,ThetaSize,42,
						  1090,
						  CUSTOM,
						  FITIN,Param.Center_X_Remap,Param.Center_Y_Remap);

//		Param.Center_X_Remap = (XSize)*rand()/RAND_MAX;	
//		Param.Center_Y_Remap = (YSize)*rand()/RAND_MAX;
		Param.Center_X_Remap += ((200)*rand()/RAND_MAX)-100;	
		Param.Center_Y_Remap += ((200)*rand()/RAND_MAX)-100;	

		if (CAMERA)
		{
			ret_size = Giotto_Acq(LPImg,NANG,NECC,ret_addr);
			LPImg[0]=0;
			for (j=0; j<Param.Size_LP; j++)
			{
				LP_Image[1][3*j+0] = LPImg[j];
				LP_Image[1][3*j+1] = LPImg[j];
				LP_Image[1][3*j+2] = LPImg[j];
			}
		}
		else
	{
//		LP_Image [1] = Get_LP_from_Large_Image(Full_Image,&Param,XSize,Cart2LP_Map);
		sprintf(File_Name,"%s%03d%s","C:\\Temp\\From Talia\\Frames\\Frame_",frames,".bmp");
		LP_Image[1] = Load_Bitmap(&a,&b,&c,File_Name);
	}

//		sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\LP Image.bmp");
//		Save_Bitmap(LP_Image[j],ThetaSize,RhoSize,Planes,File_Name);
		CenterX = 0;
		CenterY = 0;
		for (j=3; j>=2; j--)
		{
			Param.Zoom_Level = 1.0/pow(2,j);

//			sprintf(File_Name,"%s%s_%2.3f%s",Working_Folder,"RemapMap",Param.Zoom_Level,".gio");
			MapSize = (int)(Param.Resolution * Param.Zoom_Level + 0.5);
//			RemapMap[j] = (int *) malloc (MapSize * MapSize * sizeof(int));

//			fin = fopen(File_Name,"rb");	
//			fread(RemapMap[j],sizeof(int),MapSize * MapSize,fin);
//			fclose (fin);

			Param.Size_X_Remap = Size1;
			Param.Size_Y_Remap = Size1;
			Param.Size_Img_Remap = Param.Size_X_Remap * Param.Size_Y_Remap;
			CenterX2 = (CenterX*MapSize)/Param.Resolution;
			CenterY2 = (CenterY*MapSize)/Param.Resolution;

			Remapped_Image [0] = (unsigned char *) malloc (3 * Param.Size_Img_Remap * sizeof(unsigned char));
			Remap_Cropped(Remapped_Image[0],LP_Image[0],&Param,RemapMap[j],CenterX2,CenterY2);

			sprintf(File_Name,"%s_%d_%s","C:\\Temp\\From Talia\\Test Mosaic\\Remapped Image 1",j,".bmp");
			Save_Bitmap(Remapped_Image[0],Param.Size_X_Remap,Param.Size_Y_Remap,Planes,File_Name);

			if (j<3)
				Size2 = Size1-pow(2,j+1);

			Param.Size_X_Remap = Size2;
			Param.Size_Y_Remap = Size2;
			Param.Size_Img_Remap = Param.Size_X_Remap * Param.Size_Y_Remap;

			Remapped_Image [1] = (unsigned char *) malloc (3 * Param.Size_Img_Remap * sizeof(unsigned char));
			Remap_Cropped(Remapped_Image[1],LP_Image[1],&Param,RemapMap[j],0,0);

			sprintf(File_Name,"%s_%d_%s","C:\\Temp\\From Talia\\Test Mosaic\\Remapped Image 2",j,".bmp");
			Save_Bitmap(Remapped_Image[1],Param.Size_X_Remap,Param.Size_Y_Remap,Planes,File_Name);

			CorrVal = Find_Max_Corr(Size1,Size1,
									Size2,Size2,
									Remapped_Image[0],
									Remapped_Image[1]);

			MIN = Size1 * Size1 * Size2 * Size2;
			MAX = 0.0;

			for (i=0; i<(Size1-Size2) * (Size1-Size2); i++)
				if (CorrVal[i]>MAX)
					MAX = CorrVal[i];

			for (i=0; i<(Size1-Size2) * (Size1-Size2); i++)
				if (CorrVal[i]<MIN)
				{
					MIN = CorrVal[i];
					min_i = i;
				}

			for (i=0; i<(Size1-Size2) * (Size1-Size2); i++)
			{
				CorrelMap[i] = (unsigned char)(255*(CorrVal[i]/MAX));
//				CorrelMap[3*i+1] = (unsigned char)          ((int)(255*255*(CorrVal[i]/MAX))%256);
//				CorrelMap[3*i+2] = (unsigned char)          ((int)(255*255*255*(CorrVal[i]/MAX))%(256*256));
			}

			sprintf(File_Name,"%s_%d_%s","C:\\Temp\\From Talia\\Test Mosaic\\CorrMap",j,".bmp");
			Save_Bitmap(CorrelMap,(Size1-Size2),(Size1-Size2),1,File_Name);

			free(CorrVal);

			CenterX += (int)(pow(2,j)*(min_i%(Size1-Size2)-(Size1-Size2)/2));
//			if ((min_i/(Size1-Size2)-(Size1-Size2)/2)<2)
//				break;
			CenterY += (int)(pow(2,j)*(min_i/(Size1-Size2)-(Size1-Size2)/2));	
//			free(RemapMap[j]);
		}
		
		Param = Set_Param(OrigXSize,OrigYSize,
						  RemXSize,RemXSize,
						  RhoSize,ThetaSize,42,
						  1090,
						  CUSTOM,
						  FITIN,StartPosX,StartPosY);

		Param.Center_X_Remap += CenterX;
		Param.Center_Y_Remap += CenterY;
//		for (j=0; j<Param.Size_LP; j++)
//			LP_Image[1][3*j+2] = frames;
		Remap(Temp_Cart_Image,LP_Image[1],&Param,RemapMapFull);	
		Add_to_Full_Image(Temp_Cart_Image,Total_Cart_Image,Total_Map,RemLP_Map,XSize,&Param);

		memcpy(LP_Image[0],LP_Image[1],3*Param.Size_LP);
		StartPosX = Param.Center_X_Remap;
		StartPosY = Param.Center_Y_Remap;
		Coordinates [frames] = Param.Center_Y_Remap * XSize + Param.Center_X_Remap;
		

		sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Cart2.bmp");
		Save_Bitmap(Temp_Cart_Image,1090,1090,3,File_Name);

	}
		printf("Frame: %d (%d,%d)\n",frames-step,Param.Center_X_Remap,Param.Center_Y_Remap);
	
		for (frames = start; frames<=end-step; frames+=step)
			Draw_Line(Total_Cart_Image,XSize,YSize,Coordinates[frames],Coordinates [frames+step],0x8BFF01);

//	for (j=0; j<XSize*YSize; j++)
//		if(Total_Map[j]!=0)
//		{
//			Total_Cart_Image2[3*j+0] = Total_Cart_Image[3*j+0]/Total_Map[j];
//		}
	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\TotalImg.bmp");
	Save_Bitmap(Total_Cart_Image,XSize,YSize,3,File_Name);
	
}



