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
///                            #bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id:
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ipl/ipl.h>

#include <yarp/YARPColorConverter.h>
#include "ImgAtt.h"

#define DBGPF1 if (0)


YARPImgAtt::YARPImgAtt(int x, int y, int fovea, int num):
	rain(x, y, x+x%8, 13)
{
	int i;

	width=x;
	height=y;
	fovHeight=fovea;
	
	int	sobel0[]={1,2,1,
		          0,0,0,
				  -1,-2,-1};

	int	sobel1[]={2,1,0,
		          1,0,-1,
				  0,-1,-2};

	int	sobel2[]={1,0,-1,
		          2,0,-2,
				  1,0,-1};
	
	int	sobel3[]={0,-1,-2,
		          1,0,-1,
				  2,1,0};

	int	sobel4[]={-1,-2,-1,
		          0,0,0,
				  1,2,1};

	int	sobel5[]={-2,-1,0,
		          -1,0,1,
				  0,1,2};

	int	sobel6[]={-1,0,1,
		          -2,0,2,
				  -1,0,1};
	
	int	sobel7[]={0,1,2,
		          -1,0,1,
				  -2,-1,0};
	
	/*int or[]=	{2,2,2,
			 0,0,0,
			 -2,-2,-2,
			 
			 -2,-2,0,
			 -2,0,2,
			 0,2,2,
			 
			 -2,0,2,
			 -2,0,2,
			 -2,0,2,
			 
			 0,-2,-2,
			 2,0,-2,
			 2,2,0};*/

	int p_d1[9]={-1,-1,0,-1,0,1,0,1,1}; // è const???
	int p_d2[9]={0,-1,-1,1,0,-1,1,1,0}; // è const???
	
	int p_o_1[3]={1,1,1}; // è const???
	int p_o_2[3]={1,0,-1}; // è const???

	// nota: uso 2^11 perchè così il gain della maschera equivalente quadrata
	// è 2^22, 22+8<32
	
	// round(fspecial('gaussian',[5*1,1],1)*2^16)
	//int g_c[5]={3571,16004,26386,16004,3571}; // sigma di 1 e 2^16
	
	//int g_c[5]={1785,8002,13193,8002,1785}; // sigma di 1 e 2^15

    //int g_c[5]={14,63,103,63,14}; // sigma di 1 e 2^8 ok

	//int g_c[5]={28,125,206,125,28}; // sigma di 1 e 2^9 ok

	//int g_c[5]={56,250,412,250,56}; // sigma di 1 e 2^10 ok

	//int g_c[5]={112,500,825,500,112}; // sigma di 1 e 2^11 ok
	
	//round(fspecial('gaussian',[5*1,1],1)*2^11/sqrt(1.5))
	int g_c[5]={91, 408, 673, 408, 91};
	  
	//int g_c[5]={223,1000,1649,1000,223}; // sigma di 1 e 2^12 no
		
	// round(fspecial('gaussian',[5*3,1],3)*2^16)
	//int g_s[15]={580,1194,2200,3626,5350,7063,8344,8821,8344,7063,5350,3626,2200,1194,580}; // sigma di 3

	// round(fspecial('gaussian',[5*3,1],3)*2^11)
	int g_s[15]={18, 37, 69, 113, 167, 221, 261, 276, 261, 221, 167, 113, 69, 37, 18}; // sigma di 3
	
	// round(fspecial('gaussian',[5*3,1],3)*2^11/sqrt(1.5))
    //int g_s[15]={15, 30, 56, 93, 137, 180, 213, 225, 213, 180, 137, 93, 56, 30, 15};

	prewitt_diag1.Resize(1);
	prewitt_diag2.Resize(1);
	//prewitt.Resize(4);
	
	sobel.Resize(8);

	gauss_s.Resize(2);
	gauss_c_s.Resize(2);
	gauss_s_s.Resize(2);
	
	prewitt_diag1.Set(3,3,1,1,p_d1,0,0);
	prewitt_diag2.Set(3,3,1,1,p_d2,0,0);

	/*prewitt.Set(3,3,1,1,p,0,0);
	prewitt.Set(3,3,1,1,p,0,1);
	prewitt.Set(3,3,1,1,p,0,2);
	prewitt.Set(3,3,1,1,p,0,3);*/

	// the sum of the positive and negative coefficients of sobel is a power of 2
	sobel.Set(3,3,1,1,sobel0,2,0);
	sobel.Set(3,3,1,1,sobel1,2,1);
	sobel.Set(3,3,1,1,sobel2,2,2);
	sobel.Set(3,3,1,1,sobel3,2,3);
	sobel.Set(3,3,1,1,sobel4,2,4);
	sobel.Set(3,3,1,1,sobel5,2,5);
	sobel.Set(3,3,1,1,sobel6,2,6);
	sobel.Set(3,3,1,1,sobel7,2,7);

	gauss_s.SetGaussianRow(2,1,1,11,0);
	gauss_s.SetGaussianCol(2,1,1,11,1);
	//gauss_s.InitFixBorder();
	
	//gauss_c_s.Set(5,1,2,0,g_c,11,0);
	//gauss_c_s.Set(1,5,0,2,g_c,11,1);
	gauss_c_s.SetGaussianRow(2,1,1/sqrt(1.5),11,0);
	gauss_c_s.SetGaussianCol(2,1,1/sqrt(1.5),11,1);
	gauss_c_s.InitFixBorder();

	//gauss_s_s.Set(15,1,7,0,g_s,11,0);
	//gauss_s_s.Set(1,15,0,7,g_s,11,1);
	gauss_s_s.SetGaussianRow(7,3,1,11,0);
	gauss_s_s.SetGaussianCol(7,3,1,11,1);
	gauss_s_s.InitFixBorder();
	
	//colored.Resize(_stheta, _srho);
	is1.Resize(x, y);

	r1.Resize(x, y);
	b1.Resize(x, y);
	g1.Resize(x, y);
	i1.Resize(x, y);
	r2.Resize(x, y);
	b2.Resize(x, y);
	g2.Resize(x, y);

	// the images are created by default with (IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0)
	//iplSetBorderMode((IplImage *)r1, IPL_BORDER_WRAP, IPL_SIDE_ALL, 0);
	//iplSetBorderMode((IplImage *)r1, IPL_BORDER_CONSTANT, IPL_SIDE_TOP, 0);
	//iplSetBorderMode((IplImage *)r1, IPL_BORDER_CONSTANT, IPL_SIDE_BOTTOM, 0);
	((IplImage *)r1)->BorderMode[2]=3;
	((IplImage *)r1)->BorderMode[3]=3;
	((IplImage *)b1)->BorderMode[2]=3;
	((IplImage *)b1)->BorderMode[3]=3;
	((IplImage *)g1)->BorderMode[2]=3;
	((IplImage *)g1)->BorderMode[3]=3;
	((IplImage *)is1)->BorderMode[2]=3;
	((IplImage *)is1)->BorderMode[3]=3;

	r1_g_c.Resize(x, y);
	b1_g_c.Resize(x, y);
	g1_g_c.Resize(x, y);
	is1_g_c.Resize(x, y);
	r1_g_s.Resize(x, y);
	g1_g_s.Resize(x, y);
	y1_g_s.Resize(x, y);
	is1_g_s.Resize(x, y);
	
	for (i=0; i<4; i++)
		or[i].Resize(x, y);

	/*for (i=0; i<12; i++)
		ors[i].Resize(x, y);*/

	/*or_r[0].Resize(x, y);
	or_r[1].Resize(x, y);
	or_r[2].Resize(x, y);
	or_r[3].Resize(x, y);*/

	edge.Resize(x, y);
	//edge2.Resize(x, y);

	rg.Resize(x, y);
	gr.Resize(x, y);
	by.Resize(x, y);
	//ii.Resize(x, y);
	
	rgs.Resize(x, y);
	grs.Resize(x, y);
	bys.Resize(x, y);
	//iis.Resize(x, y);

	rgf.Resize(x, y);
	grf.Resize(x, y);
	byf.Resize(x, y);
	//iif.Resize(x, y);
	((IplImage *)rg)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)rg)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)rg)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)rg)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)gr)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)gr)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)gr)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)gr)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)by)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)by)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)by)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)by)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	/*((IplImage *)ii)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)ii)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;*/

	((IplImage *)rgs)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)rgs)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)rgs)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)rgs)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)grs)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)grs)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)grs)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)grs)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)bys)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)bys)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)bys)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)bys)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	/*((IplImage *)iis)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)iis)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)iis)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)iis)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;*/

	((IplImage *)edge)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)edge)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)edge)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)edge)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;

	// tmp vars are not used in convolutions?
	tmp1.Resize(x, y);
	tmp2.Resize(x, y);
	tmp3.Resize(x, y);
	tmp4.Resize(x, y);
	/*tmp5.Resize(x, y);
	tmp6.Resize(x, y);
	tmp7.Resize(x, y);
	tmp8.Resize(x, y);
	tmp9.Resize(x, y);
	tmp10.Resize(x, y);
	tmp11.Resize(x, y);
	tmp12.Resize(x, y);
	tmp13.Resize(x, y);
	tmp14.Resize(x, y);
	tmp15.Resize(x, y);
	tmp16.Resize(x, y);*/

	oldWshed.Resize(x, y);
	
	tmp1s.Resize(x, y);
	tmp2s.Resize(x, y);

	meanOppCol.Resize(x, y);
	meanCol.Resize(x, y);
	((IplImage *)meanCol)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)meanCol)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)meanCol)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)meanCol)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;

	
	tmpBGR1.Resize(x, y);
	//tmpBGR2.Resize(x, y);
	((IplImage *)tmpBGR1)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)tmpBGR1)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)tmpBGR1)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)tmpBGR1)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;

	//comb.Resize(x, y);
	out.Resize(x, y);

	blobFov.Resize(x, y);
	
	//out_colored.Resize(_stheta, _srho);
	//temp_16.Resize(_stheta, _srho, -2);

	/*array1[0]=&rg;
	array1[1]=&gr;
	array1[2]=&by;
	array1[3]=&ii;*/
	/*array1[4]=&or_r[0];
	array1[5]=&or_r[1];
	array1[6]=&or_r[2];
	array1[7]=&or_r[3];*/

	array2[0]=&or[0];
	array2[1]=&or[1];
	array2[2]=&or[2];

	/*for (i=0; i<12; i++)
		array3[i]=&ors[i];*/

	lut.key = new int [range+1];
	lut.value = new int [range];
	lut.factor = new int [range];

	for (i=0; i<=range; i++)
		lut.key[i] = i;

	int temp=0;

	scala=iplCreateConvKernel(1, 1, 0, 0, &temp, 9);

	map[0].Resize(x, y);
	map[1].Resize(x, y);
	map[2].Resize(x, y);
	map[3].Resize(x, y);
	CreateOrMap(map);
	/*ACE_OS::sprintf(savename, "./map0.ppm\0");
	YARPImageFile::Write(savename, map[0]);
	ACE_OS::sprintf(savename, "./map1.ppm\0");
	YARPImageFile::Write(savename, map[1]);
	ACE_OS::sprintf(savename, "./map2.ppm\0");
	YARPImageFile::Write(savename, map[2]);
	ACE_OS::sprintf(savename, "./map3.ppm\0");
	YARPImageFile::Write(savename, map[3]);*/

	//colorVQ.Resize(x, y, fovea);
	imgVQ.Resize(x, y);

	//blob.resize(x, y, fovea);
	/*integralRG.resize(x, y, fovea);
	integralGR.resize(x, y, fovea);
	integralBY.resize(x, y, fovea);*/

	tagged.Resize(x+x%8, y);

	//rain.resize(x, y, x+x%8, 15);
	//rain.resize(x, y, x+x%8, 13);

	blobList = new bool [x*y+1];

	searchRG=searchGR=searchBY=0;
	cmp=ect=0;
	salienceBU=1.0;
	salienceTD=0;

	IORBoxes = new YARPBox[num];
	num_IORBoxes=num;

	max_boxes = new YARPBox[3];
}


YARPImgAtt::~YARPImgAtt()
{
	delete [] lut.factor;
	delete [] lut.value;
	delete [] lut.key;

	delete [] blobList;
	delete [] IORBoxes;
	delete [] max_boxes;
}


void YARPImgAtt::LineMax(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst)
{
	for (int y=0; y<fovHeight; y++)
		for (int x=0; x<width; x++)
			dst(x,y)=src(x,y)*2.5; //con soglia 4
			//dst(x,y)=src(x,y)*3.5; //con soglia 6

	for (y=fovHeight; y<height; y++)
		for (int x=0; x<width; x++)
			dst(x,y)=src(x,y)*(3.51773*exp(-0.00832991*y)); //con soglia 4
			//dst(x,y)=src(x,y)*(5.58286*exp(-0.011389*y)); //con soglia 6
}


void YARPImgAtt::lineMax2(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst)
{
	for (int y=0; y<height; y++) {		
		YarpPixelMono max=0;
		YarpPixelMono min=255;

		for (int x=0; x<width; x++)	{
			if (src(x,y)>max) max=src(x,y);
			else if (src(x,y)<min) min=src(x,y);
		
		}
		if (max!=min)
			for (x=0; x<width; x++)	{
				dst(x,y)=(YarpPixelMono)((int)src(x,y)-min)*255/(max-min);
			}
	}
}


void YARPImgAtt::LineStat(YARPImageOf<YarpPixelMono> &src, int *vett)
{
	if (vett!=NULL) {
		int *num = new int [height];
			
		memset(num,0,sizeof(int)*height);
		memset(vett,0,sizeof(int)*height);

		for (int y=0; y<height; y++)
			for (int x=0; x<width; x++)
				if (src(x,y)>vett[y])
					vett[y]=src(x,y);
				/*if (src(x,y)!=0) {
					vett[y]+=src(x,y);
					num[y]++;
				}*/

		/*for (y=0; y<height; y++)
			if (num[y]!=0) vett[y]/=num[y];*/

		delete [] num;
	}
}


// Mean with num sources images
// Can I use IPL for this??
void YARPImgAtt::Combine(YARPImageOf<YarpPixelMono> **src, int num, YARPImageOf<YarpPixelMono> &dst)
{
	//int temp=0;
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++)	{
			int temp=0;
			for (int n=0; n<num; n++) {
				temp+=src[n]->Pixel(x,y);
			}
			temp/=n;
			dst.Pixel(x,y)=temp;
		}
	}
}


// Can I use IPL for this??
void YARPImgAtt::CombineMax(YARPImageOf<YarpPixelMono> **src, int num, YARPImageOf<YarpPixelMono> &dst)
{
	int i=0;
	const int padding = 4;

	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			//YarpPixelMono temp=src[0]->Pixel(x,y); //slower!
			int temp=src[0]->GetRawBuffer()[i];
			for (int n=1; n<num; n++) {
				if (temp<src[n]->GetRawBuffer()[i])
					temp=src[n]->GetRawBuffer()[i];
			}
			dst.GetRawBuffer()[i]=temp;
			i++;
		}
		i+=padding;
	}

	/*for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++)	{
			//YarpPixelMono temp=src[0]->Pixel(x,y); //slower!
			int temp=src[0]->Pixel(x,y);
			for (int n=1; n<num; n++) {
				if (temp<src[n]->Pixel(x,y))
					temp=src[n]->Pixel(x,y);
			}
			dst.Pixel(x,y)=temp;
		}
	}*/
}


// Can I use IPL for this??
void YARPImgAtt::CombineMaxAbs(YARPImageOf<YarpPixelMonoSigned> **src, int num, YARPImageOf<YarpPixelMono> &dst)
{
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++)	{
			//YarpPixelMono temp=src[0]->Pixel(x,y); //slower!
			int temp=src[0]->Pixel(x,y);
			for (int n=1; n<num; n++) {
				if (temp<abs(src[n]->Pixel(x,y)))
					temp=abs(src[n]->Pixel(x,y));
			}
			dst.Pixel(x,y)=temp;
		}
	}
}


void YARPImgAtt::ZeroLow(YARPImageOf<YarpPixelMono> &src, unsigned char th)
{
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++)	{
			if (src(x,y)<th) src(x,y)=0;
		}
	}
}


void YARPImgAtt::ZeroFovea(YARPGenericImage &dst, int val)
{
	memset(dst.GetRawBuffer(), val, fovHeight*((IplImage*)dst)->widthStep);
}


void YARPImgAtt::FullRange2(IplImage* img, IplImage* out)
{
	IplLUT* plut = &lut;
	int i, mn, mx;

	lut.num=range+1;
	lut.interpolateType = IPL_LUT_INTER; /// interpolation mode, not lookup
		
	/// compute histogram and find min and max values
	for (i=0; i<=range; i++)
		lut.key[i] = i;
	iplComputeHisto(img, &plut);
	mn = 0; while ( !lut.value[mn] ) mn++;
	mx = 255; while ( !lut.value[mx] ) mx--;
	
	/// prepare LUT for stretching
	lut.interpolateType = IPL_LUT_INTER; /// interpolation mode, not lookup
	lut.num = 2; /// num of key values
	lut.key[0] = 0; /// lower value
	lut.key[1] = 255; /// upper value
	lut.factor[0] = 255 / (mx - mn); /// factor to extend range
	lut.value[0] = -lut.factor[0] * mn; /// value to shift
	
	/// The operation is: x(i) = x(i) * factor + value
	iplContrastStretch(img, out, &plut);
	
	/// compute histogram and find min and max values again
	lut.num = 257;
	lut.key[1] = 1;
	iplComputeHisto(out, &plut);
	mn = 0; while ( !lut.value[mn] ) mn++;
	mx = 255; while ( !lut.value[mx] ) mx--;
}


void YARPImgAtt::MinMax(IplImage* img, int &mn, int &mx)
{
	IplLUT* plut = &lut;

	lut.num=range+1;
	lut.interpolateType = IPL_LUT_INTER; /// interpolation mode, not lookup
		
	/// compute histogram and find min and max values
	iplComputeHisto(img, &plut);
	mn = 0; while ( !lut.value[mn] ) mn++;
	mx = 255; while ( !lut.value[mx] ) mx--;
}


// potrei anche usare un semplice ciclo for con moltiplicazioni intere e shift
void YARPImgAtt::FullRange(IplImage* img, IplImage* out, const int mn, const int mx)
{
	int a;

	if (mx!=mn) {
		a = (int)((long)255 * (1<<9) / (mx - mn)); /// factor to extend range
	
		*(scala->values)=a;
		
		iplSubtractS(img, out, mn, false);
		
		iplConvolve2D(out, out, &scala, 1, IPL_SUM);
		
		/// compute histogram and find min and max values again
		/*lut.num = 257;
		lut.key[1] = 1;
		iplComputeHisto(out, &plut);
		mn = 0; while ( !lut.value[mn] ) mn++;
		mx = 255; while ( !lut.value[mx] ) mx--;*/
	}
}


void YARPImgAtt::MultiplyFloat(YARPImageOf<YarpPixelInt> &src, YARPImageOf<YarpPixelInt> &dst, const float c)
{
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
			dst(x,y)=src(x,y)*c;
}


float YARPImgAtt::DenteDiSega(short x)
{
	float y=x%(width/2);
	y=y>width/4.-0.5?(-8*y+3*width-4)/width:(8*y-width+4)/width;
	if (y<0) y=0;
	return y;
}


inline void YARPImgAtt::WTA2(short *p)
{
	int n,idx=0;

	for (n=0; n<4; n++)
		if (p[n]>p[idx]) idx=n;

	for (n=0; n<4; n++)
		if (n!=idx) p[n]=0;
}


inline void YARPImgAtt::WTA(YARPImageOf<YarpPixelMono> src[4])
{
	int n,idx;

	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++)	{
			idx=0;
			for (n=1; n<4; n++) {
				if (src[n].Pixel(x,y)>src[idx].Pixel(x,y)) idx=n;
			}

			for (n=0; n<4; n++)
				if (n!=idx) src[n].Pixel(x,y)=0;
		}
	}
}


void YARPImgAtt::RealOrientations2(YARPImageOf<YarpPixelMono> srcMap[4], YARPImageOf<YarpPixelMono> dstMap[4])
//void YARPImgAtt::RealOrientations(YARPImageOf<YarpPixelMono> in[4], YARPImageOf<YarpPixelMono> out[4])
{
	int x,y;

	/*IplImage *srcMap[4];
	IplImage *dstMap[4];

	srcMap[0]=(IplImage *)in[0];
	srcMap[1]=(IplImage *)in[1];
	srcMap[2]=(IplImage *)in[2];
	srcMap[3]=(IplImage *)in[3];

	dstMap[0]=(IplImage *)out[0];
	dstMap[1]=(IplImage *)out[1];
	dstMap[2]=(IplImage *)out[2];
	dstMap[3]=(IplImage *)out[3];*/

	/*for (y=0; y<srcMap[0]->height; y++)
		for (x=0; x<width; x++) {
			short somma[4]={0,0,0,0};
			for (n=0;n<4;n++)
				for (m=0;m<4;m++)
					somma[n]+=abs(DenteDiSega(x+(m-n)*width/8+width/2,width)*((short*)srcMap[m]->imageData)[srcMap[0]->widthStep/2*y+x]);
			WTA(somma);
			for (n=0;n<4;n++)
				((short*)dstMap[n]->imageData)[dstMap[0]->widthStep/2*y+x]=somma[n];
		}
	*/
	
	// lo metto nel costruttore?
	const float wi8=width/8.;
		
	for (x=0; x<width; x++) {
		int co=x+width/2;
		for (y=0; y<height; y++) {
			short somma[4];
			
			// posso vedere solo le somme che servono e dividere
			// il for su x in 8
			float pa,pb,pc,pd;

			pa=DenteDiSega(co+(0-0)*wi8);
			pb=DenteDiSega(co+(1-0)*wi8);
			pc=DenteDiSega(co+(2-0)*wi8);
			pd=DenteDiSega(co+(3-0)*wi8);

			somma[0]=pa*srcMap[0].Pixel(x,y); //0
			somma[0]+=pb*srcMap[1].Pixel(x,y); //1
			somma[0]+=pc*srcMap[2].Pixel(x,y); //2
			somma[0]+=pd*srcMap[3].Pixel(x,y); //3
			
			somma[1]=pd*srcMap[0].Pixel(x,y); //-1 3
			somma[1]+=pa*srcMap[1].Pixel(x,y); //0
			somma[1]+=pb*srcMap[2].Pixel(x,y); //1
			somma[1]+=pc*srcMap[3].Pixel(x,y); //2
			
			somma[2]=pc*srcMap[0].Pixel(x,y); //-2 2
			somma[2]+=pd*srcMap[1].Pixel(x,y); //-1
			somma[2]+=pa*srcMap[2].Pixel(x,y); //0
			somma[2]+=pb*srcMap[3].Pixel(x,y); //1
			
			somma[3]=pb*srcMap[0].Pixel(x,y); //-3 1
			somma[3]+=pc*srcMap[1].Pixel(x,y); //-2
			somma[3]+=pd*srcMap[2].Pixel(x,y); //-1
			somma[3]+=pa*srcMap[3].Pixel(x,y); //0

			/*somma[0]=DenteDiSega(co+(0-0)*wi8)*srcMap[0].Pixel(x,y); //0
			somma[0]+=DenteDiSega(co+(1-0)*wi8)*srcMap[1].Pixel(x,y); //1
			somma[0]+=DenteDiSega(co+(2-0)*wi8)*srcMap[2].Pixel(x,y); //2
			somma[0]+=DenteDiSega(co+(3-0)*wi8)*srcMap[3].Pixel(x,y); //3
			
			somma[1]=DenteDiSega(co+(0-1)*wi8)*srcMap[0].Pixel(x,y); //-1 3
			somma[1]+=DenteDiSega(co+(1-1)*wi8)*srcMap[1].Pixel(x,y); //0
			somma[1]+=DenteDiSega(co+(2-1)*wi8)*srcMap[2].Pixel(x,y); //1
			somma[1]+=DenteDiSega(co+(3-1)*wi8)*srcMap[3].Pixel(x,y); //2
			
			somma[2]=DenteDiSega(co+(0-2)*wi8)*srcMap[0].Pixel(x,y); //-2 2
			somma[2]+=DenteDiSega(co+(1-2)*wi8)*srcMap[1].Pixel(x,y); //-1
			somma[2]+=DenteDiSega(co+(2-2)*wi8)*srcMap[2].Pixel(x,y); //0
			somma[2]+=DenteDiSega(co+(3-2)*wi8)*srcMap[3].Pixel(x,y); //1
			
			somma[3]=DenteDiSega(co+(0-3)*wi8)*srcMap[0].Pixel(x,y); //-3 1
			somma[3]+=DenteDiSega(co+(1-3)*wi8)*srcMap[1].Pixel(x,y); //-2
			somma[3]+=DenteDiSega(co+(2-3)*wi8)*srcMap[2].Pixel(x,y); //-1
			somma[3]+=DenteDiSega(co+(3-3)*wi8)*srcMap[3].Pixel(x,y); //0*/
			
			//WTA(somma);
			// se ci sono due uguali solo uno mantiene il val massimo
			int idx=0;
			if (somma[1]>somma[0]) idx=1;
			if (somma[2]>somma[idx]) idx=2;
			if (somma[3]>somma[idx]) idx=3;

			// meglio inizializzare direttamente a zero le mappe
			/*dstMap[0].Pixel(x,y)=0;
			dstMap[1].Pixel(x,y)=0;
			dstMap[2].Pixel(x,y)=0;
			dstMap[3].Pixel(x,y)=0;*/
			
			dstMap[idx].Pixel(x,y)=somma[idx];

			/*if (0!=idx) somma[0]=0;
			if (1!=idx) somma[1]=0;
			if (2!=idx) somma[2]=0;
			if (3!=idx) somma[3]=0;
			
			dstMap[0].Pixel(x,y)=somma[0];
			dstMap[1].Pixel(x,y)=somma[1];
			dstMap[2].Pixel(x,y)=somma[2];
			dstMap[3].Pixel(x,y)=somma[3];*/
		}
	}
	
	
	//somma[n]+=abs(peso*srcMap[m].Pixel(x,y));
	//somma[n]+=abs(DenteDiSega(x+(m-n)*width/8+width/2)*((char*)srcMap[m]->imageData)[srcMap[0]->widthStep/2*y+x]);
	//somma[n]+=abs(DenteDiSega(x+(m-n)*width/8+width/2,width)*(srcMap[m].Pixel(x,y)));
	//WTA(somma);
	//for (n=0;n<4;n++)
	//((char*)dstMap[n]->imageData)[dstMap[0]->widthStep/2*y+x]=somma[n];
	//	dstMap[n].Pixel(x,y)=somma[n];
}


void YARPImgAtt::RealOrientations(YARPImageOf<YarpPixelMono> srcMap[4], YARPImageOf<YarpPixelMono> dstMap[4])
{
	// prima mappa
	iplMultiplyScale((IplImage*) srcMap[0], (IplImage*) map[0], (IplImage*) tmp1);
	iplMultiplyScale((IplImage*) srcMap[1], (IplImage*) map[1], (IplImage*) tmp2);
	iplMultiplyScale((IplImage*) srcMap[2], (IplImage*) map[2], (IplImage*) tmp3);
	iplMultiplyScale((IplImage*) srcMap[3], (IplImage*) map[3], (IplImage*) tmp4);

	iplAdd((IplImage*) tmp1, (IplImage*) tmp2, (IplImage*) dstMap[0]);
	iplAdd((IplImage*) dstMap[0], (IplImage*) tmp3, (IplImage*) dstMap[0]);
	iplAdd((IplImage*) dstMap[0], (IplImage*) tmp4, (IplImage*) dstMap[0]);

	// seconda mappa
	iplMultiplyScale((IplImage*) srcMap[0], (IplImage*) map[3], (IplImage*) tmp1);
	iplMultiplyScale((IplImage*) srcMap[1], (IplImage*) map[0], (IplImage*) tmp2);
	iplMultiplyScale((IplImage*) srcMap[2], (IplImage*) map[1], (IplImage*) tmp3);
	iplMultiplyScale((IplImage*) srcMap[3], (IplImage*) map[2], (IplImage*) tmp4);

	iplAdd((IplImage*) tmp1, (IplImage*) tmp2, (IplImage*) dstMap[1]);
	iplAdd((IplImage*) dstMap[1], (IplImage*) tmp3, (IplImage*) dstMap[1]);
	iplAdd((IplImage*) dstMap[1], (IplImage*) tmp4, (IplImage*) dstMap[1]);

	// terza mappa
	iplMultiplyScale((IplImage*) srcMap[0], (IplImage*) map[2], (IplImage*) tmp1);
	iplMultiplyScale((IplImage*) srcMap[1], (IplImage*) map[3], (IplImage*) tmp2);
	iplMultiplyScale((IplImage*) srcMap[2], (IplImage*) map[0], (IplImage*) tmp3);
	iplMultiplyScale((IplImage*) srcMap[3], (IplImage*) map[1], (IplImage*) tmp4);

	iplAdd((IplImage*) tmp1, (IplImage*) tmp2, (IplImage*) dstMap[2]);
	iplAdd((IplImage*) dstMap[2], (IplImage*) tmp3, (IplImage*) dstMap[2]);
	iplAdd((IplImage*) dstMap[2], (IplImage*) tmp4, (IplImage*) dstMap[2]);

	//quarta mappa
	iplMultiplyScale((IplImage*) srcMap[0], (IplImage*) map[1], (IplImage*) tmp1);
	iplMultiplyScale((IplImage*) srcMap[1], (IplImage*) map[2], (IplImage*) tmp2);
	iplMultiplyScale((IplImage*) srcMap[2], (IplImage*) map[3], (IplImage*) tmp3);
	iplMultiplyScale((IplImage*) srcMap[3], (IplImage*) map[0], (IplImage*) tmp4);

	iplAdd((IplImage*) tmp1, (IplImage*) tmp2, (IplImage*) dstMap[3]);
	iplAdd((IplImage*) dstMap[3], (IplImage*) tmp3, (IplImage*) dstMap[3]);
	iplAdd((IplImage*) dstMap[3], (IplImage*) tmp4, (IplImage*) dstMap[3]);
}


void YARPImgAtt::CreateOrMap(YARPImageOf<YarpPixelMono> maps[4])
{
	int x,y;
	
	// lo metto nel costruttore?
	const float wi8=width/8.;
		
	for (x=0; x<width; x++) {
		int co=x+width/2;
		for (y=0; y<height; y++) {
			// posso vedere solo le somme che servono e dividere
			// il for su x in 8

			maps[0].Pixel(x,y)=DenteDiSega(co+(0-0)*wi8)*(1<<8);
			maps[1].Pixel(x,y)=DenteDiSega(co+(1-0)*wi8)*(1<<8);
			maps[2].Pixel(x,y)=DenteDiSega(co+(2-0)*wi8)*(1<<8);
			maps[3].Pixel(x,y)=DenteDiSega(co+(3-0)*wi8)*(1<<8);
		}
	}
}


void YARPImgAtt::FindNMax(int num, Vett* pos)
{
	ACE_ASSERT(pos!=NULL);
	ACE_ASSERT(num!=0);
	
	int h=out.GetHeight();
	int w=out.GetWidth();

	memset(pos, 0, sizeof(Vett)*num);
	
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++)	{
			for (int i=0; i<num; i++) {
				if (out.Pixel(x,y)>out.Pixel(pos[i].x, pos[i].y)) {
					for (int j=num-1; j>i; j--)
						pos[j]=pos[j-1];
					pos[i].x=x;
					pos[i].y=y;
					break;
				}
			}
		}
	}
}


void YARPImgAtt::FindMax(YARPImageOf<YarpPixelMono> &src, Vett &pos)
{
	pos.x=0;
	pos.y=0;
	
	int xm=0;
	int ym=0;
	
	int h=src.GetHeight();
	int w=src.GetWidth();

	for (int y=0; y<h; y++)
		for (int x=0; x<w; x++)
			if (src(x,y)>src(xm,ym)) {
				xm=x;
				ym=y;
			}

	pos.x=xm;
	pos.y=ym;
}


void YARPImgAtt::FindMax(YARPImageOf<YarpPixelInt> &src, Vett &pos)
{
	pos.x=0;
	pos.y=0;
	
	int xm=0;
	int ym=0;
	
	int h=src.GetHeight();
	int w=src.GetWidth();

	for (int y=0; y<h; y++)
		for (int x=0; x<w; x++)
			if (src(x,y)>src(xm,ym)) {
				xm=x;
				ym=y;
			}

	pos.x=xm;
	pos.y=ym;
}


void YARPImgAtt::ZeroMask(YARPImageOf<YarpPixelMono> &dst, YARPImageOf<YarpPixelMono> &mask, int val, int xmin, int xmax, int ymin, int ymax)
{
	for (int y=ymin; y<ymax; y++)
		for (int x=xmin; x<xmax; x++)
			if (mask(x,y)==val)
				dst(x,y)=0;
}


void YARPImgAtt::GetTarget(int &x, int &y)
{
	Vett posMax;
	FindMax(out, posMax);
	x=posMax.x;
	y=posMax.y;
}


bool YARPImgAtt::Apply(YARPImageOf<YarpPixelBGR> &src)
{
	int mn;
	int mx;
	bool found=false;
	
	DBGPF1 ACE_OS::printf(">>> get original planes\n");
	//YARPImageUtils::GetAllPlanes(src, b1, g1, r1);
	YARPImageUtils::GetRed(src,r2);
	YARPImageUtils::GetGreen(src,g2);
	YARPImageUtils::GetBlue(src,b2);
	
	iplRShiftS(src, src, 1);
	/*ACE_OS::sprintf(savename, "meancol.ppm");
	YARPImageFile::Write(savename, meanCol);*/
	gauss_s.ConvolveSep2D(meanCol,meanCol);
	/*ACE_OS::sprintf(savename, "meancolg.ppm");
	YARPImageFile::Write(savename, meanCol);*/
	iplRShiftS(meanCol, meanCol, 1);
	iplAdd(src, meanCol, src);
	/*ACE_OS::sprintf(savename, "src.ppm");
	YARPImageFile::Write(savename, src);*/

	//iplLShiftS(src, src, 1);
	
	// dovrei farlo una sola volta nn ogni frame e poi dovrei salvare lo stato
	// precedente del borderMode
	((IplImage *)src)->BorderMode[IPL_SIDE_LEFT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)src)->BorderMode[IPL_SIDE_RIGHT_INDEX]=IPL_BORDER_WRAP;
	((IplImage *)src)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=IPL_BORDER_REPLICATE;
	((IplImage *)src)->BorderMode[IPL_SIDE_TOP_INDEX]=IPL_BORDER_REPLICATE;
	
	DBGPF1 ACE_OS::printf(">>> Filtering the images to remove some noise\n");
	//ACE_OS::sprintf(savename, "./osrc.ppm");
	//YARPImageFile::Write(savename, src);

	// with '3' the segmentation is worse
	//iplRShiftS(src, src, 2);
	//iplLShiftS(src, src, 2);
	
	/*iplColorMedianFilter(src, tmpBGR1, 3, 3, 1, 1); // better in the perifery, worse in fovea
	iplColorMedianFilter(tmpBGR1, src, 3, 3, 1, 1); // better in the perifery, worse in fovea*/
	//src=tmpBGR1;

	
	colorOpponency(src);
	findEdges();
	
	//iplMultiplyScale(oldWshed, edge, oldWshed);
	//iplAdd(edge, oldWshed, edge);

	normalize();
	findBlobs();
	//quantizeColors();
	drawIORTable();

	//rain.maxSalienceBlobs(tagged, max_tag, max_boxes, 3);
	rain.maxSalienceBlob(tagged, max_tag, max_boxes[0]);

	if (salienceTD>0) {
		//cout<<fovBox.cmp;
		//cout<<fovBox.ect;
		int crg=fovBox.meanRG-searchRG;
		int cgr=fovBox.meanGR-searchGR;
		int cby=fovBox.meanBY-searchBY;

		//cout<<"CMP DELTA %:"<<(fovBox.cmp-cmp)/cmp*100;
		//cout<<"  ECT DELTA %:"<<(fovBox.ect-ect)/ect*100<<endl;

		if (crg*crg+cgr*cgr+cby*cby<1000) {
			found=true;
		}
	}	
	
	//blobFov=edge;
	//rain.tags2Watershed(tagged, blobFov);

	//saveImages(src);
	
	/*MinMax(edge, mn, mx);
	FullRange(edge, edge, mn, mx);
	out=edge;*/

	return found;
}


void YARPImgAtt::colorOpponency(YARPImageOf<YarpPixelBGR> &src)
{
	//YARPConvKernelFile::Write("./prewitt.yck\0",prewitt);
	

	DBGPF1 ACE_OS::printf(">>> get planes\n");
	//YARPImageUtils::GetAllPlanes(src, b1, g1, r1);
	YARPImageUtils::GetRed(src,r1);
	YARPImageUtils::GetGreen(src,g1);
	YARPImageUtils::GetBlue(src,b1);
	//YARPColorConverter::RGB2Grayscale(src, i1);


	//DBGPF1 ACE_OS::printf(">>> Conversion intensity in signed char\n");
	//iplRShiftS((IplImage *) i1, (IplImage *) tmp1, 1);
	//memcpy(((IplImage *)is1)->imageData, ((IplImage *)tmp1)->imageData, ((IplImage *)tmp1)->imageSize);


	DBGPF1 ACE_OS::printf(">>> filter image planes\n");
	// potrei convolvere i 3 piani in contemporanea e poi estrarli???
	gauss_c_s.ConvolveSep2D(r1,r1_g_c);
	gauss_c_s.ConvolveSep2D(g1,g1_g_c);
	gauss_c_s.ConvolveSep2D(b1,b1_g_c);
	//gauss_c_s.ConvolveSep2D(is1,is1_g_c);
	/*gauss_s_s.ConvolveSep2D(r1,r1_g_s);
	gauss_s_s.ConvolveSep2D(g1,g1_g_s);*/
	//gauss_s_s.ConvolveSep2D(is1,is1_g_s);

	
	DBGPF1 ACE_OS::printf(">>> RG GR BY\n");
	/*iplSubtract((IplImage *)r1_g_c, (IplImage *)g1_g_s, (IplImage *)rg);
	iplSubtract((IplImage *)g1_g_c, (IplImage *)r1_g_s, (IplImage *)gr);
	iplRShiftS((IplImage *)r1_g_s, (IplImage *) tmp1, 1);
	iplRShiftS((IplImage *)g1_g_s, (IplImage *) tmp2, 1);
	iplAdd((IplImage *) tmp1,(IplImage *) tmp2,(IplImage *) y1_g_s);
	iplSubtract((IplImage *)b1_g_c,(IplImage *)y1_g_s,(IplImage *)by);*/
	
	iplSubtract(r1, g1_g_c, rg);
	iplSubtract(g1, r1_g_c, gr);
	iplRShiftS((IplImage *)r1_g_c, (IplImage *) tmp1, 1);
	iplRShiftS((IplImage *)g1_g_c, (IplImage *) tmp2, 1);
	iplAdd(tmp1, tmp2, y1_g_s);
	iplSubtract((IplImage *)b1,(IplImage *)y1_g_s,(IplImage *)by);
	
	//iplSubtract((IplImage *)is1_g_c,(IplImage *)is1_g_s,(IplImage *)iis);
	// vedendo i risultati sulla sega triangolare si pensa che sia meglio prendere il val assoluto
	// ma c'è il problema dei massimi: alcuni capitano dentro l'oggetto altri fuori
	//iplAbs((IplImage *)iis, (IplImage *)iis);
	//memcpy(((IplImage *)ii)->imageData, ((IplImage *)iis)->imageData, ((IplImage *)ii)->imageSize);
	// this map has only 7 bit!
	//iplLShiftS((IplImage*) ii, (IplImage*) ii, 1);
	//pOldZdi=((IplImage *)ii)->roi;
	//((IplImage *)ii)->roi=&zdi;
	//MinMax((IplImage *)ii, mn[3], mx[3]);
	//((IplImage *)ii)->roi=pOldZdi;
	//FullRange((IplImage *)ii, (IplImage *)ii, mn[3], mx[3]);

	/*DBGPF1 ACE_OS::printf(">>> Conversion color opponent maps in signed char\n");
	iplRShiftS(rg, tmp1, 1);
	memcpy(((IplImage *)rgs)->imageData, ((IplImage *)tmp1)->imageData, ((IplImage *)tmp1)->imageSize);
	//((IplImage *)rgs)->imageData=((IplImage *)tmp1)->imageData;
	iplRShiftS(gr, tmp1, 1);
	memcpy(((IplImage *)grs)->imageData, ((IplImage *)tmp1)->imageData, ((IplImage *)tmp1)->imageSize);
	//((IplImage *)grs)->imageData=((IplImage *)tmp1)->imageData;
	iplRShiftS(by, tmp1, 1);
	memcpy(((IplImage *)bys)->imageData, ((IplImage *)tmp1)->imageData, ((IplImage *)tmp1)->imageSize);
	//((IplImage *)bys)->imageData=((IplImage *)tmp1)->imageData;*/
}


void YARPImgAtt::findEdges()
{
	//YARPConvKernelFile::Write("./prewitt.yck\0",prewitt);
	
	DBGPF1 ACE_OS::printf(">>> edges in 4 orientations\n");
	// dovrei scalare i risulati di 3 o 4 in modo che
	// dopo vengono riscalati tra 0 e 255
	/*iplFixedFilter((IplImage *)iis,(IplImage *)ors[0], IPL_PREWITT_3x3_H);
	prewitt_diag1.Convolve2D(iis,ors[1],1,IPL_SUM);
	iplFixedFilter((IplImage *)iis,(IplImage *)ors[2], IPL_PREWITT_3x3_V);
	prewitt_diag2.Convolve2D(iis,ors[3],1,IPL_SUM);*/
	// I cannot use the combination with IPL_MAX because there are negative values!
	// otherwise I must use 8 filters!
	sobel.Convolve2D(rg, or[0], 8, IPL_MAX);
	sobel.Convolve2D(gr, or[1], 8, IPL_MAX);
	sobel.Convolve2D(by, or[2], 8, IPL_MAX);
	// Note that if I use the max combination the values will be all positive
	CombineMax(array2, 3, edge);
	
	/*sobel.Convolve2D(rgs, ors[0], 0);
	sobel.Convolve2D(rgs, ors[1], 1);
	sobel.Convolve2D(rgs, ors[2], 2);
	sobel.Convolve2D(rgs, ors[3], 3);
	
	sobel.Convolve2D(grs, ors[4], 0);
	sobel.Convolve2D(grs, ors[5], 1);
	sobel.Convolve2D(grs, ors[6], 2);
	sobel.Convolve2D(grs, ors[7], 3);

	sobel.Convolve2D(bys, ors[8], 0);
	sobel.Convolve2D(bys, ors[9], 1);
	sobel.Convolve2D(bys, ors[10], 2);
	sobel.Convolve2D(bys, ors[11], 3);

	CombineMaxAbs(array3, 12, edge);*/
	
	/*gauss_c_s.ConvolveSep2D(rgs,tmp1s);
	gauss_s_s.ConvolveSep2D(rgs,tmp2s);
	iplSubtract((IplImage *)tmp1s, (IplImage *)tmp2s, (IplImage *)ors[0]);

	gauss_c_s.ConvolveSep2D(grs,tmp1s);
	gauss_s_s.ConvolveSep2D(grs,tmp2s);
	iplSubtract((IplImage *)tmp1s, (IplImage *)tmp2s, (IplImage *)ors[1]);
	
	gauss_c_s.ConvolveSep2D(bys,tmp1s);
	gauss_s_s.ConvolveSep2D(bys,tmp2s);
	iplSubtract((IplImage *)tmp1s, (IplImage *)tmp2s, (IplImage *)ors[2]);

	iplAbs((IplImage *) ors[0], (IplImage *) ors[0]);
	iplAbs((IplImage *) ors[1], (IplImage *) ors[1]);
	iplAbs((IplImage *) ors[2], (IplImage *) ors[2]);
	iplAbs((IplImage *) ors[3], (IplImage *) ors[3]);*/

	/*((IplImage *)or[0])->imageData=((IplImage *)ors[0])->imageData;
	((IplImage *)or[1])->imageData=((IplImage *)ors[1])->imageData;
	((IplImage *)or[2])->imageData=((IplImage *)ors[2])->imageData;
	((IplImage *)or[3])->imageData=((IplImage *)ors[3])->imageData;*/
	/*memcpy(((IplImage *)or[0])->imageData, ((IplImage *)ors[0])->imageData, ((IplImage *)ors[0])->imageSize);
	memcpy(((IplImage *)or[1])->imageData, ((IplImage *)ors[1])->imageData, ((IplImage *)ors[1])->imageSize);
	memcpy(((IplImage *)or[2])->imageData, ((IplImage *)ors[2])->imageData, ((IplImage *)ors[2])->imageSize);*/
	//memcpy(((IplImage *)or[3])->imageData, ((IplImage *)ors[3])->imageData, ((IplImage *)ors[3])->imageSize);
	//Combine(array2, 3, edge2);

	/*int *stat = new int [height];
	LineStat(edge, stat);
	LineMax(edge, edge);
	LineStat(edge, stat);*/
	//lineMax2(edge, edge);
	LineMax(edge, edge);
}


void YARPImgAtt::normalize()
{
	int mn[8];
	int mx[8];

	//IplROI zdi;
	//IplROI *pOldZdi;


	DBGPF1 ACE_OS::printf(">>> search min and max\n");
	//pOldZdi=((IplImage *)rg)->roi;
	//((IplImage *)rg)->roi=&zdi;
	//MinMax((IplImage *)rg, mn[0], mx[0]);
	//((IplImage *)rg)->roi=pOldZdi;

	//pOldZdi=((IplImage *)gr)->roi;
	//((IplImage *)gr)->roi=&zdi;
	//MinMax((IplImage *)gr, mn[1], mx[1]);
	//((IplImage *)gr)->roi=pOldZdi;

	//pOldZdi=((IplImage *)by)->roi;
	//((IplImage *)by)->roi=&zdi;
	//MinMax((IplImage *)by, mn[2], mx[2]);
	//((IplImage *)by)->roi=pOldZdi;

	/*pOldZdi=((IplImage *)or_r[0])->roi;
	((IplImage *)or_r[0])->roi=&zdi;
	MinMax((IplImage *)or_r[0], mn[4], mx[4]);
	((IplImage *)or_r[0])->roi=pOldZdi;

	pOldZdi=((IplImage *)or_r[1])->roi;
	((IplImage *)or_r[1])->roi=&zdi;
	MinMax((IplImage *)or_r[1], mn[5], mx[5]);
	((IplImage *)or_r[1])->roi=pOldZdi;

	pOldZdi=((IplImage *)or_r[2])->roi;
	((IplImage *)or_r[2])->roi=&zdi;
	MinMax((IplImage *)or_r[2], mn[6], mx[6]);
	((IplImage *)or_r[2])->roi=pOldZdi;

	pOldZdi=((IplImage *)or_r[3])->roi;
	((IplImage *)or_r[3])->roi=&zdi;
	MinMax((IplImage *)or_r[3], mn[7], mx[7]);
	((IplImage *)or_r[3])->roi=pOldZdi;*/
	
	/*pOldZdi=((IplImage *)or[0])->roi;
	((IplImage *)or[0])->roi=&zdi;
	MinMax((IplImage *)or[0], mn[4], mx[4]);
	((IplImage *)or[0])->roi=pOldZdi;

	pOldZdi=((IplImage *)or[1])->roi;
	((IplImage *)or[1])->roi=&zdi;
	MinMax((IplImage *)or[1], mn[5], mx[5]);
	((IplImage *)or[1])->roi=pOldZdi;

	pOldZdi=((IplImage *)or[2])->roi;
	((IplImage *)or[2])->roi=&zdi;
	MinMax((IplImage *)or[2], mn[6], mx[6]);
	((IplImage *)or[2])->roi=pOldZdi;

	pOldZdi=((IplImage *)or_r[3])->roi;
	((IplImage *)or_r[3])->roi=&zdi;
	MinMax((IplImage *)or_r[3], mn[7], mx[7]);
	((IplImage *)or_r[3])->roi=pOldZdi;*/

	//pOldZdi=((IplImage *)edge)->roi;
	//((IplImage *)edge)->roi=&zdi;
	MinMax((IplImage *)edge, mn[4], mx[4]);
	//((IplImage *)edge)->roi=pOldZdi;

	/*pOldZdi=((IplImage *)edge2)->roi;
	((IplImage *)edge2)->roi=&zdi;
	MinMax((IplImage *)edge2, mn[5], mx[5]);
	((IplImage *)edge2)->roi=pOldZdi;*/

	DBGPF1 ACE_OS::printf(">>> search absolute min and max\n");
	/*for (int i=1; i<3; i++) {
		if (mn[i]<mn[0]) mn[0]=mn[i];
		if (mx[i]>mx[0]) mx[0]=mx[i];
	}*/
	/*for (i=5; i<8; i++) {
		if (mn[i]<mn[4]) mn[4]=mn[i];
		if (mx[i]>mx[4]) mx[4]=mx[i];
	}*/

	// sostituire queste normalizzazioni con la divisione per l'intensità locale?
	// oppure differenza dei logaritmi ma è più lento
	// anche perchè questa normalizzazione cambia la varianza
	DBGPF1 ACE_OS::printf(">>> stretch features map\n");
	//FullRange(rg, rg, mn[0], mx[0]);
	//FullRange(gr, gr, mn[0], mx[0]);
	//FullRange(by, by, mn[0], mx[0]);
	/*FullRange(rg, rg, mn[0], mx[0]);
	FullRange(gr, gr, mn[1], mx[1]);
	FullRange(by, by, mn[2], mx[2]);*/
	/*FullRange((IplImage *)or_r[0], (IplImage *)or_r[0], mn[4], mx[4]);
	FullRange((IplImage *)or_r[1], (IplImage *)or_r[1], mn[4], mx[4]);
	FullRange((IplImage *)or_r[2], (IplImage *)or_r[2], mn[4], mx[4]);
	FullRange((IplImage *)or_r[3], (IplImage *)or_r[3], mn[4], mx[4]);*/
	FullRange((IplImage *)edge, (IplImage *)edge, mn[4], .85*mx[4]);
	//FullRange((IplImage *)edge2, (IplImage *)edge2, mn[5], mx[5]);

	
	//DBGPF1 ACE_OS::printf(">>> combine output images\n");
	// bruttino questo array...
	// si può scrivere direttamente l'array e passarlo alla funzione??
	// forse no, a meno che l'array non abbia una dimensione predefinita
	//Combine(array1, 4, comb);
	//ACE_OS::sprintf(savename, "./out.ppm\0");
	//YARPImageFile::Write(savename, out);
	//FullRange((IplImage *)comb, (IplImage *)comb);

	//iplClose(edge, edge, 1);
}
	

void YARPImgAtt::findBlobs()
{
	//int i;

	//Vett pos2;

	int bfX0, bfY0;
	double bfA11, bfA12, bfA22;

	
	//DBGPF1 ACE_OS::printf(">>> blob detector\n");
	//integral.computeCartesian(comb);
	//blob.filterLp0255(comb);
	//blob.filterLp(comb);
	//out=blob.getSegmented();
	//ACE_OS::sprintf(savename, "./out.ppm");
	//YARPImageFile::Write(savename, out);
	
	/*tmp1=edge;
	//tmp1.Zero();
	//tmp1=ii;
	//blobFinder.DrawBoxesLP(tmp1);
	//tmp2=out;
	//FindNMax(num, pos);
	
	for (i=0; i<num; i++) {
		FindMax(tmp2, pos2);
		pos[i]=pos2;
		//pos[i].x=pos2.x;
		//pos[i].y=pos2.y;
		blobFinder.SeedColor(tmp2, tagged, pos2.x, pos2.y, 0);
		//ZeroMask(tmp2, tmp1, 255-i, 0, width, fovHeight, height);
	}*/
	
	
	//blobFinder.DrawGrayLP(tmp1, tagged, 200);
	//ACE_OS::sprintf(savename, "./tagged.ppm");
	//YARPImageFile::Write(savename, tmp1);


	//rain.setThreshold(5);
	max_tag=rain.apply(edge, tagged);
	rain.blobCatalog(tagged, rg, gr, by, r2, g2, b2, max_tag);

	//ACE_OS::printf("# tags:%d", max_tag);

	//rain.setThreshold(3);
	//rain.applyOnOld(edge, tagged);
	//rain.apply(edge, tagged);
	rain.DrawFoveaBlob(blobFov, tagged);

	//blobFov.Zero();
	//memset(blobFov.GetRawBuffer(), 255, 50*((IplImage*)blobFov)->widthStep);
	//fit.fitEllipse(blobFov, &bfX0, &bfY0, &bfA11, &bfA12, &bfA22);
	//fit.plotEllipse(bfX0, bfY0, bfA11, bfA12, bfA22, blobFov, 127);

	memset(blobList, false, sizeof(bool)*max_tag);
	// - faster
	// - it considers also "lateral" pixels
	// - it doesn't add pixels iteratively
	//rain.findNeighborhood(tagged, 0, 0, blobList, max_tag);
	//rain.fuseFoveaBlob2(tagged, blobList, max_tag);

	// alternative method
	//rain.fuseFoveaBlob(tagged, blobList, max_tag);
	
	/*blobList[1]=false;
	rain.drawBlobList(blobFov, tagged, blobList, max_tag, 127);*/
	/*ACE_OS::sprintf(savename, "./blob_fov2.ppm");
	YARPImageFile::Write(savename, blobFov);*/
	blobList[1]=true;
	rain.statBlobList(tagged, blobList, max_tag, fovBox);
	//rain.removeBlobList(blobList, max_tag);
	rain.removeFoveaBlob(tagged);

	/*int CoMX, CoMY;
	double u00, u11, u20, u02;
	moments.centerOfMassAndMass(blobFov, &CoMX, &CoMY, &u00);
	moments.centralMomentsOrder2(blobFov, CoMX, CoMY, &u11, &u20, &u02);*/
	
	/*moments.centerOfMass(blobFov, &CoMX, &CoMY);
	u00=moments.centralMoments(blobFov, CoMX, CoMY, 0, 0);
	u11=moments.centralMoments(blobFov, CoMX, CoMY, 1, 1);
	u20=moments.centralMoments(blobFov, CoMX, CoMY, 2, 0);
	u02=moments.centralMoments(blobFov, CoMX, CoMY, 0, 2);*/

	/*fovBox.cmp=(u20+u02)/(u00*u00);
	fovBox.ect=((u20-u02)*(u20-u02)+4*u11*u11)/(u00*u00*u00*u00);*/
	//fovBox.cmp=(u20+u02)/(u00*u00);
	//fovBox.ect=sqrt((u20-u02)*(u20-u02)+4*u11*u11)/(u20+u02);

	
	//rain.RemoveNonValid(max_tag, 3800, 100);
	rain.ComputeSalienceAll(max_tag, max_tag);
	rain.RemoveNonValid(max_tag, 6000, 100);

	
	//rain.ComputeSalience(max_tag, max_tag);
	
	//rain.SortAndComputeSalience(200, max_tag);
	//rain.SortAndComputeSalience(100, max_tag);
	//rain.DrawContrastLP(rg, gr, by, tmp1, tagged, max_tag, 0, 1, 30, 42, 45); // somma coeff pos=3 somma coeff neg=-3
	//rain.checkIOR(tagged, IORBoxes, num_IORBoxes);
	rain.doIOR(tagged, IORBoxes, num_IORBoxes);


	rain.DrawContrastLP2(rg, gr, by, out, tagged, max_tag, salienceBU, salienceTD, searchRG, searchGR, searchBY); // somma coeff pos=3 somma coeff neg=-3
	//rain.DrawContrastLP(rg, gr, by, out, tagged, max_tag, salienceBU, salienceTD, searchRG, searchGR, searchBY); // somma coeff pos=3 somma coeff neg=-3
	//pOldZdi=((IplImage *)tmp1)->roi;
	//((IplImage *)tmp1)->roi=&zdi;
	//MinMax((IplImage *)tmp1, mn[0], mx[0]);
	//((IplImage *)tmp1)->roi=pOldZdi;
	//FullRange((IplImage *)tmp1, (IplImage *)out, mn[0], mx[0]);
	//iplThreshold(out, out, 200);
	//ZeroLow(out, 230);

	
	tmpBGR1.Zero();
	rain.ComputeMeanColors(max_tag);
	rain.DrawMeanColorsLP(meanCol, tagged);

	
	meanOppCol.Zero();
	rain.DrawMeanOpponentColorsLP(meanOppCol, tagged);


	/*blobFinder.DrawGrayLP(tmp1, tagged, 200);
	//ACE_OS::sprintf(savename, "./rain.ppm");
	//YARPImageFile::Write(savename, tmp1);*/

	//rain.tags2Watershed(tagged, oldWshed);
}


void YARPImgAtt::drawIORTable()
{
	rain.drawIOR(out, IORBoxes, num_IORBoxes);
}


void YARPImgAtt::quantizeColors()
{
	//colorVQ.DominantQuantization(meanCol, imgVQ, 0.3*255);
	DBGPF1 ACE_OS::printf(">>> color quantization\n");
	imgVQ.Zero();
	rain.DrawVQColor(imgVQ, tagged);
	//colorVQ.Variance(rg, imgVQ, 3);
	//FindMax(imgVQ, pos2);
	//iplSubtractS(imgVQ, imgVQ, , false);
	//MultiplyFloat(imgVQ, imgVQ, 255./imgVQ(pos2.x, pos2.y));
	//iplConvert((IplImage*) imgVQ, (IplImage*) tmp1);*/
	//ACE_OS::sprintf(savename, "./imgvq.ppm");
	//YARPImageFile::Write(savename, tmpBGR1);
}


void YARPImgAtt::resetIORTable()
{
	rain.maxSalienceBlobs(tagged, max_tag, IORBoxes, num_IORBoxes);
}


void YARPImgAtt::updateIORTable()
{
	for (int j=num_IORBoxes-1; j>0; j--)
		IORBoxes[j]=IORBoxes[j-1];

	rain.foveaBlob(tagged, IORBoxes[0]);
	IORBoxes[0].valid=true;
}


void YARPImgAtt::saveImages(YARPImageOf<YarpPixelBGR> &src)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/zgarbage/va/", root); 
	
	ACE_OS::sprintf(savename, "%ssrc.ppm", path);
	YARPImageFile::Write(savename, src);
	
	ACE_OS::sprintf(savename, "%sr.ppm", path);
	YARPImageFile::Write(savename, r1);
	ACE_OS::sprintf(savename, "%sg.ppm", path);
	YARPImageFile::Write(savename, g1);
	ACE_OS::sprintf(savename, "%sb.ppm", path);
	YARPImageFile::Write(savename, b1);
	//ACE_OS::sprintf(savename, "%si.ppm", path);
	//YARPImageFile::Write(savename, i1);

	ACE_OS::sprintf(savename, "%src.ppm", path);
	YARPImageFile::Write(savename, r1_g_c);
	ACE_OS::sprintf(savename, "%sgc.ppm", path);
	YARPImageFile::Write(savename, g1_g_c);
	ACE_OS::sprintf(savename, "%sbc.ppm", path);
	YARPImageFile::Write(savename, b1_g_c);
	//ACE_OS::sprintf(savename, "%sic.ppm", path);
	//YARPImageFile::Write(savename, i1_g_c);

	//ACE_OS::sprintf(savename, "./rs.ppm");
	//YARPImageFile::Write(savename, r1_g_s);
	//ACE_OS::sprintf(savename, "./gs.ppm");
	//YARPImageFile::Write(savename, g1_g_s);
	ACE_OS::sprintf(savename, "%sys.ppm", path);
	YARPImageFile::Write(savename, y1_g_s);
	//ACE_OS::sprintf(savename, "./is.ppm");
	//YARPImageFile::Write(savename, i1_g_s);

	//ACE_OS::sprintf(savename, "./or0.ppm");
	//YARPImageFile::Write(savename, or_r[0]);
	//ACE_OS::sprintf(savename, "./or1.ppm");
	//YARPImageFile::Write(savename, or_r[1]);
	//ACE_OS::sprintf(savename, "./or2.ppm");
	//YARPImageFile::Write(savename, or_r[2]);
	//ACE_OS::sprintf(savename, "./or3.ppm");
	//YARPImageFile::Write(savename, or_r[3]);
	
	ACE_OS::sprintf(savename, "%sor0.ppm", path);
	YARPImageFile::Write(savename, or[0]);
	ACE_OS::sprintf(savename, "%sor1.ppm", path);
	YARPImageFile::Write(savename, or[1]);
	ACE_OS::sprintf(savename, "%sor2.ppm", path);
	YARPImageFile::Write(savename, or[2]);
	//ACE_OS::sprintf(savename, "./or3.ppm");
	//YARPImageFile::Write(savename, or[3]);

	ACE_OS::sprintf(savename, "%sedge.ppm", path);
	YARPImageFile::Write(savename, edge);
	//ACE_OS::sprintf(savename, "./edge2.ppm");
	//YARPImageFile::Write(savename, edge2);
	
	ACE_OS::sprintf(savename, "%srg.ppm", path);
	YARPImageFile::Write(savename, rg);
	ACE_OS::sprintf(savename, "%sgr.ppm", path);
	YARPImageFile::Write(savename, gr);
	ACE_OS::sprintf(savename, "%sby.ppm", path);
	YARPImageFile::Write(savename, by);
	/*ACE_OS::sprintf(savename, "./ii.ppm");
	YARPImageFile::Write(savename, ii);*/

	ACE_OS::sprintf(savename, "%smeanocol.ppm", path);
	YARPImageFile::Write(savename, meanOppCol);
	ACE_OS::sprintf(savename, "%smeancol.ppm", path);
	YARPImageFile::Write(savename, meanCol);
	ACE_OS::sprintf(savename, "%simgvq.ppm", path);
	YARPImageFile::Write(savename, imgVQ);
	
	/*ACE_OS::sprintf(savename, "./comb.ppm");
	YARPImageFile::Write(savename, comb);*/
	ACE_OS::sprintf(savename, "%sout.ppm", path);
	YARPImageFile::Write(savename, out);
	ACE_OS::sprintf(savename, "%sblob_fov.ppm", path);
	YARPImageFile::Write(savename, blobFov);
}
