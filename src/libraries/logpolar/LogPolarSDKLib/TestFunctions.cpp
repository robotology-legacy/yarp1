#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <string.h>

#include "LogPolarSDK.h"

/**********************************************************************
* Cart2LPReal														  *
* Funzione di rimappaggio da Cartesiano a Log Polare.				  *
* Il Rimappaggio e' eseguito integrando e mediando i pixel.           *
***********************************************************************/

void Cart2LPReal(unsigned char *in_image, 
				 unsigned char *out_image,
				 char * Path,
				 Image_Data *Par)
{
	double radius;
	int rho;
	double logar;
//	double scalefactor;
	double B,A;
	int xtot,ytot;
	double xd,yd;
	double pixsize;
	int i,j;
	double x,y;
	double theta, thtmp;
	int Temp_Size_Theta;
	unsigned int *RawImage;
	unsigned int *Contat;
	unsigned char * One_Line;
	int    * Pad_Map;
	One_Line = (unsigned char *)calloc(3*Par->Size_Theta,sizeof(unsigned char));

	char File_Name [256];

	FILE * fin;

	RawImage = (unsigned int *)calloc(3 * (Par->Size_Rho+10) * (Par->Size_Theta+10), sizeof(int));
	Contat   = (unsigned int *)calloc((Par->Size_Rho+10) * (Par->Size_Theta+10), sizeof(int));

	Pad_Map = (int *)malloc(Par->Size_Theta * Par->Size_Fovea * sizeof(int));

	sprintf(File_Name,"%s%s",Path,"PadMap.gio");

	fin = fopen(File_Name,"rb");
	fread(Pad_Map,sizeof(int),Par->Size_Theta * Par->Size_Fovea,fin);
	fclose(fin);


	if (!Par->Valid_Log_Index){
		Par->Log_Index = Compute_Index(Par->Resolution,Par->Size_Fovea,Par->Size_Rho);
		Par->Valid_Log_Index = true;
	}

	logar = 1/(log(Par->Log_Index));

//	scalefactor = (double)__min(Par->Size_X_Orig,Par->Size_Y_Orig)/Par->Resolution;
	
	B = 1/(Par->Log_Index-1);
	A = Par->Size_Fovea - B;

	for (y = -Par->Size_Y_Orig/2; y<Par->Size_Y_Orig/2; y++)
	{
		for (x = -Par->Size_X_Orig/2; x<Par->Size_X_Orig/2; x++)
		{
			radius = sqrt(x*x+y*y);
			rho = (int)(log((radius-Par->Size_Fovea+0.5)*(Par->Log_Index-1)+Par->Log_Index) 
					* logar + Par->Size_Fovea-1);

			if (rho>Par->Size_Fovea)
			{
//				pixsize = B*(pow(Par->Log_Index,rho-Par->Size_Fovea)-pow(Par->Log_Index,rho-1-Par->Size_Fovea) );
				pixsize = 10;
			}
			else
				pixsize = 10;

			pixsize = (int)((0.5+(10/pixsize)));
			
			if (pixsize<1)
				pixsize = 1;

				for (yd = 0; yd<pixsize; yd++)
				{
					ytot = (int)(pixsize * y + yd);
					for (xd = 0; xd<pixsize; xd++)
					{
						xtot = (int)(pixsize * x + xd);
						radius = sqrt(xtot*xtot+ytot*ytot)/(pixsize);
						thtmp  = atan2(ytot,-xtot);
						thtmp /= PI;

						if (radius<Par->Size_Fovea)
						{
							Temp_Size_Theta = Par->Size_Theta;
							rho=(int)radius;
							if (rho==0)
								Temp_Size_Theta = 1;
							else if (Par->Fovea_Type < 2)
								Temp_Size_Theta = (Par->Size_Theta/Par->Size_Fovea) * rho;
						}
						else
						{
							Temp_Size_Theta = Par->Size_Theta;
							rho = (int)(0.5+log ((radius-A)/B) * logar + Par->Size_Fovea);
							rho = (int)(log ((radius-Par->Size_Fovea+0.5)*(Par->Log_Index-1)+Par->Log_Index) * logar + Par->Size_Fovea-1 );		
					}

						thtmp *= (Temp_Size_Theta/2);
						thtmp += Temp_Size_Theta/2;

//						theta = (thtmp) + .5 * ((rho)%2);
						theta = (thtmp) + .0 * ((rho)%2);

						if (theta >= Par->Size_Theta)
							theta -= Par->Size_Theta;
						
						if (theta <0)
							theta += Par->Size_Theta;
						if ( (rho==1)&&((int)theta == 0))
							rho = 1;

						if (rho<Par->Size_Rho)
						{
							RawImage[3*(rho*Par->Size_Theta+(int)theta)] += in_image[3*((int)((xtot/pixsize)+Par->Size_X_Orig/2)+(int)((ytot/pixsize)+Par->Size_Y_Orig/2)*Par->Size_X_Orig)]; 
							RawImage[3*(rho*Par->Size_Theta+(int)theta)+1] += in_image[3*((int)((xtot/pixsize)+Par->Size_X_Orig/2)+(int)((ytot/pixsize)+Par->Size_Y_Orig/2)*Par->Size_X_Orig)+1]; 
							RawImage[3*(rho*Par->Size_Theta+(int)theta)+2] += in_image[3*((int)((xtot/pixsize)+Par->Size_X_Orig/2)+(int)((ytot/pixsize)+Par->Size_Y_Orig/2)*Par->Size_X_Orig)+2]; 
							Contat[rho*Par->Size_Theta+(int)theta]++;
						}
					}
				}
		}
	}
	for (i=0; i<Par->Size_LP*3;i++)
		if (Contat[i/3]!=0)
			out_image[i] = RawImage[i]/Contat[i/3];
		else
			out_image[i] = 0;

	for (i=0; i<Par->Size_Theta;i++)
	{
		out_image[3*i]=out_image[0];
		out_image[3*i+1]=out_image[1];
		out_image[3*i+2]=out_image[2];
	}

	free(RawImage);
	free(Contat);

	for (j=0;j<Par->Size_Fovea;j++)
	{
		memcpy(One_Line,out_image + 3*j*Par->Size_Theta,3*Par->Size_Theta * sizeof(unsigned char));

		memset(out_image + 3*j*Par->Size_Theta,0,3*Par->Size_Theta * sizeof(unsigned char));

		for (i=0;i<Par->Size_Theta;i++)
		{
			out_image[3*Pad_Map[j*Par->Size_Theta+i]+0] = One_Line[3*i];
			out_image[3*Pad_Map[j*Par->Size_Theta+i]+1] = One_Line[3*i+1];
			out_image[3*Pad_Map[j*Par->Size_Theta+i]+2] = One_Line[3*i+2];
		}

	}
}
