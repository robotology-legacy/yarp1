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
/// $Id: TableGeneration.cpp,v 1.19 2003-10-02 16:44:14 beltran Exp $
///
///

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 

#include "LogPolarSDK.h"

/****************************************************************************
*																			*
* Build_Tables																*
* 																			*
* Builds all the needed LUT's and saves them to  disk.						*
*																			*
* Input: Param																*
* 			Pointer to a struct containing all the needed parameters		*
* 		 Tables																*
* 			Pointer to a struct containing all the pointers					*
*        Path																*
* 			Working folder													*
*        List																*
* 			Control value used to determine which tables will be built:		*											*
*																			*
* 				1	AngShiftMap.gio											*
*				2	ColorMap.gio 											*
*				4	DownsampleMap.gio, ratio = 4							*
*				8	NeighborhoodMap.gio 									*
*			   16	PadMap.gio 												*
*			   32	RemMap_X.XX_YYxYY.gio 									*
*			   64	WeightsMap.gio 											*
*			  128	XYMap.gio												*
*			  256	DownsampleMap.gio, ratio = 2							*
*			  512	ShiftMap												*
*																			*
*		The values have to be summed when building more than one table.		*
*		Use the ALLMAPS value to build all the LUT's.						*
*																			*
****************************************************************************/	

unsigned char Build_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned char List)
{
	unsigned char retval = 0;
	int testval;
	
	if (List&1==1)
	{
		testval = Build_Ang_Shift_Map(Param,Path);
		if (testval)
			retval = retval | 1;
	}

	if ((List&2)==2)
	{
		testval = Build_Color_Map(Path);
		if (testval)
			retval = retval | 2;
	}

	if ((List&16)==16)
	{
		testval = Build_Pad_Map(Param,Path);
		if (testval)
			retval = retval | 16;
	}

	if ((List&128)==128)
	{
		testval = Build_XY_Map(Param,Path);
		if (testval)
			retval = retval | 128;
	}

	if ((List&8)==8)
	{
		testval = Build_Neighborhood_Map(Param,Path);
		if (testval)
			retval = retval | 8;
	}

	if ((List&32)==32)
	{
		testval = Build_Remap_Map(Param,Path);
		if (testval)
			testval = Crop_Remap_Map(Param,Path);
		if (testval)
			retval = retval | 32;
	}

	if ((List&64)==64)
	{
		testval = Build_Weights_Map(Param,Path);
		if (testval)
			retval = retval | 64;
	}

	if ((List&256)==256)
	{
		testval = Build_DS_Map(Param,Path,2.00);
		if (testval)
			retval = retval | 256;
	}

	if ((List&4)==4)
	{
		testval = Build_DS_Map(Param,Path,4.00);
		if (testval)
			retval = retval | 4;
	}

	return retval;
}



/************************************************************************
*																		*
* Build_Ang_Shift_Map     												*
*																		*
* Builds a Look-up Table with the information needed for a correct		*
* absolute angular shift of each ring and saves it in the working		*
* folder with the file name AngularShiftMap.gio							*																		*
*																		*
* Input: Fovea Type: it can be:     									*
*					 0. Giotto 2.0 (33k pixels, old fovea)				*
*					 1. Giotto 2.1 (33k pixels, new fovea)				*
* 		 Parameters														*
* 			Set of all the needed parameters							*
*		 Path															*
* 			Working folder												*
* 																		*
************************************************************************/	

int Build_Ang_Shift_Map (Image_Data * Parameters,char * Path)
{
	int i;
	double * Ang_Shift_Map;
	char File_Name [256];

	FILE * fout;
	
	Ang_Shift_Map = (double *)calloc(Parameters->Size_Rho,sizeof(double));

	if (Parameters->Fovea_Type < 4)
	{
		for (i=1; i<Parameters->Size_Fovea; i+=2)
		{
			Ang_Shift_Map[i+1] = - 2.0 * PI/(6*(i+1));
			Ang_Shift_Map[i]   = +       PI/(6* (i) );
		}

		for (i=Parameters->Size_Fovea; i<Parameters->Size_Rho-1; i+=2)
		{
			Ang_Shift_Map[i+1] = +     PI/(Parameters->Size_Theta);
		}
	}
	else
	{
		// Write here the code for Giotto 2.1
	}

	sprintf(File_Name,"%s%s",Path,"AngularShiftMap.gio");
	
	fout = fopen(File_Name,"wb");
	fwrite(Ang_Shift_Map,sizeof(double),Parameters->Size_Rho,fout);
	fclose (fout);

	free (Ang_Shift_Map);

	return 1;
}

/************************************************************************
*																		*
* Build_Color_Map														*
* 																		*
* Builds a Look-up Table with the color information of the pixels in	*
* the sensor and saves it in the working folder with the file name 		*
* "ColorMap.gio"														*
*																		*
* The information is taken from an image which has been built off-line.	*
*																		*
* Input: Path															*
* 			Working folder												*
* 																		*
************************************************************************/	

int Build_Color_Map (char * Path)
{
	int i,j;
	int XSize,YSize,planes;
	unsigned char * Image;
	char          * Color_Map;
	char File_Name [256];

	FILE * fout;

	sprintf(File_Name,"%s%s",Path,"ReferenceImage.bmp");

	Image = Read_Bitmap(&XSize,&YSize,&planes,File_Name);

	if (Image != NULL)
	{
		Color_Map = (char *)malloc(XSize * YSize * sizeof(char));

		for (i=0; i< XSize * YSize; i++)
		{
			Color_Map[i] = -1;
			for (j=0; j<planes; j++)
				if (Image[3*i+j] == 255)
					Color_Map[i] = j;
		}

		free (Image);

		sprintf(File_Name,"%s%s",Path,"ColorMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Color_Map,sizeof(char),XSize * YSize,fout);
		fclose (fout);

		free (Color_Map);

		return 1;
	}
	else
		return 0;
}

/************************************************************************
* Build_DownSample_Map													*
* Costruisce una tabella che permette di trasformare l'immagine			*
* originale a 33k pixel in una ridotta a 11k pixel						*
* e salva la tabella su file.											*
*																		*
* Input: Path															*
* 		 Percorso della directory di lavoro								*
* 																		*
************************************************************************/	
/*
int Build_DownSample_Map (Image_Data * Parameters, char * Path)
{
	int i,j;
	unsigned short * DownSample_Map;
	char           * Color_Map;
	unsigned short * DS_Map;
	char File_Name [256];
	int retval;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Parameters,&Tables,Path,2);

	if (retval != 2)
		return 0;
	else
	{
		Color_Map = Tables.ColorMap;

		DownSample_Map = (unsigned short *)calloc(Parameters->Size_LP, sizeof(unsigned short));

		DS_Map = DownSample_Map;
		
		for (j=0; j< 2 * Parameters->Size_Fovea; j+=2)
			for (i=0; i< 2 * Parameters->Size_Theta; i+=2)
				if (Color_Map[(j/2)*Parameters->Size_Theta+(i/2)] != -1)
					*DS_Map++ = j * 2 * Parameters->Size_Theta + i; 
				else 
					*DS_Map++ = 0;

		for (j=2 * Parameters->Size_Fovea; j<2 * Parameters->Size_Rho; j+=2)
		{
			for (i=0; i<2 * Parameters->Size_Theta; i+=2)
				if (j%4==2)
					*DS_Map++ = j * 2 * Parameters->Size_Theta + i; 
				else
					*DS_Map++ = j * 2 * Parameters->Size_Theta + i + 1; 

		}


		sprintf(File_Name,"%s%s",Path,"DownsampleMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(DownSample_Map,sizeof(unsigned short),Parameters->Size_LP,fout);
		fclose (fout);

		free(Color_Map);
		free (DownSample_Map);

		return 1;
	}

}
*/
/************************************************************************
* Build_Neighborhood_Map  												*
*																		*
* Input: Pix_Numb														*
* 		 Path															*
* 																		*
************************************************************************/	

int Build_Neighborhood_Map(Image_Data * Par,
							char * Path)
{
	int j,k;
	int ii,jj,kk;
	int plane;
	int rho,theta;
	int bottomrho,bottomtheta;
	int toprho,toptheta;
	int CurrPix,TestPix;
	double distX,distY, distXY;
	char File_Name [256];
	int Pix_Numb = MAX_PIX;

	Neighborhood * Neighbor_Map_RGB;
	double		 * X_Map;
	double		 * Y_Map;
	char		 * Color_Map;

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Par,&Tables,Path,130);

	
	Neighbor_Map_RGB = (Neighborhood *)malloc(Par->Size_LP * Pix_Numb * 3 * sizeof(Neighborhood));

	if (retval != 130)
	{
		if (Tables.ColorMap!=NULL)
			free(Tables.ColorMap);
		if (Tables.XYMap!=NULL)
			free(Tables.XYMap);
		return 0;
	}
	else
	{
		Color_Map = Tables.ColorMap;
		X_Map = Tables.XYMap;
		Y_Map = Tables.XYMap+Par->Size_LP;

//	X_Map = (double *)calloc(Par->Size_LP,sizeof(double));
//	Y_Map = (double *)calloc(Par->Size_LP,sizeof(double));
	//	sprintf(File_Name,"%s%s",Path,"XYMap.gio");

		//inserire qui la load XY
	//	fin = fopen(File_Name,"rb");
	//	fread(X_Map,sizeof(double),Par->Size_LP,fin);
	//	fread(Y_Map,sizeof(double),Par->Size_LP,fin);
	//	fclose(fin);

	//	Color_Map = (char *)malloc(Par->Size_LP * sizeof(char));

	//	sprintf(File_Name,"%s%s",Path,"ColorMap.gio");

	//	fin = fopen(File_Name,"rb");
	//	fread(Color_Map,sizeof(char),Par->Size_LP,fin);
	//	fclose(fin);

		for (j=0; j<Par->Size_LP * Pix_Numb * 3; j++)
		{
			Neighbor_Map_RGB [j].weight = (float)(Par->Size_Img_Remap);
			Neighbor_Map_RGB [j].position = 0;
		}

		for (rho=0; rho<Par->Size_Rho; rho++)
		{
			if (rho-(Pix_Numb+1)/2>0)
				bottomrho = rho-(Pix_Numb+1)/2;
			else bottomrho = 0;

			if (1+rho+(Pix_Numb+1)/2<Par->Size_Rho)
				toprho = 1+rho+(Pix_Numb+1)/2;
			else toprho = Par->Size_Rho;

			for (theta=0; theta<Par->Size_Theta; theta++)
				if (Color_Map[rho*Par->Size_Theta+theta]!=-1)
				{
					CurrPix = rho*Par->Size_Theta+theta;

					if ((theta-(Pix_Numb+1)/2>0)&&(1+theta+(Pix_Numb+1)/2<Par->Size_Theta))
					{
						bottomtheta = theta-(Pix_Numb+1)/2;
						toptheta = 1+theta+(Pix_Numb+1)/2;
					}
					else
					{
						bottomtheta = 0;
						toptheta	= Par->Size_Theta;
					}
					if (rho<Par->Size_Fovea)
					{
						bottomtheta = 0;
						toptheta	= Par->Size_Theta;
					}
					for (jj=bottomrho; jj<toprho; jj++)
						for (ii=bottomtheta; ii<toptheta; ii++)
						{
							TestPix = jj*Par->Size_Theta+ii;
							distX = X_Map[CurrPix]-X_Map[TestPix];
							distX = distX*distX;
							distY = Y_Map[CurrPix]-Y_Map[TestPix];
							distY = distY*distY;
							distXY  = distX+distY;

							for (plane = RED; plane <= BLUE; plane ++)
								for (k=0; k<Pix_Numb; k++)
									if ((distXY < Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+k].weight)&&(Color_Map[jj*Par->Size_Theta+ii] == plane))
									{
										if (Pix_Numb >1)
											for (kk=Pix_Numb-2; kk>=k; kk--)
											{
												Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+kk+1].weight  = Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+kk].weight;
												Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+kk+1].position  = Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+kk].position;
											}
										Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+k].weight = (float)(distXY);
										Neighbor_Map_RGB[(plane*Par->Size_LP * Pix_Numb)+Pix_Numb*(rho*Par->Size_Theta+theta)+k].position = jj*Par->Size_Theta+ii;
										break;
									}
						}
				}
		}


		sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Neighbor_Map_RGB,sizeof(Neighborhood),Par->Size_LP * Pix_Numb * 3,fout);
		fclose (fout);

		free(Neighbor_Map_RGB);
		free(Tables.XYMap);
//		free(X_Map);
//		free(Y_Map);

		free(Color_Map);
		return 1;
	}


}
	
/************************************************************************
*																		*
* Build_Pad_Map															*
* 																		*
* Builds a Look-up Table with the information needed for the padding	*
* and saves it in the working folder with the file name "PadMap.gio"	*
*																		*
* The information is taken from an image which has been built off-line.	*
*																		*
* Input: Path															*
* 			Working folder												*
* 		 Parameters														*
* 			Pointer to a struct containing all the needed parameters	*
* 																		*
************************************************************************/	

int Build_Pad_Map (Image_Data * Parameters,char * Path)
{
	int i,j,k=0;
	int XSize,YSize,planes;
	unsigned char  * Image;
	unsigned short * Pad_Map;
	char File_Name [256];

	FILE * fout;
	
	sprintf(File_Name,"%s%s",Path,"ReferenceImage.bmp");

	Image = Read_Bitmap(&XSize,&YSize,&planes,File_Name);

	if (Image != NULL)
	{
		Pad_Map = (unsigned short *)malloc(Parameters->Size_Theta * Parameters->Size_Fovea * sizeof(unsigned short));

		for (j=0; j<Parameters->Size_Fovea; j++)
		{	
			for (i=0; i<Parameters->Size_Theta; i++)
			{
				Pad_Map[j*Parameters->Size_Theta+i] = 1;
				if(Image[3*(j*Parameters->Size_Theta+i)] != 128)
				{
					Pad_Map[k] = j*Parameters->Size_Theta+i;
					k++;
				}
			}
			k=Parameters->Size_Theta*(j+1);
		}
		

		sprintf(File_Name,"%s%s",Path,"PadMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Pad_Map,sizeof(unsigned short),Parameters->Size_Theta * Parameters->Size_Fovea,fout);
		fclose (fout);

		free (Pad_Map);
		free (Image);

		return 1;
	}
	else 
		return 0;
}


/************************************************************************
* Build_Remap_Map     													*
* Costruisce una tabella di corrispondenza tra coordinate Log-Polari  	*
* e cartesiane. (per adesso di intende la fovea paddata)				*
*																		*
* Input: Fovea Type: it can be:     									*
*					 0. Giotto 2.0 (33k pixels, old fovea)				*
*					 1. Giotto 2.1 (33k pixels, new fovea)				*
* 																		*
************************************************************************/	

int Build_Remap_Map (Image_Data * Parameters,
					  char * Path)
{
	int i,j;
	char File_Name [256];
	int XSize, YSize;
	int rho,theta;
	double   * Ang_Shift_Map;
	unsigned short * Pad_Map;
	int      * Remap_Map;

	int startX,startY;
	int endX,endY;

	int retval;
	int PadSizeTheta;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Parameters,&Tables,Path,17);

	if (retval != 17)
	{
		if (Tables.AngShiftMap!=NULL)
			free(Tables.AngShiftMap);
		if (Tables.PadMap!=NULL)
			free(Tables.PadMap);
		return 0;
	}
	else
	{
		Ang_Shift_Map = Tables.AngShiftMap;
		Pad_Map = Tables.PadMap;

		PadSizeTheta = (((Parameters->Size_Theta * Parameters->LP_Planes) % Parameters->padding) + (Parameters->Size_Theta * Parameters->LP_Planes));
		XSize = (int)(Parameters->Resolution * Parameters->Zoom_Level + 0.5);
		YSize = XSize;

//		Remap_Map = (int *)malloc(XSize * YSize * sizeof(int));
		Remap_Map = (int *)malloc(Parameters->Size_Img_Remap * sizeof(int));

		startX = (XSize/2)-Parameters->Size_X_Remap/2;
		startY = (YSize/2)-Parameters->Size_Y_Remap/2;
		endX   = (XSize/2)+Parameters->Size_X_Remap/2;
		endY   = (YSize/2)+Parameters->Size_Y_Remap/2;
	//	if (Parameters->Fovea_Type == 0)
		if (1)
		{
//			for (j=0; j<YSize; j++)
//				for (i=0; i<XSize; i++)
			for (j=startY; j<endY; j++)
				for (i=startX; i<endX; i++)
				{
					rho   = (int)(Get_Rho  (i-XSize/2,YSize/2-j,Parameters));
					theta = (int)(Get_Theta(i-XSize/2,YSize/2-j,rho,Parameters,Ang_Shift_Map,Pad_Map));
					if ((rho<Parameters->Size_Rho)&&(rho>=0)&&(theta<Parameters->Size_Theta)&&(theta>=0))
//						Remap_Map[j*XSize+i] = 3*(rho*Parameters->Size_Theta+theta);
//						Remap_Map[j*XSize+i] = (rho*PadSizeTheta+3*theta);
						Remap_Map[(j-startY)*Parameters->Size_X_Remap+i-startX] = (rho*PadSizeTheta+3*theta);
					else
						Remap_Map[(j-startY)*Parameters->Size_X_Remap+i-startX] = 0;
				}

		}

//		sprintf(File_Name,"%s%s_%2.3f%s",Path,"RemapMap",Parameters->Zoom_Level,".gio");
		sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Parameters->Zoom_Level,Parameters->Size_X_Remap,Parameters->Size_Y_Remap,".gio");
		
		fout = fopen(File_Name,"wb");
//		fwrite(Remap_Map,sizeof(int),XSize * YSize,fout);
		fwrite(Remap_Map,sizeof(int),Parameters->Size_Img_Remap,fout);
		fclose (fout);

		free (Pad_Map);
		free (Ang_Shift_Map);
		free (Remap_Map);
		return 1;
	}

}

/************************************************************************
* Build_Weights_Map		  												*
************************************************************************/	

unsigned char Build_Weights_Map(Image_Data * Par,
								char * Path)
{
	int i,j,k;
	int rho,theta;
	int CurrPix;
	float DistSumRGB[3];
	int plane;
	Neighborhood * Weights_Map;
	Neighborhood * Neighbor_Map_RGB;
	char File_Name [256];

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;
		
	int PadSizeTheta = ((Par->Size_Theta * 1) % Par->padding);

	retval = Load_Tables(Par,&Tables,Path,8);

	if (retval != 8)
		return 0;
	else
	{
		Neighbor_Map_RGB = Tables.NeighborMap;

		if (Par->Pix_Numb>MAX_PIX)
			Par->Pix_Numb = MAX_PIX;

		Weights_Map = (Neighborhood *)malloc(Par->Size_LP * Par->Pix_Numb * 3 * sizeof(Neighborhood)); 

//		Neighbor_Map_RGB = (Neighborhood *)malloc(Par->Size_LP * MAX_PIX * 3 * sizeof(Neighborhood));
//
//		sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");
//		fin = fopen(File_Name,"rb");
//		fread(Neighbor_Map_RGB,sizeof(Neighborhood),Par->Size_LP * MAX_PIX * 3,fin);
//		fclose (fin);

		for (k=0; k<Par->Size_LP * 3; k++)
			for (j=0; j<Par->Pix_Numb; j++)
				Weights_Map[k*Par->Pix_Numb+j].position = Neighbor_Map_RGB[k*MAX_PIX+j].position;


		for (rho=0; rho<Par->Size_Rho; rho++)
			for (theta=0; theta<Par->Size_Theta; theta++)
			{
				CurrPix = rho*Par->Size_Theta+theta;
				for (plane = RED; plane <= BLUE; plane ++)
				{				
					DistSumRGB[plane] = 0;
					if (Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].position!=0)
					{
						for (k=1; k<Par->Pix_Numb; k++)
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight);
					
						if (Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].weight <= 0.00001)
						{
							if (Par->Pix_Numb > 1)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 0.75;
							else
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 1.0;
							for (k=1; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(0.25/(Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					
						else
						{
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].weight);
							for (k=0; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(1.0/(Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					}
					else
					{
						for (k=0; k<Par->Pix_Numb; k++)
						{
							Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].position = 0;				
							Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = 0.0;				
						}
					}
				}
			}

		for (k=0; k<3*Par->Size_LP*Par->Pix_Numb; k++)
		{
			i = Weights_Map[k].position % Par->Size_Theta;				
			j = Weights_Map[k].position / Par->Size_Theta;	
			Weights_Map[k].position = j*(Par->Size_Theta+PadSizeTheta)+i;
		}



		sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Par->Pix_Numb,".gio");

		fout = fopen(File_Name,"wb");
		fwrite(Weights_Map,sizeof(Neighborhood),Par->Size_LP * Par->Pix_Numb * 3,fout);
		fclose (fout);

		free(Weights_Map);
		free(Neighbor_Map_RGB);

		return Par->Pix_Numb;
	}
}

/************************************************************************
* Build_XY_Map     														*
* Costruisce una tabella di corrispondenza tra coordinate Log-Polari  	*
* e cartesiane. (per adesso di intende la fovea paddata)				*
*																		*
* Input: Fovea Type: it can be:     									*
*					 0. Giotto 2.0 (33k pixels, old fovea)				*
*					 1. Giotto 2.1 (33k pixels, new fovea)				*
*        Table File Name												*
* 		 Ang_Shift_Filename												*
* 																		*
************************************************************************/	

int Build_XY_Map (Image_Data * Parameters,
		 		   char * Path)
{
	int i,j;
	unsigned short * Pad_Map;
	double * Ang_Shift_Map;
	double * One_Line;
	double * X_Map;
	double * Y_Map;
	char File_Name [256];

	FILE * fout;

	int retval;
	LUT_Ptrs Tables;

	retval = Load_Tables(Parameters,&Tables,Path,17);

	if (retval != 17)
	{
		if (Tables.AngShiftMap!=NULL)
			free(Tables.AngShiftMap);
		if (Tables.PadMap!=NULL)
			free(Tables.PadMap);
		return 0;
	}
	else
	{
		Ang_Shift_Map = Tables.AngShiftMap;
		Pad_Map = Tables.PadMap;
		One_Line = (double *)calloc(Parameters->Size_Theta,sizeof(double));
		

		X_Map = (double *)calloc(Parameters->Size_LP,sizeof(double));
		Y_Map = (double *)calloc(Parameters->Size_LP,sizeof(double));

		for (j=1; j< Parameters->Size_Rho; j++)
			for (i=0; i<Parameters->Size_Theta; i++)
			{
				X_Map[j*Parameters->Size_Theta+i] = Get_X_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
				Y_Map[j*Parameters->Size_Theta+i] = Get_Y_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
			}
/*
		for (j=1; j< Parameters->Size_Fovea; j++)
			for (i=0; i<6*j; i++)
			{
				X_Map[j*Parameters->Size_Theta+i] = Get_X_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
				Y_Map[j*Parameters->Size_Theta+i] = Get_Y_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
			}
		for (j=Parameters->Size_Fovea; j< Parameters->Size_Rho; j++)
			for (i=0; i<Parameters->Size_Theta; i++)
			{
				X_Map[j*Parameters->Size_Theta+i] = Get_X_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
				Y_Map[j*Parameters->Size_Theta+i] = Get_Y_Center(j,i,Parameters,Ang_Shift_Map,Pad_Map);
			}
		
		//Unpadding

		for (j=0;j<Parameters->Size_Fovea;j++)
		{
			memcpy(One_Line,X_Map + j*Parameters->Size_Theta,Parameters->Size_Theta * sizeof(double));

			memset(X_Map + j*Parameters->Size_Theta,0,Parameters->Size_Theta * sizeof(double));

			for (i=0;i<Parameters->Size_Theta;i++)
				X_Map[Pad_Map[j*Parameters->Size_Theta+i]] = One_Line[i];

			memcpy(One_Line,Y_Map+j*Parameters->Size_Theta,Parameters->Size_Theta * sizeof(double));
			memset(Y_Map+j*Parameters->Size_Theta,0,Parameters->Size_Theta*sizeof(double));
			for (i=0;i<Parameters->Size_Theta;i++)
				Y_Map[Pad_Map[j*Parameters->Size_Theta+i]] = One_Line[i];
		}
*/		
		sprintf(File_Name,"%s%s",Path,"XYMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(X_Map,sizeof(double),Parameters->Size_LP,fout);
		fwrite(Y_Map,sizeof(double),Parameters->Size_LP,fout);
		fclose (fout);

		free (One_Line);
		free (Pad_Map);
		free (Ang_Shift_Map);
		free (X_Map);
		free (Y_Map);

		return 1;
	}

}

int Crop_Remap_Map(Image_Data *Par, char* Path)
{
	int x,y;
	int shiftx,shifty;
	int YSize, XSize;
	int * Rem_LUT;
	int * Crop_Rem_LUT;
	char File_Name [256];

	LUT_Ptrs Tables;

	FILE * fin, * fout;

	sprintf(File_Name,"%s%s_%2.3f%s",Path,"RemapMap",Par->Zoom_Level,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		XSize = (int)(Par->Resolution * Par->Zoom_Level + 0.5);
		YSize = XSize;

		Tables.RemapMap = (int *) malloc (YSize * XSize * sizeof(int));
		fread(Tables.RemapMap,sizeof(int),YSize * XSize,fin);
		fclose (fin);

		Rem_LUT = Tables.RemapMap;
		

		Crop_Rem_LUT = (int *)malloc(Par->Size_Img_Remap * sizeof(int));

		shiftx = (int)(Par->Resolution* Par->Zoom_Level-Par->Size_X_Remap)/2;
		shifty = (int)(Par->Resolution* Par->Zoom_Level-Par->Size_Y_Remap)/2;

		for (y=0; y<Par->Size_Y_Remap; y++)
			for (x=0; x<Par->Size_X_Remap; x++)
				if ((y+shifty>=0)&&(x+shiftx>=0)&&(y+shifty<YSize)&&(x+shiftx<XSize))
					Crop_Rem_LUT[y*Par->Size_X_Remap+x] = Rem_LUT[(y+shifty) * XSize + x+shiftx];
				else
					Crop_Rem_LUT[y*Par->Size_X_Remap+x] = 0;

		sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Par->Zoom_Level,Par->Size_X_Remap,Par->Size_Y_Remap,".gio");
		
		fout = fopen(File_Name,"wb");
		fwrite(Crop_Rem_LUT,sizeof(int),Par->Size_Img_Remap,fout);
		fclose (fout);

		free(Rem_LUT);
		free(Crop_Rem_LUT);

		return 1;
	}
	else
	{
		Tables.RemapMap = NULL;
		return 0;
	}

}


/************************************************************************
* Build_Remap_Map_No_Fov												*
* Costruisce una tabella di corrispondenza tra coordinate Log-Polari  	*
* e cartesiane. (per adesso di intende la fovea paddata)				*
*																		*
* Input: Fovea Type: it can be:     									*
*					 0. Giotto 2.0 (33k pixels, old fovea)				*
*					 1. Giotto 2.1 (33k pixels, new fovea)				*
* 																		*
************************************************************************/	

int Build_Remap_Map_No_Fov (Image_Data * Parameters,
					  char * Path)
{
	int i,j;
	char File_Name [256];
	int XSize, YSize;
	int rho,theta;
	double   * Ang_Shift_Map;
	unsigned short * Pad_Map;
	int      * Remap_Map;
	int PadSizeTheta;

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Parameters,&Tables,Path,17);

	if (retval != 17)
	{
		if (Tables.AngShiftMap!=NULL)
			free(Tables.AngShiftMap);
		if (Tables.PadMap!=NULL)
			free(Tables.PadMap);
		return 0;
	}
	else
	{
		PadSizeTheta = (((Parameters->Size_Theta * Parameters->LP_Planes) % Parameters->padding) + (Parameters->Size_Theta * Parameters->LP_Planes));
		Ang_Shift_Map = Tables.AngShiftMap;
		Pad_Map = Tables.PadMap;

		XSize = (int)(Parameters->Resolution * Parameters->Zoom_Level + 0.5);
		YSize = XSize;

		Remap_Map = (int *)malloc(XSize * YSize * sizeof(int));

	//	if (Parameters->Fovea_Type == 0)
		if (1)
		{
			for (j=0; j<YSize; j++)
				for (i=0; i<XSize; i++)
				{
					rho   = (int)(Get_Rho  (i-XSize/2,YSize/2-j,Parameters));
					theta = (int)(Get_Theta(i-XSize/2,YSize/2-j,rho,Parameters,Ang_Shift_Map,Pad_Map));
					if ((rho<Parameters->Size_Rho)&&(rho>=Parameters->Size_Fovea)&&(theta<Parameters->Size_Theta)&&(theta>=0))
//						Remap_Map[j*XSize+i] = 3*((rho-Parameters->Size_Fovea)*Parameters->Size_Theta+theta);
//						Remap_Map[j*XSize+i] = (rho*PadSizeTheta+3*theta);
						Remap_Map[j*XSize+i] = ((rho-Parameters->Size_Fovea)*PadSizeTheta+3*theta);
					else
						Remap_Map[j*XSize+i] = -1;
				}

		}

		sprintf(File_Name,"%s%s_%2.3f%s",Path,"RemapMapNoFov",Parameters->Zoom_Level,".gio");
		
		fout = fopen(File_Name,"wb");
		fwrite(Remap_Map,sizeof(int),XSize * YSize,fout);
		fclose (fout);

		free (Pad_Map);
		free (Ang_Shift_Map);
		free (Remap_Map);
		return 1;
	}

}


/************************************************************************
* Build_Neighborhood_Map_No_Fov											*
*																		*
* Input: Pix_Numb														*
* 		 Path															*
* 																		*
************************************************************************/	

int Build_Neighborhood_Map_NoFov(Image_Data * Par,
							char * Path)
{
	int j,k;
	int ii,jj,kk;
	int plane;
	int rho,theta;
	int bottomrho,bottomtheta;
	int toprho,toptheta;
	int CurrPix,TestPix;
	double distX,distY, distXY;
	char File_Name [256];
	int Pix_Numb = MAX_PIX;

	Neighborhood * Neighbor_Map_RGB;
	double		 * X_Map;
	double		 * Y_Map;
	char		 * Color_Map;

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Par,&Tables,Path,130);

	
	Neighbor_Map_RGB = (Neighborhood *)malloc((Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb * 3 * sizeof(Neighborhood));

	if (retval != 130)
	{
		if (Tables.ColorMap!=NULL)
			free(Tables.ColorMap);
		if (Tables.XYMap!=NULL)
			free(Tables.XYMap);
		return 0;
	}
	else
	{
		Color_Map = Tables.ColorMap;
		X_Map = Tables.XYMap;
		Y_Map = Tables.XYMap+Par->Size_LP;


		for (j=0; j<(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb * 3; j++)
		{
			Neighbor_Map_RGB [j].weight = (float)(Par->Size_Img_Remap);
			Neighbor_Map_RGB [j].position = 0;
		}

		for (rho=Par->Size_Fovea; rho<Par->Size_Rho; rho++)
		{
			if (rho-(Pix_Numb+1)/2>Par->Size_Fovea)
				bottomrho = rho-(Pix_Numb+1)/2;
			else bottomrho = Par->Size_Fovea;

			if (1+rho+(Pix_Numb+1)/2<Par->Size_Rho)
				toprho = 1+rho+(Pix_Numb+1)/2;
			else toprho = Par->Size_Rho;

			for (theta=0; theta<Par->Size_Theta; theta++)
				if (Color_Map[rho*Par->Size_Theta+theta]!=-1)
				{
					CurrPix = rho*Par->Size_Theta+theta;

					if ((theta-(Pix_Numb+1)/2>0)&&(1+theta+(Pix_Numb+1)/2<Par->Size_Theta))
					{
						bottomtheta = theta-(Pix_Numb+1)/2;
						toptheta = 1+theta+(Pix_Numb+1)/2;
					}
					else
					{
						bottomtheta = 0;
						toptheta	= Par->Size_Theta;
					}
					if (rho<Par->Size_Fovea)
					{
						bottomtheta = 0;
						toptheta	= Par->Size_Theta;
					}
					for (jj=bottomrho; jj<toprho; jj++)
						for (ii=bottomtheta; ii<toptheta; ii++)
						{
							TestPix = jj*Par->Size_Theta+ii;
							distX = X_Map[CurrPix]-X_Map[TestPix];
							distX = distX*distX;
							distY = Y_Map[CurrPix]-Y_Map[TestPix];
							distY = distY*distY;
							distXY  = distX+distY;

							for (plane = RED; plane <= BLUE; plane ++)
								for (k=0; k<Pix_Numb; k++)
									if ((distXY < Neighbor_Map_RGB[(plane * (Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+k].weight)&&(Color_Map[jj*Par->Size_Theta+ii] == plane))
									{
										if (Pix_Numb >1)
											for (kk=Pix_Numb-2; kk>=k; kk--)
											{
												Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta)* Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+kk+1].weight    = Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+kk].weight;
												Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta)* Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+kk+1].position  = Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+kk].position;
											}
										Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+k].weight = (float)(distXY);
										Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb)+Pix_Numb*((rho-Par->Size_Fovea)*Par->Size_Theta+theta)+k].position = (jj-Par->Size_Fovea)*Par->Size_Theta+ii;
										break;
									}
						}
				}
		}

//		for (k=0; k<10; k++)
//			printf ("%d,%d\n",Neighbor_Map_RGB[10 * ( 7 * Par->Size_Theta+2)+k].position/252,Neighbor_Map_RGB[10 * ( 7 * Par->Size_Theta+2)+k].position%252);

		sprintf(File_Name,"%s%s",Path,"NeighborhoodMapNoFov.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Neighbor_Map_RGB,sizeof(Neighborhood),(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Pix_Numb * 3,fout);
		fclose (fout);

		free(Neighbor_Map_RGB);
		free(Tables.XYMap);
//		free(X_Map);
//		free(Y_Map);

		free(Color_Map);
		return 1;
	}


}


/************************************************************************
* Build_Weights_Map_NoFov	  											*
************************************************************************/	

unsigned char Build_Weights_Map_NoFov(Image_Data * Par,
								char * Path)
{
	int i,j,k;
	int rho,theta;
	int CurrPix;
	float DistSumRGB[3];
	int plane;
	Neighborhood * Weights_Map;
	Neighborhood * Neighbor_Map_RGB;
	char File_Name [256];

///	int retval;
	LUT_Ptrs Tables;

	FILE * fout, *fin;
		
	int PadSizeTheta = ((Par->Size_Theta * 1) % Par->padding);

//	retval = Load_Tables(Par,&Tables,Path,8);
		sprintf(File_Name,"%s%s",Path,"NeighborhoodMapNoFov.gio");
		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			Tables.NeighborMap = (Neighborhood *) malloc ((Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX * 3 * sizeof(Neighborhood));
			fread(Tables.NeighborMap,sizeof(Neighborhood),(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX * 3,fin);
			fclose (fin);
		}
		else
			Tables.NeighborMap = NULL;

//	if (retval != 8)
	if (0)
		return 0;
	else
	{
		Neighbor_Map_RGB = Tables.NeighborMap;

		if (Par->Pix_Numb>MAX_PIX)
			Par->Pix_Numb = MAX_PIX;

		Weights_Map = (Neighborhood *)malloc((Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb * 3 * sizeof(Neighborhood)); 

//		Neighbor_Map_RGB = (Neighborhood *)malloc(Par->Size_LP * MAX_PIX * 3 * sizeof(Neighborhood));
//
//		sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");
//		fin = fopen(File_Name,"rb");
//		fread(Neighbor_Map_RGB,sizeof(Neighborhood),Par->Size_LP * MAX_PIX * 3,fin);
//		fclose (fin);

		for (k=0; k<(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * 3; k++)
			for (j=0; j<Par->Pix_Numb; j++)
				Weights_Map[k*Par->Pix_Numb+j].position = Neighbor_Map_RGB[k*MAX_PIX+j].position;


		for (rho=Par->Size_Fovea; rho<Par->Size_Rho; rho++)
			for (theta=0; theta<Par->Size_Theta; theta++)
			{
				CurrPix = (rho-Par->Size_Fovea)*Par->Size_Theta+theta;
				for (plane = RED; plane <= BLUE; plane ++)
				{				
					DistSumRGB[plane] = 0;
					if (Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix].position!=0)
					{
						for (k=1; k<Par->Pix_Numb; k++)
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix+k].weight);
					
						if (Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix].weight <= 0.00001)
						{
							if (Par->Pix_Numb > 1)
								Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 0.75;
							else
								Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 1.0;
							for (k=1; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(0.25/(Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					
						else
						{
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix].weight);
							for (k=0; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(1.0/(Neighbor_Map_RGB[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					}
					else
					{
						for (k=0; k<Par->Pix_Numb; k++)
						{
							Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].position = 0;				
							Weights_Map[(plane*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = 0.0;				
						}
					}
				}
			}

//		for (k=0; k<4; k++)
//			printf ("%d,%d\n",Weights_Map[4 * ( 7 * Par->Size_Theta+2)+k].position/252,Weights_Map[4 * ( 7 * Par->Size_Theta+2)+k].position%252);

		for (k=0; k<3*(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta)*Par->Pix_Numb; k++)
		{
			i = Weights_Map[k].position % Par->Size_Theta;				
			j = Weights_Map[k].position / Par->Size_Theta;	
			Weights_Map[k].position = j*(Par->Size_Theta+PadSizeTheta)+i;
		}

//		for (k=0; k<4; k++)
//			printf ("%d,%d\n",Weights_Map[4 * ( 7 * Par->Size_Theta+2)+k].position/256,Weights_Map[4 * ( 7 * Par->Size_Theta+2)+k].position%256);

		sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMapNoFov",Par->Pix_Numb,".gio");

		fout = fopen(File_Name,"wb");
		fwrite(Weights_Map,sizeof(Neighborhood),(Par->Size_Rho - Par->Size_Fovea)*(Par->Size_Theta) * Par->Pix_Numb * 3,fout);
		fclose (fout);

		free(Neighbor_Map_RGB);

		return Par->Pix_Numb;
	}
}

/************************************************************************
* Build_Fast_Weights_Map		  												*
************************************************************************/	

//unsigned char Build_Fast_Weights_Map(Image_Data * Par,
//								char * Path)
IntNeighborhood * Build_Fast_Weights_Map(Image_Data * Par,
								char * Path)
{
	int i,j,k,planes;
	int rho,theta;
	int CurrPix;
	float DistSumRGB[3];
	int plane;
	Neighborhood * Weights_Map;
	IntNeighborhood * Fast_Weights_Map;
	Neighborhood * Neighbor_Map_RGB;
///	char File_Name [256];

	int retval;
	LUT_Ptrs Tables;

///	FILE * fout;
		
	int PadSizeTheta = ((Par->Size_Theta * 1) % Par->padding);

	retval = Load_Tables(Par,&Tables,Path,8);

	if (retval != 8)
		return 0;
	else
	{
		Neighbor_Map_RGB = Tables.NeighborMap;

		if (Par->Pix_Numb>MAX_PIX)
			Par->Pix_Numb = MAX_PIX;

		Weights_Map = (Neighborhood *)malloc(Par->Size_LP * Par->Pix_Numb * 3 * sizeof(Neighborhood)); 

//		Neighbor_Map_RGB = (Neighborhood *)malloc(Par->Size_LP * MAX_PIX * 3 * sizeof(Neighborhood));
//
//		sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");
//		fin = fopen(File_Name,"rb");
//		fread(Neighbor_Map_RGB,sizeof(Neighborhood),Par->Size_LP * MAX_PIX * 3,fin);
//		fclose (fin);

		for (k=0; k<Par->Size_LP * 3; k++)
			for (j=0; j<Par->Pix_Numb; j++)
				Weights_Map[k*Par->Pix_Numb+j].position = Neighbor_Map_RGB[k*MAX_PIX+j].position;


		for (rho=0; rho<Par->Size_Rho; rho++)
			for (theta=0; theta<Par->Size_Theta; theta++)
			{
				CurrPix = rho*Par->Size_Theta+theta;
				for (plane = RED; plane <= BLUE; plane ++)
				{				
					DistSumRGB[plane] = 0;
					if (Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].position!=0)
					{
						for (k=1; k<Par->Pix_Numb; k++)
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight);
					
						if (Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].weight <= 0.00001)
						{
							if (Par->Pix_Numb > 1)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 0.75;
							else
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix].weight = 1.0;
							for (k=1; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(0.25/(Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					
						else
						{
							DistSumRGB[plane] += (float)(1.0/Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix].weight);
							for (k=0; k<Par->Pix_Numb; k++)
								Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = (float)(1.0/(Neighbor_Map_RGB[(plane*Par->Size_LP * MAX_PIX)+MAX_PIX*CurrPix+k].weight*DistSumRGB[plane]));
						}
					}
					else
					{
						for (k=0; k<Par->Pix_Numb; k++)
						{
							Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].position = 0;				
							Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*CurrPix+k].weight = 0.0;				
						}
					}
				}
			}

		for (k=0; k<3*Par->Size_LP*Par->Pix_Numb; k++)
		{
			i = Weights_Map[k].position % Par->Size_Theta;				
			j = Weights_Map[k].position / Par->Size_Theta;	
			Weights_Map[k].position = j*(Par->Size_Theta+PadSizeTheta)+i;
		}

/*
	for (plane=0;plane<3;plane++)
		for (j=0; j<Par->Size_LP; j++)
			for (i=0; i<Par->Pix_Numb; i++)
				if (Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*j+i].weight>(1/sqrt(2)))
					Fast_Weights_Map[3*j+plane].weight[i] = 1.0;
				else if (Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*j+i].weight<=(1/sqrt(256*512)))
					Fast_Weights_Map[3*j+plane].weight[i] = 0.0;
				else for (k=1; k<256; k*=2)
					if ((Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*j+i].weight<=(1/sqrt(k*(k*2))))&&(Weights_Map[(plane*Par->Size_LP * Par->Pix_Numb)+Par->Pix_Numb*j+i].weight>(1/sqrt((k*2)*(k*4)))))
						Fast_Weights_Map[3*j+plane].weight[i] = (float)(1.0)/(k*2);
*/
//Sorting
/*
	float MAX;
	int MAXIndex;
	float TempW;
	unsigned short TempP;

	for (j=0; j<3 * Par->Size_LP * Par->Pix_Numb; j+=4)
	{
		for (k=0; k<Par->Pix_Numb; k++)
		{
			MAX = -1;
			for (i=k; i<Par->Pix_Numb; i++)
			{
				if (Weights_Map[j+i].weight>=MAX)
				{
					MAX = Weights_Map[j+i].weight;
					MAXIndex = i;
				}
			}
			TempW = Weights_Map[j+MAXIndex].weight;
			TempP = Weights_Map[j+MAXIndex].position;

			Weights_Map[j+MAXIndex].weight = Weights_Map[j+k].weight;
			Weights_Map[j+MAXIndex].position = Weights_Map[j+k].position;

			Weights_Map[j+k].weight = TempW;
			Weights_Map[j+k].position = TempP;
		}
	}
*/
//Weight Quantization
		
	for (j=0; j<Par->Size_LP * Par->Pix_Numb * 3; j++)
		if (Weights_Map[j].weight>(1/sqrt((double)2)))
			Weights_Map[j].weight = 1.0;
		else if (Weights_Map[j].weight<=(1/sqrt((double(256*512)))))
			Weights_Map[j].weight = 0.0;
		else for (k=1; k<256; k*=2)
			if ((Weights_Map[j].weight<=(1/sqrt(double(k*(k*2)))))&&(Weights_Map[j].weight>(1/sqrt((double((k*2)*(k*4)))))))
				Weights_Map[j].weight = (float)(1.0)/(k*2);


	double Sum;

	for (j=0; j<3 * Par->Size_LP * Par->Pix_Numb; j+=Par->Pix_Numb)
	{
		Sum = 0;
		for (i=0; i<Par->Pix_Numb; i++)
		{
			if (Sum >= 1.0)
			{
				Weights_Map[j+i].weight = 0.0;
				Weights_Map[j+i].position = Par->Size_LP;
			}
			Sum += Weights_Map[j+i].weight;
		}
	}
	
	for (j=0; j<3 * Par->Size_LP * Par->Pix_Numb; j+=Par->Pix_Numb)
		if(Weights_Map[j+i].weight == 0.0)
			Weights_Map[j+i].position = Par->Size_LP;

//Check

	int counter = 0;
	float CheckSum;

	do
	{
		counter = 0;
		for (j=0; j<3 * Par->Size_LP * Par->Pix_Numb; j+=Par->Pix_Numb)
		{
			Sum = 0;
			for (i=0; i<Par->Pix_Numb; i++)
				Sum += Weights_Map[j+i].weight;
			
			if ((Sum != 1.0)&&(Sum != 0.0))
			{
				CheckSum = (float)(1.0-Sum);
				Sum = 0;
				for (i=0; i<Par->Pix_Numb; i++)
				{
					if ((Weights_Map[j+i].weight != 1.0)&&(Weights_Map[j+i].weight<=CheckSum-Sum))
					{
						Sum += Weights_Map[j+i].weight;
						Weights_Map[j+i].weight *= 2;
						if (Sum>=CheckSum)
							break;
					}
				}

	//			printf("%f\n",Sum - 16.0);
			}
		}

		for (j=0; j<3 * Par->Size_LP * Par->Pix_Numb; j+=Par->Pix_Numb)
		{
			Sum = 0;
			for (i=0; i<Par->Pix_Numb; i++)
				Sum += Weights_Map[j+i].weight;
			if ((Sum != 1.0)&&(Sum != 0.0))
			{
	//			printf("error %d, %f\n",counter,Sum - 16.0);
				counter ++;
			}
		}
		printf("%d\n",counter);
	}
	while (counter != 0);

//Fast Table Building
		
//		for (i=0; i<Par->Size_LP * 3; i++)
//		{
//			Fast_Weights_Map[i].NofPixels = Par->Pix_Numb;
//			Fast_Weights_Map[i].position  = (unsigned short *) malloc (Par->Pix_Numb * sizeof(unsigned short));
//			Fast_Weights_Map[i].weight    = (unsigned char  *) malloc (Par->Pix_Numb * sizeof(unsigned  char));
//		}

	Fast_Weights_Map = (IntNeighborhood *) malloc (3 * Par->Size_LP * sizeof(IntNeighborhood));

	for (j=0; j<Par->Size_LP; j++)
		for (planes=0; planes<3; planes++)
		{
			for (i=0; i<Par->Pix_Numb; i++)
				if (Weights_Map[planes * Par->Size_LP * Par->Pix_Numb+Par->Pix_Numb*j+i].position == Par->Size_LP)
					break;
			Fast_Weights_Map[3*j+planes].NofPixels = i;
			if (i!=0)
			{
				Fast_Weights_Map[3*j+planes].position = (unsigned short *) malloc (i * sizeof(unsigned short));
				Fast_Weights_Map[3*j+planes].weight   = (unsigned char  *)  malloc (i * sizeof(unsigned  char));
			}

			for (i=0; i<Fast_Weights_Map[3*j+planes].NofPixels; i++)
				if(Weights_Map[planes * Par->Size_LP * Par->Pix_Numb+Par->Pix_Numb*j+i].position!=Par->Size_LP)
				{
					Fast_Weights_Map[3*j+planes].position[i] = Weights_Map[planes * Par->Size_LP * Par->Pix_Numb+Par->Pix_Numb*j+i].position;
					Fast_Weights_Map[3*j+planes].weight[i] = (unsigned char)(-log10(double(Weights_Map[planes * Par->Size_LP * Par->Pix_Numb+Par->Pix_Numb*j+i].weight))/log10(double(2)));
				}
		}


//	sprintf(File_Name,"%s%s%02d%s",Path,"FastWeightsMap",Par->Pix_Numb,".gio");

//		fout = fopen(File_Name,"wb");
//		fwrite(Fast_Weights_Map,sizeof(IntNeighborhood),Par->Size_LP * 3,fout);
//		fclose (fout);

		free(Neighbor_Map_RGB);
//		free(Fast_Weights_Map);
//		free(Weights_Map);

//		return Par->Pix_Numb;
		return Fast_Weights_Map;
	}
}
	
int Build_DS_Map(Image_Data * LParam,char * Path, float Ratio)
{
	Image_Data SParam;

	//Tables
	double * FakeAngShift;
	unsigned short FakePadMap;
	int * LRemapMap,* SRemapMap;
	unsigned char * LRem, * SRem;
	Neighborhood ** DownSampleTable;
	IntNeighborhood * IntDownSampleTable;


	int rho,theta;
	int rhosmall, thetasmall, valid;
	int x,y;
	int i,j,k;
	int step;

	int LRho,LTheta,LPlanes;
	int SRho,STheta,SPlanes;

	char LocalPath [256]; 
	char File_Name [256]; 
	FILE * fin, * fout;

	char SearchString [] = ".00";
	char * pointer = NULL;
	bool modpath = false;
	
	pointer = strstr(Path,SearchString);

	if (pointer != NULL)
	{
		*(--pointer) = 0;
		modpath = true;
	}

//Loads the Reference Images

	sprintf(File_Name,"%s%2.2f\\ReferenceImage.bmp",Path,1.00);
	unsigned char * LLP = Read_Bitmap(&LTheta,&LRho,&LPlanes,File_Name);

	sprintf(File_Name,"%s%2.2f\\ReferenceImage.bmp",Path,Ratio);
	unsigned char * SLP = Read_Bitmap(&STheta,&SRho,&SPlanes,File_Name);

//Sets the parameters
	
	LParam->Zoom_Level = 1090.0/83.0; //83 is (SizeFovea*2)-1

	SParam = Set_Param( (int)(1090/Ratio),(int)(1090/Ratio),
						(int)(1090),(int)(1090),
						(int)(152/Ratio),(int)(252/Ratio),(int)(42/Ratio),
						(int)(1090/Ratio),
						CUSTOM,
						LParam->Zoom_Level*Ratio);

	LParam->Fovea_Type = 2;
	SParam.padding = LParam->padding;
	SParam.Fovea_Type = 0;

	int ListSize = (int)((Ratio + 2) * (Ratio + 2));

//Memory Allocation

	FakeAngShift = (double *) calloc (LParam->Size_Rho,sizeof(double)); //Not Actually used
	DownSampleTable = (Neighborhood **) malloc (SParam.Size_LP * sizeof(Neighborhood *));
	* DownSampleTable = (Neighborhood *) malloc (SParam.Size_LP * ListSize * sizeof(Neighborhood));
	LRem = (unsigned char *) malloc (LParam->Size_Img_Remap * 3 * sizeof(unsigned char));
	SRem = (unsigned char *) malloc (SParam.Size_Img_Remap * 3 * sizeof(unsigned char));
	IntDownSampleTable = (IntNeighborhood *) malloc (SParam.Size_LP * sizeof(IntNeighborhood));

	double MidZoomLevel;

	MidZoomLevel = Get_X_Center(1+(LParam->Size_Rho+LParam->Size_Fovea)/2,0,LParam,FakeAngShift,&FakePadMap)-Get_X_Center((LParam->Size_Rho+LParam->Size_Fovea)/2,0,LParam,FakeAngShift,&FakePadMap);
	MidZoomLevel /= Get_X_Center(2,0,LParam,FakeAngShift,&FakePadMap)-Get_X_Center(1,0,LParam,FakeAngShift,&FakePadMap);

	LParam->Fovea_Type = 0;

//Labels the reference images
	for (j=0; j<LRho*LTheta; j++)
	{
		LLP[3*j+0] = j/LTheta;	//rho
		LLP[3*j+1] = j%LTheta;	//theta;

		if (LLP[3*j+2]!=128)
			LLP[3*j+2] = 0;
		else
			LLP[3*j+2] = 255;
	}

	int Limit[4];

	Limit[0] = 0;
	Limit[1] = SParam.Size_Fovea;
	Limit[2] = (SParam.Size_Rho + SParam.Size_Fovea)/2;
	Limit[3] = SParam.Size_Rho;

	for (j=0; j<SParam.Size_LP; j++)
		DownSampleTable[j] = DownSampleTable[0]+(ListSize*j); 

	for (j=0; j<SParam.Size_LP * ListSize; j++)
	{
		(*(DownSampleTable[0]+j)).position = LParam->Size_LP;
		(*(DownSampleTable[0]+j)).weight   = 0;
	}

	for (step = 0; step < 3; step++)
	{
		if (step == 1)
		{
			LParam->Zoom_Level /= MidZoomLevel;
			LParam->Valid_Log_Index = false;
			SParam.Zoom_Level = LParam->Zoom_Level*Ratio;
			SParam.Valid_Log_Index = false;
		}
		if (step == 2)
		{
			LParam->Zoom_Level = 1.0;
			LParam->Valid_Log_Index = false;
			SParam.Zoom_Level = LParam->Zoom_Level*Ratio;
			SParam.Valid_Log_Index = false;
		}

		for (j=0; j<SRho*STheta; j++)
		{
			rho = j/STheta;
			SLP[3*j+0] = rho;		//rho
			SLP[3*j+1] = j%STheta;	//theta
			
			if ((SLP[3*j+2]!=128)&&(rho>=Limit[step])&&(rho<Limit[step+1]))
				SLP[3*j+2] = 0;
			else
				SLP[3*j+2] = 255;
		}

		sprintf(LocalPath,"%s%2.2f\\",Path,1.00);
		sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",	LocalPath,
												"RemapMap",
												LParam->Zoom_Level,
												LParam->Size_X_Remap,
												LParam->Size_Y_Remap,
												".gio");

		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			LRemapMap = (int *) malloc (LParam->Size_Img_Remap * sizeof(int));
			fread(LRemapMap,sizeof(int),LParam->Size_Img_Remap,fin);
			fclose (fin);
		}
		else
		{
			Build_Remap_Map(LParam,LocalPath);
			if ((fin = fopen(File_Name,"rb")) != NULL)
			{
				LRemapMap = (int *) malloc (LParam->Size_Img_Remap * sizeof(int));
				fread(LRemapMap,sizeof(int),LParam->Size_Img_Remap,fin);
				fclose (fin);
			}
			else
				LRemapMap = NULL;
		}

		Remap(LRem,LLP,LParam,LRemapMap);

		sprintf(LocalPath,"%s%2.2f\\",Path,Ratio);
		sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",	LocalPath,
												"RemapMap",
												SParam.Zoom_Level,
												SParam.Size_X_Remap,
												SParam.Size_Y_Remap,
												".gio");

		if ((fin = fopen(File_Name,"rb")) != NULL)
		{
			SRemapMap = (int *) malloc (SParam.Size_Img_Remap * sizeof(int));
			fread(SRemapMap,sizeof(int),SParam.Size_Img_Remap,fin);
			fclose (fin);
		}
		else
		{
			Build_Remap_Map(&SParam,LocalPath);
			if ((fin = fopen(File_Name,"rb")) != NULL)
			{
				SRemapMap = (int *) malloc (SParam.Size_Img_Remap * sizeof(int));
				fread(SRemapMap,sizeof(int),SParam.Size_Img_Remap,fin);
				fclose (fin);
			}
			else
				SRemapMap = NULL;
		}

		Remap(SRem,SLP,&SParam,SRemapMap);
		
		for (y = 0; y<LParam->Size_Y_Remap; y++)
			for (x=0; x<LParam->Size_X_Remap; x++)
			{
				rho   = LRem[3*(y*LParam->Size_X_Remap+x)+0];
				theta = LRem[3*(y*LParam->Size_X_Remap+x)+1];

				rhosmall   = SRem[3*(y*SParam.Size_X_Remap+x)+0];
				thetasmall = SRem[3*(y*SParam.Size_X_Remap+x)+1];
				valid      =!SRem[3*(y*SParam.Size_X_Remap+x)+2];

				if (valid)
				{
					i = 0;
					while((DownSampleTable[rhosmall*STheta+thetasmall][i].position!=LParam->Size_LP)&&
						(DownSampleTable[rhosmall*STheta+thetasmall][i].position!=rho*LTheta+theta))
							i++;
					
					DownSampleTable[rhosmall*STheta+thetasmall][i].position = rho*LTheta+theta;
					DownSampleTable[rhosmall*STheta+thetasmall][i].weight ++;
				}
			}
		free(LRemapMap);
		free(SRemapMap);
	}

//Now "weight" stores the number of pixels for each position

	float Sum;

	for (j=0; j<SParam.Size_LP; j++)
	{
		Sum = 0;

		for (i=0; i<ListSize; i++)
			Sum += DownSampleTable[j][i].weight;

		if (Sum>0)
			for (i=0; i<ListSize; i++)
				DownSampleTable[j][i].weight /= Sum;
	}

//Now I have in "weight" the actual weight, normalized to 1

//Sort the Table

	float MAX;
	int MAXIndex;
	float TempW;
	unsigned short TempP;

	for (j=0; j<SParam.Size_LP; j++)
	{
		for (k=0; k<ListSize; k++)
		{
			MAX = -1;
			for (i=k; i<ListSize; i++)
			{
				if (DownSampleTable[j][i].weight>=MAX)
				{
					MAX = DownSampleTable[j][i].weight;
					MAXIndex = i;
				}
			}
			TempW = DownSampleTable[j][MAXIndex].weight;
			DownSampleTable[j][MAXIndex].weight = DownSampleTable[j][k].weight;
			DownSampleTable[j][k].weight = TempW;
			TempP = DownSampleTable[j][MAXIndex].position;
			DownSampleTable[j][MAXIndex].position = DownSampleTable[j][k].position;
			DownSampleTable[j][k].position = TempP;
		}
	}


	for (j=0; j<SParam.Size_LP; j++)
		for (i=0; i<ListSize; i++)
			DownSampleTable[j][i].weight *= Ratio * Ratio;

//Weight Quantization

	for (j=0; j<SParam.Size_LP; j++)
		for (i=0; i<ListSize; i++)
			if (DownSampleTable[j][i].weight>(1/sqrt((double)2)))
				DownSampleTable[j][i].weight = 1.0;
			else if (DownSampleTable[j][i].weight<=(1/sqrt((double)(256*512))))
				DownSampleTable[j][i].weight = 0.0;
			else for (k=1; k<256; k*=2)
				if ((DownSampleTable[j][i].weight<=(1/sqrt((double)(k*(k*2)))))&&(DownSampleTable[j][i].weight>(1/sqrt((double)((k*2)*(k*4))))))
					DownSampleTable[j][i].weight = (float)(1.0)/(k*2);

	for (j=0; j<SParam.Size_LP; j++)
	{
		Sum = 0;
		for (i=0; i<ListSize; i++)
		{
			if (Sum >= 16.0)
			{
				DownSampleTable[j][i].weight = 0.0;
				DownSampleTable[j][i].position = LParam->Size_LP;
			}
			Sum += DownSampleTable[j][i].weight;
		}
	}

	int counter = 0;
	float CheckSum;

	do
	{
		counter = 0;
		for (j=0; j<SParam.Size_LP; j++)
		{
			Sum = 0;
			for (i=0; i<ListSize; i++)
				Sum += DownSampleTable[j][i].weight;
			if ((Sum < Ratio * Ratio)&&(Sum != 0.0))
			{
				CheckSum = (float)((Ratio * Ratio)-Sum);
				Sum = 0;
				for (i=0; i<ListSize; i++)
				{
					if ((DownSampleTable[j][i].weight != 1.0)&&(DownSampleTable[j][i].weight<=CheckSum-Sum))
					{
						Sum += DownSampleTable[j][i].weight;
						DownSampleTable[j][i].weight *= 2;
						if (Sum>=CheckSum)
							break;
					}
				}
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
			Sum = 0;
			for (i=0; i<ListSize; i++)
				Sum += DownSampleTable[j][i].weight;
			if (Sum > Ratio * Ratio)
			{
				CheckSum = (float)(Sum-(Ratio * Ratio));
				Sum = 0;
				for (i=0; i<ListSize; i++)
				{
					if ((DownSampleTable[j][i].weight != 1.0)&&(DownSampleTable[j][i].weight<=2*(CheckSum-Sum)))
					{
						if ((i>0)&&(DownSampleTable[j][i-1].weight>DownSampleTable[j][i].weight))
						{
							Sum += DownSampleTable[j][i-1].weight/(float)2.0;
							DownSampleTable[j][i-1].weight /= 2.0;
							if (Sum>=CheckSum)
								break;
						}
					}
				}
			}
///////////////////////////////////////////////////////////////////////////////////////////////////////
		}

		for (j=0; j<SParam.Size_LP; j++)
		{
			Sum = 0;
			for (i=0; i<ListSize; i++)
				Sum += DownSampleTable[j][i].weight;
			if ((Sum != Ratio * Ratio)&&(Sum != 0.0))
				counter ++;
		}
	}
	while (counter != 0);

	for (j=0; j<SParam.Size_LP; j++)
	{
		for (i=0; i<ListSize; i++)
			if (DownSampleTable[j][i].position == LParam->Size_LP)
				break;
		IntDownSampleTable[j].NofPixels = i;
		if (i!=0)
		{
			IntDownSampleTable[j].position = (unsigned short *) malloc (i * sizeof(unsigned short));
			IntDownSampleTable[j].weight   = (unsigned char *)  malloc (i * sizeof(unsigned  char));
		}

		for (i=0; i<IntDownSampleTable[j].NofPixels; i++)
			if(DownSampleTable[j][i].position!=LParam->Size_LP)
			{
				IntDownSampleTable[j].position[i] = DownSampleTable[j][i].position;
				IntDownSampleTable[j].weight[i] = (unsigned char)(-log10((double)DownSampleTable[j][i].weight)/log10((double)2));
			}
	}
	
	sprintf(File_Name,"%s1.00\\%s%1.2f%s",Path,"DSMap_",Ratio,".gio");


	MAXIndex = 0;
	unsigned short a = 0;
	unsigned char  b = 0;

	for (j=0; j<SParam.Size_LP; j++)
		if (IntDownSampleTable[j].NofPixels>MAXIndex)
			MAXIndex = IntDownSampleTable[j].NofPixels;

	fout = fopen(File_Name,"wb");
	fwrite(&MAXIndex,sizeof(int),1,fout);
	for (j=0; j<SParam.Size_LP; j++)
	{
		fwrite(&IntDownSampleTable[j].NofPixels	,sizeof(unsigned short),1,fout);
	}
	for (j=0; j<SParam.Size_LP; j++)
	{
		fwrite(IntDownSampleTable[j].position	,sizeof(unsigned short),IntDownSampleTable[j].NofPixels ,fout);
		for (i=IntDownSampleTable[j].NofPixels; i<MAXIndex; i++)
			fwrite(&a,sizeof(unsigned short),1,fout);
	}
	for (j=0; j<SParam.Size_LP; j++)
	{
		fwrite(IntDownSampleTable[j].weight		,sizeof(unsigned char) ,IntDownSampleTable[j].NofPixels ,fout);
		for (i=IntDownSampleTable[j].NofPixels; i<MAXIndex; i++)
			fwrite(&b,sizeof(unsigned char),1,fout);
	}
	fclose (fout);	
		
	free (SLP);
	free (LLP);
	free (SRem);
	free (LRem);
	free (FakeAngShift);
	free (*DownSampleTable);
	free (DownSampleTable);

	if (modpath)
		sprintf(Path,"%s1.00\\",Path);

	return 1;
}
	


void DownSample(unsigned char * InImage, unsigned char * OutImage, char * Path, Image_Data * Param, float Ratio,IntNeighborhood * IntDownSampleTable)
{	
	int i,j,k;

	const int SizeLP = (int)(Param->Size_LP / (Ratio*Ratio));
	const int div = (int)(Ratio);
	
	int i_SumR,i_SumG,i_SumB;
	int position;
	unsigned char shift; 

	for (j=0; j<SizeLP; j++)
	{
		i_SumR = 0;
		i_SumG = 0;
		i_SumB = 0;
		k=j*3;
		for (i=0; i<IntDownSampleTable[j].NofPixels; i++)
		{
			position = 3*IntDownSampleTable[j].position[i];
			shift = IntDownSampleTable[j].weight[i];
			i_SumR += InImage [position+0]>>shift;
			i_SumG += InImage [position+1]>>shift;
			i_SumB += InImage [position+2]>>shift;
		}
		OutImage[k+0] = (unsigned char) (i_SumR>>div);
		OutImage[k+1] = (unsigned char) (i_SumG>>div);
		OutImage[k+2] = (unsigned char) (i_SumB>>div);
	}
}
