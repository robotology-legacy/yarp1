////

#include <stdio.h>
#include <math.h>

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
* Read_Bitmap															*
* Legge un'immagine bitmap da file										*
************************************************************************/	

unsigned char * Read_Bitmap(int *X_Size,
							int *Y_Size,
							int *planes,
							char * filename)
{
#if !defined(__QNX__) && !defined(__LINUX__)
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
#else
	return NULL;
#endif
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
			  short *Pad_Map)
{
	
	int theta;
	double temp;

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
		theta = Pad_Map [(short)(temp)+par->Size_Theta*rho]%par->Size_Theta;
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
