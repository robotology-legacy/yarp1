#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"


void main()
{
	Image_Data LParam;
	Image_Data SParam;

	unsigned short * PadMap;	
	double * AngShiftMap;
	int XSize = 1090,YSize = 1090,Planes = 3;
	int RemXSize = 256;
	int RemYSize = 256;
	int SRho = 152;
	int STheta = 252;
	int SFovea = 42;

	int i,j,k;
	double tempX, tempY;

	char File_Name [256]; 
	char Path [256]; 
	FILE * fin;

	LUT_Ptrs Tables;
	unsigned char * New_LP_Image;
	unsigned char * LP_Image;
	unsigned char * Rem;
	unsigned char * Full;

	sprintf(Path,"%s","C:\\Temp\\Tables\\Prefix\\");

	LParam = Set_Param(XSize,YSize,
					  1090,1090,
					  SRho,STheta,SFovea,
					  1090,
					  CUSTOM,1090.0/1090.0);

	float ratio = 2.00;

	LP_Image =  Load_Bitmap(&STheta,&SRho,&Planes,"c:/Temp/1090x1090LP.bmp");

//	Build_Tables(&Param,&Tables,Path,516);

	Build_DS_Map(&LParam,Path,ratio);
//

	SParam = Set_Param(XSize,YSize,
					  RemXSize,RemYSize,
					  SRho/(int)(ratio),STheta/(int)(ratio),SFovea/(int)(ratio),
					  1090/(int)(ratio),
					  CUSTOM,(ratio *  RemXSize)/1090.0);

	SParam.Ratio = 2.00;
	
//	Build_Shift_Map(&SParam,Path);


	unsigned short retval = Load_Tables(&LParam,&Tables,Path,DS2);
	retval = Load_Tables(&SParam,&Tables,Path,SHIFT);


	

	New_LP_Image = (unsigned char *) malloc (SParam.Size_LP * 3 * sizeof(unsigned char));
	Rem = (unsigned char *) malloc (SParam.Size_Img_Remap * 3 * sizeof(unsigned char));
	Full = (unsigned char *) malloc (2 * SParam.Size_Img_Remap * 3 * sizeof(unsigned char));

	for (j=0; j<2 * SParam.Size_Img_Remap * 3; j++)
		Full [j] = 192;

	DownSample(LP_Image,New_LP_Image,Path,&LParam,ratio,Tables.DownSampleMap);

//	Save_Bitmap(LP_Image,252,152,3,"c:\\temp\\images\\orig3.bmp");
//	Save_Bitmap(New_LP_Image,252/(int)(ratio),152/(int)(ratio),3,"c:\\temp\\images\\orig4.bmp");

	Build_Remap_Map(&SParam,Path);
	retval = Load_Tables(&SParam,&Tables,Path,REMAP);
	
	int shx = (SParam.Size_X_Remap - SParam.Size_Theta)/2;
	int shy = (SParam.Size_Y_Remap - SParam.Size_Rho)/2;

	for (i=0; i<SParam.Resolution/2; i+=4)
//	for (i=0; i<10; i++)
	{
		for (j=0; j<3*SParam.Size_LP; j++)
			LP_Image[j] = New_LP_Image[Tables.ShiftMap[(i)*3*SParam.Size_LP+j]];

		Remap(Rem,LP_Image,&SParam,Tables.RemapMap);
		sprintf(File_Name,"%s%03d%s","c:/temp/images/shifted/shifted",i,".bmp");
		Save_Bitmap(LP_Image,SParam.Size_Theta,SParam.Size_Rho,3,File_Name);
		sprintf(File_Name,"%s%03d%s","c:/temp/images/shifted/shifted",SParam.Resolution-i-1,".bmp");
		Save_Bitmap(LP_Image,SParam.Size_Theta,SParam.Size_Rho,3,File_Name);

		sprintf(File_Name,"%s%03d%s","c:/temp/images/shrem/Remshifted",i,".bmp");
		Save_Bitmap(Rem,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
		sprintf(File_Name,"%s%03d%s","c:/temp/images/shrem/Remshifted",SParam.Resolution-i-1,".bmp");
		Save_Bitmap(Rem,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);

		for (j=0; j<SParam.Size_Y_Remap; j++)
			for (k=0; k<SParam.Size_X_Remap; k++)
			{
				Full [3*((j*2*SParam.Size_X_Remap+k)+SParam.Size_X_Remap)+0] = Rem [3*(j*SParam.Size_X_Remap+k)+0];
				Full [3*((j*2*SParam.Size_X_Remap+k)+SParam.Size_X_Remap)+1] = Rem [3*(j*SParam.Size_X_Remap+k)+1];
				Full [3*((j*2*SParam.Size_X_Remap+k)+SParam.Size_X_Remap)+2] = Rem [3*(j*SParam.Size_X_Remap+k)+2];
			}
		for (j=0; j<SParam.Size_Rho; j++)
			for (k=0; k<SParam.Size_Theta; k++)
			{
				Full [3*(((j+shy)*2*SParam.Size_X_Remap+k)+shx)+0] = LP_Image [3*(j*SParam.Size_Theta+k)+0];
				Full [3*(((j+shy)*2*SParam.Size_X_Remap+k)+shx)+1] = LP_Image [3*(j*SParam.Size_Theta+k)+1];
				Full [3*(((j+shy)*2*SParam.Size_X_Remap+k)+shx)+2] = LP_Image [3*(j*SParam.Size_Theta+k)+2];
			}

		sprintf(File_Name,"%s%03d%s","c:/temp/images/full/full",i,".bmp");
		Save_Bitmap(Full,2*SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
		sprintf(File_Name,"%s%03d%s","c:/temp/images/full/full",SParam.Resolution-i-1,".bmp");
		Save_Bitmap(Full,2*SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);


		for (j=0; j<3*SParam.Size_Img_Remap; j++)
			Rem[j] = 0;
		for (j=0; j<3*SParam.Size_LP; j++)
			LP_Image[j] = 0;
	}

	free(New_LP_Image);
	free(LP_Image);
//
/*	sprintf(Path,"%s","C:\\Temp\\Tables\\4.00\\");
	sprintf(File_Name,"%s%s",Path,"AngularShiftMap.gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		AngShiftMap = (double *) malloc (Param.Size_Rho * sizeof(double));
		fread(AngShiftMap,sizeof(double),Param.Size_Rho,fin);
		fclose (fin);
	}
	else
		AngShiftMap = NULL;

	sprintf(File_Name,"%s%s",Path,"PadMap.gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		PadMap = (unsigned short *) malloc (Param.Size_Theta * Param.Size_Fovea * sizeof(unsigned short));
		fread(PadMap,sizeof(unsigned short),Param.Size_Theta * Param.Size_Fovea,fin);
		fclose (fin);
	}
	else
		PadMap = NULL;

	LP_Image =  Load_Bitmap(&STheta,&SRho,&Planes,"c:\\temp\\images\\orig2.bmp");
	New_LP_Image = (unsigned char *) malloc (Param.Size_LP * Planes * sizeof(unsigned char));

	float shiftlev = 10.0;
	int newRho, newTheta;

	Param = Set_Param(XSize,YSize,
					  RemXSize/4,RemYSize/4,
					  SRho,STheta,SFovea/4,
					  1090/4,
					  CUSTOM,1090.0/1090.0);

	Param.Fovea_Type = 0;
	for(j=0; j<Param.Size_Rho; j++)
		for(i=0; i<Param.Size_Theta; i++)
		{
			tempX = Param.Size_X_Remap/shiftlev + Get_X_Center(j,i,&Param,AngShiftMap,PadMap);
			tempY = Get_Y_Center(j,i,&Param,AngShiftMap,PadMap);
			if ((tempX<Param.Size_X_Remap/2)&&(tempY<Param.Size_Y_Remap/2))
				if ((tempX>=-Param.Size_X_Remap/2)&&(tempY>=-Param.Size_Y_Remap/2))
				{
					newRho = Get_Rho(tempX,tempY,&Param);
					newTheta = Get_Theta(tempX,tempY,newRho,&Param,AngShiftMap,PadMap);
					if ((newRho>=0)&&(newRho<Param.Size_Rho))
						if ((newTheta>=0)&&(newTheta<Param.Size_Theta))
						{
							New_LP_Image[3*(j*Param.Size_Theta+i)] = LP_Image[3*(newRho*Param.Size_Theta+newTheta)];
							New_LP_Image[3*(j*Param.Size_Theta+i)+1] = LP_Image[3*(newRho*Param.Size_Theta+newTheta)+1];
							New_LP_Image[3*(j*Param.Size_Theta+i)+2] = LP_Image[3*(newRho*Param.Size_Theta+newTheta)+2];
						}
				}

		}
	Save_Bitmap(New_LP_Image,Param.Size_Theta,Param.Size_Rho,3,"c:\\Temp\\images\\TestLPShifted.bmp");

	int * RemapMap;
	unsigned char * Rem = (unsigned char*)malloc(Param.Size_Img_Remap*Planes*sizeof(unsigned char));
	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;
	
	shiftlev = 5.0;
	for(j=0; j<Param.Size_LP*3; j++)
		LP_Image[j] = 205;

	for(j=0; j<Param.Size_Rho; j++)
		for(i=0; i<Param.Size_Theta; i++)
		{
			tempX = Get_X_Center(j,i,&Param,AngShiftMap,PadMap);
			tempY = Param.Size_Y_Remap/shiftlev + Get_Y_Center(j,i,&Param,AngShiftMap,PadMap);
			if ((tempX<Param.Size_X_Remap/2)&&(tempY<Param.Size_Y_Remap/2))
				if ((tempX>=-Param.Size_X_Remap/2)&&(tempY>=-Param.Size_Y_Remap/2))
				{
					newRho = Get_Rho(tempX,tempY,&Param);
					newTheta = Get_Theta(tempX,tempY,newRho,&Param,AngShiftMap,PadMap);
					if ((newRho>=0)&&(newRho<Param.Size_Rho))
						if ((newTheta>=0)&&(newTheta<Param.Size_Theta))
						{
							LP_Image[3*(j*Param.Size_Theta+i)] = New_LP_Image[3*(newRho*Param.Size_Theta+newTheta)];
							LP_Image[3*(j*Param.Size_Theta+i)+1] = New_LP_Image[3*(newRho*Param.Size_Theta+newTheta)+1];
							LP_Image[3*(j*Param.Size_Theta+i)+2] = New_LP_Image[3*(newRho*Param.Size_Theta+newTheta)+2];
						}
				}

		}
	Save_Bitmap(New_LP_Image,252,152,3,"c:\\Temp\\TestLPShifted.bmp");

	for(j=0; j<Param.Size_Img_Remap*3; j++)
		Rem[j] = 205 ;
	
	Remap(Rem,LP_Image,&Param,RemapMap);
	Save_Bitmap(Rem,Param.Size_X_Remap,Param.Size_Y_Remap,Planes,"c:\\Temp\\TestRemOrig.bmp");

	Remap(Rem,New_LP_Image,&Param,RemapMap);
	Save_Bitmap(Rem,Param.Size_X_Remap,Param.Size_Y_Remap,Planes,"c:\\Temp\\TestRemShifted.bmp");
*/
}