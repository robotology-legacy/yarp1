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
/// $Id: TableGeneration.cpp,v 1.2 2004-07-27 09:45:04 babybot Exp $
///
///

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 

#include <yarp/LogPolarSDK.h>

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
*			 1024	ShiftMap												*
*																			*
*		The values have to be summed when building more than one table.		*
*		Use the ALLMAPS value to build all the LUT's.						*
*																			*
****************************************************************************/	

unsigned short Build_Tables(Image_Data * Par, LUT_Ptrs * Tables,char * Path,unsigned short List)
{
	unsigned short retval = 0;
	int testval;
	
	if (List&1==1)			//I need: Nothing
	{
		testval = Build_Ang_Shift_Map(Par,Path);
		if (testval)
			retval = retval | 1;
	}

	if ((List&2)==2)		//I need: Reference Image
	{
		testval = Build_Color_Map(Par,Path);
		if (testval)
			retval = retval | 2;
	}

	if ((List&16)==16)		//I need: Reference Image
	{
		testval = Build_Pad_Map(Par,Path);
		if (testval)
			retval = retval | 16;
	}

	if ((List&128)==128)	//I need: PadMap, AngShiftMap
	{
		testval = Build_XY_Map(Par,Path);
		if (testval)
			retval = retval | 128;
	}

	if ((List&8)==8)		//I need: ColorMap, XYMap
	{
		testval = Build_Neighborhood_Map(Par,Path);
		if (testval)
			retval = retval | 8;
	}

	if ((List&32)==32)		
	{
		testval = Build_Remap_Map(Par,Path);
		if (testval)
			retval = retval | 32;
	}

	if ((List&64)==64)	
	{
		testval = Build_Weights_Map(Par,Path);
		if (testval)
			retval = retval | 64;
	}

	if ((List&256)==256)
	{
		testval = Build_DS_Map(Par,Path,2.00);
		if (testval)
			retval = retval | 256;
	}

	if ((List&4)==4)
	{
		testval = Build_DS_Map(Par,Path,4.00);
		if (testval)
			retval = retval | 4;
	}

	if ((List&1024)==1024)
	{
		testval = Build_Shift_Map(Par,Path);
		if (testval)
			retval = retval | 1024;
	}

	if ((List&2048)==2048)
	{
		testval = Build_Shift_Map_Fovea(Par,Path);
		if (testval)
			retval = retval | 2048;
	}

	return retval;
}

/************************************************************************
*																		*
* Build_Ang_Shift_Map     												*
*																		*
* Builds a Look-up Table with the information needed for a correct		*
* absolute angular shift of each ring and saves it in the working		*
* folder with the file name AngularShiftMap.gio							*
*																		*
* Input: Parameters														*
* 			Set of all the needed parameters							*
*		 Path															*
* 			Working folder												*
* 																		*
************************************************************************/

int Build_Ang_Shift_Map (Image_Data * Par,char * Path)
{
	int i;
	double * Ang_Shift_Map;
	char File_Name [256];

	FILE * fout;
	
	Ang_Shift_Map = (double *)calloc(Par->Size_Rho,sizeof(double));

	if (Par->Fovea_Type < 4)
	{
		for (i=1; i<Par->Size_Fovea; i+=2)
		{
			Ang_Shift_Map[i+1] = - 2.0 * PI/(6*(i+1));
			Ang_Shift_Map[i]   = +       PI/(6* (i) );
		}

		for (i=Par->Size_Fovea; i<Par->Size_Rho-1; i+=2)
		{
			Ang_Shift_Map[i+1] = +     PI/(Par->Size_Theta);
		}
	}
	else
	{
		// Write here the code for Giotto 2.1
	}
	
	if (Par->Ratio == 1.00)
		sprintf(File_Name,"%s%s",Path,"AngularShiftMap.gio");
	else
		sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"AngularShiftMap.gio");
	
	fout = fopen(File_Name,"wb");
	fwrite(Ang_Shift_Map,sizeof(double),Par->Size_Rho,fout);
	fclose (fout);

	free (Ang_Shift_Map);

	return 1;
}

/************************************************************************
*																		*
* Build_Cart2LP_Map     												*
*																		*
* Builds a Look-up Table with the information needed for the			*
* Cartesian to LP transformation and saves it in the working			*
* folder with the file name Cart2LPMap.gio								*
*																		*
* Input: Parameters														*
* 			Set of all the needed parameters							*
*		 Path															*
* 			Working folder												*
* 																		*
************************************************************************/

int Build_Cart2LP_Map(Image_Data * Par, char * Path)
{
	int i,j,k,l;
//	int PadSizeTheta;
	int x,y; 
	int rho,theta;
	int retval;
	int Total_Size = 0;
	LUT_Ptrs Tables;
	Cart2LPInterp * Temp_Array;
	Cart2LPInterp * Cart2LP_Map;
	unsigned short  * Pad_Map;
	double * Ang_Shift_Map;
	char File_Name [256];
	FILE * fout;
	double Temp_ZoomLev = Par->Zoom_Level;
	Par->Zoom_Level = (double)Par->Size_X_Orig/(double)Par->Resolution;
	Par->Valid_Log_Index = false;

	retval = Load_Tables(Par,&Tables,Path,17);

	if (retval != 17)
	{
		if ((retval&1)==0)
			Build_Ang_Shift_Map(Par,Path);

		if ((retval&16)==0)
			Build_Pad_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,17);
	}

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
		Temp_Array = (Cart2LPInterp *)malloc(Par->Size_LP * sizeof(Cart2LPInterp));
		Cart2LP_Map = (Cart2LPInterp *)malloc(Par->Size_LP * sizeof(Cart2LPInterp));
		for (j=0; j<Par->Size_LP; j++)
		{
			Temp_Array[j].position = (unsigned int *)malloc(256 * sizeof(unsigned int));
			Temp_Array[j].NofPixels = 0;
			Temp_Array[j].position[0] = 0;
		}

		Ang_Shift_Map = Tables.AngShiftMap;
		Pad_Map = Tables.PadMap;

		for (k=0; k<Par->Size_Fovea; k++)
		{
			for (l=0; l<Par->Size_Theta; l++)
			{
				x = (int)(Get_X_Center(k,l,Par,Ang_Shift_Map,Pad_Map));
				y = (int)(Get_Y_Center(k,l,Par,Ang_Shift_Map,Pad_Map));
				if ((x<Par->Size_X_Orig/2)&&(x>=-Par->Size_X_Orig/2)&&(y<Par->Size_Y_Orig/2)&&(y>=-Par->Size_Y_Orig/2))
					Temp_Array [k*Par->Size_Theta+l].position[0] = 3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+x+Par->Size_X_Orig/2);
			}
		}

		for (j=0; j<Par->Size_Y_Orig; j++)
			for (i=0; i<Par->Size_X_Orig; i++)
			{
				rho = Get_Rho(i-Par->Size_X_Orig/2,Par->Size_Y_Orig/2-j,Par);
				if ((rho>=0)&&(rho<Par->Size_Rho))
				{
					theta = Get_Theta(i-Par->Size_X_Orig/2,Par->Size_Y_Orig/2-j,rho,Par,Ang_Shift_Map,Pad_Map);
					Temp_Array[rho*Par->Size_Theta+theta].position[Temp_Array[rho*Par->Size_Theta+theta].NofPixels] = 3*(j*Par->Size_X_Orig+i);
					Temp_Array[rho*Par->Size_Theta+theta].NofPixels ++;
				}
			}

		for (k=0; k<Par->Size_Rho; k++)
		{
			for (l=0; l<Par->Size_Theta; l++)
			{
				if (Temp_Array[k*Par->Size_Theta+l].NofPixels == 0)
				{
					Temp_Array[k*Par->Size_Theta+l].NofPixels = 1;
					if (k>=Par->Size_Fovea)
					{
						x = (int)(Get_X_Center(k,l,Par,Ang_Shift_Map,Pad_Map));
						y = (int)(Get_Y_Center(k,l,Par,Ang_Shift_Map,Pad_Map));
						Temp_Array[k*Par->Size_Theta+l].position[0] = 3*((Par->Size_Y_Orig/2-y)*Par->Size_X_Orig+x+Par->Size_X_Orig/2);
					}
				}
				Cart2LP_Map[k*Par->Size_Theta+l].position = (unsigned int *)malloc(Temp_Array[k*Par->Size_Theta+l].NofPixels * sizeof(unsigned int));
				Cart2LP_Map[k*Par->Size_Theta+l].NofPixels = Temp_Array[k*Par->Size_Theta+l].NofPixels;
				Total_Size += Temp_Array[k*Par->Size_Theta+l].NofPixels;			
				for (i=0; i<Temp_Array[k*Par->Size_Theta+l].NofPixels; i++)
					Cart2LP_Map[k*Par->Size_Theta+l].position[i] = Temp_Array[k*Par->Size_Theta+l].position[i];
			}
		}

		if (Par->Ratio== 1.00)
			sprintf(File_Name,"%s%s",Path,"Cart2LPMap.gio");
		else
			sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"Cart2LPMap.gio");


		fout = fopen(File_Name,"wb");
		for (k=0; k<Par->Size_Rho; k++)
		{
			for (l=0; l<Par->Size_Theta; l++)
			{
				fwrite(&Cart2LP_Map[k*Par->Size_Theta+l].NofPixels,sizeof(unsigned char),1,fout);
				fwrite(Cart2LP_Map[k*Par->Size_Theta+l].position  ,sizeof(unsigned  int),Cart2LP_Map[k*Par->Size_Theta+l].NofPixels,fout);
			}
		}
		fclose (fout);
		Par->Zoom_Level = Temp_ZoomLev;
		free (Pad_Map);
		free (Ang_Shift_Map);
		free (Cart2LP_Map);
		free (Temp_Array);

		return 1;
	}
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
* Input: Parameters														*
* 			Set of all the needed parameters							*
*		 Path															*
* 			Working folder												*
* 																		*
************************************************************************/	

int Build_Color_Map (Image_Data * Par, char * Path)
{
	int i,j;
	int XSize,YSize,planes;
	unsigned char * Image;
	char          * Color_Map;
	char File_Name [256];

	FILE * fout;

	if (Par->Ratio == 1.00)
		sprintf(File_Name,"%s%s",Path,"ReferenceImage.bmp");
	else
		sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"ReferenceImage.bmp");		

	Image = Load_Bitmap(&XSize,&YSize,&planes,File_Name);

	if ((XSize!=Par->Size_Theta)||(YSize!=Par->Size_Rho)||(planes != Par->LP_Planes))
		return 0;

	if (Image != NULL)
	{
		Color_Map = (char *)malloc(Par->Size_LP * sizeof(char));

		for (i=0; i< Par->Size_LP; i++)
		{
			Color_Map[i] = -1;
			for (j=0; j<Par->LP_Planes; j++)
				if (Image[3*i+j] == 255)
					Color_Map[i] = j;
		}

		free (Image);

		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s",Path,"ColorMap.gio");
		else
			sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"ColorMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Color_Map,sizeof(char),Par->Size_LP,fout);
		fclose (fout);

		free (Color_Map);

		return 1;
	}
	else
		return 0;
}


/************************************************************************
* Build_Neighborhood_Map  												*
*																		*
* Input: Pix_Numb														*
* 		 Path															*
* 																		*
************************************************************************/	

int Build_Neighborhood_Map(Image_Data * Par, char * Path)
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

	Neighbor_Map_RGB = (Neighborhood *)malloc(Par->Size_LP * Pix_Numb * 3 * sizeof(Neighborhood));

	retval = Load_Tables(Par,&Tables,Path,130);

	if (retval != 130)
	{
		if ((retval&128)==0)
			Build_XY_Map(Par,Path);

		if ((retval&2)==0)
			Build_Color_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,130);
	}
	

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


		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s",Path,"NeighborhoodMap.gio");
		else
			sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"NeighborhoodMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Neighbor_Map_RGB,sizeof(Neighborhood),Par->Size_LP * Pix_Numb * 3,fout);
		fclose (fout);

		free(Neighbor_Map_RGB);
		free(Tables.XYMap);
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

int Build_Pad_Map (Image_Data * Par,char * Path)
{
	int i,j,k=0;
	int XSize,YSize,planes;
	unsigned char  * Image;
	unsigned short * Pad_Map;
	char File_Name [256];

	FILE * fout;
	
	if (Par->Ratio == 1.00)
		sprintf(File_Name,"%s%s",Path,"ReferenceImage.bmp");
	else
		sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"ReferenceImage.bmp");		

	Image = Load_Bitmap(&XSize,&YSize,&planes,File_Name);

	if (Image != NULL)
	{
		Pad_Map = (unsigned short *)malloc(Par->Size_Theta * Par->Size_Fovea * sizeof(unsigned short));

		for (j=0; j<Par->Size_Fovea; j++)
		{	
			for (i=0; i<Par->Size_Theta; i++)
			{
				Pad_Map[j*Par->Size_Theta+i] = 1;
				if(Image[3*(j*Par->Size_Theta+i)] != 128)
				{
					Pad_Map[k] = j*Par->Size_Theta+i;
					k++;
				}
			}
			k=Par->Size_Theta*(j+1);
		}
		

		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s",Path,"PadMap.gio");
		else
			sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"PadMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(Pad_Map,sizeof(unsigned short),Par->Size_Theta * Par->Size_Fovea,fout);
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
*																		*
* Builds a Lookup Table used for the transformation from the LogPolar	*
* to the cartesian coordinates										  	*
*																		*
************************************************************************/	

int Build_Remap_Map (Image_Data * Par, char * Path)
{
	int i,j;
	char File_Name [256];
	int XSize, YSize;
	int rho,theta;
	double   * Ang_Shift_Map;
	unsigned short * Pad_Map;
	int      * Remap_Map;

	int AddedPad;
	int PadSizeTheta;

	int startX,startY;
	int endX,endY;

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;

	retval = Load_Tables(Par,&Tables,Path,17);

	if (retval != 17)
	{
		if ((retval&1)==0)
			Build_Ang_Shift_Map(Par,Path);

		if ((retval&16)==0)
			Build_Pad_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,17);
	}

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

		if ((Par->Size_Theta * Par->LP_Planes) % Par->padding == 0)
			AddedPad = 0;
		else
			AddedPad = Par->padding - (Par->Size_Theta * Par->LP_Planes) % Par->padding;

		PadSizeTheta = Par->Size_Theta * Par->LP_Planes + AddedPad;

		XSize = (int)(Par->Resolution * Par->Zoom_Level + 0.5);
		YSize = XSize;

		Remap_Map = (int *)calloc(Par->Size_Img_Remap , sizeof(int));

		startX = (XSize/2)-Par->Size_X_Remap/2;
		startY = (YSize/2)-Par->Size_Y_Remap/2;
		endX   = (XSize/2)+Par->Size_X_Remap/2;
		endY   = (YSize/2)+Par->Size_Y_Remap/2;

		for (j=startY; j<endY; j++)
			for (i=startX; i<endX; i++)
			{
				rho   = (int)(Get_Rho  (i-XSize/2,YSize/2-j,Par));
				theta = (int)(Get_Theta(i-XSize/2,YSize/2-j,rho,Par,Ang_Shift_Map,Pad_Map));
				if ((rho<Par->Size_Rho)&&(rho>=0)&&(theta<Par->Size_Theta)&&(theta>=0))
				{
					Remap_Map[(j-startY)*Par->Size_X_Remap+i-startX] = (rho*PadSizeTheta+3*theta);
				}
				else
				{
					Remap_Map[(j-startY)*Par->Size_X_Remap+i-startX] = 0;
				}
			}

		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s_%2.3f_%dx%d_P%d%s",Path,"RemapMap",Par->Zoom_Level,Par->Size_X_Remap,Par->Size_Y_Remap,Par->padding,".gio");
		else
			sprintf(File_Name,"%s%1.2f_%s_%2.3f_%dx%d_P%d%s",Path,Par->Ratio,"RemapMap",Par->Zoom_Level,Par->Size_X_Remap,Par->Size_Y_Remap,Par->padding,".gio");
		
		fout = fopen(File_Name,"wb");
		fwrite(Remap_Map,sizeof(int),Par->Size_Img_Remap,fout);
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

int Build_Weights_Map(Image_Data * Par,
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

	int AddedPad;

	int retval;
	LUT_Ptrs Tables;

	FILE * fout;
		
	if ((Par->Size_Theta * 1) % Par->padding == 0)
		AddedPad = 0;
	else
		AddedPad = Par->padding - (Par->Size_Theta * 1) % Par->padding;

	int PadSizeTheta = Par->Size_Theta * 1 + AddedPad;

	retval = Load_Tables(Par,&Tables,Path,8);

	if (retval != 8)
	{
		Build_Neighborhood_Map(Par,Path);
		retval = Load_Tables(Par,&Tables,Path,8);
	}

	if (retval != 8)
	{
		if (Tables.NeighborMap!=NULL)
			free(Tables.NeighborMap);
		return 0;
	}
	else
	{
		Neighbor_Map_RGB = Tables.NeighborMap;

		if (Par->Pix_Numb>MAX_PIX)
			Par->Pix_Numb = MAX_PIX;

		Weights_Map = (Neighborhood *)malloc(Par->Size_LP * Par->Pix_Numb * 3 * sizeof(Neighborhood)); 

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
			Weights_Map[k].position = j*(PadSizeTheta)+i;
		}



		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s%02d_P%d%s",Path,"WeightsMap",Par->Pix_Numb,Par->padding,".gio");
		else
			sprintf(File_Name,"%s%1.2f_%s%02d_P%d%s",Path,Par->Ratio,"WeightsMap",Par->Pix_Numb,Par->padding,".gio");

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
************************************************************************/	

int Build_XY_Map (Image_Data * Par, char * Path)
{
	int i,j;
	unsigned short * Pad_Map;
	double * Ang_Shift_Map;
	double * X_Map;
	double * Y_Map;
	char File_Name [256];

	FILE * fout;

	int retval;
	LUT_Ptrs Tables;

	retval = Load_Tables(Par,&Tables,Path,17);

	if (retval != 17)
	{
		if ((retval&1)==0)
			Build_Ang_Shift_Map(Par,Path);

		if ((retval&16)==0)
			Build_Pad_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,17);
	}

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

		X_Map = (double *)calloc(Par->Size_LP,sizeof(double));
		Y_Map = (double *)calloc(Par->Size_LP,sizeof(double));

		for (j=1; j< Par->Size_Rho; j++)
			for (i=0; i<Par->Size_Theta; i++)
			{
				X_Map[j*Par->Size_Theta+i] = Get_X_Center(j,i,Par,Ang_Shift_Map,Pad_Map);
				Y_Map[j*Par->Size_Theta+i] = Get_Y_Center(j,i,Par,Ang_Shift_Map,Pad_Map);
			}

		if (Par->Ratio = 1.00)
			sprintf(File_Name,"%s%s",Path,"XYMap.gio");
		else
			sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"XYMap.gio");

		fout = fopen(File_Name,"wb");
		fwrite(X_Map,sizeof(double),Par->Size_LP,fout);
		fwrite(Y_Map,sizeof(double),Par->Size_LP,fout);
		fclose (fout);

		free (Pad_Map);
		free (Ang_Shift_Map);
		free (X_Map);
		free (Y_Map);

		return 1;
	}
}


int Build_DS_Map(Image_Data * LParam,char * Path, float Ratio)
{
	Image_Data SParam;

	//Tables
	double * FakeAngShift;
	unsigned short FakePadMap;
	unsigned char * LRem, * SRem;
	Neighborhood ** DownSampleTable;
	IntNeighborhood * IntDownSampleTable;

	LUT_Ptrs STables, LTables;

	int rho,theta;
	int rhosmall, thetasmall, valid;
	int x,y;
	int i,j,k;
	int step;
	int retval;

	int LRho,LTheta,LPlanes;
	int SRho,STheta,SPlanes;

	char File_Name [256]; 
	FILE * fout;

	bool modsize = false;
	int oldsize,oldpad;
	double oldzoom;

	sprintf(File_Name,"%sReferenceImage.bmp",Path);
	unsigned char * LLP = Load_Bitmap(&LTheta,&LRho,&LPlanes,File_Name);

	sprintf(File_Name,"%s%1.2f_ReferenceImage.bmp",Path,Ratio);
	unsigned char * SLP = Load_Bitmap(&STheta,&SRho,&SPlanes,File_Name);

	if ((LLP == NULL)||(SLP == NULL))
		return 0;

	//Sets the Par

	oldsize = LParam->Size_X_Remap;
	oldzoom = LParam->Zoom_Level;
	oldpad  = LParam->padding;
	LParam->Size_X_Remap = LParam->Resolution;
	LParam->Size_Y_Remap = LParam->Resolution;
	LParam->Size_Img_Remap = LParam->Size_X_Remap * LParam->Size_Y_Remap;
	LParam->padding = 1;
	
	LParam->Zoom_Level = 1090.0/83.0; //83 is (SizeFovea*2)-1

	SParam = Set_Param( (int)(1090/Ratio),(int)(1090/Ratio),
						(int)(1090),(int)(1090),
						(int)(152/Ratio),(int)(252/Ratio),(int)(42/Ratio),
						(int)(1090/Ratio),
						CUST,
						LParam->Zoom_Level*Ratio);

	LParam->Fovea_Type = 2;
	SParam.padding = LParam->padding;
	SParam.Fovea_Type = 0;
	SParam.Ratio = Ratio;

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

		retval = Load_Tables(LParam,&LTables,Path,32);

		if (retval!=32)
		{
			Build_Remap_Map(LParam,Path);
			retval = Load_Tables(LParam,&LTables,Path,32);
			if (retval !=32)
			{
				LTables.RemapMap = NULL;
				return 0;
			}
		}

		Remap(LRem,LLP,LParam,LTables.RemapMap);


		retval = Load_Tables(&SParam,&STables,Path,32);

		if (retval!=32)
		{
			Build_Remap_Map(&SParam,Path);
			retval = Load_Tables(&SParam,&STables,Path,32);
			if (retval !=32)
			{
				STables.RemapMap = NULL;
				return 0;
			}
		}

		Remap(SRem,SLP,&SParam,STables.RemapMap);
		if (step == 2)
			for (y = 0; y<SParam.Size_Y_Remap; y++)
				for (x=0; x<SParam.Size_X_Remap; x++)
					if ((SRem[3*(y*SParam.Size_X_Remap+x)+0]==0)&&(SRem[3*(y*SParam.Size_X_Remap+x)+1]==0))
						SRem[3*(y*SParam.Size_X_Remap+x)+2] = 255;


		for (y = 0; y<LParam->Size_Y_Remap; y++)
			for (x=0; x<LParam->Size_X_Remap; x++)
			{
				rho   = LRem[3*(y*LParam->Size_X_Remap+x)+0];
				theta = LRem[3*(y*LParam->Size_X_Remap+x)+1];

				rhosmall   = SRem[3*(y*SParam.Size_X_Remap+x)+0];
				thetasmall = SRem[3*(y*SParam.Size_X_Remap+x)+1];
				valid      =!SRem[3*(y*SParam.Size_X_Remap+x)+2];
				if ((rhosmall == 0)&&(thetasmall==0))
					rhosmall = rhosmall;


				if (valid)
				{
					i = 0;
					while((DownSampleTable[rhosmall*STheta+thetasmall][i].position!=LParam->Size_LP)&&
						(DownSampleTable[rhosmall*STheta+thetasmall][i].position!=(rho*LTheta+theta)))
							i++;
					
					DownSampleTable[rhosmall*STheta+thetasmall][i].position = (rho*LTheta+theta);
					DownSampleTable[rhosmall*STheta+thetasmall][i].weight ++;
				}
			}
		free(LTables.RemapMap);
		free(STables.RemapMap);
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
	unsigned int TempP;

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

			Sum = 0;
			for (i=0; i<ListSize; i++)
				Sum += DownSampleTable[j][i].weight;
			if ((Sum < Ratio * Ratio)&&(Sum != 0.0))
			{
				CheckSum = (float)((Ratio * Ratio)-Sum);
				Sum = 0;
				for (i=0; i<ListSize-1; i++)
				{
					if ((DownSampleTable[j][i].weight == 1.0)&&(DownSampleTable[j][i].weight<=CheckSum-Sum)&&(DownSampleTable[j][i+1].weight == 0.0))
					{
						DownSampleTable[j][i+1].weight = 1.0;
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
			IntDownSampleTable[j].position = (unsigned int *) malloc (i * sizeof(unsigned int));
			IntDownSampleTable[j].weight   = (unsigned char *)  malloc (i * sizeof(unsigned  char));
		}

		for (i=0; i<IntDownSampleTable[j].NofPixels; i++)
			if(DownSampleTable[j][i].position!=LParam->Size_LP)
			{
				IntDownSampleTable[j].position[i] = 3*DownSampleTable[j][i].position;
				IntDownSampleTable[j].weight[i] = (unsigned char)(-log10((double)DownSampleTable[j][i].weight)/log10((double)2));
			}
	}
	LParam->padding = oldpad;
	SParam.padding = oldpad;
	
	sprintf(File_Name,"%s%s%1.2f_P%d%s",Path,"DSMap_",Ratio,LParam->padding,".gio");


	MAXIndex = 0;
	unsigned short a = 0;
	unsigned char  b = 0;

	//Finds the max number of pixels per location
	for (j=0; j<SParam.Size_LP; j++)
		if (IntDownSampleTable[j].NofPixels>MAXIndex)
			MAXIndex = IntDownSampleTable[j].NofPixels;

	fout = fopen(File_Name,"wb");
	fwrite(&MAXIndex,sizeof(int),1,fout);


	int AddedPadSize = computePadSize(3*LParam->Size_Theta,LParam->padding) - 3 * LParam->Size_Theta;

	for (j=0; j<SParam.Size_Rho; j++)
		for (i=0; i<SParam.Size_Theta; i++)
			for (k=0; k<IntDownSampleTable[j*SParam.Size_Theta+i].NofPixels; k++)
				IntDownSampleTable[j*SParam.Size_Theta+i].position[k] += (IntDownSampleTable[j*SParam.Size_Theta+i].position[k]/(LParam->LP_Planes*LParam->Size_Theta))*AddedPadSize;

	for (j=0; j<SParam.Size_LP; j++)
	{
		fwrite(&IntDownSampleTable[j].NofPixels	,sizeof(unsigned short),1,fout);
	}
	
	for (j=0; j<SParam.Size_LP; j++)
	{
		fwrite(IntDownSampleTable[j].position	,sizeof(unsigned int),IntDownSampleTable[j].NofPixels ,fout);
		for (i=IntDownSampleTable[j].NofPixels; i<MAXIndex; i++)
			fwrite(&a,sizeof(unsigned int),1,fout);
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

	LParam->Size_X_Remap = oldsize;
	LParam->Size_Y_Remap = oldsize;
	LParam->Size_Img_Remap = LParam->Size_X_Remap * LParam->Size_Y_Remap;
	LParam->Zoom_Level = oldzoom;

	return 1;
}
	

int Build_Shift_Map(Image_Data * Par, char * Path)
{
#define FLOATRES
	int i,j,l;
	double tempX,tempY;
	int newRho, newTheta;

#ifdef FLOATRES
	double shiftlev = 1.0/Par->dres;
#else
	double shiftlev = 1.0/Par->Resolution;
#endif

	int * ShiftMap;
	LUT_Ptrs Tables;
	char File_Name [256];
	FILE * fout;
	int steps;
	double OldZLevel = Par->Zoom_Level;
	Par->Zoom_Level = 1.0;

	int AddedPad = computePadSize(Par->LP_Planes * Par->Size_Theta,Par->padding) - (Par->LP_Planes * Par->Size_Theta);

#ifdef FLOATRES
	steps = (int)(0.5+3*Par->dres/4);
#else
	steps = (int)(0.5+3*Par->Resolution/4);
#endif

	ShiftMap = (int*) calloc((1+2*steps)*Par->Size_LP,sizeof(int));

	unsigned short retval = Load_Tables(Par,&Tables,Path,17);

	if (retval != 17)
	{
		if ((retval&1)==0)
			Build_Ang_Shift_Map(Par,Path);

		if ((retval&16)==0)
			Build_Pad_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,17);
	}

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
		for (l = -steps; l<steps+1; l++)
		{
			for(j=0; j<Par->Size_Rho; j++)
				for(i=0; i<Par->Size_Theta; i++)
				{
#ifdef FLOATRES
					tempX = l*Par->dres*shiftlev + getXfloatRes(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					tempY = getYfloatRes(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					if ((tempX<Par->Zoom_Level*Par->dres/2)&&(tempY<Par->Zoom_Level*Par->dres/2))
					{
						if ((tempX>=-Par->Zoom_Level*Par->dres/2)&&(tempY>=-Par->Zoom_Level*Par->dres/2))
#else
					tempX = l*Par->Resolution*shiftlev + Get_X_Center(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					tempY = Get_Y_Center(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					if ((tempX<Par->Zoom_Level*Par->Resolution/2)&&(tempY<Par->Zoom_Level*Par->Resolution/2))
					{
						if ((tempX>=-Par->Zoom_Level*Par->Resolution/2)&&(tempY>=-Par->Zoom_Level*Par->Resolution/2))
#endif
						{
							newRho = Get_Rho(tempX,tempY,Par);
							newTheta = Get_Theta(tempX,tempY,newRho,Par,Tables.AngShiftMap,Tables.PadMap);
							if ((newRho>=0)&&(newRho<Par->Size_Rho))
								if ((newTheta>=0)&&(newTheta<Par->Size_Theta))
								{
									ShiftMap[(l+steps)*(1*Par->Size_LP)+1*(j*Par->Size_Theta+i)] = 3*(newRho*Par->Size_Theta+newTheta)+newRho * AddedPad;
								}
						}
					}
				}
		}

		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s_P%d%s",Path,"ShiftMap",Par->padding,".gio");
		else
			sprintf(File_Name,"%s%1.2f_%s_P%d%s",Path,Par->Ratio,"ShiftMap",Par->padding,".gio");

		fout = fopen(File_Name,"wb");
		fwrite(ShiftMap,sizeof(int),(1+2*steps)*1*Par->Size_LP,fout);
		fclose (fout);

		free (Tables.PadMap);
		free (Tables.AngShiftMap);
		free (ShiftMap);

		Par->Zoom_Level = OldZLevel;

		return 1;
	}
}

int Build_Shift_Map_Fovea(Image_Data * Par, char * Path)
{
	int i,j,k,l;
	double tempX,tempY;
	int newRho, newTheta;
	double shiftlev = 1.0/Par->Resolution;
	int * ShiftMap;
	LUT_Ptrs Tables;
	char File_Name [256];
	FILE * fout;
	int steps;
	double OldZLevel = Par->Zoom_Level;
	Par->Zoom_Level = 1.0;

	int AddedPad = computePadSize(Par->LP_Planes * Par->Size_Theta,Par->padding) - (Par->LP_Planes * Par->Size_Theta);

	steps = (int)(0.5+3*Par->Resolution/4);

	ShiftMap = (int*) malloc((1+2*steps)*1*Par->Size_Fovea*Par->Size_Theta*sizeof(int));
	
	unsigned short retval = Load_Tables(Par,&Tables,Path,17);

	if (retval != 17)
	{
		if ((retval&1)==0)
			Build_Ang_Shift_Map(Par,Path);

		if ((retval&16)==0)
			Build_Pad_Map(Par,Path);

		retval = Load_Tables(Par,&Tables,Path,17);
	}

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
		for (k=0; k<(2*steps+1)*Par->Size_Fovea*Par->Size_Theta*1; k++)
			ShiftMap[k] = 0;

		for (l = -steps; l<steps+1; l++)
		{
			for(j=0; j<Par->Size_Fovea; j++)
				for(i=0; i<Par->Size_Theta; i++)
				{
					tempX = l*Par->Resolution*shiftlev + Get_X_Center(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					tempY = Get_Y_Center(j,i,Par,Tables.AngShiftMap,Tables.PadMap);
					if ((tempX<Par->Zoom_Level*Par->Resolution/2)&&(tempY<Par->Zoom_Level*Par->Resolution/2))
					{
						if ((tempX>=-Par->Zoom_Level*Par->Resolution/2)&&(tempY>=-Par->Zoom_Level*Par->Resolution/2))
						{
							newRho = Get_Rho(tempX,tempY,Par);
							newTheta = Get_Theta(tempX,tempY,newRho,Par,Tables.AngShiftMap,Tables.PadMap);
							if ((newRho>=0)&&(newRho<Par->Size_Rho))
								if ((newTheta>=0)&&(newTheta<Par->Size_Theta))
								{
										ShiftMap[(l+steps)*(1*Par->Size_Fovea*Par->Size_Theta)+1*(j*Par->Size_Theta+i)] = 3*(newRho*Par->Size_Theta+newTheta)+newRho * AddedPad;
								}
						}
					}
				}
		}

		if (Par->Ratio == 1.00)
			sprintf(File_Name,"%s%s_P%d%s",Path,"ShiftMapF",Par->padding,".gio");
		else
			sprintf(File_Name,"%s%1.2f_%s_P%d%s",Path,Par->Ratio,"ShiftMapF",Par->padding,".gio");

		fout = fopen(File_Name,"wb");
		fwrite(ShiftMap,sizeof(int),(1+2*steps)*1*Par->Size_Fovea*Par->Size_Theta,fout);
		fclose (fout);

		Par->Zoom_Level = OldZLevel;

		free (Tables.PadMap);
		free (Tables.AngShiftMap);
		free (ShiftMap);

		return 1;
	}
}


void DownSample(unsigned char * InImage, unsigned char * OutImage, char * Path, Image_Data * Par, float Ratio,IntNeighborhood * IntDownSampleTable)
{	
	int i,j,l;

	const int SizeLP = (int)(Par->Size_LP / (Ratio*Ratio));
	const int RHO = (int)(Par->Size_Rho / (Ratio));
	const int THETA = (int)(Par->Size_Theta / (Ratio));
	const int div = (int)(Ratio);
	
	int i_SumR,i_SumG,i_SumB;
	unsigned int position;
	unsigned char shift; 
	int AddedPadSize;// = (Par->Size_Theta * 3) % Par->padding;
	int PadSizeSmall;// = ((THETA * 3) % Par->padding)+3*THETA;

	PadSizeSmall = computePadSize(3*THETA,Par->padding);
	AddedPadSize = PadSizeSmall - 3*THETA;
	unsigned char * SmallImgPtr = OutImage;

	for (j=0; j<RHO; j++)
	{
		for (l=0; l<THETA; l++)
		{
			i_SumR = 0;
			i_SumG = 0;
			i_SumB = 0;
			for (i=0; i<IntDownSampleTable[j*THETA+l].NofPixels; i++)
			{
				position = IntDownSampleTable[j*THETA+l].position[i];
				shift    = IntDownSampleTable[j*THETA+l].weight[i];
				i_SumR += InImage [position+0]>>shift;
				i_SumG += InImage [position+1]>>shift;
				i_SumB += InImage [position+2]>>shift;
			}
			*SmallImgPtr++ = (unsigned char) (i_SumR>>div);
			*SmallImgPtr++ = (unsigned char) (i_SumG>>div);
			*SmallImgPtr++ = (unsigned char) (i_SumB>>div);
		}
		SmallImgPtr += AddedPadSize;
	}

}

void DownSampleFovea(unsigned char * InImage, unsigned char * OutImage, char * Path, Image_Data * Par, float Ratio,IntNeighborhood * IntDownSampleTable,int Rows)
{	
	int i,j,l;

	const int SizeLP = (int)(Par->Size_LP / (Ratio*Ratio));
	const int FOV = (int)(Par->Size_Fovea / (Ratio));
	const int THETA = (int)(Par->Size_Theta / (Ratio));
	const int div = (int)(Ratio);
	
	int i_SumR,i_SumG,i_SumB;
	unsigned int position;
	unsigned char shift; 
	int AddedPadSize = (Par->Size_Theta * 3) % Par->padding;
	int PadSizeSmall = ((THETA * 3) % Par->padding)+3*THETA;

	PadSizeSmall = computePadSize(3*THETA,Par->padding);
	AddedPadSize = PadSizeSmall - 3*THETA;
	unsigned char * SmallImgPtr = OutImage;

	for (j=0; j<Rows; j++)
	{
		for (l=0; l<THETA; l++)
		{
			i_SumR = 0;
			i_SumG = 0;
			i_SumB = 0;
			for (i=0; i<IntDownSampleTable[j*THETA+l].NofPixels; i++)
			{
				position = IntDownSampleTable[j*THETA+l].position[i];
				shift    = IntDownSampleTable[j*THETA+l].weight[i];
				i_SumR += InImage [position+0]>>shift;
				i_SumG += InImage [position+1]>>shift;
				i_SumB += InImage [position+2]>>shift;
			}
			*SmallImgPtr++ = (unsigned char) (i_SumR>>div);
			*SmallImgPtr++ = (unsigned char) (i_SumG>>div);
			*SmallImgPtr++ = (unsigned char) (i_SumB>>div);
		}
		SmallImgPtr += AddedPadSize;
	}

}

void Build_Step_Function(char * Path, Image_Data * Par)
{
	int i,j,k;

	int MAX = -1000;
	int maxind;

	FILE * fout;
	char File_Name [256];

	double oldzoom = Par->Zoom_Level;
	int oldft = Par->Fovea_Type;

	Par->Zoom_Level = 1.00;
	Par->Fovea_Type = 2;

	double * FakeAngShiftMap = (double*)malloc(4*Par->Size_Rho*sizeof(double));
	unsigned short * FakePadMap = NULL;
	int * StepFunct = (int*)malloc(8*Par->Size_Rho * sizeof(int));

	for (j=0; j<4*Par->Size_Rho; j++)
		FakeAngShiftMap[j] = 0.0;
	
	for (j=0; j<4*Par->Size_Rho; j++) //generates values
	{
		StepFunct[j] = -(int)Get_X_Center(2.0*Par->Size_Rho-(j/2.0)-0.5,0,Par,FakeAngShiftMap,FakePadMap);
		StepFunct[8*Par->Size_Rho-j-1] = (int)Get_X_Center(2*Par->Size_Rho-(j/2.0)-0.5,0,Par,FakeAngShiftMap,FakePadMap);
	}

	for (j=0; j<8*Par->Size_Rho; j++)//deletes out of bounds values
		if (abs(StepFunct[j])>3*Par->Resolution/4)
		{
			for (i=j; i<8*Par->Size_Rho-1; i++)
				StepFunct[i] = StepFunct[i+1];
			StepFunct [8*Par->Size_Rho-1] = -1;
			j --;
		}

	for (k=0; k<8*Par->Size_Rho; k++)//computes max value
		if (StepFunct[k]>MAX)
		{
			MAX = StepFunct[k];
			maxind = k;
		}


	bool found;
	do//search for duplicate values
	{
		found = false;
		for (j=0; j<maxind+1; j++)
			if (StepFunct[j]==StepFunct[j+1])
			{
				for (i=j; i<maxind; i++)
					StepFunct[i] = StepFunct[i+1];
				StepFunct[maxind] = 0;
				maxind --;
				found = true;
			}
	}
	while (found);

	//writes the StepFunction
	for (j=maxind+1; j<8*Par->Size_Rho; j++)
		StepFunct[j] = 4*Par->Size_Rho;

	maxind++;
	if (Par->Ratio == 1.00)
		sprintf(File_Name,"%s%s",Path,"StepList.gio");
	else
		sprintf(File_Name,"%s%1.2f_%s",Path,Par->Ratio,"StepList.gio");

	fout = fopen(File_Name,"wb");
	fwrite(&maxind,sizeof(int),1,fout);
	fwrite(StepFunct,sizeof(int),maxind,fout);
	fclose (fout);

	free (StepFunct);

	Par->Zoom_Level = oldzoom;
	Par->Fovea_Type = oldft;
}

