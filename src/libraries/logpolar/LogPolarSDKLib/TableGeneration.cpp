#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

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
*				4	DownsampleMap.gio 										*
*				8	NeighborhoodMap.gio 									*
*			   16	PadMap.gio 												*
*			   32	RemMap_X.XX_YYxYY.gio 									*
*			   64	WeightsMap.gio 											*
*			  128	XYMap.gio												*
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

	if ((List&4)==4)
	{
		testval = Build_DownSample_Map(Param,Path);
		if (testval)
			retval = retval | 4;
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

	if (Parameters->Fovea_Type == 0)
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
		free(X_Map);
		free(Y_Map);

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
	unsigned char * Image;
	short         * Pad_Map;
	char File_Name [256];

	FILE * fout;
	
	sprintf(File_Name,"%s%s",Path,"ReferenceImage.bmp");

	Image = Read_Bitmap(&XSize,&YSize,&planes,File_Name);

	if (Image != NULL)
	{
		Pad_Map = (short *)malloc(Parameters->Size_Theta * Parameters->Size_Fovea * sizeof(short));

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
		fwrite(Pad_Map,sizeof(short),Parameters->Size_Theta * Parameters->Size_Fovea,fout);
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
	short    * Pad_Map;
	int      * Remap_Map;

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

		Remap_Map = (int *)malloc(XSize * YSize * sizeof(int));

	//	if (Parameters->Fovea_Type == 0)
		if (1)
		{
			for (j=0; j<YSize; j++)
				for (i=0; i<XSize; i++)
				{
					rho   = (int)(Get_Rho  (i-XSize/2,YSize/2-j,Parameters));
					theta = (int)(Get_Theta(i-XSize/2,YSize/2-j,rho,Parameters,Ang_Shift_Map,Pad_Map));
					if ((rho<Parameters->Size_Rho)&&(rho>=0)&&(theta<Parameters->Size_Theta)&&(theta>=0))
//						Remap_Map[j*XSize+i] = 3*(rho*Parameters->Size_Theta+theta);
						Remap_Map[j*XSize+i] = (rho*PadSizeTheta+3*theta);
					else
						Remap_Map[j*XSize+i] = 0;
				}

		}

		sprintf(File_Name,"%s%s_%2.3f%s",Path,"RemapMap",Parameters->Zoom_Level,".gio");
		
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
		
	int PadSizeTheta = (((Par->Size_Theta * 1) % Par->padding) + (Par->Size_Theta * 1));

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
	short  * Pad_Map;
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

		for (j=1; j< Parameters->Size_Fovea; j++)
			for (i=0; i<6*j; i++)
			{
				X_Map[j*Parameters->Size_Theta+i] = Get_X_Center(j,i,Parameters,Ang_Shift_Map);
				Y_Map[j*Parameters->Size_Theta+i] = Get_Y_Center(j,i,Parameters,Ang_Shift_Map);
			}
		for (j=Parameters->Size_Fovea; j< Parameters->Size_Rho; j++)
			for (i=0; i<Parameters->Size_Theta; i++)
			{
				X_Map[j*Parameters->Size_Theta+i] = Get_X_Center(j,i,Parameters,Ang_Shift_Map);
				Y_Map[j*Parameters->Size_Theta+i] = Get_Y_Center(j,i,Parameters,Ang_Shift_Map);
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
	short    * Pad_Map;
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
						Remap_Map[j*XSize+i] = 0;
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
