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
/// $Id: LogPolarSDK.cpp,v 1.24 2003-11-26 14:30:08 fberton Exp $
///
///

#include <stdio.h>
#include <stdlib.h>

#if !defined(__QNX__) && !defined(__QNX6__)

#include <conio.h>
#include <io.h>
#include <windows.h>

#endif

#include <math.h>
#include <time.h>
#include <fcntl.h>

#include "LogPolarSDK.h"

static VCHANNEL vchannel;

Image_Data Set_Param(int SXO,
					 int SYO,
					 int SXR,
					 int SYR,
					 int rho,
					 int theta,
					 int fovea,
					 int resolution,
					 int LPMode, 
					 double ZoomLevel)
{
	int Color = 3;
	bool Landscape = true;
    Image_Data image;
	image.padding = 1; //No Padding

	switch (LPMode)
	{

	case Giotto1:

		image.Size_Rho = 76;
		image.Size_Theta = 128;
		image.Size_Fovea = 20;
		image.Resolution = 600;
		break;

	case Giotto2:

		image.Size_Rho = 152;
		image.Size_Theta = 252;
		image.Size_Fovea = 42;
		image.Resolution = 1090;
		break;

	case CUST:

		image.Size_Rho = rho;
		image.Size_Theta = theta;
		image.Size_Fovea = fovea;
		image.Resolution = resolution;
		image.Size_X_Remap = SXR;
		image.Size_Y_Remap = SYR;
//		image.Center_X_Remap = Cx;
//		image.Center_Y_Remap = Cy;
		break;
	}
	
	image.Size_LP = image.Size_Rho * image.Size_Theta;
	image.Size_X_Orig = SXO;
	image.Size_Y_Orig = SYO;
	image.Size_Img_Orig = image.Size_X_Orig*image.Size_Y_Orig;
	image.Size_Img_Remap = image.Size_X_Remap * image.Size_Y_Remap;
	image.LP_Planes = Color;
	image.Orig_Planes = Color;
	image.Remap_Planes = Color;
	image.Valid_Log_Index = false;

	if (ZoomLevel == FITIN){
		image.Zoom_Level = (double)(image.Size_Y_Remap);
		image.Zoom_Level /= (double)(image.Resolution);
	}
	else if (ZoomLevel == FITOUT){
		image.Zoom_Level = (double)(image.Size_Y_Remap*image.Size_Y_Remap);
		image.Zoom_Level += (double)(image.Size_X_Remap*image.Size_X_Remap);
		image.Zoom_Level = (double)sqrt(image.Zoom_Level);
		image.Zoom_Level /= (double)(image.Resolution);
	}
	else image.Zoom_Level = ZoomLevel;
	image.Orig_LandScape  = Landscape;
	image.Remap_LandScape = Landscape;
	image.Pix_Numb = 4;
	image.Fovea_Type = 0;
	image.Ratio = 1.00;


	return image;
}

/************************************************************************
* Camera_Init															*
* Inizializza la telecamera per l'acquisizione (Wrapper)				*
************************************************************************/	


/************************************************************************
* Giotto_Acq															*
* Acquisisce un frame dalla telecamera o da un file						*
************************************************************************/	











double Compute_Index(int Resolution, int Fovea, int SizeRho)
{
	double DValue,Value, Tempt, Dx, ADx;
	double x1,x2;
	double Tolerance = 0.0001;

	int exp = SizeRho - Fovea;
	int j;

	x1 = 1.0;
	x2 = 3.0;
	Dx = 100;
	ADx = 100;

	j=0;
	Tempt = (double)(x1+x2)/2;
	while  (ADx>Tolerance) 
	{
		if (Dx>=0)
			ADx = Dx;
		else
			ADx = -Dx;

		Value = pow(Tempt,exp+1)-((Resolution/2)-Fovea+0.5)*(Tempt-1)-Tempt;
		Value = ((Tempt*(pow(Tempt,exp)-1))/(Tempt-1)) -(Resolution/2)+Fovea-0.5;
		DValue = (exp+1)*pow(Tempt,exp)-((Resolution/2)-Fovea+0.5)-1;
		DValue = ((exp)*pow(Tempt,exp+1)-(exp+1)*pow(Tempt,exp)+1)/((Tempt-1)*(Tempt-1));
		Dx = Value/DValue;
		Tempt -= Dx;
		j++;
	}

	return Tempt;
}

/************************************************************************
* Get_Rho		  														*
************************************************************************/	

int Get_Rho(double x,
			double y,
			Image_Data * par)
{
	double radius;
	int rho;
	double logar;

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}
	
	logar = 1/(log(par->Log_Index));

	radius = sqrt(x*x + y*y)/par->Zoom_Level;

	if (radius>par->Size_Fovea)
		rho = (int)((log((radius-par->Size_Fovea+0.5)*(par->Log_Index-1)+par->Log_Index) 
					* logar + par->Size_Fovea-1)-0.0);
	else 
		rho = (int)(radius+0.5);

	return rho;
}

/************************************************************************
* Get_XY_Center  														*
************************************************************************/	
int Get_XY_Center(double *xx, double *yy, int rho, int theta, Image_Data *par, double *Ang_Shift)
{
	double scalefactor;
	int Temp_Size_Theta;
	double A,B;
	double mod;
	double rd, td;

	if (rho != 0)
	{
		rd = rho+0.5;
		td = theta+0.5;
	}
	else
	{
		rd = rho;
		td = theta;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}

	scalefactor = par->Zoom_Level;
	B = par->Log_Index/(par->Log_Index-1);
	A = par->Size_Fovea - B - 0.5;

	if (rho<par->Size_Fovea)
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = rd-0.5;
		if (rho==0)
		{
			Temp_Size_Theta = 1;
			mod = 0;
		}
		else if (par->Fovea_Display_Mode < 2)
			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * rho;
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rd-par->Size_Fovea);
	}
		if (Temp_Size_Theta>par->Size_Theta)
			Temp_Size_Theta = par->Size_Theta;

	*xx = mod * cos(Ang_Shift[rho]+td*PI/(Temp_Size_Theta/2.0)) * scalefactor;
	*yy = mod * sin(Ang_Shift[rho]+td*PI/(Temp_Size_Theta/2.0)) * scalefactor;

	return 0;
}



/************************************************************************
* Get_X_Center  														*
************************************************************************/	

double Get_X_Center_Old(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{

	double scalefactor;
	int Temp_Size_Theta;
	double x,A,B;
	double mod;
	double rd, td;

	if (rho != 0)
	{
		rd = rho+0.5;
		td = theta+0.5;
	}
	else
	{
		rd = rho;
		td = theta;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}

	scalefactor = par->Zoom_Level;
	B = par->Log_Index/(par->Log_Index-1);
	A = par->Size_Fovea - B - 0.5;

	if (rho<par->Size_Fovea)
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = rd-0.5;
		if (rho==0)
		{
			Temp_Size_Theta = 1;
			mod = 0;
		}
		else if (par->Fovea_Display_Mode < 2)
			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rd-par->Size_Fovea);
	}
		if (Temp_Size_Theta>par->Size_Theta)
			Temp_Size_Theta = par->Size_Theta;

		x = mod * cos(+Ang_Shift[(int)rho]+td*PI/(Temp_Size_Theta/2.0));

		return x*scalefactor;
}

/************************************************************************
* Get_Y_Center  														*
************************************************************************/	

double Get_Y_Center_Old(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap){

	double scalefactor;
	int Temp_Size_Theta;
	double y,A,B;
	double mod;
	double rd, td;
	if (rho != 0)
	{
		rd = rho+0.5;
		td = theta+0.5;
	}
	else 
	{
		rd = rho;
		td = theta;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}

	scalefactor = par->Zoom_Level;
	B = par->Log_Index/(par->Log_Index-1);
	A = par->Size_Fovea - B - 0.5;
		
	if (rho<par->Size_Fovea)
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = rd-0.5;
		if (rho==0)
		{
			Temp_Size_Theta = 1;
			mod = 0;
		}
		else if (par->Fovea_Display_Mode < 2)
			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rd-par->Size_Fovea);
	}

		if (Temp_Size_Theta>par->Size_Theta)
			Temp_Size_Theta = par->Size_Theta;

		y = mod * sin(+Ang_Shift[(int)rho]+td*PI/(Temp_Size_Theta/2.0));

		return y*scalefactor;

}


/************************************************************************
* Reconstruct_Color		  												*
************************************************************************/	

void Reconstruct_Color(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   Neighborhood * Weights_Map,
					   int Pix_Numb)
{
#ifdef SLOW

	int i,j,k,plane;
	int Size = height * width;
	int shiftPN = Size * Pix_Numb;
	double Sum;
	int step = Size;
	int end = 3*Size;
	unsigned char * buffer = Out_Image;
	Neighborhood * Weights_Map_R = Weights_Map;
	Neighborhood * Weights_Map_G = &Weights_Map[shiftPN];
	Neighborhood * Weights_Map_B = &Weights_Map[2*shiftPN];

	int PadShift;

	if ((width * 3) % padding == 0)
		PadShift = 0;
	else
		PadShift = padding - (width * 3) % padding;



	if (Pix_Numb == 1)
	{
		for (j=0; j<Size; j++)
			for (plane = 0; plane < end; plane +=step)
				*buffer++ = In_Image[Weights_Map[plane+j].position];
	}
	else
	{
//		for (j=0; j<Pix_Numb*Size; j+=Pix_Numb)
		for (j=0; j<height; j++)
		{
			for (k=0; k<Pix_Numb*width; k+=Pix_Numb)
			{
				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_R[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_R[j*Pix_Numb*width+k  +i].position];
				*buffer++ = (unsigned char)Sum;

				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_G[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_G[j*Pix_Numb*width+k  +i].position];
				*buffer++ = (unsigned char)Sum;

				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_B[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_B[j*Pix_Numb*width+k  +i].position];
				*buffer++ = (unsigned char)Sum;
			}
			buffer+=PadShift;
		}
	}

#else
	int i,j,k,plane;
	int Size = height * width;
	int shiftPN = Size * Pix_Numb;
	float SumR, SumG, SumB;
	int step = Size;
	int end = 3*Size;
	unsigned char * buffer = Out_Image;
	const int H = height;
	const int W = Pix_Numb*width;
	const int PN = Pix_Numb;
	int index;
	Neighborhood * Weights_Map_R = Weights_Map;
	Neighborhood * Weights_Map_G = &Weights_Map[shiftPN];
	Neighborhood * Weights_Map_B = &Weights_Map[2*shiftPN];

	int PadShift;

	if ((width * 3) % padding == 0)
		PadShift = 0;
	else
		PadShift = padding - (width * 3) % padding;

	if (Pix_Numb == 1)
	{
		for (j=0; j<Size; j++)
			for (plane = 0; plane < end; plane +=step)
				*buffer++ = In_Image[Weights_Map[plane+j].position];
	}
	else
	{
		for (j=0; j<H; j++)
		{
			for (k=0; k<W; k+=PN)
			{
				index = j*W+k;
				SumR = 0.0;
				SumG = 0.0;
				SumB = 0.0;
				for (i=0; i<PN; i++)
				{
					SumR += Weights_Map_R[index+i].weight*In_Image[Weights_Map_R[index+i].position];
					SumG += Weights_Map_G[index+i].weight*In_Image[Weights_Map_G[index+i].position];
					SumB += Weights_Map_B[index+i].weight*In_Image[Weights_Map_B[index+i].position];
				}
				*buffer++ = (unsigned char)SumR;

				*buffer++ = (unsigned char)SumG;

				*buffer++ = (unsigned char)SumB;
			}
			buffer+=PadShift;
//			buffer+=1;
		}
	}
#endif
}

/************************************************************************
* Reconstruct_Grays
************************************************************************/	

void Reconstruct_Grays(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   Neighborhood * Weights_Map,
					   int Pix_Numb)
{
	int i,j,k,plane;
	int Size = height * width;
	int shiftPN = Size * Pix_Numb;
	double Sum;
	int step = Size;
	int end = 3*Size;
	unsigned char * buffer = Out_Image;
	Neighborhood * Weights_Map_R = Weights_Map;
	Neighborhood * Weights_Map_G = &Weights_Map[shiftPN];
	Neighborhood * Weights_Map_B = &Weights_Map[2*shiftPN];

	int PadShift = (3*width)%padding;

	if (Pix_Numb == 1)
	{
		for (j=0; j<Size; j++)
			for (plane = 0; plane < end; plane +=step)
				*buffer++ = In_Image[Weights_Map[plane+j].position];
	}
	else
	{
//		for (j=0; j<Pix_Numb*Size; j+=Pix_Numb)
		for (j=0; j<height; j++)
		{
			for (k=0; k<Pix_Numb*width; k+=Pix_Numb)
			{
				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_R[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_R[j*Pix_Numb*width+k  +i].position];
				*buffer = (unsigned char)(Sum*0.212671);

				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_G[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_G[j*Pix_Numb*width+k  +i].position];
				*buffer += (unsigned char)(Sum*0.715160);

				Sum = 0.0;
				for (i=0; i<Pix_Numb; i++)
					Sum += Weights_Map_B[j*Pix_Numb*width+k  +i].weight*In_Image[Weights_Map_B[j*Pix_Numb*width+k  +i].position];
				*buffer += (unsigned char)(Sum*0.072169);

				buffer++;
			}
			buffer+=PadShift;
		}
	}
}

/************************************************************************
* Remap					  												*
************************************************************************/
	
void Remap(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT)
{
	int i,j;
//	int max;
	const int SizeRemapX = Par->Size_X_Remap;
	const int SizeRemapY = Par->Size_Y_Remap;
	int PadSize = computePadSize(Par->Size_X_Remap * 3,Par->padding) - 3 * SizeRemapX;
//	const int SizeRemap  = PadSize * Par->Size_Y_Remap;

	In_Image[0] = 0; // 192;
	In_Image[1] = 0; // 192;
	In_Image[2] = 0; // 192;
	//const int Size = Par->Size_X_Remap * Par->Size_Y_Remap;
//	int CenterX = Par->Center_X_Remap;
//	int CenterY = Par->Center_Y_Remap;

//	int starti = (Par->Resolution/2)+CenterX-(SizeRemapX/2);
//	int startj = (Par->Resolution/2)-CenterY-(SizeRemapY/2);
//	int endi = (Par->Resolution - starti);
//	int endj = (Par->Resolution - startj);

	unsigned char * RemImgPtr = Out_Image;
	int * LPImgPtr = Rem_LUT;

///	for (j=0; j<SizeRemapY; j++)
//	for (j=0; j<SizeRemap; j++)
	for (j=0; j<SizeRemapY; j++)
	{
//		LPImgPtr = &Rem_LUT[(j+startj)*Par->Resolution+starti];
		for (i=0; i<SizeRemapX; i++)
///		{
//			if (*LPImgPtr>max)
//				max = *LPImgPtr;
///			if (*LPImgPtr!=-1)
///			{
//				*RemImgPtr++ = In_Image[*LPImgPtr++];
		{
				*RemImgPtr++ = In_Image[*LPImgPtr];
				*RemImgPtr++ = In_Image[(*LPImgPtr)+1];
				*RemImgPtr++ = In_Image[(*LPImgPtr++)+2];
		}
		RemImgPtr += PadSize;
//			}
//			else
//			{
//				*RemImgPtr++ = 0;
//				*RemImgPtr++ = 0;
//				*RemImgPtr++ = 0;
//				LPImgPtr++;
//			}
//		}
	}
}
/************************************************************************
* Remap					  												*
************************************************************************/
	
void Remap_Mono(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT)
{
	int j;
	const int SizeRemap  = Par->Size_Img_Remap;

	In_Image[0] = 192;
	unsigned char * RemImgPtr = Out_Image;
	int * LPImgPtr = Rem_LUT;

	for (j=0; j<SizeRemap; j++)
		*RemImgPtr++ = In_Image[(*LPImgPtr++)/3];
}
long Get_Time()
{
#if !defined(__QNX__) && !defined(__LINUX__)

	LARGE_INTEGER perfTime;
	LARGE_INTEGER perfFreq;
	long l_tempo;
	double d_tempo;
	time_t offset_ora;

        QueryPerformanceFrequency(&perfFreq);
		time(&offset_ora);//secs from Jan 1st, 1970
		QueryPerformanceCounter(&perfTime);
	    d_tempo = (double)perfTime.QuadPart/(double)perfFreq.QuadPart;//Elapsed Time (secs from boot)
		l_tempo = (long)(d_tempo * 1000.0);//(msecs from boot)

		return l_tempo;
#else
		return -1;
#endif    
}

void Set_Calibration(Image_Data * Par, int PortAddress)
{
#if !defined(__QNX__) && !defined(__LINUX__)

	const int NUM_IMG = 10;
	int i,j,k,val;
	unsigned char *cl[NUM_IMG];
	int * CalibImage;
	unsigned char * buffer;
	struct timeval elap_time;

	int CNumb;

	CalibImage = (int *)malloc(Par->Size_LP * sizeof(int));
	buffer = (unsigned char *)malloc(Par->Size_LP * sizeof(unsigned char));

	// Aquire NUM_IMG images for calibration
	for(i=0;i<NUM_IMG;i++){
		cl[i]=(unsigned char *)calloc(Par->Size_LP,sizeof(unsigned char));
		if(cl[i]==NULL)
			return;

#ifdef WINNT
		Giotto_Acq(buffer,
				  &elap_time,
				  0,
				  &CNumb,
				  4);
#else
		Giotto_Acq_Wrapper(buffer,
				  &elap_time,
				  0,
				  &CNumb,
				  PortAddress);
#endif

		Sleep(100);
		memcpy(cl[i],buffer,Par->Size_LP);
	}

	for(i=0; i < Par->Size_Rho ; i++){
		for(j=0;j < Par->Size_Theta ; j++){
			val=0;
			for(k=0;k<NUM_IMG;k++)
				val+=(int)(*(cl[k]+i*Par->Size_Theta+j));
			
			*(CalibImage+j*Par->Size_Theta+i)=(unsigned char)(((double)val/(double)NUM_IMG)+(double)0.5);
		}
	}

	for(i=0;i<NUM_IMG;i++)
		free(cl[i]);
#else
	//// QNX, Unimplemented.
#endif
}

/************************************************************************
* RGB_2_YUV																*
************************************************************************/	

void RGB_2_YUV(unsigned char * OutImageYUV,
			   unsigned char * InImageRGB,
			   int Size_Rho,
			   int Size_Theta)
{
	int i,j;
	int Hor_Size;
	int Vert_Size;
	double Y_Weights[3];
	double U_Weights[3];
	double V_Weights[3];

	unsigned char * OutImageY;
	unsigned char * OutImageU;
	unsigned char * OutImageV;

	if (Size_Theta > 176)
	{
		Hor_Size  = 352;
		Vert_Size = 288;
	}
	else
	{
		Hor_Size  = 176;
		Vert_Size = 144;
	}


	Y_Weights [RED] = 0.25678824;
	Y_Weights [GREEN] = 0.50412941;
	Y_Weights [BLUE] = 0.09790588;
	U_Weights [RED] = 0.43921569;
	U_Weights [GREEN] = -0.36778824;
	U_Weights [BLUE] = -0.07142745;
	V_Weights [RED] = -0.14822353;
	V_Weights [GREEN] = -0.29099216;
	V_Weights [BLUE] = 0.43921569;

	OutImageY =  OutImageYUV;
	OutImageU = &OutImageYUV[Hor_Size*Vert_Size];
	OutImageV = &OutImageYUV[5*Hor_Size*Vert_Size/4];

	for (j=0; j<Size_Rho; j+=2)
	{
		for (i=0; i<Size_Theta; i+=2)
		{
			OutImageY[j*Hor_Size+i] = (int)
											  (Y_Weights[RED]   * InImageRGB[3*(j*Size_Theta+i)+0] + 
											   Y_Weights[GREEN] * InImageRGB[3*(j*Size_Theta+i)+1] +
											   Y_Weights[BLUE]  * InImageRGB[3*(j*Size_Theta+i)+2] +  16.5);

			OutImageY[j*Hor_Size+i+1] = (int)
											    (Y_Weights[RED]   * InImageRGB[3*(j*Size_Theta+i+1)+0] + 
											     Y_Weights[GREEN] * InImageRGB[3*(j*Size_Theta+i+1)+1] +
											     Y_Weights[BLUE]  * InImageRGB[3*(j*Size_Theta+i+1)+2] +  16.5);

			OutImageY[(j+1)*Hor_Size+i] = (int)
											    (Y_Weights[RED]   * InImageRGB[3*((j+1)*Size_Theta+i)+0] + 
											     Y_Weights[GREEN] * InImageRGB[3*((j+1)*Size_Theta+i)+1] +
											     Y_Weights[BLUE]  * InImageRGB[3*((j+1)*Size_Theta+i)+2] +  16.5);

			OutImageY[(j+1)*Hor_Size+i+1] = (int)
											      (Y_Weights[RED]   * InImageRGB[3*((j+1)*Size_Theta+i+1)+0] + 
											       Y_Weights[GREEN] * InImageRGB[3*((j+1)*Size_Theta+i+1)+1] +
											       Y_Weights[BLUE]  * InImageRGB[3*((j+1)*Size_Theta+i+1)+2] +  16.5);

			OutImageU[(j/2)*Hor_Size/2+i/2] = (int)
									  (U_Weights[RED]   * InImageRGB[3*(j*Size_Theta+i)+0] + 
									   U_Weights[GREEN] * InImageRGB[3*(j*Size_Theta+i)+1] +
									   U_Weights[BLUE]  * InImageRGB[3*(j*Size_Theta+i)+2] + 128.5);

			OutImageV[(j/2)*Hor_Size/2+i/2] = (int)
									  (V_Weights[RED]   * InImageRGB[3*(j*Size_Theta+i)+0] + 
									   V_Weights[GREEN] * InImageRGB[3*(j*Size_Theta+i)+1] +
									   V_Weights[BLUE]  * InImageRGB[3*(j*Size_Theta+i)+2] + 128.5);
		}
		for (i=Size_Theta; i<Hor_Size; i+=2)
		{
			OutImageY[j*Hor_Size+i] = 128;
			OutImageY[j*Hor_Size+i+1] = 128;
			OutImageY[(j+1)*Hor_Size+i] = 128;
			OutImageY[(j+1)*Hor_Size+i+1] = 128;
			OutImageU[(j/2)*Hor_Size/2+(i/2)] = 128;
			OutImageV[(j/2)*Hor_Size/2+(i/2)] = 128;
		}
	}

	for (j=Size_Rho; j<Vert_Size; j+=2)
		for (i=0; i<Hor_Size; i+=2)
		{
			OutImageY[j*Hor_Size+i] = 128;
			OutImageY[j*Hor_Size+i+1] = 128;
			OutImageY[(j+1)*Hor_Size+i] = 128;
			OutImageY[(j+1)*Hor_Size+i+1] = 128;
			OutImageU[(j/2)*Hor_Size/2+(i/2)] = 128;
			OutImageV[(j/2)*Hor_Size/2+(i/2)] = 128;
		}
}


/************************************************************************
* YUV_2_RGB																*
************************************************************************/	

void YUV_2_RGB(unsigned char * InImageYUV,
						unsigned char * OutImageRGB,
						Image_Data * Par)
{
	double cy1 =  1.16438356;
	double cy2 =  1.16438356;
	double cy3 =  1.16438356;
	double cu1 =  0.00000000;
	double cu2 = -0.39176254;
	double cu3 =  2.01723264;
	double cv1 =  1.59602689;
	double cv2 = -0.81296829;
	double cv3 =  0.00000000;

	enum planes {
					Y,
					U,
					V
	};

	int i,j;
	int Hor_Size;
	int Vert_Size;
	int Size;
	double R_Weights[3];
	double G_Weights[3];
	double B_Weights[3];

	unsigned char * InImageY;
	unsigned char * InImageU;
	unsigned char * InImageV;

	if (Par->Size_Theta > 176)
	{
		Hor_Size  = 352;
		Vert_Size = 288;
	}
	else
	{
		Hor_Size  = 176;
		Vert_Size = 144;
	}

	Size = Hor_Size * Vert_Size/4;

	R_Weights [Y] = 1.16438356;
	R_Weights [U] = 1.59602689;
	R_Weights [V] = 0.00000000;
	G_Weights [Y] = 1.16438356;
	G_Weights [U] =-0.81296829;
	G_Weights [V] =-0.39176254;
	B_Weights [Y] = 1.16438356;
	B_Weights [U] = 0.00000000;
	B_Weights [V] = 2.01723264;

	InImageY =  InImageYUV;
	InImageU = &InImageYUV[Hor_Size*Vert_Size];
	InImageV = &InImageYUV[(5/4)*Hor_Size*Vert_Size];

	for (j=0; j<Par->Size_Rho; j++)
	{
		for (i=0; i<Par->Size_Theta; i++)
		{
			OutImageRGB[3*(j*Par->Size_Theta+i)] = (int)
												   (R_Weights[Y] * (InImageYUV[j*Hor_Size+i]			   - 16) + 
												    R_Weights[U] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+4*Size]  -128) +
												    R_Weights[V] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+5*Size]-128));

			OutImageRGB[3*(j*Par->Size_Theta+i)+1] = (int)
												   (G_Weights[Y] * (InImageYUV[j*Hor_Size+i]			   - 16) + 
												    G_Weights[U] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+4*Size]  -128) +
												    G_Weights[V] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+5*Size]-128));

			OutImageRGB[3*(j*Par->Size_Theta+i)+2] = (int)
												   (B_Weights[Y] * (InImageYUV[j*Hor_Size+i]			   - 16) + 
												    B_Weights[U] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+4*Size]  -128) +
												    B_Weights[V] * (InImageYUV[(j/2)*Hor_Size/2+(i/2)+5*Size]-128));

		}
	}
}

void Simulate_Camera(unsigned char * InCartImage,
				unsigned char * OutLPImage,
				Image_Data * Par,
				char * Path,
				int flag)
{
	int i,j,x,y;
	int rho,theta;
	double Act_ZL = Par->Zoom_Level;

	double * Ang_Shift_Map;
	char * Color_Map;
	unsigned short * Pad_Map;
	char File_Name [256];

	struct RLP
	{
		int value;
		int counter;
	};

	RLP * ImageArray;

	FILE * fin;
	
	Par->Zoom_Level = ((double)Par->Size_X_Orig/(double)Par->Resolution);

	ImageArray = (RLP *)calloc(3*Par->Size_LP , sizeof(RLP));

	Pad_Map = (unsigned short *)malloc(Par->Size_Theta * Par->Size_Fovea * sizeof(unsigned short));

	sprintf(File_Name,"%s%s",Path,"PadMap.gio");

	fin = fopen(File_Name,"rb");
	fread(Pad_Map,sizeof(unsigned short),Par->Size_Theta * Par->Size_Fovea,fin);
	fclose(fin);

	Ang_Shift_Map = (double *)calloc(Par->Size_Rho,sizeof(double));
	sprintf(File_Name,"%s%s",Path,"AngularShiftMap.gio");

	fin = fopen(File_Name,"rb");
	fread(Ang_Shift_Map,sizeof(double),Par->Size_Rho,fin);
	fclose(fin);

	Color_Map = (char *)malloc(Par->Size_LP * sizeof(char));

	sprintf(File_Name,"%s%s",Path,"ColorMap.gio");

	fin = fopen(File_Name,"rb");
	fread(Color_Map,sizeof(char),Par->Size_LP,fin);
	fclose(fin);

	if (flag == SIMULATE)
	{
		for (j=0; j<Par->Size_Rho; j++)
	//	for (j=0; j<Par->Size_Fovea; j++)
			for (i=0; i<Par->Size_Theta; i++)
			{
				x = (int)(Get_X_Center(j,i,Par,Ang_Shift_Map,Pad_Map));
				y = (int)(Get_Y_Center(j,i,Par,Ang_Shift_Map,Pad_Map));
				if (j<Par->Size_Fovea)
				{
					OutLPImage[Pad_Map[j*Par->Size_Theta+i]] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+Color_Map[Pad_Map[j*Par->Size_Theta+i]]];
	//				OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+0] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+0];
	//				OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+1] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+1];
	//				OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+2] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+2];
				}
				else
				{
					OutLPImage[j*Par->Size_Theta+i] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+Color_Map[j*Par->Size_Theta+i]];
	//				OutLPImage[3*(j*Par->Size_Theta+i)+0] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+0];
	//				OutLPImage[3*(j*Par->Size_Theta+i)+1] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+1];
	//				OutLPImage[3*(j*Par->Size_Theta+i)+2] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+2];
				}
			}
	}
	else
	{
		for (j=0; j<Par->Size_Rho; j++)
			for (i=0; i<Par->Size_Theta; i++)
			{
				x = (int)(Get_X_Center(j,i,Par,Ang_Shift_Map,Pad_Map));
				y = (int)(Get_Y_Center(j,i,Par,Ang_Shift_Map,Pad_Map));
				if (j<Par->Size_Fovea)
				{
					OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+0] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+0];
					OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+1] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+1];
					OutLPImage[3*(Pad_Map[j*Par->Size_Theta+i])+2] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+2];
				}
				else
				{
					OutLPImage[3*(j*Par->Size_Theta+i)+0] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+0];
					OutLPImage[3*(j*Par->Size_Theta+i)+1] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+1];
					OutLPImage[3*(j*Par->Size_Theta+i)+2] = InCartImage[3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+(x+Par->Size_X_Orig/2))+2];
				}
			}
	}


	for (j=0; j<Par->Size_Y_Orig; j++)
		for (i=0; i<Par->Size_X_Orig; i++)
		{
			rho = Get_Rho(i-Par->Size_X_Orig/2,Par->Size_Y_Orig/2-j,Par);
			theta = Get_Theta(i-Par->Size_X_Orig/2,Par->Size_Y_Orig/2-j,rho,Par,Ang_Shift_Map,Pad_Map);
			if ((rho>0)&&(rho<Par->Size_Rho))
			{
				ImageArray[3*(rho*Par->Size_Theta+theta)+0].value += InCartImage[3*((j)*Par->Size_X_Orig+(i))+0];
				ImageArray[3*(rho*Par->Size_Theta+theta)+0].counter ++;
				ImageArray[3*(rho*Par->Size_Theta+theta)+1].value += InCartImage[3*((j)*Par->Size_X_Orig+(i))+1];
				ImageArray[3*(rho*Par->Size_Theta+theta)+1].counter ++;
				ImageArray[3*(rho*Par->Size_Theta+theta)+2].value += InCartImage[3*((j)*Par->Size_X_Orig+(i))+2];
				ImageArray[3*(rho*Par->Size_Theta+theta)+2].counter ++;
			}
		}

	if (flag == SIMULATE)
	{
		for (j=0; j<Par->Size_LP; j++)
			if (ImageArray[3*j+Color_Map[j]].counter!=0)
				OutLPImage[j] = ImageArray[3*j+Color_Map[j]].value/ImageArray[3*j+Color_Map[j]].counter;
	}
	else
	{
		for (j=0; j<3*Par->Size_LP; j++)
			if (ImageArray[j].counter!=0)
				OutLPImage[j] = ImageArray[j].value/ImageArray[j].counter;
	}

	Par->Zoom_Level = Act_ZL;

/*
	for (j=0; j<252*42; j++)
	{
		OutLPImage[3*j] = OutLPImage[3*Pad_Map[j]];
		OutLPImage[3*j+1] = OutLPImage[3*Pad_Map[j]+1];
		OutLPImage[3*j+2] = OutLPImage[3*Pad_Map[j]+2];
	}
*/
}



int Make_LP_Real (unsigned char * Output_Image, unsigned char * Input_Image, Image_Data * Par, Cart2LPInterp * Cart2LP_Map)
{
	int i,k,l;
	int sumR,sumG,sumB;
	Cart2LPInterp * TablePtr = Cart2LP_Map;	
	int PadSizeTheta = (((Par->Size_Theta * Par->LP_Planes) % Par->padding) + (Par->Size_Theta * Par->LP_Planes));
	unsigned int max = 0;

	Input_Image[0] = 0;
	Input_Image[1] = 0;
	Input_Image[2] = 0;

	//Check Table:
	const int rho = Par->Size_Rho;
	const int theta = Par->Size_Theta;
	const int pad = (Par->Size_Theta * Par->LP_Planes) % Par->padding;
	unsigned char *tmp = Output_Image;

	for (k = 0; k < rho; k++)
	{
		for (l = 0; l < theta; l++)
		{
			sumR = 0;
			sumG = 0;
			sumB = 0;
			const int nopix = TablePtr[0].NofPixels;
			unsigned int *pos = TablePtr[0].position;
			for (i=0; i < nopix; i++, pos++)
			{
				sumR += Input_Image[*pos];
				sumG += Input_Image[*pos+1];
				sumB += Input_Image[*pos+2];
			}

			*Output_Image++ = sumR/nopix;
			*Output_Image++ = sumG/nopix;
			*Output_Image++ = sumB/nopix;
			TablePtr++;
		}

		Output_Image += pad;
	}
	
	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;

	return 0;
}

/************************************************************************
* Get_X_Center  														*
************************************************************************/	

double Get_X_Center(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{

	double scalefactor;
	int Temp_Size_Theta;
	double x,A,B;
	double mod;
//	int intmod;
	bool found = true;

	int j;

	
	if ((int)(rho) != 0)
	{
		rho   += 0.5;
		theta += 0.5;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}

	scalefactor = par->Zoom_Level;
	B = par->Log_Index/(par->Log_Index-1);
	A = par->Size_Fovea - B - 0.5;

	if (rho<par->Size_Fovea)
	{
		if (par->Fovea_Type == 0)
		{
			found = false;
			for (j=0; j<par->Size_Theta;j++)
			{
				if (PadMap[(int)(rho)*par->Size_Theta+j]%par->Size_Theta == (int) (theta))
					if (PadMap[(int)(rho)*par->Size_Theta+j]!=1) //Non Valid Pixel
					{
						theta = j+0.5;
						found = true;
						break;
					}

			}

			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;

			if ((int)rho<1)
			{
				Temp_Size_Theta = 1;
				mod = 0;
			}
			else 
				mod = rho-0.5;
		}
		else if (par->Fovea_Type == 1)
		{
			if (PadMap[(int)(rho)*par->Size_Theta+(int)theta]!=1)
				Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;
			else found = false;

			if ((int)rho<1)
			{
				Temp_Size_Theta = 1;
				mod = 0;
			}
			else
				mod = rho - 0.5;
		}
		else //if (par->Fovea_Type == 2)
		{
			Temp_Size_Theta = par->Size_Theta;
			found = true;
			if ((int)rho<1)
				mod = 0;
			else mod = rho-0.5;
		}
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rho-par->Size_Fovea);
	}
	if (Temp_Size_Theta>par->Size_Theta)
		Temp_Size_Theta = par->Size_Theta;

	x = mod * cos(+Ang_Shift[(int)rho]+theta*PI/(Temp_Size_Theta/2.0));

	if (found)
		return x*scalefactor;
	else return 2*par->Size_X_Remap;


}/************************************************************************
* Get_Y_Center  														*
************************************************************************/	

double Get_Y_Center(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{

	double scalefactor;
	int Temp_Size_Theta;
	double y,A,B;
	double mod;
//	int intmod;
	bool found = true;

	int j;

	
	if ((int)(rho) != 0)
	{
		rho   += 0.5;
		theta += 0.5;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->Resolution,par->Size_Fovea,par->Size_Rho);
		par->Valid_Log_Index = true;
	}

	scalefactor = par->Zoom_Level;
	B = par->Log_Index/(par->Log_Index-1);
	A = par->Size_Fovea - B - 0.5;

	if (rho<par->Size_Fovea)
	{
		if (par->Fovea_Type == 0)
		{
			found = false;
			for (j=0; j<par->Size_Theta;j++)
			{
				if (PadMap[(int)(rho)*par->Size_Theta+j]%par->Size_Theta == (int) (theta))
					if (PadMap[(int)(rho)*par->Size_Theta+j]!=1)
					{
						theta = j+0.5;
						found = true;
						break;
					}

			}

			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;
			
			if ((int)rho<1)
			{
				Temp_Size_Theta = 1;
				mod = 0;
			}
			else 
				mod = rho-0.5;
		}
		else if (par->Fovea_Type == 1)
		{
			if (PadMap[(int)(rho)*par->Size_Theta+(int)theta]!=1)
				Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * (int)rho;
			else found = false;

			if ((int)rho<1)
			{
				Temp_Size_Theta = 1;
				mod = 0;
			}
			else
				mod = rho - 0.5;
		}
		else //if (par->Fovea_Type == 2)
		{
			Temp_Size_Theta = par->Size_Theta;
			found = true;
			if ((int)rho<1)
				mod = 0;
			else mod = rho-0.5;
		}
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rho-par->Size_Fovea);
	}
	if (Temp_Size_Theta>par->Size_Theta)
		Temp_Size_Theta = par->Size_Theta;

	y = mod * sin(+Ang_Shift[(int)rho]+theta*PI/(Temp_Size_Theta/2.0));

	if (found)
		return y*scalefactor;
	else return 2*par->Size_Y_Remap;
}

/************************************************************************
* Fast_Reconstruct_Color  												*
************************************************************************/	

void Fast_Reconstruct_Color(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   IntNeighborhood * WeightsMap,
					   int Pix_Numb)
{
	const int limit = height*width*3;
	const int Nopix = Pix_Numb;
	int i,j;
	unsigned char Sum;
	unsigned char * OI = Out_Image;

	for (j=0; j<limit; j++)
	{
		Sum = 0;
		for (i=0; i<WeightsMap[j].NofPixels; i++)
//		for (i=0; i<Nopix; i++)
			Sum += In_Image [WeightsMap[j].position[i]]>> WeightsMap[j].weight[i];

		*OI++ = Sum;

	}
}


int Shift_and_Corr (unsigned char * Left, unsigned char * Right, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val)//, int * pixCount)
{
	int i,j,k,k1;
	int iR,iL;
//	int ShiftLev;
	double d_1;
	double d_2;
//	double corr_val;
	double MIN = 10000;
	int MAX = 0;
	int minindex;
	int count;


	unsigned char * Lptr,* Rptr;

	Lptr = Left;
	Rptr = Right;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{
		double average_Lr = 0;
		double average_Lg = 0;
		double average_Lb = 0;
		double average_Rr = 0;
		double average_Rg = 0;
		double average_Rb = 0;

//		ShiftLev = StepFunct[k+1] + 3 * Par->Resolution/4; // = 75% of the image
//old		k1 = k * 3 * Par->Size_LP; //Positioning on the table
		k1 = k * 1 * Par->Size_LP; //Positioning on the table
		Lptr = Left;

		count = 0;

		for (j=0; j<Par->Size_Rho; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
//old			iR = ShiftMap[k1 + 3 * i];
				iR = ShiftMap[k1 + j*Par->Size_Theta+i];
				iL = 3 * (j*Par->Size_Theta+i);
				if (iR > 0)
				{
					average_Lr += *Lptr++;//Left [iL];
					average_Rr += Right[iR];
					average_Lg += *Lptr++;//Left [iL+1];
					average_Rg += Right[iR+1];
					average_Lb += *Lptr++;//Left [iL+2];
					average_Rb += Right[iR+2];
					count++;
				}
				else Lptr +=3;
			}
			Lptr += AddedPadSize;
		}
		
//		if (pixCount[k] != 0)
		if (count != 0)
			{
				average_Lr /= count;
				average_Rr /= count;
				average_Lg /= count;
				average_Rg /= count;
				average_Lb /= count;
				average_Rb /= count;
			}

			double numr   = 0;
			double den_1r = 0;
			double den_2r = 0;
			double numg   = 0;
			double den_1g = 0;
			double den_2g = 0;
			double numb   = 0;
			double den_1b = 0;
			double den_2b = 0;

			Lptr = Left;

		for (j=0; j<Par->Size_Rho; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
					iR = ShiftMap[k1 + 3 * i];
					iL = 3 * i;
					iR = ShiftMap[k1 + j*Par->Size_Theta+i];
					iL = 3 * (j*Par->Size_Theta+i);

					if (iR > 0)
					{
						d_1 = *Lptr++ - average_Lr;
						d_2 = Right[iR] - average_Rr;
						numr   += (d_1 * d_2);
						den_1r += (d_1 * d_1);
						den_2r += (d_2 * d_2);

						d_1 = *Lptr++ - average_Lg;
						d_2 = Right[iR+1] - average_Rg;
						numg   += (d_1 * d_2);
						den_1g += (d_1 * d_1);
						den_2g += (d_2 * d_2);

						d_1 = *Lptr++ - average_Lb;
						d_2 = Right[iR+2] - average_Rb;
						numb   += (d_1 * d_2);
						den_1b += (d_1 * d_1);
						den_2b += (d_2 * d_2);
					}
					else Lptr +=3;
				}
			Lptr += AddedPadSize;
		}
			corr_val[k]  = (1.0 - (numr * numr) / (den_1r * den_2r + 0.00001));	
			corr_val[k] += (1.0 - (numg * numg) / (den_1g * den_2g + 0.00001));	
			corr_val[k] += (1.0 - (numb * numb) / (den_1b * den_2b + 0.00001));	

			if (corr_val[k]<MIN)
			{
				MIN = corr_val[k];
				minindex = k;
			}
			if (count>MAX)
				MAX = count;
//		printf("%03d     %2.5f\n",k-SParam.Resolution/2,corr_val);
			corr_val[k] = (3-corr_val[k])*count;
		}
	
	for (k=0; k<Steps; k++)
		corr_val[k] = 3-(corr_val[k]/(double)MAX);
//		corr_val[k] = 3-((3-corr_val[k])*pixCount[k]/(double)MAX);

	return minindex;
}


int shiftnCorrFovea (unsigned char * Left, unsigned char * Right, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val)
{
	int i,j,k,k1;
	int iR,iL;
	int count;
	double d_1;
	double d_2;
	double MIN = 10000;
	int minindex;
//aggiungere il check sul fatto che si sia in fovea o fuori (step function)
	unsigned char * Lptr,* Rptr;

	Lptr = Left;
	Rptr = Right;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{
		double average_Lr = 0;
		double average_Lg = 0;
		double average_Lb = 0;
		double average_Rr = 0;
		double average_Rg = 0;
		double average_Rb = 0;

		k1 = k * 1 * Par->Size_LP; //Positioning on the table
		Lptr = Left;

		count = 0;

		for (j=0; j<Par->Size_Fovea; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
				iR = ShiftMap[k1 + j*Par->Size_Theta+i];
				iL = 3 * (j*Par->Size_Theta+i);
				if (iR > 0)
				{
					average_Lr += *Lptr++;//Left [iL];
					average_Rr += Right[iR];
					average_Lg += *Lptr++;//Left [iL+1];
					average_Rg += Right[iR+1];
					average_Lb += *Lptr++;//Left [iL+2];
					average_Rb += Right[iR+2];
					count++;
				}
				else Lptr +=3;
			}
			Lptr += AddedPadSize;
		}
		
		if (count != 0)
			{
				average_Lr /= count;
				average_Rr /= count;
				average_Lg /= count;
				average_Rg /= count;
				average_Lb /= count;
				average_Rb /= count;
			}

			double numr   = 0;
			double den_1r = 0;
			double den_2r = 0;
			double numg   = 0;
			double den_1g = 0;
			double den_2g = 0;
			double numb   = 0;
			double den_1b = 0;
			double den_2b = 0;

			Lptr = Left;

		for (j=0; j<Par->Size_Fovea; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
					iR = ShiftMap[k1 + 3 * i];
					iL = 3 * i;
					iR = ShiftMap[k1 + j*Par->Size_Theta+i];
					iL = 3 * (j*Par->Size_Theta+i);

					if (iR > 0)
					{
						d_1 = *Lptr++ - average_Lr;
						d_2 = Right[iR] - average_Rr;
						numr   += (d_1 * d_2);
						den_1r += (d_1 * d_1);
						den_2r += (d_2 * d_2);

						d_1 = *Lptr++ - average_Lg;
						d_2 = Right[iR+1] - average_Rg;
						numg   += (d_1 * d_2);
						den_1g += (d_1 * d_1);
						den_2g += (d_2 * d_2);

						d_1 = *Lptr++ - average_Lb;
						d_2 = Right[iR+2] - average_Rb;
						numb   += (d_1 * d_2);
						den_1b += (d_1 * d_1);
						den_2b += (d_2 * d_2);
					}
					else Lptr +=3;
				}
			Lptr += AddedPadSize;
		}
			corr_val[k]  = (1.0 - (numr * numr) / (den_1r * den_2r + 0.00001));	
			corr_val[k] += (1.0 - (numg * numg) / (den_1g * den_2g + 0.00001));	
			corr_val[k] += (1.0 - (numb * numb) / (den_1b * den_2b + 0.00001));	

			if (corr_val[k]<MIN)
			{
				MIN = corr_val[k];
				minindex = k;
			}
		}

	return minindex;
}

void Make_Disp_Histogram(unsigned char * hist,int height, int width, int shiftLevels, double * corrFunct)
{
	int i,j;
//	int height = 64;
//	int width = 256;
//	unsigned char * hist = (unsigned char *)malloc(height*width*sizeof(unsigned char));

	int offset = (width-shiftLevels+1)/2;

	for (j=0;j<height*width;j++)
		hist[j] = 0;

	for (i=0; i<shiftLevels-1; i++)
		if ((i+offset >=0)&&(i+offset<width))
			for (j=height-(int)(height/3.0*(3-corrFunct[i])); j<height; j++)
//			for (j=0;j<height*(corrFunct[i]-2); j++)
					hist[(j*width+i+offset)] = 128;
		
	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;
}