#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

#define NECC 152
#define NANG 252
#define CAMERA 1

void main ()
{
	Image_Data Param;
	char File_Name [256]; 

	int i,j;
	int OrigXSize = 1090;
	int OrigYSize = 1090;
	int RemXSize  = 1090;
	int RemYSize  = 1090;
	int RhoSize   =  152;
	int ThetaSize =  252;
	int DestSize  = 1090;

	unsigned char * LP_Image; 
	unsigned char * LPImg; 

	int ret_addr;
	int ret_size;

	Param = Set_Param(OrigXSize,OrigYSize,
					  RemXSize,RemYSize,
					  RhoSize,ThetaSize,42,
					  1090,
					  CUSTOM,
					  FITIN,0,0);

	LPImg = (unsigned char *)malloc(Param.Size_LP * sizeof(unsigned char));
	LP_Image = (unsigned char *)malloc(3 * Param.Size_LP * sizeof(unsigned char));

	
	ret_addr = Giotto_Init(0x378);
	printf("Start\n");
	for (i=0; i<200; i++)
	{
		ret_size = Giotto_Acq(LPImg,NANG,NECC,ret_addr);
		LPImg[0]=0;
		for (j=0; j<Param.Size_LP; j++)
		{
			LP_Image[3*j+0] = LPImg[j];
			LP_Image[3*j+1] = LPImg[j];
			LP_Image[3*j+2] = LPImg[j];
		}
		printf("Frame: %d\n",i);

		sprintf(File_Name,"%s%03d%s","C:\\Temp\\From Talia\\Frames\\Frame_",i,".bmp");
		Save_Bitmap(LP_Image,NANG,NECC,3,File_Name);
	}


}