#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "LogPolarSDK.h"


void main ()
{
	unsigned char test = 0;
	Image_Data Param;
	LUT_Ptrs Tables;
	char Working_Folder[256];
	char File_Name [256]; 

	int j;

	unsigned char *Cart_Image;
	unsigned char *Total_Cart_Image;
	unsigned char *Total_Map;
	unsigned char *Image1;
	unsigned char *Image2;
	unsigned char *LP_Image;
	unsigned char *LPMap;
	unsigned char *Rem_Image;
	unsigned char *Rem_Image1;
	unsigned char *Rem_Image2;
	unsigned char *Rem_Map;
	Cart2LPInterp * Cart2LP_Map;

	int OrigXSize = 1090;
	int OrigYSize = 1090;
	int RemXSize  = 128;
	int RemYSize  = 128;
	int RhoSize   =  152;
	int ThetaSize =  252;
	int DestSize = 1090;

	int step;
	const int MAXSTEPS = 2;

	int XSize,YSize,planes;

	int CenterX;
	int CenterY;

	sprintf(Working_Folder,"%s","C:\\Temp\\From Talia\\Tables\\Test\\Giotto2.0\\");

	Param = Set_Param(OrigXSize,OrigYSize,
					  RemXSize,RemYSize,
					  RhoSize,ThetaSize,42,
					  1090,
					  CUSTOM,
					  FITOUT);

//	test = Build_Tables(&Param,&Tables,Working_Folder,4);

	test = Build_Tables(&Param,&Tables,Working_Folder,ALLMAPS-12);

 	test = Build_Cart2LP_Map(&Param,Working_Folder);
	Cart2LP_Map = Load_Cart2LP_Map(&Param,Working_Folder);
	test = Load_Tables(&Param,&Tables,Working_Folder,ALLMAPS-12);


	if (test != ALLMAPS)
	{
		printf("error\n");
	}

	int i,k,l;
	sprintf(File_Name,"C:\\Temp\\From Talia\\Test Mosaic\\2428x2428B.bmp");

	Cart_Image = Load_Bitmap(&OrigXSize,&OrigYSize,&planes,File_Name);
	Rem_Image  = (unsigned char *)calloc(3 * Param.Size_Img_Remap , sizeof(unsigned char));
	Rem_Image1  = (unsigned char *)calloc(3 * Param.Size_Img_Remap , sizeof(unsigned char));
	Rem_Image2  = (unsigned char *)calloc(3 * Param.Size_Img_Remap , sizeof(unsigned char));
	Rem_Map    = (unsigned char *)malloc(Param.Size_Img_Remap * sizeof(unsigned char));
	Total_Cart_Image = (unsigned char *)calloc(3 * OrigXSize * OrigYSize , sizeof(unsigned char));
	Total_Map = (unsigned char *)malloc(OrigXSize * OrigYSize * sizeof(unsigned char));

	srand( (unsigned)time( NULL ) );

//	for (j=0; j<Param.Size_Img_Remap; j++)
//		Rem_Map[j] = Rem_Image[3*j];

	int XC[MAXSTEPS];
	int YC[MAXSTEPS];
	for (step = 0; step < MAXSTEPS; step ++)
	{
		Param.Center_X_Remap = (OrigXSize-Param.Size_X_Orig)*rand()/RAND_MAX;
		Param.Center_Y_Remap = (OrigYSize-Param.Size_X_Orig)*rand()/RAND_MAX;
		Param.Center_X_Remap = ((OrigXSize-Param.Size_X_Orig)/2);
		Param.Center_Y_Remap = ((OrigXSize-Param.Size_X_Orig)/2);
//		Param.Center_X_Remap += (400)*rand()/RAND_MAX-200;
//		Param.Center_Y_Remap += (400)*rand()/RAND_MAX-200;
		XC[step] = (400)*rand()/RAND_MAX-200;
		YC[step] = (400)*rand()/RAND_MAX-200;
		XC[step] = 0;
		YC[step] = 0;
		Param.Center_X_Remap += XC[step];
		Param.Center_Y_Remap += YC[step];
		printf("X: %d, Y: %d\n",Param.Center_X_Remap,Param.Center_Y_Remap);

		Image1 = Crop_Image(Cart_Image,&Param,OrigXSize,DestSize);

		LP_Image = Make_LP_Real(Image1,&Param,Cart2LP_Map);

		Remap(Rem_Image,LP_Image,&Param,Tables.RemapMap);

		if (step%2)
			memcpy(Rem_Image1,Rem_Image,Param.Size_Img_Remap);
		else
			memcpy(Rem_Image2,Rem_Image,Param.Size_Img_Remap);

		
		free(LP_Image);

		Add_to_Full_Image(Rem_Image,
						  Total_Cart_Image,
						  Total_Map,
						  Rem_Map,
						  OrigXSize,
						  &Param);
			
	}
	
	double * sum;
	unsigned char * sumimage;
	int nopix = 0;
	sum = (double *)malloc(4 * Param.Size_Img_Remap * sizeof (double));
	sumimage = (unsigned char *)malloc(4 * Param.Size_Img_Remap * sizeof (unsigned char));
	double max,min;

	for (j=0; j<2 * Param.Size_Y_Remap; j++)
		for (i=0; i<2 * Param.Size_X_Remap; i++)
		{
			sum [j*2*Param.Size_X_Remap+i] = 0.0;
			nopix = 0;
			for (k=0; k<Param.Size_Y_Remap; k++)
				for (l=0; l<Param.Size_X_Remap; l++)
				{
					if (((k+Param.Size_Y_Remap-j)>=0)&&((k-j)<0)&&(l+Param.Size_X_Remap-i>=0)&&(l-i<0))
//						if ((Rem_Image1[3*(k*Param.Size_X_Remap+l)+0]!=0)&&(Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+0]!=0))
						{
							sum[j*2*Param.Size_X_Remap+i] += (Rem_Image1[3*(k*Param.Size_X_Remap+l)+0]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+0])
															 *(Rem_Image1[3*(k*Param.Size_X_Remap+l)+0]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+0]);

							sum[j*2*Param.Size_X_Remap+i] += (Rem_Image1[3*(k*Param.Size_X_Remap+l)+1]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+1])
															 *(Rem_Image1[3*(k*Param.Size_X_Remap+l)+1]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+1]);

							sum[j*2*Param.Size_X_Remap+i] += (Rem_Image1[3*(k*Param.Size_X_Remap+l)+2]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+2])
															 *(Rem_Image1[3*(k*Param.Size_X_Remap+l)+2]
															 -Rem_Image2[3*((k+Param.Size_Y_Remap-j)*Param.Size_X_Remap+l+Param.Size_X_Remap-i)+2]);

							nopix ++;
						}
				}

			if (nopix > 0) 
				sum [j*2*Param.Size_X_Remap+i] = sum [j*2*Param.Size_X_Remap+i] / nopix;
			else
				sum [j*2*Param.Size_X_Remap+i] = -1.0;
		}

	max = -100000.0;
	min = 100000;
	int minj;
	for (j=0; j<4 * Param.Size_Img_Remap; j++)
	{
		if (sum[j]>=0)
		{
			if (sum[j]> max)
				max = sum[j];
			if (sum[j]<= min)
			{
				min = sum[j];
				minj = j;
			}
		}
	}

	for (j=0; j<4 * Param.Size_Img_Remap; j++)
	{
		if (sum[j]>=0)
			sumimage [j] = (unsigned char)(128.0 * (sum [j]/max));
		else 
			sumimage [j] = 128;
	}
	sumimage [minj] = 255;


	sprintf(File_Name,"C:\\Temp\\From Talia\\Test Mosaic\\sumimage.bmp");
	Save_Bitmap(sumimage,2*Param.Size_X_Remap,2*Param.Size_Y_Remap,1,File_Name);
//	Save_Bitmap(Total_Cart_Image,OrigXSize,OrigYSize,3,File_Name);

	int estx,esty;

	estx = minj%(2*Param.Size_X_Remap);

	estx = (int)((estx-128)*1090.0/128.0);

	esty = minj/(2*Param.Size_Y_Remap);

	esty = (int)((esty-128)*1090.0/128.0);

	printf ("Real Shift X: %d\n",XC[0]-XC[1]); 
	printf ("Est. Shift X: %d\n",estx); 
	printf ("Real Shift Y: %d\n",YC[0]-YC[1]); 
	printf ("Est. Shift Y: %d\n",esty); 


	free(Rem_Image);
	free(Total_Cart_Image);
	free(Cart_Image);
	free(Total_Map);
	free(Rem_Map);
	

	/*	
//	Simulate_Camera(Cart_Image,LP_Image,&Param,Working_Folder,1);

	sprintf(File_Name,"W:\\Test Images\\1090x1090LP.bmp");
	Save_Bitmap(LP_Image,Param.Size_Theta,Param.Size_Rho,3,File_Name);

//	for (j=3*(Param.Size_LP-1); j>=0; j-=3)
//	{
//		LP_Image[j] = LP_Image[j/3];
//		LP_Image[j+1] = LP_Image[j/3];
//		LP_Image[j+2] = LP_Image[j/3];
//	}

	Remap(Rem_Image,LP_Image,&Param,Tables.RemapMap);

	sprintf(File_Name,"W:\\Test Images\\1090x1090Rem.bmp");
	Save_Bitmap(Rem_Image,Param.Size_X_Remap,Param.Size_Y_Remap,3,File_Name);
	*/
}
/*
	int Pix_Numb;

	int i;
	int planes;

	int address = 0x378;
	int retaddr;
	struct timeval elap_time;
	int retval;
	int cnumb;
	long start,stop;


	unsigned char *LP_Image_1D;
	unsigned char *LP_Image_DS_1D;
	unsigned char *LP_Image_3D;
	unsigned char *LP_Image_DS_3D;
	unsigned char *LP_Image_YUV;
	unsigned char *Rem_Image;

	unsigned short * Downsample_Map;

	int * Rem_LUT;

	FILE * fin;


	Pix_Numb = 4;


//	Cart_Image = Load_Bitmap(&OrigXSize,&OrigYSize,&planes,File_Name);


	Neighborhood * Weights_Map = NULL;

	void * test;
	double * test2;

	test2 = TestLoad(test);

	Weights_Map = Load_Weights_Map(Pix_Numb,&Param,Working_Folder);

	LP_Image_1D = (unsigned char *)calloc(Param.Size_LP    , sizeof(unsigned char));
	LP_Image_DS_1D = (unsigned char *)calloc(Param.Size_LP/4    , sizeof(unsigned char));
	LP_Image_3D = (unsigned char *)calloc(Param.Size_LP * 3, sizeof(unsigned char));
	LP_Image_DS_3D = (unsigned char *)calloc((Param.Size_LP/4) * 3, sizeof(unsigned char));
	LP_Image_YUV = (unsigned char *)calloc(352 * 144 * 3, sizeof(unsigned char));

	Rem_LUT = (int *)malloc(Param.Size_X_Remap * Param.Size_Y_Remap * sizeof(int));

	sprintf(File_Name,"%s%s%02d%s",Working_Folder,"WeightsMap",Pix_Numb,".gio");

	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		fread(Weights_Map,sizeof(Neighborhood),Param.Size_LP * Pix_Numb * 3,fin);
		fclose (fin);
	}

	sprintf(File_Name,"%s%s_%2.2f_%dx%d%s",Working_Folder,"RemapLUT",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");

	
	fin = fopen(File_Name,"rb");
	fread(Rem_LUT,sizeof(int),Param.Size_Img_Remap,fin);
	fclose (fin);

//	Simulate_Camera(Cart_Image,
//		LP_Image,
//		&Param,
//		Working_Folder);

	
	sprintf(File_Name,"%s","W:\\Test Images\\Port_Simul.bmp");
//	Save_Bitmap(LP_Image,Param.Size_Theta,Param.Size_Rho,1,File_Name);


	sprintf(File_Name,"%s","W:\\Test Images\\Port_Simul.raw");

	retaddr = Giotto_Init_Wrapper(Param.Size_Theta,
								  Param.Size_Rho,
//								  "\\\\.\\LPTSIMPLE1",
								  File_Name,
								  0x378,
								  COLOR,
								  0,
								  &cnumb);
	for (i=0; i<10;i++)
	{
		retval = Giotto_Acq_Wrapper(LP_Image_1D,
							  &elap_time,
							  0,
							  &cnumb,
							  retaddr);
		printf("%d\n",i);
	}

	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_Raw.bmp");
	Save_Bitmap(LP_Image_1D,Param.Size_Theta,Param.Size_Rho,1,File_Name);

	Reconstruct_Color(LP_Image_3D,LP_Image_1D,Param.Size_LP,Weights_Map,Pix_Numb);
	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_RGB.bmp");
	Save_Bitmap(LP_Image_3D,Param.Size_Theta,Param.Size_Rho,3,File_Name);

	RGB_2_YUV(LP_Image_YUV,LP_Image_3D,Param.Size_Rho,Param.Size_Theta);
	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_YUV.bmp");
	Save_Bitmap(LP_Image_YUV,352,144*3,1,File_Name);

	Rem_Image = (unsigned char *)calloc(3*Param.Size_Img_Remap , sizeof(unsigned char));
	Remap(Rem_Image,LP_Image_3D,&Param,Rem_LUT);
	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_Rem.bmp");
	Save_Bitmap(Rem_Image,Param.Size_X_Remap,Param.Size_Y_Remap,3,File_Name);


	sprintf(Working_Folder,"%s","W:\\Tables\\Test\\Giotto1.1\\");
	sprintf(File_Name,"%s%s",Working_Folder,"DownsampleMap.gio");

	Downsample_Map = (unsigned short *)malloc(Param.Size_LP * sizeof(unsigned short));
	fin = fopen(File_Name,"rb");
	fread(Downsample_Map,sizeof(unsigned short),Param.Size_LP,fin);
	fclose (fin);
		
	for(i=0; i<Param.Size_LP/4; i++)
		LP_Image_DS_1D[i] = LP_Image_1D[Downsample_Map[i]];

	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_DS_Raw.bmp");
	Save_Bitmap(LP_Image_DS_1D,Param.Size_Theta/2,Param.Size_Rho/2,1,File_Name);

	Param = Set_Param(OrigXSize/2,OrigYSize/2,
					  RemXSize/2,RemYSize/2,
					  RhoSize/2,ThetaSize/2,42/2,
					  1090/2,
					  CUSTOM,
					  1);

	free(Weights_Map);
	Weights_Map = Load_Weights_Map(Pix_Numb,&Param,Working_Folder);

	Reconstruct_Color(LP_Image_DS_3D,LP_Image_DS_1D,Param.Size_LP,Weights_Map,Pix_Numb);
	sprintf(File_Name,"%s","W:\\Test Images\\Acq_Img_DS_RGB.bmp");
	Save_Bitmap(LP_Image_DS_3D,Param.Size_Theta,Param.Size_Rho,3,File_Name);

//	sprintf(File_Name,"%s","W:\\Test Images\\Port_Simul.bmp");

//	Save_Bitmap(LP_Image_YUV,352,144*3,1,File_Name);
//	Save_Bitmap(LP_Image_3D,Param.Size_Theta,Param.Size_Rho,3,File_Name);

/*


	
	sprintf(File_Name,"%s","W:\\Test Images\\Port_Simul_Rem.bmp");

/*
/*
	int size;
	int Pix_Numb = 4;
	int i,j;
	int nof = 1;
	long start,end,mid1,mid2;
	int XSize,YSize,planes;



	unsigned char *buffer;
	unsigned char *LP_Image2;
	unsigned char *Rem_Image;
	unsigned short *Downsample_Map;
	int * Rem_LUT;
	Neighborhood * Weights_Map;

	time_t stime;
	time_t etime;

//	
	char Path []  = "W:\\Tables\\Giotto2.0\\";
	char OutputFilename []  = "W:\\Tables\\Giotto1.1\\Images\\OutputImage.bmp";
	char OutputFilenameOrig []  = "W:\\Tables\\Giotto1.1\\Images\\OutputImageOrig.bmp";
	char OutputFilenameRem []  = "W:\\Tables\\Giotto1.1\\Images\\OutputImageRem.bmp";
	char File_Name [] = "W:\\Test Images\\Squares_Cart_1090.bmp";

	FILE * fin, * fout;

	Cart_Image = Load_Bitmap(&XSize,&YSize,&planes,File_Name);



	Cart2LPReal(Cart_Image,
		LP_Image,
		Path,
		&Param);

//	Simulate_Camera(Cart_Image,
//		LP_Image,
//		&Param,
//		Path);

//	RGB_2_YUV(LP_Image,LP_Image2,&Param);
//	YUV_2_RGB(LP_Image,LP_Image2,&Param);

	sprintf(File_Name,"%s","W:\\Test Images\\Sail_LP_Simul.bmp");
//	Save_Bitmap(LP_Image2,252,152,3,File_Name);

/*	buffer = (unsigned char *)malloc(Param.Size_LP * sizeof(unsigned char));

	address = Giotto_Init(Param.Size_Theta,
						  Param.Size_Rho,
						  "\\\\.\\LPTSIMPLE1",
				 		  0x378,
						  0,
						  0,
						  &cnumb);

	size = Giotto_Acq(buffer,
					  &elap_time,
					  0,
					  &cnumb,
					  address);

	Save_Bitmap(buffer,Param.Size_Theta,Param.Size_Rho,1,OutputFilenameOrig);



	Param = Set_Param(545,545,
					  545,545,
					  76,126,21,
					  545,
					  CUSTOM,
					  1);



	Rem_LUT = (int *)malloc(Param.Resolution * Param.Resolution * sizeof(int));

	sprintf(File_Name,"%s%s_%2.2f_%dx%d%s",Path,"RemapLUT",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	
	fin = fopen(File_Name,"rb");
	fread(Rem_LUT,sizeof(int),Param.Size_Img_Remap,fin);
	fclose (fin);

	Weights_Map = (Neighborhood *)malloc(Param.Size_LP * Pix_Numb * 3 * sizeof(Neighborhood));

	sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Pix_Numb,".gio");

	fin = fopen(File_Name,"rb");
	fread(Weights_Map,sizeof(Neighborhood),Param.Size_LP * Pix_Numb * 3,fin);
	fclose (fin);

	LP_Image = (unsigned char *)calloc(Param.Size_LP * 3 , sizeof(unsigned char));
	Rem_Image = (unsigned char *)malloc(Param.Size_Img_Remap * 3 * sizeof(unsigned char));

	sprintf(File_Name,"%s%s",Path,"Excel\\OutputColorRec.txt");
	fout = fopen(File_Name,"w");

	Param.Size_Img_Remap = Param.Size_X_Remap * Param.Size_Y_Remap;


//	Reconstruct_Color(LP_Image,
//					  buffer,
//					  &Param,
//					  Weights_Map,
//					  Pix_Numb);

		Remap(Rem_Image,
			  LP_Image,
			  &Param,
			  Rem_LUT);



		fclose (fout);


	Save_Bitmap(LP_Image,Param.Size_Theta,Param.Size_Rho,Param.LP_Planes,OutputFilename);
	Save_Bitmap(Rem_Image,Param.Size_X_Remap,Param.Size_Y_Remap,Param.Remap_Planes,OutputFilenameRem);

/*	// Prepare LPMap
	LPMap   = (unsigned char *)malloc(3 * Param.Size_LP	* sizeof(unsigned char));
	for (j=0; j<Param.Size_Fovea; j++)
		for (i=0; i<Param.Size_Theta; i++)
		{
			LPMap[3*(j*Param.Size_Theta+i)+0] = 41;
			LPMap[3*(j*Param.Size_Theta+i)+1] = 41;
			LPMap[3*(j*Param.Size_Theta+i)+2] = 41;
		}

	for (j=Param.Size_Fovea; j<Param.Size_Rho; j++)
		for (i=0; i<Param.Size_Theta; i++)
		{
			LPMap[3*(j*Param.Size_Theta+i)+0] = j;
			LPMap[3*(j*Param.Size_Theta+i)+1] = j;
			LPMap[3*(j*Param.Size_Theta+i)+2] = j;
		}
		LPMap[0] = Param.Size_Rho;
		LPMap[1] = Param.Size_Rho;
		LPMap[2] = Param.Size_Rho;

	Remap(Rem_Image,LPMap,&Param,Tables.RemapMap);
	free(LPMap);
*/

}*/