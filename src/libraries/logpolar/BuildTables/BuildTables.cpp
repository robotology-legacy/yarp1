#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include"LogPolarSDK.h"
#ifndef __WIN__
#include <assert.h>
#endif

void main()
{

//	int XSizeLP,YSizeLP,plLP;
//	int i,j;
///	unsigned char * LP_Image;
//	unsigned char * LP_Image2;
//	unsigned char * Rem_Image;
//	int * Pad_Map;
//	int * Rem_LUT;
//	Neighborhood * Weights_Map;
//	FILE *fin;
	int i;
	int SIZEREMAP = 1090;
	int Pix_Numb = MAX_PIX;

	Image_Data Par;

//	char File_Name [256];
	char Path [256];

	Par = Set_Param(1090,1090,
					1090,1090,
					152,252,42,
					1090,CUSTOM,1);

	Par.padding = 8;

#ifndef __WIN__
	assert(getenv("YARP_ROOT") != NULL);
	sprintf(Path,"%s/%s",getenv("YARP_ROOT"),"conf/");
#else
	sprintf(Path,"%s","C:\\Temp\\Tables\\");
#endif


	printf("Building Color Map for Giotto 2.0\n");

	Build_Color_Map(Path);

	printf("Building Pad Map for Giotto 2.0\n");

	Build_Pad_Map(&Par,Path);

	printf("Building Angular Shift Map for Giotto 2.0\n");

	Build_Ang_Shift_Map(&Par,Path);

	printf("Building XY Map for Giotto 2.0\n");

	Build_XY_Map(&Par,Path);

//	printf("Building Neighborhood Map (%d pixels) for Giotto 2.0\n",Pix_Numb);

//	Build_Neighborhood_Map_NoFov(&Par,Path);	
//	Build_Weights_Map_NoFov(&Par,Path);	
//	Build_Neighborhood_Map(&Par,Path);

	for (i=1; i<=1; i++)
	{
		printf("Building Weights Map (%d pixels) for Giotto 2.0\n",i);
//		Build_Weights_Map(&Par,Path);	
	}

	printf("Building Remap LUT for Zoom Level = %2.2f for Giotto 2.0\n",Par.Zoom_Level);

	Build_Remap_Map(&Par,Path);

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 2.0\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_Map(&Par,Path);

	Par.Size_X_Remap = 352;
	Par.Size_Y_Remap = 288;

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 2.0\n",Par.Size_X_Remap,Par.Size_Y_Remap);

/*	Crop_Remap_LUT(&Par,Path);

	Par.Size_X_Remap = 640;
	Par.Size_Y_Remap = 480;

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 2.0\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_LUT(&Par,Path);

	Par.Size_X_Remap = 320;
	Par.Size_Y_Remap = 240;

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 2.0\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_LUT(&Par,Path);	
	
	
	
	
	Par = Set_Param(545,545,
					545,545,
					76,126,21,
					545,CUSTOM,1);

	sprintf(Path,"%s","W:\\Tables\\Test\\Giotto1.1\\");

	printf("Building Downsample Map for Giotto 1.1\n");

	Build_DownSample_Map(Path);

	printf("Building Color Map for Giotto 1.1\n");

	Build_Color_Map(Path);

	printf("Building Pad Map for Giotto 1.1\n");

	Build_Pad_Map(&Par,Path);

	printf("Building Angular Shift Map for Giotto 1.1\n");

	Build_Ang_Shift_Map(0,&Par,Path);

	printf("Building XY Map for Giotto 1.1\n");

	Build_XY_Map(&Par,Path);

	printf("Building Neighborhood Map (%d pixels) for Giotto 1.1\n",Pix_Numb);

	Build_Neighborhood_Map(Pix_Numb,&Par,Path);

	for (i=1; i<=10; i++)
	{
		printf("Building Weights Map (%d pixels) for Giotto 1.1\n",i);
		Build_Weights_Map (i,&Par,Path);	
	}

	printf("Building Remap LUT for Zoom Level = %2.2f for Giotto 1.1\n",Par.Zoom_Level);

	Build_Remap_LUT(&Par,Path);

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 1.1\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_LUT(&Par,Path);

	Par.Size_X_Remap = 352;
	Par.Size_Y_Remap = 288;

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 1.1\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_LUT(&Par,Path);

	Par.Size_X_Remap = 320;
	Par.Size_Y_Remap = 240;

	printf("Cropping Remap LUT for Size = %d x %d for Giotto 1.1\n",Par.Size_X_Remap,Par.Size_Y_Remap);

	Crop_Remap_LUT(&Par,Path);

//////////////////////////////////////////////////////////////////////////
//Build Color Map														//
//////////////////////////////////////////////////////////////////////////

//	Build_DownSample_Map(Path);

//	Build_Color_Map(Path);

//////////////////////////////////////////////////////////////////////////
//Build Pad Map															//
//////////////////////////////////////////////////////////////////////////
//	Par = Set_Param(545,545,
//					545,545,
//					76,126,21,
//					545,CUSTOM,1);

//	Build_Pad_Map(&Par,Path);

//////////////////////////////////////////////////////////////////////////
//Build Angular Shift map												//
//////////////////////////////////////////////////////////////////////////

//	Build_Ang_Shift_Map(0,&Par,Path);

//////////////////////////////////////////////////////////////////////////
// Build X and Y positions maps											//
//////////////////////////////////////////////////////////////////////////

//	Build_XY_Map(&Par,Path);

//////////////////////////////////////////////////////////////////////////
// Build Neighborhood Map												//
//////////////////////////////////////////////////////////////////////////


//	Build_Neighborhood_Map(Pix_Numb,&Par,Path);

//////////////////////////////////////////////////////////////////////////
// Build Weights Map												//
//////////////////////////////////////////////////////////////////////////

//	Pix_Numb = 4;
//
//	Build_Weights_Map (Pix_Numb,&Par,Path);

//	Weights_Map = (Neighborhood *)malloc(Par.Size_LP * Pix_Numb * 3 * sizeof(Neighborhood));

//	sprintf(File_Name,"%s%s%02d%s",Path,"WeightsMap",Pix_Numb,".gio");

//	fin = fopen(File_Name,"rb");
//	fread(Weights_Map,sizeof(Neighborhood),Par.Size_LP * Pix_Numb * 3,fin);
//	fclose (fin);

//////////////////////////////////////////////////////////////////////////
// Reconstruct Color													//
//////////////////////////////////////////////////////////////////////////

//	LP_Image = Read_Bitmap(&XSizeLP,&YSizeLP,&plLP,Generic_Image_Filename);
//	LP_Image2 = (unsigned char *)calloc(Par.Size_LP * 3 , sizeof(unsigned char));

/*	Reconstruct_Color(LP_Image2,
					  LP_Image,
					  &Par,
					  Weights_Map,
					  Pix_Numb);
*/
//	Save_Bitmap(LP_Image2,XSizeLP,YSizeLP,3,filenameGenOut);

//////////////////////////////////////////////////////////////////////////
// Build Remap LUT														//
//////////////////////////////////////////////////////////////////////////

//	Build_Remap_LUT(&Par,Path);

//	Crop_Remap_LUT(&Par,Path);
/*
	Rem_LUT = (int *)malloc(Par.Size_Img_Remap * sizeof(int));

	sprintf(File_Name,"%s%s%2.2f%s\0",Path,"RemapLUT",Par.Zoom_Level,".gio");
	
	fin = fopen(File_Name,"rb");
	fread(Rem_LUT,sizeof(int),Par.Size_Img_Remap,fin);
	fclose (fin);

	int SizeRemapX = 352;
	int SizeRemapY = 288;

	int CenterX = 0;
	int CenterY = 0;

	int starti = (SIZEREMAP - SizeRemapX)/2;
	starti = (SIZEREMAP/2)+CenterX-(SizeRemapX/2);
	int startj = (SIZEREMAP/2)-CenterY-(SizeRemapY/2);
	int endi = (SIZEREMAP - starti);
	int endj = (SIZEREMAP - startj);

	Rem_Image= (unsigned char *)malloc(SizeRemapX * SizeRemapY * 3 * sizeof(unsigned char));
	unsigned char * RemImgPtr = Rem_Image;
	int * LPImgPtr;

	printf("Start\n");
	for (j=0; j<SizeRemapY; j++)
	{
		LPImgPtr = &Rem_LUT[(j+startj)*SIZEREMAP+starti];
		for (i=0; i<SizeRemapX; i++)
		{
			*RemImgPtr++ = LP_Image2[*LPImgPtr];
			*RemImgPtr++ = LP_Image2[(*LPImgPtr)+1];
			*RemImgPtr++ = LP_Image2[(*LPImgPtr++)+2];
		}
	}
	printf("End\n");
//	Save_Bitmap(Rem_Image,SizeRemapX,SizeRemapY,3,filenameGenOut2);/*
/*
	int rho, theta;

	for (j=0; j<1090; j++)
		for (i=0; i<1090; i++)
		{
			rho = New_Get_Rho(i-545,j-545,&Par);
			theta = New_Get_Theta(i-545,j-545,rho,&Par,Ang_Shift_Map,Pad_Map);
			if ((rho<152)&&(rho>0))
			{
				Rem_Image[3*(j*1090+i)] = LP_Image2[3*(rho*252+theta)];
				Rem_Image[3*(j*1090+i)+1] = LP_Image2[3*(rho*252+theta)+1];
				Rem_Image[3*(j*1090+i)+2] = LP_Image2[3*(rho*252+theta)+2];
			}
		}	
	

/*	LP_Image = (unsigned char *)malloc(Par.Size_LP * 3 * sizeof(unsigned char));

	Cart_Image = Read_Bitmap(&XSizeCart,&YSizeCart,&plCart,Generic_Image_Filename);

	for (j=0; j<Par.Size_LP; j++)
	{
		if (X_Map[j]!=0)
		{
			LP_Image [3*j] = Cart_Image[3*((int)(545+Y_Map[j])*1090+(int)(X_Map[j]+545))];
			LP_Image [3*j+1] = Cart_Image[3*((int)(545+Y_Map[j])*1090+(int)(X_Map[j]+545))+1];
			LP_Image [3*j+2] = Cart_Image[3*((int)(545+Y_Map[j])*1090+(int)(X_Map[j]+545))+2];
		}
		else
		{
			LP_Image [3*j]   = 128;
			LP_Image [3*j+1] = 128;
			LP_Image [3*j+2] = 128;
		}
	}	
	Save_Bitmap(LP_Image,252,152,3,filenameGenOut2);
	/*
//Punti a caso
	srand( (unsigned)time( NULL ) );
	do 
	{
		i = (Par.Size_Theta*rand()/RAND_MAX);
		j = (Par.Size_Rho*rand()/RAND_MAX);
		j = (42*rand()/RAND_MAX);
		k = j*Par.Size_Theta+i;
	}
	while (Color_Map[k]==-1);

	LP_Image[3*k+0] = 255;
	LP_Image[3*k+1] = 255;
	LP_Image[3*k+2] = 255;

	for (l=0; l<Pix_Numb+1; l++)
	{
		i = Neighbor_Map_R[Pix_Numb*k+l].theta;
		j = Neighbor_Map_R[Pix_Numb*k+l].rho;
		LP_Image[3*(j*Par.Size_Theta+i)+0] *= Neighbor_Map_R[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+1] *= Neighbor_Map_R[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+2] *= Neighbor_Map_R[Pix_Numb*k+l].weight;
	}

	do 
	{
		i = (Par.Size_Theta*rand()/RAND_MAX);
		j = (Par.Size_Rho*rand()/RAND_MAX);
		j = (42*rand()/RAND_MAX);
		k = j*Par.Size_Theta+i;
	}
	while (Color_Map[k]==-1);

	LP_Image[3*k+0] = 255;
	LP_Image[3*k+1] = 255;
	LP_Image[3*k+2] = 255;

	for (l=0; l<Pix_Numb+1; l++)
	{
		i = Neighbor_Map_G[Pix_Numb*k+l].theta;
		j = Neighbor_Map_G[Pix_Numb*k+l].rho;
		LP_Image[3*(j*Par.Size_Theta+i)+0] *= Neighbor_Map_G[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+1] *= Neighbor_Map_G[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+2] *= Neighbor_Map_G[Pix_Numb*k+l].weight;
	}

	do 
	{
		i = (Par.Size_Theta*rand()/RAND_MAX);
		j = (Par.Size_Rho*rand()/RAND_MAX);
		j = (42*rand()/RAND_MAX);
		k = j*Par.Size_Theta+i;
	}
	while (Color_Map[k]==-1);

	LP_Image[3*k+0] = 255;
	LP_Image[3*k+1] = 255;
	LP_Image[3*k+2] = 255;

	for (l=0; l<Pix_Numb+1; l++)
	{
		i = Neighbor_Map_B[Pix_Numb*k+l].theta;
		j = Neighbor_Map_B[Pix_Numb*k+l].rho;
		LP_Image[3*(j*Par.Size_Theta+i)+0] *= Neighbor_Map_B[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+1] *= Neighbor_Map_B[Pix_Numb*k+l].weight;
		LP_Image[3*(j*Par.Size_Theta+i)+2] *= Neighbor_Map_B[Pix_Numb*k+l].weight;
	}

	Save_Bitmap(LP_Image,XSizeLP,YSizeLP,plLP,filenameGenOut);
	
	Rem_Image= (unsigned char *)malloc(SIZEREMAP * SIZEREMAP * 3 * sizeof(unsigned char));

	int rho, theta;

	for (j=0; j<1090; j++)
		for (i=0; i<1090; i++)
		{
			rho = New_Get_Rho(i-545,545-j,&Par);
			theta = New_Get_Theta(i-545,545-j,rho,&Par,Ang_Shift_Map,Pad_Map);
			if ((rho<152)&&(rho>0))
			{
				Rem_Image[3*(j*1090+i)] = LP_Image[3*(rho*252+theta)];
				Rem_Image[3*(j*1090+i)+1] = LP_Image[3*(rho*252+theta)+1];
				Rem_Image[3*(j*1090+i)+2] = LP_Image[3*(rho*252+theta)+2];
			}
		}	
	
	Save_Bitmap(Rem_Image,SIZEREMAP,SIZEREMAP,plLP,filenameGenOut2);
	
	
*/	
	
/*	
	int ii,jj,kk;
	int topj,bottomj;
	int topi,bottomi;
	int N = 6;
	double distX,distY,dist;
	int CurrPixCoord,TestPixCoord;


	for (j=0; j<YSizeLP; j++)
	{
			printf ("%d\n",j);
		for (i=0; i<XSizeLP; i++)
		{
//			i = 129;
//			j = 102;
			CurrPixCoord = j*XSizeLP+i;
			if (j-N>0)
				bottomj = j-N;
			else bottomj = 0;

			if (j+N+1<YSizeLP)
				topj = j+N+1;
			else topj = YSizeLP;

			if (i-N>0)
				bottomi = i-N;
			else bottomi = i-N+XSizeLP;

			if (i+N+1<XSizeLP)
				topi = i+N+1;
			else topi = i+N+1-YSizeLP;

			for (jj=bottomj; jj<topj; jj++)
				if (topi>bottomi)
				for (ii=0; ii<XSizeLP; ii++)
				{
					TestPixCoord = jj*XSizeLP+ii;
					distX = X_Map[CurrPixCoord]-X_Map[TestPixCoord];
					distX = distX*distX;
					distY = Y_Map[CurrPixCoord]-Y_Map[TestPixCoord];
					distY = distY*distY;
					dist  = distX+distY;
					for (k=0; k<N; k++)
						if ((dist < Neighbor_Map_R[N*(j*XSizeLP+i)+k].distance)&&(Color_Map[jj*XSizeLP+ii] == 0))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Neighbor_Map_R[N*(j*XSizeLP+i)+kk+1].distance  = Neighbor_Map_R[N*(j*XSizeLP+i)+kk].distance;
									Neighbor_Map_R[N*(j*XSizeLP+i)+kk+1].rho   = Neighbor_Map_R[N*(j*XSizeLP+i)+kk].rho;
									Neighbor_Map_R[N*(j*XSizeLP+i)+kk+1].theta = Neighbor_Map_R[N*(j*XSizeLP+i)+kk].theta;
							}
							Neighbor_Map_R[N*(j*XSizeLP+i)+k].distance = dist;
							Neighbor_Map_R[N*(j*XSizeLP+i)+k].rho   = jj;
							Neighbor_Map_R[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}

					for (k=0; k<N; k++)
						if ((dist < Neighbor_Map_G[N*(j*XSizeLP+i)+k].distance)&&(Color_Map[jj*XSizeLP+ii] == 1))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Neighbor_Map_G[N*(j*XSizeLP+i)+kk+1].distance  = Neighbor_Map_G[N*(j*XSizeLP+i)+kk].distance;
									Neighbor_Map_G[N*(j*XSizeLP+i)+kk+1].rho   = Neighbor_Map_G[N*(j*XSizeLP+i)+kk].rho;
									Neighbor_Map_G[N*(j*XSizeLP+i)+kk+1].theta = Neighbor_Map_G[N*(j*XSizeLP+i)+kk].theta;
							}
							Neighbor_Map_G[N*(j*XSizeLP+i)+k].distance = dist;
							Neighbor_Map_G[N*(j*XSizeLP+i)+k].rho   = jj;
							Neighbor_Map_G[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}

					for (k=0; k<N; k++)
						if ((dist < Neighbor_Map_B[N*(j*XSizeLP+i)+k].distance)&&(Color_Map[jj*XSizeLP+ii] == 2))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Neighbor_Map_B[N*(j*XSizeLP+i)+kk+1].distance  = Neighbor_Map_B[N*(j*XSizeLP+i)+kk].distance;
									Neighbor_Map_B[N*(j*XSizeLP+i)+kk+1].rho   = Neighbor_Map_B[N*(j*XSizeLP+i)+kk].rho;
									Neighbor_Map_B[N*(j*XSizeLP+i)+kk+1].theta = Neighbor_Map_B[N*(j*XSizeLP+i)+kk].theta;
							}
							Neighbor_Map_B[N*(j*XSizeLP+i)+k].distance = dist;
							Neighbor_Map_B[N*(j*XSizeLP+i)+k].rho   = jj;
							Neighbor_Map_B[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}
				}
		}
	}
	
	
	int rho,theta;
	double tempX,tempY;

	rho = 1;
	theta = 42;

	tempX = X_Map[rho*252+theta]; 
	tempY = Y_Map[rho*252+theta]; 

	rho = New_Get_Rho(tempX,tempY,&Par);
	theta = New_Get_Theta(tempX,tempY,rho,&Par,Ang_Shift_Map,Pad_Map);

//	for(j=0; j<NANG*FOV; j++)
//		Inv_Pad_Map[Pad_Map[j]] = j;
	



	LP_Image = Read_Bitmap(&XSizeLP,&YSizeLP,&plLP,Generic_Image_Filename);
	Rem_Image= (unsigned char *)malloc(SIZEREMAP * SIZEREMAP * 3 * sizeof(unsigned char));

	for (j=0; j<1090; j++)
		for (i=0; i<1090; i++)
		{
			rho = New_Get_Rho(i-545,545-j,&Par);
			theta = New_Get_Theta(i-545,545-j,rho,&Par,Ang_Shift_Map,Pad_Map);
			if ((rho<152)&&(rho>0))
			{
				Rem_Image[3*(j*1090+i)] = LP_Image[3*(rho*252+theta)];
				Rem_Image[3*(j*1090+i)+1] = LP_Image[3*(rho*252+theta)+1];
				Rem_Image[3*(j*1090+i)+2] = LP_Image[3*(rho*252+theta)+2];
			}
		}

	for (j=0; j<152; j++)
		for (i=0; i<252; i++)
		{
			if ((Y_Map[j*252+i]>-545)&&(Y_Map[j*252+i]<545))
				if ((X_Map[j*252+i]>-545)&&(X_Map[j*252+i]<545))
			{
				Rem_Image[3*((int)((545-Y_Map[j*252+i]))*1090+(int)((545+X_Map[j*252+i])))] = 0;
				Rem_Image[3*((int)(545-Y_Map[j*252+i])*1090+(int)(545+X_Map[j*252+i]))+1] = 0;
				Rem_Image[3*((int)(545-Y_Map[j*252+i])*1090+(int)(545+X_Map[j*252+i]))+2] = 0;
			}
		}

	Save_Bitmap(Rem_Image,1090,1090,3,filenameGenOut);
/**/
//	Build_RT_Map(0,&Par,XY_Table_Filename,Ang_Shift_Table_Filename);

/*
*/

/*	
	for (j=0; j<5; j++)
	{
		for (i=0; i<6; i++)
			printf("(%1.2f,%1.2f) ", X_Map[j*NANG+i],Y_Map[j*NANG+i]);
//			printf("%1.3f ", X_Map[j*NANG+i]);
		printf("\n");
	}
	
	LP_Image = Read_Bitmap(&XSizeLP,&YSizeLP,&plLP,Input_Image_Filename);

	for (j=1;j<50;j++)
	{
		for (i=0;i<NANG;i++)
		{
			Rem_Image[3*(SIZEREMAP*((int)((SIZEREMAP/2-mult*Y_Map[j*252+i])))+(int)((SIZEREMAP/2+(mult)*X_Map[j*252+i])))+0] = LP_Image[3*(j*NANG+i)];
			Rem_Image[3*(SIZEREMAP*((int)((SIZEREMAP/2-mult*Y_Map[j*NANG+i])))+(int)((SIZEREMAP/2+(mult)*X_Map[j*NANG+i])))+1] = LP_Image[3*(j*NANG+i)+1];
			Rem_Image[3*(SIZEREMAP*((int)((SIZEREMAP/2-mult*Y_Map[j*NANG+i])))+(int)((SIZEREMAP/2+(mult)*X_Map[j*NANG+i])))+2] = LP_Image[3*(j*NANG+i)+2];
		}
	}
    Save_Bitmap(Rem_Image,SIZEREMAP,SIZEREMAP,3,filenameGenOut);
	X_Map = (double *)calloc(XSizeLP * YSizeLP , sizeof(double));
	Y_Map = (double *)calloc(XSizeLP * YSizeLP , sizeof(double));

	Image_Data Par;


//	memset(Array_Image,0,XSizeLP*YSizeLP);

	for (j=1; j< FOV; j++)
	{	
		for (k=0; k<6*j; k++)
		{
			X_Map[j*XSizeLP+k] = Get_X_Center(j,k,&Par,angshift);
			Y_Map[j*XSizeLP+k] = Get_Y_Center(j,k,&Par,angshift);
		}
	}

	for (j=FOV; j< YSizeLP; j++)
	{	
		for (k=0; k<XSizeLP; k++)
		{
			X_Map[j*XSizeLP+k] = Get_X_Center(j,k,&Par,angshift);
			Y_Map[j*XSizeLP+k] = Get_Y_Center(j,k,&Par,angshift);
		}
	}


	fout = fopen(filenameOut4,"w");

	for (j=0; j<XSizeLP*42; j++)
		fprintf(fout,"%0.3f,%0.3f\n",X_Map[j],Y_Map[j]);

	fclose(fout);
/*
//	fprintf(fout,"\n\n\nstatic double X_Map[] = {\n\n");

	for (j=0; j<YSizeLP; j++)
	{	
		for (k=0; k<XSizeLP; k++)
		{
			if (X_Image[j*XSizeLP+k]>=0)
				fprintf(fout," ");
			fprintf(fout,"%0.3f,",X_Image[j*XSizeLP+k]);
		}

		fprintf(fout,"\n");
	}

	fprintf(fout,"\n\n};\n");

	fclose(fout);


// Find N closest Pixels

	int ii,jj,kk;
	int topj,bottomj;
	int topi,bottomi;
	int N = 6;
	double distX,distY,dist;
	int CurrPixCoord,TestPixCoord;
	Dist_Map_R = (proximity *)malloc(XSizeLP * YSizeLP * N * sizeof(proximity));
	Dist_Map_G = (proximity *)malloc(XSizeLP * YSizeLP * N * sizeof(proximity));
	Dist_Map_B = (proximity *)malloc(XSizeLP * YSizeLP * N * sizeof(proximity));
	for (j=0; j<XSizeLP * YSizeLP * N; j++)
	{
		Dist_Map_R [j].dist = 1090*1090;
		Dist_Map_R [j].rho = -1;
		Dist_Map_R [j].theta = -1;
		Dist_Map_G [j].dist = 1090*1090;
		Dist_Map_G [j].rho = -1;
		Dist_Map_G [j].theta = -1;
		Dist_Map_B [j].dist = 1090*1090;
		Dist_Map_B [j].rho = -1;
		Dist_Map_B [j].theta = -1;
	}

	for (j=0; j<YSizeLP; j++)
	{
			printf ("%d\n",j);
		for (i=0; i<XSizeLP; i++)
		{
//			i = 129;
//			j = 102;
			CurrPixCoord = j*XSizeLP+i;
			if (j-N>0)
				bottomj = j-N;
			else bottomj = 0;

			if (j+N+1<YSizeLP)
				topj = j+N+1;
			else topj = YSizeLP;

			if (i-N>0)
				bottomi = i-N;
			else bottomi = i-N+XSizeLP;

			if (i+N+1<XSizeLP)
				topi = i+N+1;
			else topi = i+N+1-YSizeLP;

			for (jj=bottomj; jj<topj; jj++)
				if (topi>bottomi)
				for (ii=0; ii<XSizeLP; ii++)
				{
					TestPixCoord = jj*XSizeLP+ii;
					distX = X_Map[CurrPixCoord]-X_Map[TestPixCoord];
					distX = distX*distX;
					distY = Y_Map[CurrPixCoord]-Y_Map[TestPixCoord];
					distY = distY*distY;
					dist  = distX+distY;
					for (k=0; k<N; k++)
						if ((dist < Dist_Map_R[N*(j*XSizeLP+i)+k].dist)&&(Color_Map[jj*XSizeLP+ii] == 0))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Dist_Map_R[N*(j*XSizeLP+i)+kk+1].dist  = Dist_Map_R[N*(j*XSizeLP+i)+kk].dist;
									Dist_Map_R[N*(j*XSizeLP+i)+kk+1].rho   = Dist_Map_R[N*(j*XSizeLP+i)+kk].rho;
									Dist_Map_R[N*(j*XSizeLP+i)+kk+1].theta = Dist_Map_R[N*(j*XSizeLP+i)+kk].theta;
							}
							Dist_Map_R[N*(j*XSizeLP+i)+k].dist = dist;
							Dist_Map_R[N*(j*XSizeLP+i)+k].rho   = jj;
							Dist_Map_R[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}

					for (k=0; k<N; k++)
						if ((dist < Dist_Map_G[N*(j*XSizeLP+i)+k].dist)&&(Color_Map[jj*XSizeLP+ii] == 1))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Dist_Map_G[N*(j*XSizeLP+i)+kk+1].dist  = Dist_Map_G[N*(j*XSizeLP+i)+kk].dist;
									Dist_Map_G[N*(j*XSizeLP+i)+kk+1].rho   = Dist_Map_G[N*(j*XSizeLP+i)+kk].rho;
									Dist_Map_G[N*(j*XSizeLP+i)+kk+1].theta = Dist_Map_G[N*(j*XSizeLP+i)+kk].theta;
							}
							Dist_Map_G[N*(j*XSizeLP+i)+k].dist = dist;
							Dist_Map_G[N*(j*XSizeLP+i)+k].rho   = jj;
							Dist_Map_G[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}

					for (k=0; k<N; k++)
						if ((dist < Dist_Map_B[N*(j*XSizeLP+i)+k].dist)&&(Color_Map[jj*XSizeLP+ii] == 2))
						{
							for (kk=N-2; kk>=k; kk--)
							{
									Dist_Map_B[N*(j*XSizeLP+i)+kk+1].dist  = Dist_Map_B[N*(j*XSizeLP+i)+kk].dist;
									Dist_Map_B[N*(j*XSizeLP+i)+kk+1].rho   = Dist_Map_B[N*(j*XSizeLP+i)+kk].rho;
									Dist_Map_B[N*(j*XSizeLP+i)+kk+1].theta = Dist_Map_B[N*(j*XSizeLP+i)+kk].theta;
							}
							Dist_Map_B[N*(j*XSizeLP+i)+k].dist = dist;
							Dist_Map_B[N*(j*XSizeLP+i)+k].rho   = jj;
							Dist_Map_B[N*(j*XSizeLP+i)+k].theta = ii;
							break;
						}
				}
		}
	}
	


//	ReconstructColor(Lum_Image,LP_Image,0);

		for (j=0; j<XSizeLP*YSizeLP*3; j++)
			LP_Image[j] = 0;
	double sumR=0;
	double sumG=0;
	double sumB=0;
	bool flagR = false;
	bool flagG = false;
	bool flagB = false;

	i=129;
	j= 102;

	LP_Image [3*(j*XSizeLP+i)] = 255;
	LP_Image [3*(j*XSizeLP+i)+1] = 255;
	LP_Image [3*(j*XSizeLP+i)+2] = 0;

	for (k=0; k<N; k++)
		if (Dist_Map_R[N*(j*XSizeLP+i)+k].dist!=0)
			sumR += 1.0/Dist_Map_R[N*(j*XSizeLP+i)+k].dist;
		else flagR = true;

	for (k=0; k<N; k++)
		if (Dist_Map_G[N*(j*XSizeLP+i)+k].dist!=0)
			sumG += 1.0/Dist_Map_G[N*(j*XSizeLP+i)+k].dist;
		else flagG = true;

	for (k=0; k<N; k++)
		if (Dist_Map_B[N*(j*XSizeLP+i)+k].dist!=0)
			sumB += 1.0/Dist_Map_B[N*(j*XSizeLP+i)+k].dist;
		else flagB = true;

		if (flagR)
			Dist_Map_R[N*(j*XSizeLP+i)].dist = 1/sumR;
		if (flagG)
			Dist_Map_G[N*(j*XSizeLP+i)].dist = 1/sumG;
		if (flagB)
			Dist_Map_B[N*(j*XSizeLP+i)].dist = 1/sumB;
//	sum = 1.0/sum;

	for (k=0; k<N; k++)
	{
		LP_Image[3*(Dist_Map_R[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_R[N*(j*XSizeLP+i)+k].theta)+0] = 255/(Dist_Map_R[N*(j*XSizeLP+i)+k].dist*sumR)+0.5;
		LP_Image[3*(Dist_Map_R[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_R[N*(j*XSizeLP+i)+k].theta)+1] = 0;
		LP_Image[3*(Dist_Map_R[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_R[N*(j*XSizeLP+i)+k].theta)+2] = 0;
		LP_Image[3*(Dist_Map_G[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_G[N*(j*XSizeLP+i)+k].theta)+0] = 0;
		LP_Image[3*(Dist_Map_G[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_G[N*(j*XSizeLP+i)+k].theta)+1] = 255/(Dist_Map_G[N*(j*XSizeLP+i)+k].dist*sumG)+0.5;
		LP_Image[3*(Dist_Map_G[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_G[N*(j*XSizeLP+i)+k].theta)+2] = 0;
		LP_Image[3*(Dist_Map_B[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_B[N*(j*XSizeLP+i)+k].theta)+0] = 0;
		LP_Image[3*(Dist_Map_B[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_B[N*(j*XSizeLP+i)+k].theta)+1] = 0;
		LP_Image[3*(Dist_Map_B[N*(j*XSizeLP+i)+k].rho*XSizeLP+Dist_Map_B[N*(j*XSizeLP+i)+k].theta)+2] = 255/(Dist_Map_B[N*(j*XSizeLP+i)+k].dist*sumB)+0.5;
	}

	LP2Cart(LP_Image,Rem_Image,&Par,angshift);

//	Save_Bitmap(LP_Image,XSizeLP,YSizeLP,3,filenameGenOut);
	Save_Bitmap(Rem_Image,1090,1090,3,filenameGenOut);

	free(LP_Image);
	*/
}
