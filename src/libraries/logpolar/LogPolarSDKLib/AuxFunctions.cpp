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
/// $Id: AuxFunctions.cpp,v 1.6 2003-10-08 17:13:12 fberton Exp $
///
///

#include <stdio.h>
#include <math.h>
#include <malloc.h>

#if !defined(__QNX__) && !defined(__LINUX__)
#include <windows.h>
#endif	/// __WIN32__

#include "LogPolarSDK.h"

void Set_Path()
{
}

void Get_Path()
{
}

/************************************************************************
* Load_Bitmap															*
* Legge un'immagine bitmap da file										*
************************************************************************/	

unsigned char * Load_Bitmap(int *X_Size,
							int *Y_Size,
							int *planes,
							char * filename)
{
#if defined(__QNX__) || defined(__LINUX__)

#define WORD short
#define DWORD int
#define LONG long
#define BYTE char

#ifdef __LINUX__
#pragma align 1
#endif

#ifdef __QNX4__
#pragma  pack (push) ;
#pragma  pack (1) ;
#endif

#ifdef __QNX6__
#pragma pack(1)
#endif

	typedef struct tagBITMAPFILEHEADER { 
	  WORD    bfType; 
	  DWORD   bfSize; 
	  WORD    bfReserved1; 
	  WORD    bfReserved2; 
	  DWORD   bfOffBits; 
	} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

	typedef struct tagBITMAPINFOHEADER{
	  DWORD  biSize; 
	  LONG   biWidth; 
	  LONG   biHeight; 
	  WORD   biPlanes; 
	  WORD   biBitCount; 
	  DWORD  biCompression; 
	  DWORD  biSizeImage; 
	  LONG   biXPelsPerMeter; 
	  LONG   biYPelsPerMeter; 
	  DWORD  biClrUsed; 
	  DWORD  biClrImportant; 
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 

	typedef struct tagRGBQUAD {
	  BYTE    rgbBlue; 
	  BYTE    rgbGreen; 
	  BYTE    rgbRed; 
	  BYTE    rgbReserved; 
	} RGBQUAD; 

#ifdef __LINUX__
#pragma align 0
#endif

#ifdef __QNX4__
#pragma  pack (pop) ;
#endif

#ifdef __QNX6__
#pragma pack()
#endif

#endif

	unsigned char *image;	
	unsigned char c=0;
	int x,y,z;
	int Offset;
	FILE* fin;
	BITMAPFILEHEADER bmpfh;
	BITMAPINFOHEADER bmpih;
	RGBQUAD palette[256];

	if ((fin = fopen(filename,"rb")) != NULL)
	{
		fread (&bmpfh,sizeof(BITMAPFILEHEADER),1,fin);
		fread (&bmpih,sizeof(BITMAPINFOHEADER),1,fin);
		*X_Size = bmpih.biWidth;
		*Y_Size = bmpih.biHeight;
		*planes = bmpih.biBitCount/8;
		image = (unsigned char *)malloc(*X_Size * *Y_Size * *planes *sizeof(unsigned char));
		Offset = (4 - ((*X_Size * *planes) %4))%4;

		if (*planes == 1)
			fread (&palette,sizeof(RGBQUAD),256,fin);
		
		for (y=*Y_Size-1; y>=0; y--)
		{
			for (x=0; x<*X_Size; x++)
				for (z=*planes-1; z>=0; z--)
				{
						fread (&c,1,1,fin);
						image[y**planes* *X_Size + *planes *x+z] = (unsigned char)(c);
				}
			for (x=0;x<Offset;x++)
				fread(&c,1,1,fin);
		}
		fclose(fin);
	}
	else
		image = NULL;
	
	return image;
}


/************************************************************************
* Save_Bitmap															*
* Scrive un'immagine bitmap su file										*
************************************************************************/	

void Save_Bitmap(unsigned char *image,
				 int X_Size,
				 int Y_Size,
				 int planes,
				 char * filename)
{
#if !defined(__QNX__) && !defined(__LINUX__)

	FILE* fout;
	int size = X_Size * Y_Size * planes;
	int x,y,z;
	int Offset = (4 - ((X_Size * planes) %4))%4;
	BITMAPFILEHEADER bmpfh;
	BITMAPINFOHEADER bmpih;

	fout = fopen(filename,"wb");

	bmpfh.bfType = 'MB';
	bmpfh.bfOffBits = 54;
	if (planes==1)
		bmpfh.bfOffBits = 54+1024;

	bmpfh.bfSize = size + bmpfh.bfOffBits;
	bmpfh.bfReserved1 = 0;
	bmpfh.bfReserved2 = 0;

	bmpih.biSize = 40;
	bmpih.biWidth = X_Size;
	bmpih.biHeight = Y_Size;
	bmpih.biPlanes = 1;
	bmpih.biBitCount = planes *8;
	bmpih.biCompression = 0;
	bmpih.biSizeImage = Y_Size * (X_Size * planes + Offset);
	bmpih.biXPelsPerMeter = 2835;
	bmpih.biYPelsPerMeter = bmpih.biXPelsPerMeter;
	bmpih.biClrUsed = 0;
	bmpih.biClrImportant = 0;
	if (planes==1)
	{
		bmpih.biClrUsed = 256;
		bmpih.biClrImportant = 256;
	}

	fwrite(&bmpfh,sizeof(BITMAPFILEHEADER),1,fout);
	fwrite(&bmpih,sizeof(BITMAPINFOHEADER),1,fout);

	if (planes ==1)
		for (x=0; x<256;x++)
		{
			y=0;
			fwrite(&x,sizeof(unsigned char),1,fout);
			fwrite(&x,sizeof(unsigned char),1,fout);
			fwrite(&x,sizeof(unsigned char),1,fout);
			fwrite(&y,sizeof(unsigned char),1,fout);
		}

	for (y=Y_Size-1; y>=0; y--)
	{	
		for (x=0; x<X_Size; x++)
			for (z=planes-1;z>=0; z--)
				fwrite(image+(planes*(y*X_Size+x)+z),sizeof(unsigned char),1,fout);
		for (x=0;x<Offset;x++)
			fwrite(image,sizeof(unsigned char),1,fout);
	}

	fclose(fout);
#endif
}

/************************************************************************
* Get_Theta		  														*
************************************************************************/	

int Get_Theta_Old(double x,
			  double y,
			  int rho,
			  Image_Data * par, 
			  double *Ang_Shift, 
			  unsigned short *Pad_Map)
{
	
	int theta;
	double temp;
//	int jj;
//	int counter;

	int halfth = par->Size_Theta/2;
	double thmult = (par->Size_Theta/2)/PI;

//	tmpth  = (halfth-atan2(y,-x)*thmult);
//	theta = (int)((tmpth) + .5 * ((rho)%2)) ;
	
	temp = -atan2(y,-x);
	temp += PI-Ang_Shift[rho];

	if (temp<0)
		temp += 2*PI;
	if (temp > 2*PI)
		temp -= 2*PI;

	if (rho<par->Size_Fovea)
	{
		temp = temp * (3*rho) / PI;
//		counter = 0;
//		for(jj=0; jj<par->Size_Theta; jj++)
//		{
//			if (Pad_Map[rho*par->Size_Theta+jj]!=1)
//				counter ++;
//		}
//		temp = temp * counter / (2*PI);
		theta = Pad_Map [(unsigned short)(temp)+par->Size_Theta*rho]%par->Size_Theta;
	}
	else 
	{
		temp = temp * (par->Size_Theta/2) / PI;
		theta = (int) (temp);	
	}
	
	if (theta<0)
		theta += par->Size_Theta;
	if (theta>=par->Size_Theta)
		theta -= par->Size_Theta;

	return theta;
}


/************************************************************************
* Get_Theta		  														*
************************************************************************/	

int Get_Theta(double x,
			  double y,
			  int rho,
			  Image_Data * par, 
			  double *Ang_Shift, 
			  unsigned short *Pad_Map)
{
	
	int theta;
	double temp;
//	int jj;
//	int counter;

	int halfth = par->Size_Theta/2;
	double thmult = (par->Size_Theta/2)/PI;

//	tmpth  = (halfth-atan2(y,-x)*thmult);
//	theta = (int)((tmpth) + .5 * ((rho)%2)) ;
	
	temp = -atan2(y,-x);
	temp += PI-Ang_Shift[rho];

	if (temp<0)
		temp += 2*PI;
	if (temp >= 2*PI)
		temp -= 2*PI;

	if (rho<par->Size_Fovea)
	{
		if (par->Fovea_Type == 0)
		{
			if (rho != 0)
			{
				temp = temp * (3*rho) / PI;
				theta = Pad_Map [(unsigned short)(temp)+par->Size_Theta*rho]%par->Size_Theta;
			}
			else 
			{
				temp = temp / (2*PI);
				theta = Pad_Map [(unsigned short)(temp)+par->Size_Theta*rho]%par->Size_Theta;
			}
		}
		else if (par->Fovea_Type == 1)
		{
			if (rho != 0)
			{
				temp = temp * (3*rho) / PI;
				theta = (int)(temp+0.5);
			}
			else 
			{
				temp = temp / (2*PI);
				theta = (int)(temp+0.5);
			}
		}
		else //if (par->Fovea_Type == 2)
		{
			temp = temp * (par->Size_Theta) / (2*PI);
			theta = (int)(temp+0.5);
		}
//		counter = 0;
//		for(jj=0; jj<par->Size_Theta; jj++)
//		{
//			if (Pad_Map[rho*par->Size_Theta+jj]!=1)
//				counter ++;
//		}
//		temp = temp * counter / (2*PI);
	}
	else 
	{
		temp = temp * (par->Size_Theta/2.0) / PI;
		theta = (int) (temp+0.5);	
	}
	
	if (theta<0)
		theta += par->Size_Theta;
	if (theta>=par->Size_Theta)
		theta -= par->Size_Theta;

	return theta;
}
