#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 

#include "LogPolarSDK.h"
#include "Mosaic.h"


unsigned char * Crop_Image (unsigned char * Input_Image,Image_Data * Par,int OrigXSize, int DestSize)
{
	int i,j;
	unsigned char * Output_Image;
	int TotSize = DestSize * DestSize;
	int XOrigin = Par->Center_X_Remap - DestSize/2;
	int YOrigin = Par->Center_Y_Remap - DestSize/2;
	
	Output_Image  = (unsigned char *)malloc(3 * TotSize * sizeof(unsigned char));

	for (j=0; j<DestSize; j++)
		for (i=0; i<DestSize; i++)
		{
			if ((j>=-YOrigin)&&(i>=-XOrigin)&&(j<OrigXSize-YOrigin)&&(i<OrigXSize-XOrigin))
			{
				Output_Image[3*(j*DestSize+i)+0] = Input_Image[3*((j+YOrigin)*OrigXSize+(i+XOrigin))+0];
				Output_Image[3*(j*DestSize+i)+1] = Input_Image[3*((j+YOrigin)*OrigXSize+(i+XOrigin))+1];
				Output_Image[3*(j*DestSize+i)+2] = Input_Image[3*((j+YOrigin)*OrigXSize+(i+XOrigin))+2];
			}
			else
			{
				Output_Image[3*(j*DestSize+i)+0] = 192;
				Output_Image[3*(j*DestSize+i)+1] = 192;
				Output_Image[3*(j*DestSize+i)+2] = 192;
			}
		}

	return Output_Image;
}

void Add_to_Full_Image (unsigned char * Input_Image,
						unsigned char * Full_Image, 
						unsigned char * Mos_Map, 
						unsigned char * Tex_Map,
						int MosaicXSize,
						int MosaicYSize,
						int TesseraXSize,
						int TesseraYSize,
						Image_Data * Par)
{
	int i,j;
//	int displX = (int)((Par->Center_X_Remap) - (Par->Resolution/2)*Par->Zoom_Level);
//	int displY = (int)((Par->Center_Y_Remap) - (Par->Resolution/2)*Par->Zoom_Level);
	int displX = (int)(Par->Center_X_Remap) - TesseraXSize/2;
	int displY = (int)(Par->Center_Y_Remap) - TesseraYSize/2;

	for (j=0; j<TesseraYSize; j++)
		for (i=0; i<TesseraXSize; i++)
		{
			if((j+displY<MosaicYSize)&&(i+displX<MosaicXSize))
			{
				if((j+displY>=0)&&(i+displX>=0))
				{
					if (1)
					{
						if (Tex_Map[j*TesseraXSize+i]<=(Mos_Map[(j+displY)*MosaicXSize+(i+displX)]))
//						if (Full_Image[3*((j+displY)*OrigXSize+(i+displX))+2]!=0)
						{
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+0] = (9*Input_Image[3*(j*Par->Size_X_Remap+i)+0]+1*Full_Image[3*((j+displY)*OrigXSize+(i+displX))+0])/10;
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+1] = (Input_Image[3*(j*Par->Size_X_Remap+i)+1]+Full_Image[3*((j+displY)*OrigXSize+(i+displX))+1])/2;
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+2] = (Input_Image[3*(j*Par->Size_X_Remap+i)+2]+9*Full_Image[3*((j+displY)*OrigXSize+(i+displX))+2])/10;
							Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+0] = (Input_Image[3*(j*TesseraXSize+i)+0]);
							Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+1] = (Input_Image[3*(j*TesseraXSize+i)+1]);
							Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+2] = (Input_Image[3*(j*TesseraXSize+i)+2]);
//							Full_Map[(j+displY)*OrigXSize+(i+displX)] ++;			
							Mos_Map[(j+displY)*MosaicXSize+(i+displX)] = Tex_Map[j*TesseraXSize+i];			
						}
//						else
//						{
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+0] = (9*Input_Image[3*(j*Par->Size_X_Remap+i)+0]+1*Full_Image[3*((j+displY)*OrigXSize+(i+displX))+0])/10;
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+1] = (Input_Image[3*(j*Par->Size_X_Remap+i)+1]+Full_Image[3*((j+displY)*OrigXSize+(i+displX))+1])/2;
//							Full_Image[3*((j+displY)*OrigXSize+(i+displX))+2] = (Input_Image[3*(j*Par->Size_X_Remap+i)+2]+0*Full_Image[3*((j+displY)*OrigXSize+(i+displX))+2])/1;
//						}
					}
					else
					{
						Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+0] = (Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+0]+Input_Image[3*(j*TesseraXSize+i)+0])/2;
						Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+1] = (Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+1]+Input_Image[3*(j*TesseraXSize+i)+1])/2;
						Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+2] = (Full_Image[3*((j+displY)*MosaicXSize+(i+displX))+2]+Input_Image[3*(j*TesseraXSize+i)+2])/2;
						Mos_Map[(j+displY)*MosaicXSize+(i+displX)] = Tex_Map[j*TesseraXSize+i];			
					}
				}
			}

		}
}

double * Find_Max_Corr(int X1, int Y1, int X2, int Y2, unsigned char * Image1, unsigned char * Image2)
{
	//Image 1 is the reference image, while Image 2 is the moving image

	int i,j;
	int k,l;
	double * sum;
	int tempdiff;
	unsigned char Im1Value, Im2Value;
	int total;

	sum = (double *)malloc( (X1-X2) * (Y1-Y2) * sizeof(double));

	for (j=0; j<(Y1-Y2); j++)
		for (i=0; i<(X1-X2); i++)
		{
			sum[j*(X1-X2)+i] = 0.0;
			total = 0;
			for (l=0; l<Y2; l++)
				for (k=0; k<X2; k++)
				{
					Im1Value = Image1[3*((l+j)*X1+(i+k))];
					Im2Value = Image2[3*(l*X2+k)];
					if (Im1Value!=0)
						if (Im2Value!=0)
						{
							tempdiff = Im1Value - Im2Value;
							sum[j*(X1-X2)+i] += (tempdiff*tempdiff);
							total ++;
						}
				}
			if (total < ((X2*Y2/4)*3*PI/4))
				sum[j*(X1-X2)+i] = -1;
			else
				sum[j*(X1-X2)+i] = sum[j*(X1-X2)+i]/total;
		}

	return sum;
}



/************************************************************************
* Remap	Cropped			  												*
************************************************************************/
	
void Remap_Cropped(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT,
		   int StartX,
		   int StartY)
{
	int i,j;

	int SizeRemapX = Par->Size_X_Remap;
	int SizeRemapY = Par->Size_Y_Remap;
	int MapSizeX = (int)(Par->Resolution * Par->Zoom_Level + 0.5);

	StartX = StartX - SizeRemapX/2 + MapSizeX/2;
	StartY = StartY - SizeRemapY/2 + MapSizeX/2;

//	StartX = Par->Resolution - (MapSizeX/2) - StartX;
//	StartY = Par->Resolution - (MapSizeX/2) - StartY;
//	int CenterX = Par->Center_X_Remap;
//	int CenterY = Par->Center_Y_Remap;

//	int starti = (Par->Resolution/2)+CenterX-(SizeRemapX/2);
//	int startj = (Par->Resolution/2)-CenterY-(SizeRemapY/2);
//	int endi = (Par->Resolution - starti);
//	int endj = (Par->Resolution - startj);

	unsigned char * RemImgPtr = Out_Image;
	int * LPImgPtr = Rem_LUT;

	for (j=StartY; j<StartY+SizeRemapY; j++)
	{
		for (i=StartX; i<StartX+SizeRemapX; i++)
		{
			Out_Image[3*((j-StartY)*SizeRemapX+(i-StartX))+0] = In_Image[Rem_LUT[(j*MapSizeX+i)]+0];
			Out_Image[3*((j-StartY)*SizeRemapX+(i-StartX))+1] = In_Image[Rem_LUT[(j*MapSizeX+i)]+1];
			Out_Image[3*((j-StartY)*SizeRemapX+(i-StartX))+2] = In_Image[Rem_LUT[(j*MapSizeX+i)]+2];
//			*RemImgPtr++ = In_Image[*LPImgPtr];
//			*RemImgPtr++ = In_Image[(*LPImgPtr)+1];
//			*RemImgPtr++ = In_Image[(*LPImgPtr++)+2];
		}
	}
}


unsigned char * Get_LP_from_Large_Image(unsigned char * Large_Img, Image_Data * Par, int Large_Img_Size,Cart2LPInterp * Cart2LP_Map)
{
	char File_Name [256]; 
	unsigned char * Cropped_Img;
	unsigned char * LP_Img;
	int PadSizeLP;

	PadSizeLP = (((Par->Size_Theta * Par->LP_Planes) % Par->padding) + (Par->Size_Theta * Par->LP_Planes))*Par->Size_Rho;

	Cropped_Img = Crop_Image(Large_Img,Par,Large_Img_Size,Par->Resolution);
	LP_Img = (unsigned char *) malloc (PadSizeLP * sizeof (unsigned char));

	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\Cropped Image.bmp");

	Save_Bitmap(Cropped_Img,Par->Resolution,Par->Resolution,3,File_Name);

	Make_LP_Real(LP_Img,Cropped_Img,Par,Cart2LP_Map);

	free(Cropped_Img);

	return LP_Img;
}

void Draw_Line(unsigned char *Image, int width, int height, int Start, int End, int RGB)
{
	int XCo1, YCo1;
	int XCo2, YCo2;
	int passi;
	int i;
	unsigned char Red,Green,Blue;
	double stepX,stepY;

	XCo1 = Start%width;
	YCo1 = Start/width;
	XCo2 = End%width;
	YCo2 = End/width;

	Blue = RGB%256;
	RGB/=256;
	Green = RGB%256;
	RGB/=256;
	Red = RGB;

	if (abs(XCo1-XCo2)>abs(YCo1-YCo2))
	{
		passi = abs(XCo1-XCo2);
		stepX = -abs(XCo1-XCo2)/(double)(XCo1-XCo2);
		stepY = -((double)abs(YCo1-YCo2)/(double)abs(XCo1-XCo2))*(abs(YCo1-YCo2)/(double)(YCo1-YCo2));
	}
	else
	{
		passi = abs(YCo1-YCo2);
		stepY = -abs(YCo1-YCo2)/(double)(YCo1-YCo2);
		stepX = -((double)abs(XCo1-XCo2)/(double)abs(YCo1-YCo2))*(abs(XCo1-XCo2)/(double)(XCo1-XCo2));
	}

	for (i=0; i<=passi; i++)
	{
		Image[3*( (YCo1 + (int)(i*stepY)) *width + XCo1+(int)(i*stepX))+0] = Red;
		Image[3*( (YCo1 + (int)(i*stepY)) *width + XCo1+(int)(i*stepX))+1] = Green;
		Image[3*( (YCo1 + (int)(i*stepY)) *width + XCo1+(int)(i*stepX))+2] = Blue;
	}


}

/*
void Init_Mosaic(Tables * TabPtr, Images * ImgPtr, Image_Data * Param, int ImgID)
{
	char Working_Folder [256];
	char File_Name [256];

	FILE *fin;
	
	int j;
	int MapSize;
		
	sprintf(Working_Folder,"%s","C:\\Temp\\From Talia\\Tables\\Test\\Giotto2.0\\");

	TabPtr->SmMosaicSize = (TabPtr->SmMosaicX) * (TabPtr->SmMosaicY);

	ImgPtr->Small_Mosaic     = (unsigned char *)calloc(3 * TabPtr->SmMosaicSize, sizeof(unsigned char));
	TabPtr->Small_Mosaic_Map = (unsigned char *)malloc(TabPtr->SmMosaicSize * sizeof(unsigned char));

	for (j=0; j<TabPtr->SmMosaicSize; j++)
		TabPtr->Small_Mosaic_Map[j] = 255;

	for (j=0; j<5; j++) //Crea e Legge le Remap LUT, a tutti i livelli di zoom.
	{
		Param->Zoom_Level = 1.0/pow(2,j);
		Build_Remap_Map(Param,Working_Folder);

		sprintf(File_Name,"%s%s_%2.3f%s",Working_Folder,"RemapMap",Param->Zoom_Level,".gio");
		TabPtr->RemLUTFullX[j]  = (int)(Param->Resolution * Param->Zoom_Level + 0.5);
		TabPtr->RemLUTFullY[j]  = TabPtr->RemLUTFullX[j];
		TabPtr->RemLUTFullXY[j] = TabPtr->RemLUTFullY[j] * TabPtr->RemLUTFullY[j];
		TabPtr->RemapMap[j] = (int *) malloc (TabPtr->RemLUTFullXY[j] * sizeof(int));

		fin = fopen(File_Name,"rb");	
		fread(TabPtr->RemapMap[j],sizeof(int),TabPtr->RemLUTFullXY[j],fin);
		fclose (fin);
	}
	
	Param->Size_X_Remap   = TabPtr->RemLUTFullX[4];
	Param->Size_Y_Remap   = TabPtr->RemLUTFullY[4];
	Param->Size_Img_Remap = TabPtr->RemLUTFullXY[4];

	ImgPtr->Small_Remapped [0] = (unsigned char *) malloc (3 * Param->Size_Img_Remap * sizeof(unsigned char));

	//Legge la Mappa LogPolare delle risoluzioni e la rimappa
	sprintf(File_Name,"%s","C:\\Temp\\From Talia\\Test Mosaic\\MapLP.bmp");
	TabPtr->LP_Map = Read_Bitmap(&Param->Size_Theta,&Param->Size_Rho,&Param->LP_Planes,File_Name);

	TabPtr->Small_Partial_Map  = (unsigned char *) malloc (Param->Size_Img_Remap * sizeof(unsigned char));
	Remap_Cropped(ImgPtr->Small_Remapped[0],TabPtr->LP_Map,Param,TabPtr->RemapMap[4],0,0);

	for (j=0; j<Param->Size_Img_Remap; j++)
		TabPtr->Small_Partial_Map[j] = ImgPtr->Small_Remapped [0][3*j];

	//Legge la LogPolare iniziale e la rimappa
	sprintf(File_Name,"%s%03d%s","C:\\Temp\\From Talia\\Frames\\Frame_",ImgID,".bmp");
	ImgPtr->LP_Image[0] = Read_Bitmap(&Param->Size_Theta,&Param->Size_Rho,&Param->LP_Planes,File_Name);

	Remap_Cropped(ImgPtr->Small_Remapped[0],ImgPtr->LP_Image[0],Param,TabPtr->RemapMap[4],0,0);

	Param->Center_X_Remap = TabPtr->SmMosaicX/2;
	Param->Center_Y_Remap = TabPtr->SmMosaicY/2;

	Add_to_Full_Image(ImgPtr->Small_Remapped[0],ImgPtr->Small_Mosaic,TabPtr->Small_Mosaic_Map,TabPtr->Small_Partial_Map,TabPtr->SmMosaicX,Param);

	free(TabPtr->LP_Map);
	free(ImgPtr->LP_Image[0]);
	free(ImgPtr->Small_Remapped[0]);

//	free(TabPtr->Small_Mosaic_Map);
//	free(ImgPtr->Small_Mosaic);
//	for (j=0; j<5; j++)
//		free(TabPtr->RemapMap[j]);
//	free(TabPtr->Small_Partial_Map);
}
*/
int Corr_w_Mosaic(Tables * TabPtr, Images * ImgPtr, Image_Data * Param, int ImgID)
{
	char File_Name [256];
	int Size1X;
	int Size1Y;
	int Size2X;
	int Size2Y;
	int i, min_i;
	double MIN,MAX;

	Size1X = TabPtr->SmMosaicX;
	Size1Y = TabPtr->SmMosaicY;
	Size2X = TabPtr->RemLUTFullX[4];
	Size2Y = TabPtr->RemLUTFullY[4];


	Param->Size_X_Remap = TabPtr->RemLUTFullX[4];
	Param->Size_Y_Remap = TabPtr->RemLUTFullY[4];
	Param->Size_Img_Remap = TabPtr->RemLUTFullXY[4];

	ImgPtr->Small_Remapped [1] = (unsigned char *) malloc (3 * Param->Size_Img_Remap * sizeof(unsigned char));
	Remap_Cropped(ImgPtr->Small_Remapped[1],ImgPtr->LP_Image[1],Param,TabPtr->RemapMap[4],0,0);
	TabPtr->Raw_Corr_Map = Find_Max_Corr(Size1X,Size1Y,Size2X,Size2Y,ImgPtr->Small_Mosaic,ImgPtr->Small_Remapped[1]);

	MIN = Size1X * Size1Y * Size2X * Size2Y;
	MAX = 0.0;

	for (i=0; i<(Size1X-Size2X) * (Size1Y-Size2Y); i++)
		if (TabPtr->Raw_Corr_Map[i]>MAX)
			MAX = TabPtr->Raw_Corr_Map[i];

	for (i=0; i<(Size1X-Size2X) * (Size1Y-Size2Y); i++)
	{
		if (TabPtr->Raw_Corr_Map[i]==-1)
			TabPtr->Raw_Corr_Map[i]=MAX;

		if (TabPtr->Raw_Corr_Map[i]<MIN)
		{
			MIN = TabPtr->Raw_Corr_Map[i];
			min_i = i;
		}
	}

	for (i=0; i<(Size1X-Size2X) * (Size1Y-Size2Y); i++)
	{
		ImgPtr->Corr_Map[i] = (unsigned char)(255*(TabPtr->Raw_Corr_Map[i]/MAX));
//				Corr_Map[i] = (unsigned char)(Raw_Corr_Map[i]);
//				CorrelMap[3*i+1] = (unsigned char)          ((int)(255*255*(CorrVal[i]/MAX))%256);
//				CorrelMap[3*i+2] = (unsigned char)          ((int)(255*255*255*(CorrVal[i]/MAX))%(256*256));
	}

	sprintf(File_Name,"%s_%d_%s","C:\\Temp\\From Talia\\Test Mosaic\\CorrMap",ImgID,".bmp");
	Save_Bitmap(ImgPtr->Corr_Map,(Size1X-Size2X),(Size1Y-Size2Y),1,File_Name);
	
	free(ImgPtr->Small_Remapped[1]);
	return min_i;
}

//int Corr_w_Frame()
//{
//}
