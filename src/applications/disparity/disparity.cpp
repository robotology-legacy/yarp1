//
// disparity.cpp
//	- logpolar disparity.
// Sept 2001.
//
#include <sys/kernel.h>
#include <iostream.h>

#include "YARPImageServices.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "temporary/YARPConverter.h"

#include <VisDMatrix.h>

YARPInputPortOf<YARPGenericImage> inImgPortL;
YARPInputPortOf<YARPGenericImage> inImgPortR;

YARPOutputPortOf<YARPGenericImage> outImgPort;
YARPOutputPortOf<YARPGenericImage> histoPort;

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

YARPOutputPortOf<DisparityData> out_disparity;

char *my_name = "/yarp/disparity";

char *in_channell = "/yarp/disparity/in/left";
char *in_channelr = "/yarp/disparity/in/right";
char *out_channel = "/yarp/disparity/out/img";
char *histo_channel = "/yarp/disparity/out/histo";
char *position_channel = "/yarp/disparity/out/disparity";

#ifndef DISPLAYCARTESIAN
// comment this line to display logpolar
#define DISPLAYCARTESIAN TRUE
#endif

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
	int i;

	for(i=1;i<argc;i++)
	{
		switch(argv[i][0])
		{
			case '-':
				switch(argv[i][1])
				{
					case 'x':
						histo_channel = &argv[i][2];
						break;

					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
						cout << "-x histogram channel name" << endl;
						cout << endl;
						exit(0);
						break;

					default:
						break;
				}
				break;
			case '+':
				my_name = &argv[i][1];
				break;
			default:
				break;
		}
	}
}

// logpolar disparity.
void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	YARPImageOf<YarpPixelBGR> workImgL;	// just a reference.
	YARPImageOf<YarpPixelBGR> workImgR;	// just a reference.
	
	YARPImageOf<YarpPixelRGB> outWorkImg;
	outWorkImg.Resize (Size, Size);
	YARPImageOf<YarpPixelMono> out_histo;
	out_histo.Resize (Size, Size);

	YARPImageOf<YarpPixelRGB> left_cart;
	YARPImageOf<YarpPixelRGB> right_cart;
	YARPImageOf<YarpPixelRGB> logpolarl;
	YARPImageOf<YarpPixelRGB> logpolarr;

#ifndef DISPLAYCARTESIAN
	YARPImageOf<YarpPixelMono> left_lp;
	YARPImageOf<YarpPixelMono> right_lp;
#else
	YARPImageOf<YarpPixelMono> left_ca;
	YARPImageOf<YarpPixelMono> right_ca;
#endif
	YARPImageOf<YarpPixelMono> left_mono;
	YARPImageOf<YarpPixelMono> right_mono;

	left_cart.Resize(Size, Size);
	right_cart.Resize(Size, Size);
	logpolarl.Resize(nEcc, nAng);
	logpolarr.Resize(nEcc, nAng);
	
#ifndef DISPLAYCARTESIAN
	left_lp.Resize(nEcc, nAng);
	right_lp.Resize(nEcc, nAng);
#else
	left_ca.Resize(Size, Size);
	right_ca.Resize(Size, Size);
#endif
	left_mono.Resize(Size, Size);
	right_mono.Resize(Size, Size);

	// assume image is square.
	YARPLogPolar lp (nEcc, nAng, rfMin, Size);
	YARPLpDisparity dfilter (nEcc, nAng, rfMin, Size);

	DisparityData disparity_data;

	inImgPortL.Register (in_channell);
	inImgPortR.Register (in_channelr);
	outImgPort.Register (out_channel);
	histoPort.Register (histo_channel);

	out_disparity.Register (position_channel);
	
	while (1)
    {
		inImgPortL.Read();
		YARPGenericImage& inImgL = inImgPortL.Content(); // Lifetime until next Read()
		workImgL.Refer(inImgL);
		inImgPortR.Read();
		YARPGenericImage& inImgR = inImgPortR.Content(); // Lifetime until next Read()
		workImgR.Refer(inImgR);

		// LATER: there's a BGR to RGB missing conversion here.
		// NOTE: conversion is not needed!
		lp.Cart2LpSwap (workImgL, logpolarl);
		lp.Cart2LpSwap (workImgR, logpolarr);

#ifndef DISPLAYCARTESIAN
		YARPColorConverter::RGB2Grayscale (logpolarl, left_lp);
		YARPColorConverter::RGB2Grayscale (logpolarr, right_lp);
#else
		YARPColorConverter::RGB2Grayscale (workImgL, left_ca);
		YARPColorConverter::RGB2Grayscale (workImgR, right_ca);
#endif
		disparity_data.disparity = (double)dfilter.ComputeDisparity (logpolarl, logpolarr);
	
		YARPImageOf<YarpPixelMono>& histogram = dfilter.GetHistogramAsImage();

		// Converts histogram into a 128 sq. out_histo
		YARPImageUtils::PasteInto (histogram, 0, 0, 1, out_histo);

		out_disparity.Content() = disparity_data;
		out_disparity.Write();
	
		// logpolar display.
#ifndef DISPLAYCARTESIAN
		lp.Lp2Cart (left_lp, left_mono);
		lp.Lp2Cart (right_lp, right_mono);
		YARPImageUtils::SetRed (left_mono, outWorkImg);
		YARPImageUtils::SetGreen (right_mono, outWorkImg);
#else
		YARPImageUtils::SetRed (left_ca, outWorkImg);
		YARPImageUtils::SetGreen (right_ca, outWorkImg);
#endif
		YARPGenericImage& outImg = outImgPort.Content();
		outImg.Refer (outWorkImg);
		outImgPort.Write();

		YARPGenericImage& outhImg = histoPort.Content();
		outhImg.Refer(out_histo);
		histoPort.Write();
    }
}

