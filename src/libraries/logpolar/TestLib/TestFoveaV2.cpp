#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

void DownSample_Figo_V2(Image_Data * par,unsigned char * LargeImage, char * Path)
{
//	int rho, theta, temptheta;
//	int tempval;
	char  FN [256];
	FILE * fin;
//	double Ratio = 4.00;
	unsigned short * PadMapL;
//	unsigned short * PadMapS;
	
	unsigned char * TempImage  = (unsigned char *) malloc ((par->Size_LP/4) *  sizeof (unsigned char));
	int * TempImageInt  = (int *) calloc ((par->Size_LP/16) ,  sizeof (int));

	sprintf(FN,"%s%s",Path,"1.00\\PadMap.gio");
	if ((fin = fopen(FN,"rb")) != NULL)
	{
		PadMapL = (unsigned short *) malloc (par->Size_Theta * par->Size_Fovea * sizeof(unsigned short));
		fread(PadMapL,sizeof(unsigned short),par->Size_Theta * par->Size_Fovea,fin);
		fclose (fin);
	}
	else
		PadMapL = NULL;



	for (int j=0; j<par->Size_LP/16; j++)
		TempImage[j] = TempImageInt[j]/64;

	Save_Bitmap(TempImage,par->Size_Theta/4,par->Size_Fovea/4,3,"C:\\Temp\\testImgC.bmp");
}

void main()
{

	const int NECC = 152;
	const int NANG = 252;

	double x,y;
	int i,j,k;

	Image_Data ParamS;
	Image_Data ParamL;

	double * AngShiftMapL;
	char * ColorMapL;
	int * RemapMapL;

	double * AngShiftMapS;
	char * ColorMapS;
	int * RemapMapS;

	unsigned char * LLP;
	unsigned char * SLP;

	char Path [256]; 
	char File_Name [256]; 
	FILE* fin;

	ParamL = Set_Param(	1090,1090,
						1090,1090,
						NECC,NANG,42,
						1090,
						CUSTOM,
						1090.0/83.0);

	ParamL.padding = 1;
	ParamL.Fovea_Type = 0;

	sprintf(Path,"%s%2.2f\\","C:\\Temp\\Tables\\",1.0);


	//Load Large ColorMap
	sprintf(File_Name,"%s%s",Path,"ColorMap.gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		ColorMapL = (char *) malloc (ParamL.Size_LP * sizeof(char));
		fread(ColorMapL,sizeof(char),ParamL.Size_LP,fin);
		fclose (fin);
	}
	else
		ColorMapL = NULL;

	LLP = (unsigned char *) malloc (ParamL.Size_LP * 3 * sizeof (unsigned char));

	//Put the blue plane to max  value
	for (j=0; j< ParamL.Size_LP; j++)
		LLP [3*j+2] = (unsigned char)255;

	//Fovea (when the pixel is valid):  Red plane -> Rising from top to bottom 
	//									Gre plane -> Rising from left to right
	//									Blu plane -> All set to zero 
	for (j=0; j< ParamL.Size_Fovea; j++)
		for (k=0; k< ParamL.Size_Theta; k++)
			if (ColorMapL[j*ParamL.Size_Theta+k] != -1)
			{
				LLP [3*(j*ParamL.Size_Theta+k)]   = (unsigned char)j;
				LLP [3*(j*ParamL.Size_Theta+k)+1] = (unsigned char)k;
				LLP [3*(j*ParamL.Size_Theta+k)+2] = (unsigned char)0;
			}
			else
				LLP [3*(j*ParamL.Size_Theta+k)+2] = (unsigned char)255;

	Save_Bitmap(LLP,ParamL.Size_Theta,ParamL.Size_Rho,3,"C:\\Temp\\testfovea.bmp");

	Build_Remap_Map(&ParamL, Path);

	//Load RemapMap
	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",ParamL.Zoom_Level,ParamL.Size_X_Remap,ParamL.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMapL = (int *) malloc (ParamL.Size_Img_Remap * sizeof(int));
		fread(RemapMapL,sizeof(int),ParamL.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMapL = NULL;

	//Remap
	unsigned char * Rem = (unsigned char *) malloc(ParamL.Size_Img_Remap * 3 * sizeof(unsigned char));

	for (j=0; j< ParamL.Size_Img_Remap; j++)
		Rem[3*j+2] = 255;

	Remap(Rem,LLP,&ParamL,RemapMapL);

	Save_Bitmap(Rem,ParamL.Size_X_Remap,ParamL.Size_Y_Remap,3,"C:\\Temp\\testfovearem.bmp");

	//Set Parameters for the Small Image
	ParamS = Set_Param(	1090,1090,
						1090,1090,
						NECC/4,NANG/4,42/4,
						1090/4,
						CUSTOM,
						1090.0/19.0);

	ParamL.padding = 1;
	ParamL.Fovea_Type = 0;

	sprintf(Path,"%s%2.2f\\","C:\\Temp\\Tables\\",4.0);
	
	//Load Small Color Map
	sprintf(File_Name,"%s%s",Path,"ColorMap.gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		ColorMapS = (char *) malloc (ParamS.Size_LP * sizeof(char));
		fread(ColorMapS,sizeof(char),ParamS.Size_LP,fin);
		fclose (fin);
	}
	else
		ColorMapS = NULL;

	SLP = (unsigned char *) malloc (ParamS.Size_LP * 3 * sizeof (unsigned char));


	//Sets Fovea Colors as in the Large Image
	for (j=0; j< ParamS.Size_LP; j++)
		SLP [3*j+2] = (unsigned char)255;

	for (j=0; j< ParamS.Size_Fovea; j++)
		for (k=0; k< ParamS.Size_Theta; k++)
			if (ColorMapS[j*ParamS.Size_Theta+k] != -1)
			{
				SLP [3*(j*ParamS.Size_Theta+k)]   = (unsigned char)j;
				SLP [3*(j*ParamS.Size_Theta+k)+1] = (unsigned char)k;
				SLP [3*(j*ParamS.Size_Theta+k)+2] = (unsigned char)0;
			}
			else
				SLP [3*(j*ParamS.Size_Theta+k)+2] = (unsigned char)255;



	Save_Bitmap(SLP,ParamS.Size_Theta,ParamS.Size_Rho,3,"C:\\Temp\\testfoveaS.bmp");


	Build_Remap_Map(&ParamS, Path);
	
	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",ParamS.Zoom_Level,ParamS.Size_X_Remap,ParamS.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMapS = (int *) malloc (ParamS.Size_Img_Remap * sizeof(int));
		fread(RemapMapS,sizeof(int),ParamS.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMapS = NULL;

	unsigned char * RemS = (unsigned char *) malloc(ParamS.Size_Img_Remap * 3 * sizeof(unsigned char));

	for (j=0; j< ParamS.Size_Img_Remap; j++)
		RemS[3*j+2] = 255;

	Remap(RemS,SLP,&ParamS,RemapMapS);

	Save_Bitmap(RemS,ParamS.Size_X_Remap,ParamS.Size_Y_Remap,3,"C:\\Temp\\testfovearemS.bmp");

	//Sets a Table with position and weight
	Neighborhood * Table;

	Table = (Neighborhood *) malloc(32 * ParamS.Size_Theta * ParamS.Size_Fovea * 3 * sizeof(Neighborhood));
	
	int counter, red,gre,blu;
	int rho, theta;
	for  (rho = 0; rho< ParamS.Size_Fovea; rho++)
	{
		for  (theta = 0; theta< ParamS.Size_Theta; theta++)
		{
			theta = theta;
			counter = 0;
			for (j=0; j<ParamL.Size_Y_Remap; j++)
			{
				for (i=0; i<ParamL.Size_X_Remap; i++)
				{
					red = Rem[3*(j*ParamL.Size_X_Remap+i)];
					gre = Rem[3*(j*ParamL.Size_X_Remap+i)+1];
					blu = Rem[3*(j*ParamL.Size_X_Remap+i)+2];
					if (blu !=  255)
					{
						if ((red == rho) && (gre == theta))
							counter ++;
					}
				}
			}
			if (counter != 0)
				printf ("%d\n", counter);
		}
	}

}