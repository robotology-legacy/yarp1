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
/// $Id: LogPolarSDK.h,v 1.9 2003-09-24 11:03:31 fberton Exp $
///
///

#ifndef ___LOGPOLARSDK___
#define ___LOGPOLARSDK___


#define Giotto1 0
#define Giotto2 1
#define CUSTOM 20
#define FITIN   99
#define FITOUT 101

#define MONO   0
#define COLOR  1

#define DOWNSAMPLE  1
#define SIMULATE 0

#define ALLMAPS  255

#define RED   0
#define GREEN 1
#define BLUE  2

#define MAX_PIX 10

#define PI 3.1415926535897932384626433832795

struct Image_Data{

	// Logarithm Index
	double Log_Index;
	bool Valid_Log_Index;

	// Zoom Level of the Remapped Image
	double Zoom_Level;

	// Ratio between the diameter of the image and the size of the smallest pixel
	int Resolution;

	int Fovea_Display_Mode; //0 Sawtooth (Raw); 1 Triangular; 2 Complete

	// Log Polar Metrics
	int Size_Rho;
	int Size_Theta;
	int Size_Fovea;
	int Size_LP;
	int Fovea_Type; //0->3 Giotto 2.0; 4->7 Giotto 2.1 //0;4 Sawtooth (Raw); 1;5 Triangular; 2;6 Complete
	int Pix_Numb;

	// Remapped Cartesian Metrics
	int Size_X_Remap;
	int Size_Y_Remap;
	int Center_X_Remap;
	int Center_Y_Remap;
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
};

struct Neighborhood{
	unsigned short position;
	float weight;
};

struct IntNeighborhood{
	unsigned short NofPixels;
	unsigned short * position;
	unsigned char * weight;
};

struct Cart2LPInterp{
	unsigned char NofPixels;
	unsigned int * position;
};

struct LUT_Ptrs{
	double			* AngShiftMap;
	char			* ColorMap;
	unsigned short	* DownSampleMap;
	Neighborhood	* NeighborMap;
	unsigned short	* PadMap;
	int				* RemapMap;
	Neighborhood	* WeightsMap;
	double			* XYMap;
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

int Camera_Init(int width,
				int height,
				char * dev_name,
				int address,
				int format,
				int format_flags,
				int *vdesc);

int Giotto_Init(int address);

int Giotto_Init_Wrapper(int width,
						int height,
						char * dev_name,
						int address,
						int format,
						int format_flags,
						int *vdesc);

int Camera_Acq(unsigned char * image,
			   struct timeval *acq_time,
			   int *time_stamp,
			   int *vdesc,
			   int address);

int Giotto_Acq(unsigned char * image,
			   int width,
			   int height,			   
			   int address);

int Giotto_Acq_Wrapper(unsigned char * image,
					   struct timeval *acq_time,
					   int *time_stamp,
					   int *vdesc,
					   int address);

void Giotto_Close();
void Giotto_Close_Wrapper(int *vdesc);

unsigned char * Read_Bitmap(
				 int *X_Size,
				 int *Y_Size,
				 int *planes,
				 char * filename);

void Save_Bitmap(unsigned char *image,
				 int X_Size,
				 int Y_Size,
				 int planes,
				 char * filename);

int Build_Ang_Shift_Map (Image_Data * Parameters,char * Path);

int Build_Color_Map (char * Path);

int Build_Pad_Map (Image_Data * Parameters,char * Path);

int Build_XY_Map (Image_Data * Parameters,
		 		   char * Path);


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

int Build_Neighborhood_Map(Image_Data * Par,
							char * Path);

int Build_Neighborhood_Map_NoFov(Image_Data * Par,
							char * Path);

unsigned char Build_Weights_Map(Image_Data * Par,
								char * Path);

IntNeighborhood * Build_Fast_Weights_Map(Image_Data * Par,
								char * Path);

unsigned char Build_Weights_Map_NoFov(Image_Data * Par,
								char * Path);

int Build_Remap_Map (Image_Data * Parameters,
					  char * Path);

int Build_Remap_Map_No_Fov (Image_Data * Parameters,
					  char * Path);

int Crop_Remap_Map(Image_Data *Par, char* Path);

void Remap(unsigned char * Out_Image,
		   unsigned char * In_Image,
		   Image_Data * Par,
		   int * Rem_LUT);

long Get_Time();

double Compute_Index(int Resolution, int Fovea, int SizeRho);

int Build_DownSample_Map (Image_Data * Parameters, char * Path);

void RGB_2_YUV(unsigned char * OutImageYUV,
			   unsigned char * InImageRGB,
			   int Size_Rho,
			   int Size_Theta);

void YUV_2_RGB(unsigned char * InImageYUV,
						unsigned char * OutImageRGB,
						Image_Data * Par);

void Simulate_Camera(unsigned char * InCartImage,
				unsigned char * OutLPImage,
				Image_Data * Par,
				char * Path,
				int flag);

void Cart2LPReal(unsigned char *in_image, 
				 unsigned char *out_image,
				 char * Path,
				 Image_Data *Par);

char * Load_Color_Map (Image_Data * Param, char * Path);

Neighborhood * Load_Weights_Map (int PixNumb, Image_Data * Param, char * Path);

double *  TestLoad(void * testptr);
unsigned char Load_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned char List);

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

int Get_XY_Center(double *xx, double *yy, int rho, int theta, Image_Data *par, double *Ang_Shift);

unsigned char Build_Tables(Image_Data * Param, LUT_Ptrs * Tables,char * Path,unsigned char List);

int Build_Cart2LP_Map(Image_Data * Par,
		 			  char * Path);

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

#endif
