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
/// $Id: AuxFunctions.cpp,v 1.2 2004-07-27 09:45:04 babybot Exp $
///
///

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__QNX__) && !defined(__LINUX__)
#include <windows.h>
#endif	/// __WIN32__

#include <yarp/LogPolarSDK.h>

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

int Get_Theta(double x,
			  double y,
			  int rho,
			  Image_Data * par, 
			  double *Ang_Shift, 
			  unsigned short *Pad_Map)
{
	
	int theta;
	double temp;
	float castval = 0.01f;
	
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
				theta = Pad_Map [(unsigned short)(temp+castval)+par->Size_Theta*rho]%par->Size_Theta;
			}
			else 
			{
				temp = temp / (2*PI);
				theta = Pad_Map [(unsigned short)(temp+castval)+par->Size_Theta*rho]%par->Size_Theta;
			}
		}
		else if (par->Fovea_Type == 1)
		{
			if (rho != 0)
			{
				temp = temp * (3*rho) / PI;
				theta = (int)(temp+castval);
			}
			else 
			{
				temp = temp / (2*PI);
				theta = (int)(temp+castval);
			}
		}
		else //if (par->Fovea_Type == 2)
		{
			temp = temp * (par->Size_Theta) / (2*PI);
			theta = (int)(temp+castval);
		}
	}
	else 
	{
		temp = temp * (par->Size_Theta/2.0) / PI;
		theta = (int) (temp+castval);	
	}
	
	if (theta<0)
		theta += par->Size_Theta;
	if (theta>=par->Size_Theta)
		theta -= par->Size_Theta;

	return theta;
}

int computePadSize(int width,int padding)
{
	int AddedPad;

	if (width % padding == 0)
		AddedPad = 0;
	else
		AddedPad = padding - width % padding;

	return (width + AddedPad);
}

rgbPixel computeAvg(int SizeRho,int SizeTheta, int padding, unsigned char * image)
{
	rgbPixel avg;

	int Size = SizeRho*SizeTheta;
	int SizeFovea = SizeTheta/6;
	int diff = SizeTheta-SizeFovea*6;
	Size -= (SizeFovea*(SizeFovea-1))*3+(SizeFovea-1)*diff+SizeTheta-1;
	int paddedLine = 3 * SizeTheta+padding;

	int i,j;
	int sumR = 0;
	int sumG = 0;
	int sumB = 0;

	for (j=0; j<SizeRho; j++)
		for (i=0; i<3*SizeTheta; i+=3)
		{
			sumR += image[j*paddedLine+i];
			sumG += image[j*paddedLine+i+1];
			sumB += image[j*paddedLine+i+2];
		}


	
	avg.Red = sumR/Size;
	avg.Gre = sumG/Size;
	avg.Blu = sumB/Size;

	return avg;
}

void sawt2Uniform(unsigned char * outImage, unsigned char * inImage, Image_Data * par, unsigned short * padMap)
{
	int i,j;
	float k;
	unsigned char * oneLine;

	int PadLine = computePadSize(par->Size_Theta * 3, par->padding);

	oneLine = (unsigned char *) malloc (PadLine * sizeof (unsigned char));

	for (i=0; i<par->Size_Fovea; i++)
		for (j=0; j<par->Size_Theta*3; j++)
		{
				outImage[i*PadLine+j] = inImage[3*padMap[i*252+j/3]+4*i+j%3];
		}
	
	// Replication of the first pixel
	for (j=3; j<3*par->Size_Theta; j+=3)
	{
		outImage[j]= outImage[0];
		outImage[j+1]= outImage[1];
		outImage[j+2]= outImage[2];
	}

	for (i=2; i<par->Size_Fovea; i+=2)
	{
		// copy of one line
		for (j=0; j<3*par->Size_Theta; j++)
			oneLine[j] = outImage[i*PadLine+j];

		for (j = 0; j<par->Size_Theta; j++)
		{
			if (1)
			{
				outImage[(i*PadLine+3*j)]   = oneLine[3*((i*j)/(par->Size_Fovea))];
				outImage[(i*PadLine+3*j)+1]   = oneLine[3*((i*j)/(par->Size_Fovea))+1];
				outImage[(i*PadLine+3*j)+2]   = oneLine[3*((i*j)/(par->Size_Fovea))+2];
			}
			else
			{
				outImage[3*(i*par->Size_Theta+j)]   = 0;
				outImage[3*(i*par->Size_Theta+j)+1] = 0;
				outImage[3*(i*par->Size_Theta+j)+2] = 0;
			}
		}
	}

	for (i=1; i<par->Size_Fovea; i+=2)
	{
		// copy of one line
		for (j=0; j<3*par->Size_Theta; j++)
			oneLine[j] = outImage[i*PadLine+j];

		for (j = 0; j<par->Size_Theta; j++)
		{
			k = (-1.5f+i*j/(par->Size_Fovea));
			if (k<0)
				k+=i*6;
			if (1)
			{
				outImage[(i*PadLine+3*j)]   = oneLine[3*((int)k)];
				outImage[(i*PadLine+3*j)+1] = oneLine[3*((int)k)+1];
				outImage[(i*PadLine+3*j)+2] = oneLine[3*((int)k)+2];
			}
			else
			{
				outImage[i*PadLine+j]   = 0;
			}
		}
	}
	
	// Remaining Lines (non Fovea)
	memcpy(outImage+PadLine * par->Size_Fovea,
		inImage+PadLine * par->Size_Fovea,
		PadLine * (par->Size_Rho-par->Size_Fovea) * sizeof(unsigned char));
	
	free (oneLine);
}

void uniform2Sawt(unsigned char * outImage, unsigned char * inImage, Image_Data * par, unsigned short * padMap)
{
	int i,j;
	float k;
	unsigned char * Fovea;
	unsigned char * oneLine;
	int PadLine = computePadSize(par->Size_Theta * 3, par->padding);

	Fovea = (unsigned char *) malloc (par->Size_Fovea * PadLine * sizeof (unsigned char));
	oneLine = (unsigned char *) malloc (PadLine * sizeof (unsigned char));

	for (j = 0; j<par->Size_Fovea * PadLine; j++)
		Fovea[j] = 0;


	for (i=0; i<par->Size_Fovea; i+=2)
	{
		for (j=0; j<3*par->Size_Theta; j++)
			oneLine[j] = 0;
		for (j = 0; j<par->Size_Theta; j++)
		{
			if (1)
			{
				oneLine[3*((i*j)/par->Size_Fovea)] = inImage[(i*PadLine+3*j)] ;
				oneLine[3*(i*j/par->Size_Fovea)+1] = inImage[(i*PadLine+3*j)+1] ;
				oneLine[3*(i*j/par->Size_Fovea)+2] = inImage[(i*PadLine+3*j)+2] ;
			}
			else
			{
				outImage[(i*PadLine+3*j)]   = 0;
				outImage[(i*PadLine+3*j)+1] = 0;
				outImage[(i*PadLine+3*j)+2] = 0;
			}

		}
		//copy of one line
		for (j=0; j<3*par->Size_Theta; j++)
			outImage[i*PadLine+j]= oneLine[j];
	}

	for (i=1; i<par->Size_Fovea; i+=2)
	{
		for (j=0; j<3*par->Size_Theta; j++)
			oneLine[j] = 0;

		for (j = 0; j<par->Size_Theta; j++)
		{
			k = (-1.5f+i*j/par->Size_Fovea);
			if (k<0)
				k+=i*6;
			if (1)
			{
				oneLine[3*((int)k)]   = inImage[(i*PadLine+3*j)];
				oneLine[3*((int)k)+1] = inImage[(i*PadLine+3*j)+1];
				oneLine[3*((int)k)+2] = inImage[(i*PadLine+3*j)+2];
			}
			else
			{
				outImage[(i*PadLine+3*j)]   = 0;
				outImage[(i*PadLine+3*j)+1] = 0;
				outImage[(i*PadLine+3*j)+2] = 0;
			}
		}
		//copy of one line
		for (j=0; j<3*par->Size_Theta; j++)
			outImage[i*PadLine+j] = oneLine[j];
	}

	//From triangular to sawtooth
	
	for (i=0; i<par->Size_Fovea; i++)
		for (j=0; j<par->Size_Theta*3; j++)
	{
		if (padMap[i*252+j/3] != 1)
			Fovea[3*padMap[i*252+j/3]+4*i+j%3] = outImage[i*PadLine+j];
		else
			outImage[i*PadLine+j]=0;
	}

	for (j=0; j<  PadLine * par->Size_Fovea; j++)
	{
		outImage[j] = Fovea[j];
	}
	

	//Remaining Lines (non Fovea)
	memcpy(outImage+PadLine * par->Size_Fovea,
		inImage+PadLine * par->Size_Fovea,
		PadLine * (par->Size_Rho-par->Size_Fovea) * sizeof(unsigned char));
	
	free (oneLine);
	free (Fovea);
}
