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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: vergence.cpp,v 1.18 2004-07-12 08:36:31 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPImages.h>
#include <YARPImagePortContent.h>

#include <iostream>
#include <math.h>

#include <YARPTime.h>
#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPImageDraw.h>
#include <YARPLogpolar.h>
#include <YARPMath.h>

#include <YARPParseParameters.h>
#include <YARPVectorPortContent.h>
#include <YARPDisparity.h>
#include <YARPImageFile.h>

#include "headvergencelimits.h"

///
///
///
YARPInputPortOf<YARPGenericImage> in_left;
YARPInputPortOf<YARPGenericImage> in_right;
YARPInputPortOf<YVector> in_head;

YARPOutputPortOf<YARPGenericImage> out_img1 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
//YARPOutputPortOf<YARPGenericImage> out_img2 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
//YARPOutputPortOf<YARPGenericImage> out_img3 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
//YRPOutputPortOf<YARPGenericImage> out_img4 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
//YARPOutputPortOf<YARPGenericImage> out_img5 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
//YARPOutputPortOf<YARPGenericImage> out_img6 (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
YARPOutputPortOf<YVector> out_disp (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

const char *DEFAULT_NAME = "/vergence";

const int __nScales = 1;
const int __nRings[] = {80, 80, 80, 80, 80, 80};
const float __ratios[] = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
const int __histoHeight = 64;
const int __histoWidth = 256;

//const float __ratios[] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};

void makeHistogram(YARPImageOf<YarpPixelMono>& hImg, const double *corr1, const double *corr2, const double *corr3, int sl, double val);
void makeHistogram(YARPImageOf<YarpPixelMono>& hImg, const double *corr, int sl);
void addLimits2Histogram(YARPImageOf<YarpPixelMono>& hImg, int max, int min, int sl);
void addMax2Histogram(YARPImageOf<YarpPixelMono>& hImg, int max1, int max2, int max3, int sl);

HeadVergenceLimits _limits;

int main(int argc, char *argv[])
{
	using namespace _logpolarParams;

	YARPString name;
	YARPString network_i;
	YARPString network_o;
	char buf[256];

	if (!YARPParseParameters::parse(argc, argv, "name", name))
	{
		name = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "neti", network_i))
	{
		network_i = "default";
	}

	if (!YARPParseParameters::parse(argc, argv, "neto", network_o))
	{
		network_o = "default";
	}

	char path[80]; 
	char filename[] = "head.ini";
	sprintf(path, "%s/%s", GetYarpRoot(), ConfigFilePath);
	_limits.init(YARPString(path), YARPString(filename));

	/// images are coming from the input network.
	sprintf(buf, "%s/i:left", name.c_str());
	in_left.Register(buf, network_i.c_str());
	sprintf(buf, "%s/i:right", name.c_str());
	in_right.Register(buf, network_i.c_str());

	sprintf(buf, "%s/head/i", name.c_str());
	in_head.Register(buf, "default");

	sprintf(buf, "%s/o:histo", name.c_str());
	out_img1.Register(buf, network_i.c_str());
//	sprintf(buf, "%s/o:histo2", name.c_str());
//	out_img2.Register(buf, network_i.c_str());
//	sprintf(buf, "%s/o:img", name.c_str());
//	out_img3.Register(buf, network_i.c_str());
//	sprintf(buf, "%s/o:img2", name.c_str());
//	out_img4.Register(buf, network_i.c_str());
//	sprintf(buf, "%s/o:img3", name.c_str());
//	out_img5.Register(buf, network_i.c_str());
//	sprintf(buf, "%s/o:img4", name.c_str());
//	out_img6.Register(buf, network_i.c_str());

	sprintf(buf, "%s/o:disparity", name.c_str());
	out_disp.Register(buf, network_o.c_str());

	YARPImageOf<YarpPixelMono> inl;
	YARPImageOf<YarpPixelMono> inr;

	YARPImageOf<YarpPixelMono> out1;
	YARPImageOf<YarpPixelMono> out2;
	// YARPImageOf<YarpPixelMono> histo[__nScales];
	
	YARPImageOf<YarpPixelBGR> col_left;
	YARPImageOf<YarpPixelBGR> col_right;

	YARPImageOf<YarpPixelBGR> sub_left;
	YARPImageOf<YarpPixelBGR> sub_right;

	YARPImageOf<YarpPixelBGR> outImageLp;
	YARPImageOf<YarpPixelBGR> outImageCart;
	YARPImageOf<YarpPixelBGR> outImageLp2;
	YARPImageOf<YarpPixelBGR> outImageCart2;
	YARPImageOf<YarpPixelBGR> outImageMapLp;
	YARPImageOf<YarpPixelBGR> outImageMapCart;
	YARPImageOf<YarpPixelBGR> outImageCorrMapLp;
	YARPImageOf<YarpPixelBGR> outImageCorrMapCart;

	col_left.Resize (_stheta, _srho);
	col_right.Resize (_stheta, _srho);
	sub_left.Resize (_stheta/__ratios[0], _srho/__ratios[0]);
	sub_right.Resize (_stheta/__ratios[0], _srho/__ratios[0]);
	outImageLp.Resize(_stheta/__ratios[0], _srho/__ratios[0]);
	outImageLp2.Resize(_stheta/__ratios[0], _srho/__ratios[0]);
	outImageMapLp.Resize(_stheta/__ratios[0], _srho/__ratios[0]);
	outImageCorrMapLp.Resize(_stheta/__ratios[0], _srho/__ratios[0]);
	outImageCart.Resize(_xsize, _ysize);
	outImageCart2.Resize(_xsize, _ysize);
		
	
	outImageMapCart.Resize(_xsize, _ysize);
	outImageCorrMapCart.Resize(_xsize, _ysize);
	outImageCorrMapLp.Zero();
	outImageMapLp.Zero();
	outImageLp.Zero();
	outImageLp2.Zero();
	
	//out1.Resize (512, 64*3);
	//out2.Resize (512, 64*3);

	out1.Resize (__histoWidth, __histoHeight);
	// out2.Resize (256, 64*3);

	YARPLogpolar mapper;
	YARPDisparityTool disparity[__nScales];
	int s = 0;
	for(s=0; s<__nScales;s++)
	{
		ACE_OS::printf("Flag: %d\n",s);
		disparity[s].init(__nRings[s], __ratios[s]);
		// disparity[s].setRings(__nRings[s]);

	//	histo[s].Resize(256, 64);
	}

	/*
	FILE *fp = fopen ("Y:/conf/vergence_bugs.txt", "w");
	ACE_ASSERT (fp != NULL);
	*/

	
	YVector disparityval(2);

	float max, min;
	
	while (1)
	{
		in_left.Read();
		in_right.Read();

		// poll head position
		if (in_head.Read(0))
		{
			_limits.update(in_head.Content());
			_limits.computeLimits(max, min);
			
			disparity[0].setLimits(max, min);
			//	printf("Max: %lf\tMin: %lf\n", max(1), min(1)); 
		}

		inl.Refer (in_left.Content());
		inr.Refer (in_right.Content());

		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inl, col_left);
		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inr, col_right);
		ACE_ASSERT (inl.GetWidth() == _stheta && inl.GetHeight() == _srho);
		ACE_ASSERT (inr.GetWidth() == _stheta && inr.GetHeight() == _srho);

		/// subsample
		disparity[0].downSample (col_left, sub_left);
		disparity[0].downSample (col_right, sub_right);

		double value;
		// disparity[0].computeSSDRGBxVar2 (sub_right, sub_left, &value);
		// disparity[0].plotRegion(sub_right, outImageLp2, disparity[0].getShift());
		// disparity[0].remap(outImageLp2, outImageCart2);

		//disparity[3].computeRGBChain (sub_right, sub_left, &value);
		//disparity[1].plotRegion(sub_right, outImageLp, disparity[1].getShift());
		//disparity[1].remap(outImageLp, outImageCart);

//		disparity[1].computeCorrProd (sub_right, sub_left, &value);
//		disparity[1].plotRegion(sub_right, outImageLp, disparity[1].getShift());
//		disparity[1].remap(outImageLp, outImageCart);
		
		disparityval(1) = (double)disparity[0].computeMono (sub_right, sub_left, &value);
//		disparity[2].plotRegion(sub_right, outImageLp, disparity[1].getShift());
//		disparity[2].remap(outImageLp, outImageCart);
		
		// disparityval(1) = (double)disparity[1].computeABSRGB (sub_right, sub_left, &value);
		// disparityval(1) = (double)disparity[2].computeRGBAv (sub_left, sub_left, &value);

		//disparity[0].plotCorrelationMap (sub_right, sub_left, outImageCorrMapLp, disparity[0].zeroShift());
		//	disparity[0].remap(outImageCorrMapLp, outImageCorrMapCart);
		
		// disparity[3].computeSSDRGBxVar (sub_right, sub_left, &value);
		//disparity[3].plotRegion(sub_right, outImageLp2, disparity[3].getShift());
		//disparity[3].remap(outImageLp2, outImageCart2);
		//	disparity[3].plotSSDMap(sub_right, sub_left, outImageMapLp, disparity[3].zeroShift());
		//	disparity[3].remap(outImageMapLp, outImageMapCart);

		// disparityval(1) = (double)disparity[4].computeSSDRGBxVar (sub_right, sub_left, &value);
		// disparityval(1) = (double)disparity[5].computeSSDRGBVar (sub_right, sub_left, &value);

		// const double *tmpc = disparity[s].getCorrFunction();
		
		disparityval(2) = 1.0;
		
		const double *correlation0 = disparity[0].getCorrFunction();
		// const double *correlation1 = disparity[0].getCorrFunction();
		// const double *correlation2 = disparity[0].getCorrFunction();

		// const double *correlation3 = disparity[0].getCorrFunction();
		// const double *correlation4 = disparity[0].getSSDFunction();
		// const double *correlation5 = disparity[0].getVarFunction();
		
		
		out1.Zero();
		makeHistogram(out1, correlation0, disparity[0].getShiftLevels());
	//	makeHistogram(out2, correlation3, correlation4, correlation5, disparity[0].getShiftLevels(), value);

		addLimits2Histogram(out1, disparity[0].getLimitsMax(), disparity[0].getLimitsMin(), disparity[0].getShiftLevels());
		addMax2Histogram(out1, disparity[0].getShift(0), disparity[0].getShift(1), disparity[0].getShift(2), disparity[0].getShiftLevels()); 
	
		out_img1.Content().Refer(out1);
		out_img1.Write();

	//	out_img2.Content().Refer(out2);
	//	out_img2.Write();

		// out_img3.Content().Refer(outImageCart);
		// out_img3.Write();

		// out_img4.Content().Refer(outImageCart2);
		// out_img4.Write();

		//	out_img5.Content().Refer(outImageMapCart);
		//	out_img5.Write();

		//	out_img6.Content().Refer(outImageCorrMapCart);
		//	out_img6.Write();

		out_disp.Content() = disparityval;
		out_disp.Write();
	}

	return 0;
}

void makeHistogram(YARPImageOf<YarpPixelMono>& hImg, const double *corr1, const double *corr2, const double *corr3, int sl, double val)
{
	int i,j;
	int height = hImg.GetHeight();
	int width = hImg.GetWidth();

	unsigned char * hist = (unsigned char *)hImg.GetRawBuffer();

	int offset = (width-sl+1)/2;

	unsigned char color;
	if (val>0.16)
		color = 128;
	else
		color = 60;

	for (i=0; i<sl-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			if (corr1[i]>=0 && corr1[i]<=1)
				for (j=height-(int)(height/3*(corr1[i])); j<height; j++)
					hist[(j*width+i+offset)] = color;
			else if (corr1[i]>1)
				for (j=height-(int)(height/3); j<height; j++)
					hist[(j*width+i+offset)] = 255;

				// printf("Warning corr1[%d] would have been %lf\n", i, corr1[i]);
		}
	}

	for (i=0; i<sl-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			if (corr2[i]>=0 && corr2[i]<=1)
				for (j=(2*height/3)-(int)(height/3*(corr2[i])); j<2*height/3; j++)
					hist[j*width+i+offset] = color;
			else if (corr2[i]>1)
				for (j=(2*height/3)-(int)(height/3); j<2*height/3; j++)
					hist[j*width+i+offset] = color;
		}
	}

	for (i=0; i<sl-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			if (corr3[i]>=0 && corr3[i]<=1)
				for (j=(1*height/3)-(int)(height/3*(corr3[i])); j<height/3; j++)
					hist[j*width+i+offset] = color;
			else if (corr3[i]>1)
				for (j=(1*height/3)-(int)(height/3); j<height/3; j++)
					hist[j*width+i+offset] = color;
		}
	}

	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;

	for(i=0; i<width; i++)
		hist[(height/3)*width+i] = 255;

	for(i=0; i<width; i++)
		hist[(2*height/3)*width+i] = 255;
}

void makeHistogram(YARPImageOf<YarpPixelMono>& hImg, const double *corr, int sl)
{
	int i,j;
	int height = hImg.GetHeight();
	int width = hImg.GetWidth();

	unsigned char * hist = (unsigned char *)hImg.GetRawBuffer();

	int offset = (width-sl+1)/2;

	unsigned char color = 128;
	
	for (i=0; i<sl-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			if (corr[i]>=0 && corr[i]<=1)
				for (j=height-(int)(height*(corr[i])); j<height; j++)
					hist[(j*width+i+offset)] = color;
			else if (corr[i]>1)
				for (j=height-(int)(height); j<height; j++)
					hist[(j*width+i+offset)] = 255;
		}
	}

	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;
}

void addLimits2Histogram(YARPImageOf<YarpPixelMono> &hImg, int max, int min, int sl)
{
	// printf("Shift: %d\t %d\n", max, min);
	int j;
	int height = hImg.GetHeight();
	int width = hImg.GetWidth();

	int offset = (width-sl+1)/2;

	int v1 = max+offset;
	int v2 = min+offset;

	if (v1<0)
		v1 = 0;
	if (v2<0)
		v2 = 0;

	if (v1>=width)
		v1 = width;
	if (v2>=width)
		v2 = width;

	for(j=0; j < height; j++)
	{
		hImg(v1, j) = 255;
		hImg(v2, j) = 255;
		hImg(v1-1, j) = 255;
		hImg(v2-1, j) = 255;
		hImg(v1+1, j) = 255;
		hImg(v2+1, j) = 255;
	}
}

void addMax2Histogram(YARPImageOf<YarpPixelMono>& hImg, int max1, int max2, int max3, int sl)
{
	int j;
	int height = hImg.GetHeight();
	int width = hImg.GetWidth();

	int offset = (width-sl+1)/2;

	int v1 = max1+offset;
	int v2 = max2+offset;
	int v3 = max3+offset;

	if (v1<0)
		v1 = 0;
	if (v2<0)
		v2 = 0;
	if (v3<0)
		v3 = 0;

	if (v1>=width)
		v1 = width;
	if (v2>=width)
		v2 = width;
	if (v3>=width)
		v3 = width;

	for(j=0; j < (int) 3*height/10.0; j++)
	{
		hImg(v1-1, j) = 255;
		hImg(v1, j) = 255;
		hImg(v1+1, j) = 255;
	}
	
	for(j=0; j < (int) 2*height/10.0; j++)
	{
		hImg(v2-1, j) = 255;
		hImg(v2, j) = 255;
		hImg(v2+1, j) = 255;
	}

	for(j=0; j < (int) 2*height/10.0; j++)
	{
		hImg(v3-1, j) = 255;
		hImg(v3, j) = 255;
		hImg(v3+1, j) = 255;
	}
}