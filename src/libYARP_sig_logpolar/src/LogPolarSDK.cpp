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
/// $Id: LogPolarSDK.cpp,v 1.3 2004-08-04 15:23:25 orfra Exp $
///
///

#include <stdio.h>
#include <stdlib.h>

#if !defined(__QNX__) && !defined(__QNX6__) && !defined(__LINUX__)

///#include <conio.h>
///#include <io.h>
#include <windows.h>
#else
#include <string.h>
#endif

#include <math.h>
///#include <fcntl.h>

#include <yarp/LogPolarSDK.h>

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
	image.Log_Index = 1.0;

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
	image.dres = (double) image.Resolution;
	image.Fovea_Display_Mode = 0;

	return image;
}


double Compute_Index(double Resolution, int Fovea, int SizeRho)
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
		rho = (int)(radius+0.51);

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
* Get_XY_Center_Uniform													*
************************************************************************/
int Get_XY_Center_Uniform(double *xx, double *yy, int rho, int theta, Image_Data *par, double *Ang_Shift)
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
		/*else if (par->Fovea_Display_Mode < 2)
			Temp_Size_Theta = (par->Size_Theta/par->Size_Fovea) * rho;*/
	}
	else
	{
		Temp_Size_Theta = par->Size_Theta;
		mod = A+B*pow(par->Log_Index,rd-par->Size_Fovea);
	}
		/*if (Temp_Size_Theta>par->Size_Theta)
			Temp_Size_Theta = par->Size_Theta;*/

	*xx = mod * cos(Ang_Shift[rho]+td*PI/(Temp_Size_Theta/2.0)) * scalefactor;
	*yy = mod * sin(Ang_Shift[rho]+td*PI/(Temp_Size_Theta/2.0)) * scalefactor;

	return 0;
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
	const int SizeRemapX = Par->Size_X_Remap;
	const int SizeRemapY = Par->Size_Y_Remap;
	int PadSize = computePadSize(Par->Size_X_Remap * 3,Par->padding) - 3 * SizeRemapX;

	In_Image[0] = 0; // 192;
	In_Image[1] = 0; // 192;
	In_Image[2] = 0; // 192;

	unsigned char * RemImgPtr = Out_Image;
	int * LPImgPtr = Rem_LUT;

	for (j=0; j<SizeRemapY; j++)
	{
		for (i=0; i<SizeRemapX; i++)
		{
				*RemImgPtr++ = In_Image[*LPImgPtr];
				*RemImgPtr++ = In_Image[(*LPImgPtr)+1];
				*RemImgPtr++ = In_Image[(*LPImgPtr++)+2];
		}
		RemImgPtr += PadSize;
	}
}

/************************************************************************
* Remap_Mono				  											*
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
	bool found = true;
	int j;

	if (rho>=0.5)
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


}
/************************************************************************
* getXfloatRes  														*
************************************************************************/	

double getXfloatRes(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{

	double scalefactor;
	int Temp_Size_Theta;
	double x,A,B;
	double mod;
	bool found = true;
	int j;

	if (rho>=0.5)
	{
		rho   += 0.5;
		theta += 0.5;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->dres,par->Size_Fovea,par->Size_Rho);
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
}


/************************************************************************
* Get_Y_Center  														*
************************************************************************/	
double Get_Y_Center(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{
	double scalefactor;
	int Temp_Size_Theta;
	double y,A,B;
	double mod;
	bool found = true;
	int j;

	if (rho>=0.5)
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
* getYfloatRes  														*
************************************************************************/	

double getYfloatRes(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap)
{

	double scalefactor;
	int Temp_Size_Theta;
	double y,A,B;
	double mod;
	bool found = true;
	int j;
	
	if (rho>=0.5)
	{
		rho   += 0.5;
		theta += 0.5;
	}

	if (!par->Valid_Log_Index){
		par->Log_Index = Compute_Index(par->dres,par->Size_Fovea,par->Size_Rho);
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
			Sum += In_Image [WeightsMap[j].position[i]]>> WeightsMap[j].weight[i];

		*OI++ = Sum;

	}
}

void shiftnCorrFoveaRGB (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double *corr_val, double *phase, double *coeff, rgbPixel aFull, rgbPixel aFov,int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double rxy[4];
	double rxx[4];
	double ryy[4];
	int pX[3];
	int pY[3];

	memset(rxx, 0, 4*sizeof(double));
	memset(ryy, 0, 4*sizeof(double));
	memset(rxy, 0, 4*sizeof(double));

	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{
		// reset counters
		memset(rxx, 0, 4*sizeof(double));
		memset(ryy, 0, 4*sizeof(double));
		memset(rxy, 0, 4*sizeof(double));

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}

		}

		fovPtr = fovImg;
		fullPtr = fullImg;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pX[0] = *fovPtr++ - aFov.Red;
						pY[0] = fullPtr[i2] - aFull.Red;

						pX[1] = *fovPtr++ - aFov.Gre;
						pY[1] = fullPtr[i2+1] - aFull.Gre;
						
						pX[2] = *fovPtr++ - aFov.Blu;
						pY[2] = fullPtr[i2+2] - aFull.Blu;

					
						rxy[0] += (-pX[0]*pY[0] - pX[1]*pY[1] - pX[2]*pY[2]);
						rxy[1] += (pX[1]*pY[2] - pX[2]*pY[1]);
						rxy[2] += (pX[0]*pY[1] - pX[1]*pY[0]);
						rxy[3] += (pX[2]*pY[0] - pX[0]*pY[2]);

						rxx[0] += (-pX[0]*pX[0] - pX[1]*pX[1] - pX[2]*pX[2]);
						rxx[1] += (pX[1]*pX[2] - pX[2]*pX[1]);
						rxx[2] += (pX[0]*pX[1] - pX[1]*pX[0]);
						rxx[3] += (pX[2]*pX[0] - pX[0]*pX[2]);

						ryy[0] += (-pY[0]*pY[0] - pY[1]*pY[1] - pY[2]*pY[2]);
						ryy[1] += (pY[1]*pY[2] - pY[2]*pY[1]);
						ryy[2] += (pY[0]*pY[1] - pY[1]*pY[0]);
						ryy[3] += (pY[2]*pY[0] - pY[0]*pY[2]);
					}
					else fovPtr +=3;
				}
			fovPtr+=AddedPadSize;
		}

		double absRxySq=rxy[0]*rxy[0] + rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3];  
		double absV = sqrt(rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3]);  
		double absRxxSq=rxx[0]*rxx[0] + rxx[1]*rxx[1] + rxx[2]*rxx[2] + rxx[3]*rxx[3];  
		double absRyySq=ryy[0]*ryy[0] + ryy[1]*ryy[1] + ryy[2]*ryy[2] + ryy[3]*ryy[3];  

		coeff[k]  = (absRxySq/(sqrt(absRxxSq)*sqrt(absRyySq)));
		phase[k] = (atan2(absV,rxy[0])/PI);
		corr_val[k] = coeff[k]*phase[k];
	}
}

int Shift_and_Corr (unsigned char * Left, unsigned char * Right, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, rgbPixel aL, rgbPixel aR)//, int * pixCount)
{
	int i,j,k,k1;
	int i2,i1;
	double d_1;
	double d_2;
	double MIN = 10000;
	int MAX = 0;
	int minindex;
	int count;


	unsigned char * img1ptr,* img2ptr;

	img1ptr = Right;
	img2ptr = Left;

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
		img1ptr = Left;

		count = 0;

		for (j=0; j<Par->Size_Rho; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + j*Par->Size_Theta+i];
				i1 = 3 * (j*Par->Size_Theta+i);
				if (i2 > 0)
				{
					average_Lr += *img1ptr++;//Left [i1];
					average_Rr += Right[i2];
					average_Lg += *img1ptr++;//Left [i1+1];
					average_Rg += Right[i2+1];
					average_Lb += *img1ptr++;//Left [i1+2];
					average_Rb += Right[i2+2];
					count++;
				}
				else img1ptr +=3;
			}
			img1ptr += AddedPadSize;
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

			img1ptr = Left;

		for (j=0; j<Par->Size_Rho; j++)
		{
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + 3 * i];
					i1 = 3 * i;
					i2 = ShiftMap[k1 + j*Par->Size_Theta+i];
					i1 = 3 * (j*Par->Size_Theta+i);

					if (i2 > 0)
					{
						d_1 = *img1ptr++ - aL.Red;
						d_2 = Right[i2] - aR.Red;
						numr   += (d_1 * d_2);
						den_1r += (d_1 * d_1);
						den_2r += (d_2 * d_2);

						d_1 = *img1ptr++ - aL.Gre;
						d_2 = Right[i2+1] - aR.Gre;
						numg   += (d_1 * d_2);
						den_1g += (d_1 * d_1);
						den_2g += (d_2 * d_2);

						d_1 = *img1ptr++ - aL.Blu;
						d_2 = Right[i2+2] - aR.Blu;
						numb   += (d_1 * d_2);
						den_1b += (d_1 * d_1);
						den_2b += (d_2 * d_2);
					}
					else img1ptr +=3;
				}
			img1ptr += AddedPadSize;
		}
			corr_val[k]  = (1.0 - (numr * numr) / (den_1r * den_2r + 0.00001));	
			corr_val[k] += (1.0 - (numg * numg) / (den_1g * den_2g + 0.00001));	
			corr_val[k] += (1.0 - (numb * numb) / (den_1b * den_2b + 0.00001));	

			if (count>MAX)
				MAX = count;
			corr_val[k] = (3-corr_val[k])*count;
		}
	
	for (k=0; k<Steps; k++)
		corr_val[k] = 3-(corr_val[k]/((double)MAX+0.00001));

	for (k=0; k<Steps; k++)
		if (corr_val[k]<MIN)
		{
			MIN = corr_val[k];
			minindex = k;
		}
		
		return minindex;
}


void shiftnCorrFovea (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, rgbPixel aFull, rgbPixel aFov,int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL
	double d_1;
	double d_2;

	double numr   = 0;
	double den_1r = 0;
	double den_2r = 0;
	double numg   = 0;
	double den_1g = 0;
	double den_2g = 0;
	double numb   = 0;
	double den_1b = 0;
	double den_2b = 0;

	double sum1R = 0;
	double sum2R = 0;
	double sum1G = 0;
	double sum2G = 0;
	double sum1B = 0;
	double sum2B = 0;
	double nElem = 0;

	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		numr   = 0;
		den_1r = 0;
		den_2r = 0;
		numg   = 0;
		den_1g = 0;
		den_2g = 0;
		numb   = 0;
		den_1b = 0;
		den_2b = 0;

		sum1R = 0;
		sum2R = 0;
		sum1G = 0;
		sum2G = 0;
		sum1B = 0;
		sum2B = 0;
		nElem = 0;

		fovPtr = fovImg;
		fullPtr = fullImg;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						d_1 = (*fovPtr++) - aFov.Red;
						d_2 = fullPtr[i2] - aFull.Red;
						numr   += (d_1 * d_2);
						
						den_1r += (d_1 * d_1);
						den_2r += (d_2 * d_2);

						d_1 = *fovPtr++ - aFov.Gre;
						d_2 = fullPtr[i2+1] - aFull.Gre;
						numg   += (d_1 * d_2);
						den_1g += (d_1 * d_1);
						den_2g += (d_2 * d_2);

						d_1 = *fovPtr++ - aFov.Blu;
						d_2 = fullPtr[i2+2] - aFull.Blu;
						numb   += (d_1 * d_2);
						den_1b += (d_1 * d_1);
						den_2b += (d_2 * d_2);

						nElem++;
					}
					else fovPtr +=3;
				}
			fovPtr+=AddedPadSize;
		}

		if (nElem>0)
		{
			corr_val[k] = numr/sqrt(den_1r*den_2r);
			corr_val[k] += numg/sqrt(den_1g*den_2g);
			corr_val[k] += numb/sqrt(den_1b*den_2b);
			corr_val[k] /= 3.0;
		}
		else
			corr_val[k] = 0;
	}
}

void Make_Disp_Histogram (unsigned char * hist,int height, int width, int shiftLevels, double * corrFunct)
{
	int i,j;
	int offset = (width-shiftLevels+1)/2;

	for (j=0;j<height*width;j++)
		hist[j] = 0;

	for (i=0; i<shiftLevels-1; i++)
		if ((i+offset >=0)&&(i+offset<width))
			for (j=height-(int)(height/1.0*(1-corrFunct[i])); j<height; j++)
					hist[(j*width+i+offset)] = 128;
		
	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;
}

void shiftnCorrFoveaNoAverage (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double *corr_val, double *std1, double *std2, int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL
	
	double numr   = 0;
	double den_1r = 0;
	double den_2r = 0;
	double numg   = 0;
	double den_1g = 0;
	double den_2g = 0;
	double numb   = 0;
	double den_1b = 0;
	double den_2b = 0;

	double sigma2 = 0;
	double sigma1 = 0;
	double grayAv1 = 0;
	double grayAv2 = 0;

	struct pixelDouble
	{double Red; double Gre; double Blu; };
	pixelDouble pixel1;
	pixelDouble pixel2;

	double sum1R = 0;
	double sum2R = 0;
	double sum1G = 0;
	double sum2G = 0;
	double sum1B = 0;
	double sum2B = 0;
	double nElem = 0;

	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		numr   = 0;
		den_1r = 0;
		den_2r = 0;
		numg   = 0;
		den_1g = 0;
		den_2g = 0;
		numb   = 0;
		den_1b = 0;
		den_2b = 0;

		sum1R = 0;
		sum2R = 0;
		sum1G = 0;
		sum2G = 0;
		sum1B = 0;
		sum2B = 0;
		nElem = 0;

		sigma1 = 0;
		sigma2 = 0;
		grayAv1 = 0;
		grayAv2 = 0;

		fovPtr = fovImg;
		fullPtr = fullImg;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pixel1.Red = *fovPtr++;
						pixel2.Red = fullPtr[i2];
						pixel1.Gre = *fovPtr++;
						pixel2.Gre = fullPtr[i2+1];
						pixel1.Blu = *fovPtr++;
						pixel2.Blu = fullPtr[i2+2];

						double gray1 = (pixel1.Red + pixel1.Gre+pixel1.Blu)/3.0;
						double gray2 = (pixel2.Red+pixel2.Gre+pixel2.Blu)/3.0;

						numr   += (pixel1.Red * pixel2.Red);
						sum1R += pixel1.Red;
						sum2R += pixel2.Red;
						den_1r += (pixel1.Red * pixel1.Red);
						den_2r += (pixel2.Red * pixel2.Red);

						numg   += (pixel1.Gre * pixel2.Gre);
						sum1G += pixel1.Gre;
						sum2G += pixel2.Gre;
						den_1g += (pixel1.Gre * pixel1.Gre);
						den_2g += (pixel2.Gre * pixel2.Gre);

						numb   += (pixel1.Blu * pixel2.Blu);
						sum1B += pixel1.Blu;
						sum2B += pixel2.Blu;
						den_1b += (pixel1.Blu * pixel1.Blu);
						den_2b += (pixel2.Blu * pixel2.Blu);

						sigma1 += gray1*gray1;
						sigma2 += gray2*gray2;
						grayAv1 += gray1;
						grayAv2 += gray2;

						nElem++;
					}
					else fovPtr +=3;
				}
			fovPtr+=AddedPadSize;
		}

		if (nElem>0)
		{
			corr_val[k] = (numr-sum1R*sum2R/nElem)/sqrt((den_1r-sum1R*sum1R/nElem)*(den_2r-sum2R*sum2R/nElem));
			corr_val[k] += (numg-sum1G*sum2G/nElem)/sqrt((den_1g-sum1G*sum1G/nElem)*(den_2g-sum2G*sum2G/nElem));
			corr_val[k] += (numb-sum1B*sum2B/nElem)/sqrt((den_1b-sum1B*sum1B/nElem)*(den_2b-sum2B*sum2B/nElem));
			corr_val[k] /= 3.0;

			std2[k] = (1/nElem)*(sigma2-grayAv2*grayAv2/nElem)/(128*128);
			std1[k]= (1/nElem)*(sigma1-grayAv1*grayAv1/nElem)/(128*128);
			
		}
		else
		{
			corr_val[k] = 0;
			std2[k] = 0;
			std1[k] = 0;
		}
	}
}

void shiftnCorrFoveaNoAverageNorm (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double *corr_val, double *std1, double *std2, int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL
	
	double numr   = 0;
	double den_1r = 0;
	double den_2r = 0;
	double numg   = 0;
	double den_1g = 0;
	double den_2g = 0;
	double numb   = 0;
	double den_1b = 0;
	double den_2b = 0;

	double sigma2 = 0;
	double sigma1 = 0;
	double grayAv1 = 0;
	double grayAv2 = 0;

	struct pixelDouble
	{double Red; double Gre; double Blu; };
	pixelDouble pixel1;
	pixelDouble pixel2;

	double sum1R = 0;
	double sum2R = 0;
	double sum1G = 0;
	double sum2G = 0;
	double sum1B = 0;
	double sum2B = 0;
	double nElem = 0;

	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		numr   = 0;
		den_1r = 0;
		den_2r = 0;
		numg   = 0;
		den_1g = 0;
		den_2g = 0;
		numb   = 0;
		den_1b = 0;
		den_2b = 0;

		sum1R = 0;
		sum2R = 0;
		sum1G = 0;
		sum2G = 0;
		sum1B = 0;
		sum2B = 0;
		nElem = 0;

		sigma1 = 0;
		sigma2 = 0;
		grayAv1 = 0;
		grayAv2 = 0;

		fovPtr = fovImg;
		fullPtr = fullImg;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pixel1.Red = *fovPtr++;
						pixel2.Red = fullPtr[i2];
						pixel1.Gre = *fovPtr++;
						pixel2.Gre = fullPtr[i2+1];
						pixel1.Blu = *fovPtr++;
						pixel2.Blu = fullPtr[i2+2];

						double gray1 = (pixel1.Red + pixel1.Gre+pixel1.Blu)/3.0;
						double gray2 = (pixel2.Red+pixel2.Gre+pixel2.Blu)/3.0;

						numr   += (pixel1.Red * pixel2.Red);
						sum1R += pixel1.Red;
						sum2R += pixel2.Red;
						den_1r += (pixel1.Red * pixel1.Red);
						den_2r += (pixel2.Red * pixel2.Red);

						numg   += (pixel1.Gre * pixel2.Gre);
						sum1G += pixel1.Gre;
						sum2G += pixel2.Gre;
						den_1g += (pixel1.Gre * pixel1.Gre);
						den_2g += (pixel2.Gre * pixel2.Gre);

						numb   += (pixel1.Blu * pixel2.Blu);
						sum1B += pixel1.Blu;
						sum2B += pixel2.Blu;
						den_1b += (pixel1.Blu * pixel1.Blu);
						den_2b += (pixel2.Blu * pixel2.Blu);

						sigma1 += gray1*gray1;
						sigma2 += gray2*gray2;
						grayAv1 += gray1;
						grayAv2 += gray2;

						nElem++;
					}
					else fovPtr +=3;
				}
			fovPtr+=AddedPadSize;
		}

		if (nElem>0)
		{
			corr_val[k] = (numr-sum1R*sum2R/nElem)/sqrt((den_1r-sum1R*sum1R/nElem)*(den_1r-sum1R*sum1R/nElem));
			corr_val[k] += (numg-sum1G*sum2G/nElem)/sqrt((den_1g-sum1G*sum1G/nElem)*(den_1g-sum1G*sum1G/nElem));
			corr_val[k] += (numb-sum1B*sum2B/nElem)/sqrt((den_1b-sum1B*sum1B/nElem)*(den_1b-sum1B*sum1B/nElem));
			corr_val[k] /= 3.0;

			std2[k] = (1/nElem)*(sigma2-grayAv2*grayAv2/nElem)/(128*128);
			std1[k]= (1/nElem)*(sigma1-grayAv1*grayAv1/nElem)/(128*128);
			
		}
		else
		{
			corr_val[k] = 0;
			std2[k] = 0;
			std1[k] = 0;
		}
	}
}

void shiftSSDRGB (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	struct pixelDouble
	{double Red; double Gre; double Blu; };

	pixelDouble first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		fovPtr = fovImg;
		fullPtr = fullImg;

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		
		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.Red = fovPtr[0];
						first.Gre = fovPtr[1];
						first.Blu = fovPtr[2];
					
						second.Red = fullPtr[i2];
						second.Gre = fullPtr[i2+1];
						second.Blu = fullPtr[i2+2];
						
						ssd += (first.Red-second.Red)*(first.Red-second.Red);
						ssd += (first.Gre-second.Gre)*(first.Gre-second.Gre);
						ssd += (first.Blu-second.Blu)*(first.Blu-second.Blu);

						den1 += (first.Red*first.Red);
						den1 += (first.Gre*first.Gre);
						den1 += (first.Blu*first.Blu);

						den2 += (second.Red*second.Red);
						den2 += (second.Gre*second.Gre);
						den2 += (second.Blu*second.Blu);

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
			}
			fovPtr+=AddedPadSize;
		}

		corr[k] = 1-ssd/sqrt(den1*den1);
	}
}

void shiftSSD (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	struct pixelDouble
	{double Red; double Gre; double Blu; };

	pixelDouble first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		fovPtr = fovImg;
		fullPtr = fullImg;

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		
		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.Red = fovPtr[0];
						first.Gre = fovPtr[1];
						first.Blu = fovPtr[2];
					
						second.Red = fullPtr[i2];
						second.Gre = fullPtr[i2+1];
						second.Blu = fullPtr[i2+2];
						
						double gray1 = (first.Red+first.Gre+first.Blu)/3.0;
						double gray2 = (second.Red+second.Gre+second.Blu)/3.0;

						ssd += (gray1-gray2)*(gray1-gray2);
						den1 += (gray1*gray1);
						den2 += (gray2*gray2);
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
			}
			fovPtr+=AddedPadSize;
		}

		// corr[k] = 1-ssd/sqrt(den1*den2);	old normalization
		corr[k] = 1-ssd/sqrt(den1*den1);
	}
}

void shiftSSDWorstCase (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	int nElem;
	struct pixelDouble
	{double Red; double Gre; double Blu; };
	pixelDouble ssd;
	pixelDouble den1,den2;

	pixelDouble first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = fullImg;
	fovPtr = fovImg;

	int tIndex;

	int AddedPadSize = computePadSize(Par->Size_Theta*Par->LP_Planes,Par->padding) - Par->Size_Theta*Par->LP_Planes;

	for (k=0; k<Steps; k++)
	{

		k1 = k * Par->Size_LP; //Positioning on the table

		count[k] = 0;

		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
				i2 = ShiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}

		fovPtr = fovImg;
		fullPtr = fullImg;

		ssd.Red = 0.0;
		ssd.Gre = 0.0;
		ssd.Blu = 0.0;

		den1.Red = 0.0;
		den1.Gre = 0.0;
		den1.Blu = 0.0;
		
		den2.Red = 0.0;
		den2.Gre = 0.0;
		den2.Blu = 0.0;
		
		nElem = 0;
		
		for (j=0; j<Rows; j++)
		{
			tIndex = j*Par->Size_Theta;
			for (i=0; i<Par->Size_Theta; i++)
			{
					i2 = ShiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);
					if (i2 > 0)
					{
						first.Red = fovPtr[0];
						first.Gre = fovPtr[1];
						first.Blu = fovPtr[2];
					
						second.Red = fullPtr[i2];
						second.Gre = fullPtr[i2+1];
						second.Blu = fullPtr[i2+2];
						
						ssd.Red += (first.Red-second.Red)*(first.Red-second.Red);
						ssd.Gre += (first.Gre-second.Gre)*(first.Gre-second.Gre);
						ssd.Blu += (first.Blu-second.Blu)*(first.Blu-second.Blu);
					
						den1.Red += (first.Red)*(first.Red);
						den1.Gre += (first.Gre)*(first.Gre);
						den1.Blu += (first.Blu)*(first.Blu);

						den2.Red += (second.Red)*(second.Red);
						den2.Gre += (second.Gre)*(second.Gre);
						den2.Blu += (second.Blu)*(second.Blu);
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
			}
			fovPtr+=AddedPadSize;
		}

		pixelDouble corrTmp;

		corrTmp.Red = 1-ssd.Red/sqrt(den1.Red*den1.Red);
		corrTmp.Gre = 1-ssd.Gre/sqrt(den1.Gre*den1.Gre);
		corrTmp.Blu = 1-ssd.Blu/sqrt(den1.Blu*den1.Blu);

		double min1;
		double min2;
		if (corrTmp.Red < corrTmp.Gre)
			min1 = corrTmp.Red;
		else
			min1 = corrTmp.Gre;
	
		if (corrTmp.Gre < corrTmp.Blu)
			min2 = corrTmp.Gre;
		else
			min2 = corrTmp.Blu;

		if (min1<min2)
			corr[k] = min1;
		else
			corr[k] = min2;
	}
}
