#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"



void main()
{
	int XSize,YSize,planes;

	unsigned char * BWLeft;
	unsigned char * BWRight;
	unsigned char * ColLeft;
	unsigned char * ColRight;
	unsigned char * RemLeft;
	unsigned char * RemRight;
	unsigned char * DSLeft;
	unsigned char * DSRight;
	unsigned char * Shifted;
	int i,j,k;

	unsigned char a;

	int retval;

	Image_Data LParam,SParam;

	LUT_Ptrs Tables;

	char File_Name [256];
	char Path [256];


	//Loads BW images
	sprintf(File_Name,"%s","c:/temp/images/a1.bmp");
	BWLeft = Load_Bitmap(&XSize,&YSize,&planes,File_Name);
	sprintf(File_Name,"%s","c:/temp/images/right.bmp");
	BWRight = Load_Bitmap(&XSize,&YSize,&planes,File_Name);

	//Sets Small and Large Params
	LParam = Set_Param(1090,1090,256,256,
						YSize,XSize,42,
						1090,
						CUSTOM,256.0/1090.0);
	SParam = Set_Param(256,256,256,256,
						YSize/4,XSize/4,42/4,
						1090/4,
						CUSTOM,4*256.0/1090.0);

	SParam.Ratio = 4.00;


	sprintf(Path,"%s","c:/temp/tables/prefix/");
	Build_Tables(&LParam,&Tables,Path,WEIGHTS|REMAP|DS4);
	Load_Tables(&LParam,&Tables,Path,WEIGHTS|REMAP|DS4);
//	Build_Tables(&SParam,&Tables,Path,SHIFT);
	Load_Tables(&SParam,&Tables,Path,SHIFT|ANGSHIFT|PAD);

	ColLeft = (unsigned char *) malloc (LParam.Size_LP * 3 * sizeof(unsigned char));
	ColRight = (unsigned char *) malloc (LParam.Size_LP * 3 * sizeof(unsigned char));
	RemLeft = (unsigned char *) malloc (SParam.Size_Img_Remap * 3 * sizeof(unsigned char));
	RemRight = (unsigned char *) malloc (SParam.Size_Img_Remap * 3 * sizeof(unsigned char));
	DSLeft = (unsigned char *) malloc ((SParam.Size_LP) * 3 * sizeof(unsigned char));
	DSRight = (unsigned char *) malloc ((SParam.Size_LP) * 3 * sizeof(unsigned char));
	Shifted = (unsigned char *) malloc ((LParam.Size_LP/16) * 3 * sizeof(unsigned char));

	Reconstruct_Color(ColLeft,BWLeft,LParam.Size_Rho,LParam.Size_Theta,1,Tables.WeightsMap,LParam.Pix_Numb);
	Reconstruct_Color(ColRight,BWRight,LParam.Size_Rho,LParam.Size_Theta,1,Tables.WeightsMap,LParam.Pix_Numb);

	//Swaps Red and Blue
	for (j=0; j<LParam.Size_LP; j++)
	{
		a = ColLeft[3*j];
		ColLeft[3*j] = ColLeft [3*j+2];
		ColLeft [3*j+2] = a;
		a = ColRight[3*j];
		ColRight[3*j] = ColRight [3*j+2];
		ColRight [3*j+2] = a;
	}
	
	sprintf(File_Name,"%s","c:/temp/images/02_colleft.bmp");
	Save_Bitmap(ColLeft,LParam.Size_Theta,LParam.Size_Rho,3,File_Name);
	sprintf(File_Name,"%s","c:/temp/images/02_colright.bmp");
	Save_Bitmap(ColRight,LParam.Size_Theta,LParam.Size_Rho,3,File_Name);

	Remap(RemLeft,ColLeft,&LParam,Tables.RemapMap);
	Remap(RemRight,ColRight,&LParam,Tables.RemapMap);
	
	sprintf(File_Name,"%s","c:/temp/images/02_remleft.bmp");
	Save_Bitmap(RemLeft,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,File_Name);
	sprintf(File_Name,"%s","c:/temp/images/02_remright.bmp");
	Save_Bitmap(RemRight,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,File_Name);

	DownSample(ColLeft,DSLeft,Path,&LParam,4.00,Tables.DownSampleMap);
	DownSample(ColRight,DSRight,Path,&LParam,4.00,Tables.DownSampleMap);

	sprintf(File_Name,"%s","c:/temp/images/02_dsleft.bmp");
	Save_Bitmap(DSLeft,LParam.Size_Theta/4,LParam.Size_Rho/4,3,File_Name);
	sprintf(File_Name,"%s","c:/temp/images/02_dsright.bmp");
	Save_Bitmap(DSRight,LParam.Size_Theta/4,LParam.Size_Rho/4,3,File_Name);

	Build_Step_Function(Path,&SParam);

	Load_Tables(&SParam,&Tables,Path,1024);

	retval = Shift_and_Corr(DSLeft,DSRight,&SParam,Tables.ShiftLevels,Tables.ShiftMap);

	SParam.Zoom_Level = 4.0*256.0/1090.0;
	
	
//	TimeI = Get_Time()-TimeI;
//	printf("Time to shift: %f ms\n", TimeI/100.0);
	
 	for (j=0; j<3*SParam.Size_LP; j++)
		Shifted[j] = DSRight[Tables.ShiftMap[(retval)*3*SParam.Size_LP+j]];

	sprintf(File_Name,"%s","c:/temp/images/02_RDSSh.bmp");
	Save_Bitmap(Shifted,LParam.Size_Theta/4,LParam.Size_Rho/4,3,File_Name);

	Load_Tables(&SParam,&Tables,Path,REMAP);

	Remap(RemLeft,DSLeft,&SParam,Tables.RemapMap);
	Remap(RemRight,Shifted,&SParam,Tables.RemapMap);

	sprintf(File_Name,"%s","c:/temp/images/02_remrightsh.bmp");
	Save_Bitmap(RemRight,SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
	sprintf(File_Name,"%s","c:/temp/images/02_remleftsh.bmp");
	Save_Bitmap(RemLeft,LParam.Size_X_Remap,LParam.Size_Y_Remap,3,File_Name);

	unsigned char * shiftimage = (unsigned char *) malloc (3*3*SParam.Size_Img_Remap* sizeof(unsigned char));

	for (j=0; j<SParam.Size_Y_Remap; j++)
		for (i=0; i<SParam.Size_X_Remap; i++)
			for (k=0; k<3; k++)
				shiftimage[3*((j)*(SParam.Size_X_Remap*3)+i+SParam.Size_X_Remap)+k] = RemLeft[3*(j*SParam.Size_X_Remap+i)+k];

	double jf;
	int maxind;

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
	sprintf(File_Name,"%s","c:/temp/images/02_table.bmp");
	Save_Bitmap(shiftimage,3*SParam.Size_X_Remap,SParam.Size_Y_Remap,3,File_Name);
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