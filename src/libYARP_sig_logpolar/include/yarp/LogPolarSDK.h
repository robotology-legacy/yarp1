/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #fberton#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: LogPolarSDK.h,v 1.4 2004-08-05 14:16:28 babybot Exp $
///
///

#ifndef ___LOGPOLARSDK___
#define ___LOGPOLARSDK___


#define Giotto1 0
#define Giotto2 1
#define CUST 20
#define FITIN   99
#define FITOUT 101

#define MONO   0
#define COLOR  1

#define DOWNSAMPLE  1
#define SIMULATE 0

#define ALLMAPS  1023

#define RED   0
#define GREEN 1
#define BLUE  2

#define MAX_PIX 10

#define PI 3.1415926535897932384626433832795

#define ANGSHIFT	  1
#define COL			  2
#define DS4			  4
#define PAD			 16
#define REMAP		 32
#define WEIGHTS		 64
#define DS2			256
///#define SHIFT	   1024
///#define SHIFTF	   2048

struct Image_Data
{
	// Logarithm Index
	double Log_Index;
	bool Valid_Log_Index;

	// Zoom Level of the Remapped Image
	double Zoom_Level;

	// Ratio between the diameter of the image and the size of the smallest pixel
	int Resolution;
	double dres;

//	int Fovea_Display_Mode; //0 Sawtooth (Raw); 1 Triangular; 2 Complete

	// Log Polar Metrics
	int Size_Rho;
	int Size_Theta;
	int Size_Fovea;
	int Size_LP;
	int Fovea_Type; //0->3 Giotto 2.0; 4->7 Giotto 2.1 //0;4 Sawtooth (Raw); 1;5 Triangular; 2;6 Complete
	int Pix_Numb;
	int Fovea_Display_Mode;

	// Remapped Cartesian Metrics
	int Size_X_Remap;
	int Size_Y_Remap;
	int Size_Img_Remap;

	// Original Cartesian Metrics
	int Size_X_Orig;
	int Size_Y_Orig;
	int Size_Img_Orig;

	// Color Depth of the Images
	int Orig_Planes;
	int Remap_Planes;
	int LP_Planes;

	// Orientation of the Cartesian Image
	bool Orig_LandScape;
	bool Remap_LandScape;

	int padding;

	float Ratio;  //Used just for naming purpose
};

struct Neighborhood
{
	unsigned short position;
	float weight;
};

struct IntNeighborhood
{
	unsigned short NofPixels;
	unsigned int * position;
	unsigned char * weight;
};

struct Cart2LPInterp
{
	unsigned char NofPixels;
	unsigned int * position;
};

struct LUT_Ptrs
{
	double			* AngShiftMap;
	char			* ColorMap;
	IntNeighborhood	* DownSampleMap;
	Neighborhood	* NeighborMap;
	unsigned short	* PadMap;
	int				* RemapMap;
	Neighborhood	* WeightsMap;
	double			* XYMap;
	int				* ShiftMap;
	int				* ShiftMapF;
	int				  ShiftLevels;
	int				* ShiftFunction;
	double			* CorrLevels;
};


struct rgbPixel
{
	unsigned char Red;
	unsigned char Gre;
	unsigned char Blu;
};


typedef struct tag_vchannel 
{
	int width;
	int height;
	int format;
	int grab_fd;
	int err;
	char source;
	int elapsed_time;
} VCHANNEL;

Image_Data Set_Param(int SXO,
					 int SYO,
					 int SXR,
					 int SYR,
					 int rho,
					 int theta,
					 int fovea,
					 int resolution,
					 int LPMode, 
					 double ZoomLevel);

unsigned char * Load_Bitmap(
				 int *X_Size,
				 int *Y_Size,
				 int *planes,
				 char * filename);

void Save_Bitmap(unsigned char *image,
				 int X_Size,
				 int Y_Size,
				 int planes,
				 char * filename);

void Reconstruct_Color(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   Neighborhood * Weights_Map,
					   int Pix_Numb);

void Reconstruct_Grays(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   Neighborhood * Weights_Map,
					   int Pix_Numb);

void Remap(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT);

void Remap_Mono(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT);

void DownSample(unsigned char * InImage, unsigned char * OutImage, char * Path, Image_Data * Param, float Ratio,IntNeighborhood * IntDownSampleTable);
void DownSampleFovea(unsigned char * InImage, unsigned char * OutImage, char * Path, Image_Data * Param, float Ratio,IntNeighborhood * IntDownSampleTable,int Rows);

double Compute_Index(double Resolution, int Fovea, int SizeRho);

void Cart2LPReal(unsigned char *in_image, 
				 unsigned char *out_image,
				 char * Path,
				 Image_Data *Par);

char * Load_Color_Map (Image_Data * Param, char * Path);

Neighborhood * Load_Weights_Map (int PixNumb, Image_Data * Param, char * Path);

double *  TestLoad(void * testptr);
unsigned short Load_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned short List);

int Get_Rho(double x,
			double y,
			Image_Data * par);

int Get_Theta(double x,
			  double y,
			  int rho,
			  Image_Data * par, 
			  double *Ang_Shift, 
			  unsigned short *Pad_Map);

double Get_X_Center(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap);
double Get_Y_Center(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap);
double getXfloatRes(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap);
double getYfloatRes(double rho, double theta, Image_Data *par, double *Ang_Shift,unsigned short * PadMap);


int Get_XY_Center(double *xx, double *yy, int rho, int theta, Image_Data *par, double *Ang_Shift);
int Get_XY_Center_Uniform(double *xx, double *yy, int rho, int theta, Image_Data *par, double *Ang_Shift);

Cart2LPInterp * Load_Cart2LP_Map(Image_Data * Param, char * Path);

void Free_Cart2LP_Map (Cart2LPInterp *map);

unsigned char * Crop_Image (unsigned char * Input_Image,Image_Data * Par,int OrigXSize, int DestSize);

int Make_LP_Real (unsigned char * Output_Image, unsigned char * Input_Image, Image_Data * Par, Cart2LPInterp * Cart2LP_Map);

void Fast_Reconstruct_Color(unsigned char * Out_Image,
					   unsigned char * In_Image,
					   int height,
					   int width,
					   int padding,
					   IntNeighborhood * WeightsMap,
					   int Pix_Numb);

void Make_Disp_Histogram(unsigned char * hist,int height, int width, int shiftLevels, double * corrFunct);
int computePadSize(int width,int padding);

rgbPixel computeAvg(int SizeRho,int SizeTheta, int padding, unsigned char * image);
int Shift_and_Corr (unsigned char * Left, unsigned char * Right, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, rgbPixel aL, rgbPixel aR);
void shiftnCorrFovea (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, rgbPixel aL, rgbPixel aR, int Rows, int * count);
void shiftnCorrFoveaRGB (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, double *phase, double *coeff, rgbPixel aL, rgbPixel aR, int Rows, int * count);
void shiftnCorrFoveaNoAverage (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, double *std1, double *std2, int Rows, int * count);
void shiftnCorrFoveaNoAverageNorm (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr_val, double *std1, double *std2, int Rows, int * count);
void shiftSSDRGB (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count);
void shiftSSD (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count);
void shiftSSDWorstCase (unsigned char * fullImg, unsigned char * fovImg, Image_Data * Par, int Steps, int * ShiftMap, double * corr, int Rows, int * count);
void sawt2Uniform(unsigned char * outImage, unsigned char * inImage, Image_Data * par, unsigned short * padMap);
void uniform2Sawt(unsigned char * outImage, unsigned char * inImage, Image_Data * par, unsigned short * padMap);

// Functions defined in TableGeneration.cpp
unsigned short Build_Tables (Image_Data * Param, LUT_Ptrs * Tables,char * Path, unsigned short List);
int Build_Ang_Shift_Map (Image_Data * Par, char * Path);
int Build_Cart2LP_Map(Image_Data * Par, char * Path);
int Build_Color_Map (Image_Data * Par, char * Path);
int Build_Pad_Map (Image_Data * Par, char * Path);
int Build_XY_Map (Image_Data * Par, char * Path);
int Build_Neighborhood_Map (Image_Data * Par, char * Path);
int Build_Remap_Map (Image_Data * Par, char * Path);
int Build_Cart2LP_Map (Image_Data * Par, char * Path);
int Build_Weights_Map(Image_Data * Par, char * Path);
int Build_DS_Map(Image_Data * LParam,char * Path, float Ratio);
int Build_Shift_Map(Image_Data * Par, char * Path);
int Build_Shift_Map_Fovea(Image_Data * Par, char * Path);
void Build_Step_Function(char * Path, Image_Data * Par);

#endif
