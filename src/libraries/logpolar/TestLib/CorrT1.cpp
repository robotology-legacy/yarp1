#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define JUSTF 

struct Images{
	unsigned char * BW;
	unsigned char * BWPad;
	unsigned char * Color;
	unsigned char * ColorPad;
	unsigned char * Rem;
	unsigned char * RemPad;
	unsigned char * RemSh;
	unsigned char * RemShPad;
	unsigned char * DS;
	unsigned char * DSPad;
	unsigned char * Shift;
	unsigned char * ShiftPad;
	unsigned char * Histogram;
	unsigned char * TestImg;
};



void addPad(unsigned char * out,unsigned char * in, Image_Data * LParam, int PadSize)
{
	int i,j;

	for (j=0; j<LParam->Size_Rho; j++)
		for (i=0; i<LParam->LP_Planes*LParam->Size_Theta; i++)
			out[j*PadSize+i] = in[LParam->LP_Planes*j*LParam->Size_Theta+i];
}

void removePad(unsigned char * out,unsigned char * in, int width, int height, int planes, int PadSize)
{
	int i,j;

	for (j=0; j<height; j++)
		for (i=0; i<planes*width; i++)
			out[j*planes*width+i] = in[j*PadSize+i];
}

void allocateImages(Images * img, Image_Data * LPar, Image_Data * SPar)
{
	img->BWPad    = (unsigned char *) malloc (computePadSize(LPar->Size_Theta,LPar->padding) * LPar->Size_Rho * sizeof(unsigned char));
	img->Color    = (unsigned char *) malloc (LPar->Size_LP * 3 * sizeof(unsigned char));
	img->ColorPad = (unsigned char *) malloc (computePadSize(3 * LPar->Size_Theta,LPar->padding) * LPar->Size_Rho * sizeof(unsigned char));
	img->RemSh	  = (unsigned char *) malloc (SPar->Size_Img_Remap * 3 * sizeof(unsigned char));
	img->RemShPad = (unsigned char *) malloc (computePadSize(3 * SPar->Size_X_Remap,SPar->padding) * LPar->Size_Y_Remap * sizeof(unsigned char));
	img->Rem	  = (unsigned char *) malloc (SPar->Size_Img_Remap * 3 * sizeof(unsigned char));
	img->RemPad	  = (unsigned char *) malloc (computePadSize(3 * SPar->Size_X_Remap,SPar->padding) * LPar->Size_Y_Remap * sizeof(unsigned char));
	img->DS		  = (unsigned char *) malloc (SPar->Size_LP * 3 * sizeof(unsigned char));
	img->DSPad	  = (unsigned char *) malloc (computePadSize(3 * SPar->Size_Theta,SPar->padding) * SPar->Size_Rho * sizeof(unsigned char));
	img->Shift	  = (unsigned char *) malloc (SPar->Size_LP * 3 * sizeof(unsigned char));
	img->ShiftPad = (unsigned char *) malloc (computePadSize(3 * SPar->Size_Theta,SPar->padding) * SPar->Size_Rho * sizeof(unsigned char));
	img->Histogram= (unsigned char *) malloc (128 * 1024 * sizeof(unsigned char));
	img->TestImg  = (unsigned char *) calloc (SPar->Size_LP , sizeof(unsigned char));
}

void RBSwap(unsigned char * Image, Image_Data * Par)
{
	int i,j;
	unsigned char a;
	int PadSize = computePadSize(3 * Par->Size_Theta,Par->padding);

	for (j=0; j<Par->Size_Rho; j++)
		for (i=0; i<3 * Par->Size_Theta; i+=3)
		{
			a = Image[j*PadSize+i];
			Image[j*PadSize+i] = Image [j*PadSize+i+2];
			Image [j*PadSize+i+2] = a;
		}
}

void paramInit(Image_Data * Small,Image_Data * Large, int XSize, int YSize, int planes)
{

	int Rat = 4;

	*Large = Set_Param(1090,1090,256,256,
						YSize,XSize,42,
						1090,
						CUST,256.0/1090.0);

	*Small = Set_Param(256,256,256,256,
						YSize/Rat,XSize/Rat,42/Rat,
						1090/Rat,
						CUST,Rat*256.0/1090.0);

	Small->Ratio = (float)(Rat);

	Large->padding = 8;
	Small->padding = Large->padding;

	Large->LP_Planes = planes;
	Small->LP_Planes = planes;
}

void shiftImage(Images * img,int shiftVal,int * ShiftMap,Image_Data *Par)
{
	int i,j;
	int APS = (computePadSize(3 * Par->Size_Theta,Par->padding) - 3 * Par->Size_Theta);
	unsigned char * Shiftedptr = img->ShiftPad;
	
#ifdef JUSTF
	for (j=0; j<Par->Size_Fovea; j++)
	{
		for (i=0; i<Par->Size_Theta; i++)
			{
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_Theta * Par->Size_Fovea+(j*Par->Size_Theta+i)]];
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_Theta * Par->Size_Fovea+(j*Par->Size_Theta+i)]+1];
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_Theta * Par->Size_Fovea+(j*Par->Size_Theta+i)]+2];
#else
	for (j=0; j<Par->Size_Rho; j++)
	{
		for (i=0; i<Par->Size_Theta; i++)
			{
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_LP+(j*Par->Size_Theta+i)]];
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_LP+(j*Par->Size_Theta+i)]+1];
				* Shiftedptr++   = img->DSPad[ShiftMap[(shiftVal)*1*Par->Size_LP+(j*Par->Size_Theta+i)]+2];
#endif
			}
			Shiftedptr+=APS;
	}
}


void main()
{
	int XSize,YSize,planes;


	Images Left,Right;
	rgbPixel avgL, avgR;

//	unsigned char * Shifted;
//	int i,j,k;

	int retval;

	Image_Data LParam,SParam;
	Image_Data TParam;

	TParam = Set_Param(1090,1090,128,128,152,252,42,1090,CUST,512.0/1090.0);

//	int tr = Get_Rho((TParam.Size_X_Remap/2)+40,(TParam.Size_Y_Remap/2)+0,&TParam);
	int tr = Get_Rho(64,0,&TParam);

	tr  = tr;

	LUT_Ptrs Tables;

	char File_Name [256];
	char Path [256];

	int ActiveRows = 126;

	//Loads BW images
	sprintf(File_Name,"%s","c:/temp/images/testpad2/left.bmp");
//	sprintf(File_Name,"%s","c:/temp/images/testpad2/Sim_BWLP_R.bmp");
//	sprintf(File_Name,"%s","c:/temp/images/testpad2/Test2_BWLP_L.bmp");
	Right.BW = Load_Bitmap(&XSize,&YSize,&planes,File_Name);

	sprintf(File_Name,"%s","c:/temp/images/testpad2/right.bmp");
//	sprintf(File_Name,"%s","c:/temp/images/testpad2/Sim_BWLP_L.bmp");
//	sprintf(File_Name,"%s","c:/temp/images/testpad2/Test2_BWLP_L.bmp");
	Left.BW = Load_Bitmap(&XSize,&YSize,&planes,File_Name);

	paramInit(&SParam,&LParam,XSize,YSize,planes);

	allocateImages( &Left, &LParam, &SParam);
	allocateImages(&Right, &LParam, &SParam);
	addPad(  Left.BWPad, Left.BW,&LParam,computePadSize(LParam.Size_Theta,LParam.padding));
	addPad( Right.BWPad,Right.BW,&LParam,computePadSize(LParam.Size_Theta,LParam.padding));

	removePad( Left.BW, Left.BWPad,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes,computePadSize(LParam.Size_Theta,LParam.padding));
	removePad(Right.BW,Right.BWPad,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes,computePadSize(LParam.Size_Theta,LParam.padding));

	Save_Bitmap(Left.BW,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes, "c:/temp/images/testpad2/BW_LP_L.bmp");
	Save_Bitmap(Right.BW,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes,"c:/temp/images/testpad2/BW_LP_R.bmp");
	Save_Bitmap(Left.BWPad,computePadSize(LParam.Size_Theta,LParam.padding),LParam.Size_Rho,LParam.LP_Planes, "c:/temp/images/testpad2/BW_LP_L_Pad.bmp");
	Save_Bitmap(Right.BWPad,computePadSize(LParam.Size_Theta,LParam.padding),LParam.Size_Rho,LParam.LP_Planes,"c:/temp/images/testpad2/BW_LP_R_Pad.bmp");

	LParam.LP_Planes = 3;
	SParam.LP_Planes = 3;

	sprintf(Path,"%s","c:/temp/tables/prefix/");
//	Build_Tables(&LParam,&Tables,Path,WEIGHTS|REMAP|DS4);
//	Build_Tables(&LParam,&Tables,Path,DS2);
	Load_Tables(&LParam,&Tables,Path,WEIGHTS|REMAP);
	Load_Tables(&LParam,&Tables,Path,DS4);
//	Build_Tables(&SParam,&Tables,Path,SHIFT);
//	Build_Tables(&SParam,&Tables,Path,2048);
	Build_Step_Function(Path,&SParam);
	Load_Tables(&SParam,&Tables,Path,SHIFT|SHIFTF|ANGSHIFT|PAD);


//Color Reconstruction
	
	Reconstruct_Color(Left.ColorPad,Left.BWPad,LParam.Size_Rho,LParam.Size_Theta,LParam.padding,Tables.WeightsMap,LParam.Pix_Numb);
	Reconstruct_Color(Right.ColorPad,Right.BWPad,LParam.Size_Rho,LParam.Size_Theta,LParam.padding,Tables.WeightsMap,LParam.Pix_Numb);
//	RBSwap(Left.ColorPad,&LParam);
//	RBSwap(Right.ColorPad,&LParam);

	removePad( Left.Color, Left.ColorPad,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes,computePadSize(3 * LParam.Size_Theta,LParam.padding));
	removePad(Right.Color,Right.ColorPad,LParam.Size_Theta,LParam.Size_Rho,LParam.LP_Planes,computePadSize(3 * LParam.Size_Theta,LParam.padding));

	Save_Bitmap( Left.Color,LParam.Size_Theta,LParam.Size_Rho,3,"c:/temp/images/testpad2/Col_LP_L.bmp");
	Save_Bitmap(Right.Color,LParam.Size_Theta,LParam.Size_Rho,3,"c:/temp/images/testpad2/Col_LP_R.bmp");
	Save_Bitmap( Left.ColorPad,computePadSize(3 * LParam.Size_Theta,LParam.padding),LParam.Size_Rho,1,"c:/temp/images/testpad2/Col_LP_L_Pad.bmp");
	Save_Bitmap(Right.ColorPad,computePadSize(3 * LParam.Size_Theta,LParam.padding),LParam.Size_Rho,1,"c:/temp/images/testpad2/Col_LP_R_Pad.bmp");

	Remap( Left.RemPad, Left.ColorPad,&LParam,Tables.RemapMap);
	Remap(Right.RemPad,Right.ColorPad,&LParam,Tables.RemapMap);
	
	removePad(Left.Rem,Left.RemPad,LParam.Size_X_Remap,LParam.Size_Y_Remap,LParam.Remap_Planes,  computePadSize(3 * LParam.Size_X_Remap,LParam.padding));
	removePad(Right.Rem,Right.RemPad,LParam.Size_X_Remap,LParam.Size_Y_Remap,LParam.Remap_Planes,computePadSize(3 * LParam.Size_X_Remap,LParam.padding));

	Save_Bitmap( Left.Rem,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,"c:/temp/images/testpad2/Rem_L.bmp");
	Save_Bitmap(Right.Rem,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,"c:/temp/images/testpad2/Rem_R.bmp");
	Save_Bitmap( Left.RemPad,computePadSize(3 * LParam.Size_X_Remap,LParam.padding),LParam.Size_Y_Remap,1,"c:/temp/images/testpad2/Rem_L_Pad.bmp");
	Save_Bitmap(Right.RemPad,computePadSize(3 * LParam.Size_X_Remap,LParam.padding),LParam.Size_Y_Remap,1,"c:/temp/images/testpad2/Rem_R_Pad.bmp");

	DownSample( Left.ColorPad, Left.DSPad,Path,&LParam,SParam.Ratio,Tables.DownSampleMap);
	DownSample(Right.ColorPad,Right.DSPad,Path,&LParam,SParam.Ratio,Tables.DownSampleMap);

	avgL = computeAvg(SParam.Size_Rho,SParam.Size_Theta,SParam.padding,Left.DSPad);
	avgR = computeAvg(SParam.Size_Rho,SParam.Size_Theta,SParam.padding,Right.DSPad);

	DownSampleFovea( Left.ColorPad, Left.DSPad,Path,&LParam,SParam.Ratio,Tables.DownSampleMap,(int)(ActiveRows/SParam.Ratio));

/*	for (int j = 0; j<SParam.Size_Rho; j++)
		for (int i = 0; i<192; i++)
			if (i == 20)
			{
				Right.DSPad[j*192+i] = 255;
				Left.DSPad[j*192+i] = 255;
			}
*/

	removePad( Left.DS, Left.DSPad,SParam.Size_Theta,SParam.Size_Rho,3,computePadSize(3 * SParam.Size_Theta,SParam.padding));
	removePad(Right.DS,Right.DSPad,SParam.Size_Theta,SParam.Size_Rho,3,computePadSize(3 * SParam.Size_Theta,SParam.padding));

	Save_Bitmap(Left.DS,SParam.Size_Theta,SParam.Size_Rho,3, "c:/temp/images/testpad2/DS_L.bmp");
	Save_Bitmap(Right.DS,SParam.Size_Theta,SParam.Size_Rho,3,"c:/temp/images/testpad2/DS_R.bmp");
	Save_Bitmap(Left.DSPad,computePadSize(3*SParam.Size_Theta,SParam.padding),SParam.Size_Rho,1,"c:/temp/images/testpad2/DS_L_Pad.bmp");
	Save_Bitmap(Right.DSPad,computePadSize(3*SParam.Size_Theta,SParam.padding),SParam.Size_Rho,1,"c:/temp/images/testpad2/DS_R_Pad.bmp");

//	retval = Shift_and_Corr(Left.DSPad,Right.DSPad,&SParam,Tables.ShiftLevels,Tables.ShiftMap,Tables.CorrLevels);
	retval = shiftnCorrFovea(Left.DSPad,Right.DSPad,&SParam,Tables.ShiftLevels,Tables.ShiftMap,Tables.CorrLevels,avgL,avgR,(int)(ActiveRows/SParam.Ratio));

	int actshift = Tables.ShiftFunction[retval];

	int realshift = (actshift*SParam.Size_X_Remap/SParam.Resolution)+SParam.Size_X_Remap/2;

	Make_Disp_Histogram(Left.Histogram,128,1024,Tables.ShiftLevels,Tables.CorrLevels);

	Save_Bitmap(Left.Histogram,1024,128,1,"c:/temp/images/testpad2/Histogram.bmp");

	shiftImage(&Right,retval,Tables.ShiftMapF,&SParam);
    removePad(Right.Shift,Right.ShiftPad,SParam.Size_Theta,SParam.Size_Rho,3,computePadSize(3*SParam.Size_Theta,SParam.padding));
	Save_Bitmap(Right.Shift,SParam.Size_Theta,SParam.Size_Rho,3,"c:/temp/images/testpad2/Shift_R.bmp");

	free(Tables.RemapMap);

	Build_Tables(&SParam,&Tables,Path,REMAP);
	Load_Tables(&SParam,&Tables,Path,REMAP);

	DownSample( Left.ColorPad, Left.DSPad,Path,&LParam,SParam.Ratio,Tables.DownSampleMap);
	Remap( Left.RemShPad, Left.DSPad,   &SParam,Tables.RemapMap);
//	Remap(Right.RemShPad,Right.ShiftPad,&SParam,Tables.RemapMap);
	Remap(Right.RemShPad,Right.DSPad,&SParam,Tables.RemapMap);

	removePad( Left.RemSh, Left.RemShPad,SParam.Size_X_Remap,SParam.Size_Y_Remap,SParam.Remap_Planes,computePadSize(3*SParam.Size_X_Remap,SParam.padding));
	removePad(Right.RemSh,Right.RemShPad,SParam.Size_X_Remap,SParam.Size_Y_Remap,SParam.Remap_Planes,computePadSize(3*SParam.Size_X_Remap,SParam.padding));

	Right.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+realshift)+0]=255;
	Right.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+realshift)+1]=0;
	Right.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+realshift)+2]=0;
	Left.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+(SParam.Size_X_Remap/2))+0]=255;
	Left.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+(SParam.Size_X_Remap/2))+1]=255;
	Left.RemSh[3*((SParam.Size_Y_Remap/2)*SParam.Size_Y_Remap+(SParam.Size_X_Remap/2))+2]=0;

	float FovRadius = ((int)(ActiveRows/SParam.Ratio)) * SParam.Size_X_Remap/(float)SParam.Resolution;

	int x,y;

	for (float j=0; j<PI*2.0; j+= 0.01f)
	{
		x = (int)(0.5+realshift+FovRadius*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+FovRadius*sin(j));
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=255;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=0;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=0;

	}

	for (j=0; j<PI*2.0; j+= 0.01f)
	{
		x = (int)(0.5+realshift+(1.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(1.0+FovRadius)*sin(j));
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/4;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1])/4;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2])/4;

		x = (int)(0.5+realshift+(-1.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(-1.0+FovRadius)*sin(j));
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/4;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+0)/4;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(3*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]+0)/4;

		x = (int)(0.5+realshift+(2.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(2.0+FovRadius)*sin(j));
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/8;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1])/8;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2])/8;

		x = (int)(0.5+realshift+(-2.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(-2.0+FovRadius)*sin(j));
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/8;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+0)/8;
		Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(7*Right.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]+0)/8;

	}

	for (j=0; j<PI*2.0; j+= 0.01f)
	{
		x = (int)(0.5+(SParam.Size_X_Remap/2)+FovRadius*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+FovRadius*sin(j));
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=255;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=255;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=0;

	}

	for (j=0; j<PI*2.0; j+= 0.01f)
	{
		x = (int)(0.5+(SParam.Size_X_Remap/2)+(1.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(1.0+FovRadius)*sin(j));
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/4;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+255)/4;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2])/4;

		x = (int)(0.5+(SParam.Size_X_Remap/2)+(-1.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(-1.0+FovRadius)*sin(j));
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/4;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+255)/4;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(3*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]+0)/4;

		x = (int)(0.5+(SParam.Size_X_Remap/2)+(2.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(2.0+FovRadius)*sin(j));
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/8;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+255)/8;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2])/8;

		x = (int)(0.5+(SParam.Size_X_Remap/2)+(-2.0+FovRadius)*cos(j));
		y = (int)(0.5+(SParam.Size_Y_Remap/2)+(-2.0+FovRadius)*sin(j));
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+0]+255)/8;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+1]+255)/8;
		Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]=(7*Left.RemSh[3*(y*SParam.Size_Y_Remap+x)+2]+0)/8;

	}

	Save_Bitmap(Left.RemSh,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,
				"c:/temp/images/testpad2/Final_L.bmp");
	Save_Bitmap(Right.RemSh,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,
				"c:/temp/images/testpad2/Final_R.bmp");

/*	unsigned char * Shiftedptr = Right.ShiftPad;

	int APS = (computePadSize(3*SParam.Size_Theta,SParam.padding)-3*SParam.Size_Theta);
 	for (j=0; j<SParam.Size_Rho; j++)
	{
 		for (i=0; i<SParam.Size_Theta; i++)
		{
//			Left.Shift[3*(j*SParam.Size_Theta+i)]   = Left.DSPad[Tables.ShiftMap[(100)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]];
//			Left.Shift[3*(j*SParam.Size_Theta+i)+1] = Left.DSPad[Tables.ShiftMap[(100)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]+1];
//			Left.Shift[3*(j*SParam.Size_Theta+i)+2] = Left.DSPad[Tables.ShiftMap[(100)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]+2];
			* Shiftedptr++   = Right.DSPad[Tables.ShiftMap[(retval)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]];
			* Shiftedptr++   = Right.DSPad[Tables.ShiftMap[(retval)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]+1];
			* Shiftedptr++   = Right.DSPad[Tables.ShiftMap[(retval)*1*SParam.Size_LP+(j*SParam.Size_Theta+i)]+2];
		}
		Shiftedptr+=APS;
	}

    removePad(Right.Shift,Right.ShiftPad,SParam.Size_Theta,SParam.Size_Rho,3,computePadSize(3*SParam.Size_Theta,SParam.padding));

	Save_Bitmap(Right.ShiftPad,computePadSize(3*SParam.Size_Theta,SParam.padding),SParam.Size_Rho,1,"c:/temp/images/TestPad/PaddedShift.bmp");
	Save_Bitmap(Right.Shift,SParam.Size_Theta,SParam.Size_Rho,3,"c:/temp/images/TestPad/RightShift.bmp");


	Remap(Right.RemShPad,Right.DSPad,&SParam,Tables.RemapMap);
	removePad(Right.RemSh,Right.RemShPad,SParam.Size_X_Remap,SParam.Size_Y_Remap,SParam.Remap_Planes,computePadSize(3*SParam.Size_X_Remap,SParam.padding));
	Save_Bitmap(Right.RemSh,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,
				"c:/temp/images/TestPad/FinalRight2.bmp");
	
	SParam.Zoom_Level = 4.0*256.0/1090.0;
	
	
	
 //	for (j=0; j<3*SParam.Size_LP; j++)
//		Shifted[j] = Right.DSPad[Tables.ShiftMap[(retval)*3*SParam.Size_LP+j]];

//	sprintf(File_Name,"%s","c:/temp/images/02_RDSSh.bmp");
//	Save_Bitmap(Shifted,LParam.Size_Theta/4,LParam.Size_Rho/4,3,File_Name);

	Load_Tables(&SParam,&Tables,Path,REMAP);

//	Remap(RemLeft,DSLeft,&SParam,Tables.RemapMap);
//	Remap(RemRight,Shifted,&SParam,Tables.RemapMap);

//	sprintf(File_Name,"%s","c:/temp/images/02_remrightsh.bmp");
//	Save_Bitmap(RemRight,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
//	sprintf(File_Name,"%s","c:/temp/images/02_remleftsh.bmp");
//	Save_Bitmap(RemLeft,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,File_Name);

	unsigned char * shiftimage = (unsigned char *) malloc (3*3*SParam.Size_Img_Remap* sizeof(unsigned char));

	for (j=0; j<SParam.Size_Y_Remap; j++)
		for (i=0; i<SParam.Size_X_Remap; i++)
			for (k=0; k<3; k++)
				shiftimage[3*((j)*(SParam.Size_X_Remap*3)+i+SParam.Size_X_Remap)+k] = Left.RemPad[3*(j*SParam.Size_X_Remap+i)+k];
*/
//	double jf;
//	int maxind;

/*	for (k=0; k<maxind/2+1; k++)
		for (jf=0.0; jf<2.0*PI; jf+=PI/720.0)
		{
//			j = sin(jf)*SParam.Size_Y_Remap/2+SParam.Size_Y_Remap/2;
//			i = cos(jf)*SParam.Size_X_Remap/2+SParam.Size_X_Remap/2+SParam.Size_X_Remap+(272.0*ShiftFunction[k]/256.0);
//			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+0] = 2*k;
//			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+1] = 0;
//			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+2] = 255-2*k;

			shiftimage[3*(128*SParam.Size_X_Remap*3+(int)(3*SParam.Size_X_Remap/2+(272.0*ShiftFunction[k]/256.0)))+0] = 255;
			shiftimage[3*(128*SParam.Size_X_Remap*3+(int)(3*SParam.Size_X_Remap/2+(272.0*ShiftFunction[k]/256.0)))+1] = 0;
			shiftimage[3*(128*SParam.Size_X_Remap*3+(int)(3*SParam.Size_X_Remap/2+(272.0*ShiftFunction[k]/256.0)))+2] = 0;
		}

		for (jf=0.0; jf<2.0*PI; jf+=PI/720.0)
		{
			j = sin(jf)*SParam.Size_Y_Remap/2+SParam.Size_Y_Remap/2;
			i = cos(jf)*SParam.Size_X_Remap/2+SParam.Size_X_Remap/2+SParam.Size_X_Remap+(272.0*ShiftFunction[0]/256.0);
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+0] = 0;
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+1] = 255;
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+2] = 0;
			j = sin(jf)*SParam.Size_Y_Remap/2+SParam.Size_Y_Remap/2;
			i = cos(jf)*SParam.Size_X_Remap/2+SParam.Size_X_Remap/2+SParam.Size_X_Remap+(272.0*ShiftFunction[maxind]/256.0);
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+0] = 0;
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+1] = 0;
			shiftimage[3*(j*SParam.Size_X_Remap*3+i)+2] = 255;

		}
*/
//	sprintf(File_Name,"%s","c:/temp/images/02_table.bmp");
//	Save_Bitmap(shiftimage,3*SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
	}
	
	
/*
double YARPLpDisparity::ShiftAndCorrelate (const YARPImageOf<YarpPixelRGB>& l, const YARPImageOf<YarpPixelRGB>& r, int shift)
{
	int i, j, i1, j1;
	int count = 0;
	float d1 = 0;
	int jt;

	unsigned char **src1p0 = (unsigned char **)l.GetArray();
	unsigned char **src2p0 = (unsigned char **)r.GetArray();

	int thetaint;
	if (shift >= 0)
		thetaint = 0;
	else
	{
		shift = -shift;
		thetaint = nAng / 2;
	}

	// use normalized cross correlation.
	double average_1r = 0;
	double average_1g = 0;
	double average_1b = 0;
	double average_2r = 0;
	double average_2g = 0;
	double average_2b = 0;
	
	double d_1 = 0;
	double d_2 = 0;


	// blind implementation.
	// there should be a smarter formula.
	for(i = 0; i < nEcc * nAng; i++)
//		for(j = 0; j < nAng; j++)
		{
			jt = (j + thetaint) % nAng;
			i1 = lut[shift][i][jt].ecc;
			j1 = lut[shift][i][jt].ang;
			j1 = (j1 + nAng - thetaint) % nAng;

			if (i1 >= 0)
			{
				average_1r += src1p0[3*i];
				average_1g += src1p0[3*i+1];
				average_1b += src1p0[3*i+2];
				average_2r += src2p0[i1];
				average_2g += src2p0[i1+1];
				average_2b += src2p0[i1+2];
				count++;
			}
		}

	average_1r /= count;
	average_1g /= count;
	average_1b /= count;
	average_2r /= count;
	average_2g /= count;
	average_2b /= count;

	double num = 0;
	double den_1 = 0;
	double den_2 = 0;

	for(i = 0; i < nEcc; i++)
		for(j = 0; j < nAng; j++)
		{
			jt = (j + thetaint) % nAng;
			i1 = lut[shift][i][jt].ecc;
			j1 = lut[shift][i][jt].ang;
			j1 = (j1 + nAng - thetaint) % nAng;

			if (i1 >= 0)
			{
				d_1 = src1p0[3*i] - average_1r;
				d_2 = src2p0[j1][i1*3] - average_2r;
				num += (d_1 * d_2);
				den_1 += (d_1 * d_1);
				den_2 += (d_2 * d_2);

				d_1 = src1p0[3*i+1] - average_1g;
				d_2 = src2p0[j1][i1*3+1] - average_2g;
				num += (d_1 * d_2);
				den_1 += (d_1 * d_1);
				den_2 += (d_2 * d_2);

				d_1 = src1p0[3*i+2] - average_1b;
				d_2 = src2p0[j1][i1*3+2] - average_2b;
				num += (d_1 * d_2);
				den_1 += (d_1 * d_1);
				den_2 += (d_2 * d_2);
			}
		}

	return (1.0 - (num * num) / (den_1 * den_2 + 0.00001));
}/**/
