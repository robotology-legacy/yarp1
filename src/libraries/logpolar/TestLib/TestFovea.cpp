#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#include "LogPolarSDK.h"

void DownSample_Figo(Image_Data * par,unsigned char * LargeImage, char * Path)
{
	int rho, theta, temptheta;
	int tempval;
	int t0,t1,tm1,tm2,tm3,tp1,tp2,tp3,k,l;
	char  FN [256];
	FILE * fin;
	double Ratio = 4.00;
	unsigned short * PadMapL;
	unsigned short * PadMapS;
	int * divtab;
	
	unsigned char * TempImage  = (unsigned char *) malloc ((par->Size_LP/4) *  sizeof (unsigned char));
	int * TempImageInt  = (int *) calloc ((par->Size_LP/16) ,  sizeof (int));


	//Loads the Padding Map for the Large Image
	sprintf(FN,"%s%s",Path,"1.00\\PadMap.gio");
	if ((fin = fopen(FN,"rb")) != NULL)
	{
		PadMapL = (unsigned short *) malloc (par->Size_Theta * par->Size_Fovea * sizeof(unsigned short));
		fread(PadMapL,sizeof(unsigned short),par->Size_Theta * par->Size_Fovea,fin);
		fclose (fin);
	}
	else
		PadMapL = NULL;

	//I am downsampling to 63x38, New
	if (Ratio == 4.00)
	{
		//Loads the Padding Map for the Downsampled Image
		sprintf(FN,"%s%s",Path,"4.00\\PadMap.gio");
		if ((fin = fopen(FN,"rb")) != NULL)
		{
			PadMapS = (unsigned short *) malloc (par->Size_Theta/4 * par->Size_Fovea/4 * sizeof(unsigned short));
			fread(PadMapS,sizeof(unsigned short),par->Size_Theta/4 * par->Size_Fovea/4,fin);
			fclose (fin);
		}
		else
			PadMapS = NULL;
		
		//Starts from the 8th row, Ends (approx) at the end of Fovea
		for (rho=8; rho<par->Size_Fovea; rho+=8)
		{
			//rho*6 is the size in pixel of the rho-th row
			for (theta = -3; theta<((rho-2)*6)-3; theta +=4)
			{
				//Check the boundaries
				if (theta < 0)
					temptheta = theta+((rho-2)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+((theta+3)/4))+l] += 2*LargeImage[3*((rho-2)*par->Size_Theta+temptheta+k)+l];
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+3)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+3)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+3)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
				}

			}

			for (theta = -5; theta<((rho-1)*6)-5; theta +=4)
			{
				if (theta < 0)
					temptheta = theta+((rho-1)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
				}
			}

			for (theta = -3; theta<(rho*6)-3; theta +=4)
			{
				if (theta < 0)
					temptheta = theta+(rho*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=0; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+3)/4]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho)*par->Size_Theta+PadMapL[(rho)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];

			}

			for (theta = -5; theta<((rho+1)*6)-5; theta +=4)
			{
				if (theta < 0)
					temptheta = theta+((rho+1)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+5)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
				}


			}

			for (theta = -4; theta<((rho+2)*6)-4; theta +=4)
			{
				if (theta < 0)
					temptheta = theta+((rho+2)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+4)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+4)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta+4)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
				}
			}
		}

		for (rho=4; rho<par->Size_Fovea; rho+=8)
		{
			for (theta = +2; theta<((rho-2)*6)+2; theta +=4)
			{
//				if (theta >= rho*6)
				if (theta >= (rho-2)*6)
					temptheta = theta-((rho-2)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)/4]%(par->Size_Theta/4)))+l] += 2;

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)*(rho)/(4*(rho-2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho-2)*par->Size_Theta+PadMapL[(rho-2)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)/4]%(par->Size_Theta/4)))+l] += 1;
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-2)/4]%(par->Size_Theta/4)))+l] += 1;
				}
//		if (TempImageInt[3*(9*63+1)] != 0)
//			theta = theta;
			}
			
			for (theta = +1; theta<((rho-1)*6)+1; theta +=4)
			{
				if (theta >= rho*6)
					temptheta = theta-((rho-1)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 4;

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho-1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 2;
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 2;
				}
			}

			for (theta = +3; theta<(rho*6)+3; theta +=4)
			{
				if (theta >= rho*6)
					temptheta = theta-(rho*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=0; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)/4]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho)*par->Size_Theta+PadMapL[(rho)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)/4]%(par->Size_Theta/4)))+l] += 4;
			}

			for (theta = +1; theta<((rho+1)*6)+1; theta +=4)
			{
				if (theta >= rho*6)
//				if (theta >= (rho+1)*6)
					temptheta = theta-((rho+1)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 4*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 4;

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)*(rho)/(4*(rho+1))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 2;
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-1)/4]%(par->Size_Theta/4)))+l] += 2;
				}
			}

			for (theta = +3; theta<((rho+2)*6)+3; theta +=4)
			{
				if (theta < 0)
					temptheta = theta+((rho+2)*6);
				else
					temptheta = theta;

				for (l=0; l<3; l++)
					for (k=1; k<4; k++)
						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 2*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+k]%par->Size_Theta)+l];
//						TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)/4]%(par->Size_Theta/4)))+l] += 2;

				for (l=0; l<3; l++)
				{
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+0]%par->Size_Theta)+l];
					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)*(rho)/(4*(rho+2))]%(par->Size_Theta/4)))+l] += 1*LargeImage[3*((rho+2)*par->Size_Theta+PadMapL[(rho+2)*par->Size_Theta+temptheta+4]%par->Size_Theta)+l];
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)/4]%(par->Size_Theta/4)))+l] += 1;
//					TempImageInt[3*(((rho)/4)*(par->Size_Theta/4)+(PadMapS[(rho/4)*(par->Size_Theta/4)+(theta-3)/4]%(par->Size_Theta/4)))+l] += 1;
				}

			}
		}


	}
	else	//Ratio = 2.00;
	{
		sprintf(FN,"%s%s",Path,"2.00\\PadMap.gio");
		if ((fin = fopen(FN,"rb")) != NULL)
		{
			PadMapS = (unsigned short *) malloc (par->Size_Theta/2 * par->Size_Fovea/2 * sizeof(unsigned short));
			fread(PadMapS,sizeof(unsigned short),par->Size_Theta/2 * par->Size_Fovea/2,fin);
			fclose (fin);
		}
		else
			PadMapS = NULL;

		for (rho=4; rho<par->Size_Fovea; rho+=4)
		{
			for (theta = 0; theta<rho*6; theta += 2)
			{

				t0 = theta;
				t1 = theta - 1;
				if (t1<0) t1 += (rho) * 6;

				tm1 = theta - 1;
				if (tm1<0) tm1 += (rho-1) * 6;
				tm2 = theta - 2;
				if (tm2<0) tm2 += (rho-1) * 6;
				tm3 = theta - 3;
				if (tm3<0) tm3 += (rho-1) * 6;

				tp1 = theta - 1;
				if (tp1<0) tp1 += (rho+1) * 6;
				tp2 = theta - 2;
				if (tp2<0) tp2 += (rho+1) * 6;
				tp3 = theta - 3;
				if (tp3<0) tp3 += (rho+1) * 6;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[(rho/2)*(par->Size_Theta/2)+theta/2]%(par->Size_Theta/2))] = tempval/16;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)+1];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)+1];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)+1];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)+1];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[rho/2*par->Size_Theta/2+theta/2]%(par->Size_Theta/2))+1] = tempval/16;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)+2];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)+2];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)+2];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)+2];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[(rho/2)*(par->Size_Theta/2)+theta/2]%(par->Size_Theta/2))+2] = tempval/16;
			}
		}

		for (rho=2; rho<par->Size_Fovea; rho+=4)
		{
			for (theta = 0; theta<rho*6; theta += 2)
			{

				t0 = theta + 2;
				if (t0>rho*6) t0-=rho*6;
				t1 = theta + 3;
				if (t1>rho*6) t1-=rho*6;

				tm1 = theta;
				tm2 = theta + 1;
				if (tm2>(rho-1) * 6) tm2 -= (rho-1) * 6;
				tm3 = theta + 2;
				if (tm3>(rho-1) * 6) tm3 -= (rho-1) * 6;

				tp1 = theta;
				tp2 = theta + 1;
				if (tp2>(rho+1) * 6) tp2 -= (rho+1) * 6;
				tp3 = theta + 2;
				if (tp3>(rho+1) * 6) tp3 -= (rho+1) * 6;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[rho/2*par->Size_Theta/2+theta/2]%(par->Size_Theta/2))] = tempval/16;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)+1];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)+1];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)+1];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)+1];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)+1];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[rho/2*par->Size_Theta/2+theta/2]%(par->Size_Theta/2))+1] = tempval/16;

				tempval = 0;
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t0]%par->Size_Theta)+2];
				tempval += 4 * LargeImage [3*(rho*par->Size_Theta+PadMapL[rho*par->Size_Theta+t1]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm3]%par->Size_Theta)+2];
				tempval += 2 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm2]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho-1)*par->Size_Theta+PadMapL[(rho-1)*par->Size_Theta+tm1]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp3]%par->Size_Theta)+2];
				tempval += 2 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp2]%par->Size_Theta)+2];
				tempval += 1 * LargeImage [3*((rho+1)*par->Size_Theta+PadMapL[(rho+1)*par->Size_Theta+tp1]%par->Size_Theta)+2];

				TempImage[3*((rho/2)*par->Size_Theta/2+PadMapS[rho/2*par->Size_Theta/2+theta/2]%(par->Size_Theta/2))+2] = tempval/16;
			}
		}
	}

	for (int j=0; j<par->Size_LP/16; j++)
		TempImage[j] = TempImageInt[j]/64;

	Save_Bitmap(TempImage,par->Size_Theta/4,par->Size_Fovea/4,3,"C:\\Temp\\testImgC.bmp");
}

void main()
{

	const int NECC = 152;
	const int NANG = 252;

	double x,y;
	int i,j;
	int SRho, STheta, Planes;

	Image_Data Param;

	double * AngShiftMap;
	unsigned short * PadMap;
	int * RemapMap;

	char Path [256]; 
	char File_Name [256]; 
	FILE* fin;

	Param = Set_Param(	1090,1090,
						1090,1090,
						NECC,NANG,42,
						1090,
						CUSTOM,
						20);



	unsigned char * TLP = Load_Bitmap(&STheta,&SRho,&Planes,"c:\\Temp\\Test_Rem_Large_Image.bmp");

	sprintf(Path,"%s","C:\\Temp\\Tables\\");

	DownSample_Figo(&Param,TLP, Path);

	Param.padding = 1;
	Param.Fovea_Type = 0;

	sprintf(Path,"%s%2.2f\\","C:\\Temp\\Tables\\",1.0);

//	Build_Remap_Map(&Param, Path);
	
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


	sprintf(File_Name,"%s%s_%2.3f_%dx%d%s",Path,"RemapMap",Param.Zoom_Level,Param.Size_X_Remap,Param.Size_Y_Remap,".gio");
	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		RemapMap = (int *) malloc (Param.Size_Img_Remap * sizeof(int));
		fread(RemapMap,sizeof(int),Param.Size_Img_Remap,fin);
		fclose (fin);
	}
	else
		RemapMap = NULL;

	unsigned char * LP = Load_Bitmap(&STheta,&SRho,&Planes,"c:\\Temp\\Tables\\4.00\\ReferenceImage.bmp");
	unsigned char * Rem = (unsigned char *) malloc(Param.Size_Img_Remap * 3 * sizeof(unsigned char));

	Remap(Rem,LP,&Param,RemapMap);

	Save_Bitmap(Rem,Param.Size_X_Remap,Param.Size_Y_Remap,3,"C:\\Temp\\testfovea.bmp");

}