//
// color_segm.cpp
//	- color segmentation.
//
//	= revised Sept 2001.

#include <sys/kernel.h>
#include <iostream.h>

// the new stuff.
#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPBoxer.h"
#include "YARPlogpolar.h"
#include "YARPFirstDerivativeT.h"
#include "YARPBlurFilter.h"
#include "YARPBinarizeFilter.h"
#include "YARPSimpleOperations.h"
#include "YARPColorSegmentation.h"
#include "YARPImageUtils.h"

#include "YARPPort.h"
#include "YARPImagePort.h"
#include "temporary/YARPConverter.h"	// YARP conversion (old)

#include <VisDMatrix.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

YARPInputPortOf<YARPGenericImage> inImgPortL;
YARPInputPortOf<YARPGenericImage> inImgPortR;

YARPOutputPortOf<YARPGenericImage> outImgPortL;
YARPOutputPortOf<YARPGenericImage> outImgPortR;

YARPOutputPortOf<YARPGenericImage> histoPort;
bool lp_out_flag = false; 

YARPOutputPortOf<FiveBoxesInARow> out_position_l;
YARPOutputPortOf<FiveBoxesInARow> out_position_r;

char *my_name = "/yarp/color_segm";

char *in_channell = "/yarp/color_segm/in/left";
char *out_channell = "/yarp/color_segm/out/left";
char *in_channelr = "/yarp/color_segm/in/right";
char *out_channelr = "/yarp/color_segm/out/right";
char *histo_channel = "/yarp/color_segm/out/histo";
char *position_channel_l = "/yarp/color_segm/out/boxl";
char *position_channel_r = "/yarp/color_segm/out/boxr";

enum VisionThread2Status 
{
	VTS_WaitForTarget = 0,
	VTS_ColorSegmentationInit = 1,
	VTS_ColorSegmentation = 2,
	VTS_AbortSegmentation = 4,
	VTS_ResetHistograms = 5
};

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

const int NECC = 64;
const int NANG = 128;

//
void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	YARPImageOf<YarpPixelBGR> workImgL;	// just a reference.
	YARPImageOf<YarpPixelBGR> workImgR;	// just a reference.
	
    YARPImageOf<YarpPixelMono> lpImg;
	lpImg.Resize (Size, Size);

	YARPImageOf<YarpPixelMono>	cartesianl;
	YARPImageOf<YarpPixelRGB>	in_cartl;
	YARPImageOf<YarpPixelRGB>	logpolarl;
	YARPImageOf<YarpPixelMono>	m_storeleft;
	YARPImageOf<YarpPixelMono>	cartesianr;
	YARPImageOf<YarpPixelRGB>	in_cartr;
	YARPImageOf<YarpPixelRGB>	logpolarr;
	YARPImageOf<YarpPixelMono>	m_storeright;

	cartesianl.Resize (Size, Size);
	in_cartl.Resize (Size, Size);
	logpolarl.Resize (NECC, NANG);
	m_storeleft.Resize (NECC, NANG);
	cartesianr.Resize (Size, Size);
	in_cartr.Resize (Size, Size);
	logpolarr.Resize (NECC, NANG);
	m_storeright.Resize (NECC, NANG);

	YARPImageOf<YarpPixelMono> m_back_histo;
	m_back_histo.Resize (15, 10);
	YARPImageOf<YarpPixelMono> m_histo;
	m_histo.Resize (15, 10);

	VisionThread2Status m_status = VTS_WaitForTarget;
	int m_counter = 0;

	YARPLogPolar lp (NECC, NANG, rfMin, Size);
	YARPLpClrSegmentation m_color_segmentation (NECC, NANG, rfMin, Size);
	YARPLpBoxer m_boxer (NECC, NANG, rfMin, Size);

	FiveBoxesInARow target_boxes_l;
	FiveBoxesInARow target_boxes_r;

	inImgPortL.Register(in_channell);
	inImgPortR.Register(in_channelr);
	outImgPortL.Register(out_channell);
	outImgPortR.Register(out_channelr);

	histoPort.Register(histo_channel);

	out_position_l.Register (position_channel_l);
	out_position_r.Register (position_channel_r);

	while (1)
    {
		inImgPortL.Read();
		YARPGenericImage& inImgL = inImgPortL.Content(); // Lifetime until next Read()
		workImgL.Refer(inImgL);
		inImgPortR.Read();
		YARPGenericImage& inImgR = inImgPortR.Content(); // Lifetime until next Read()
		workImgR.Refer(inImgR);

		lp.Cart2LpSwap (workImgL, logpolarl);
		lp.Cart2LpSwap (workImgR, logpolarr);

		switch (m_status)
		{
		case VTS_WaitForTarget:
			if (m_color_segmentation.InitialDetection (logpolarl, logpolarr, m_storeleft, m_storeright))
				m_status = VTS_ColorSegmentationInit;
			break;

		case VTS_ColorSegmentationInit:
			{
				if (m_color_segmentation.StartupSegmentation (logpolarl, logpolarr, m_storeleft, m_storeright))
				{
					m_status = VTS_ColorSegmentation;
					m_counter = 0;
				}
				else
					m_status = VTS_WaitForTarget;
			}
			break;

		case VTS_ColorSegmentation:
			{
				// wait 6 steps before aborting in case of bad target...
				bool can_continue = m_color_segmentation.ColorSegmentation (logpolarl, logpolarr, m_storeleft, m_storeright);
				if (!can_continue)
				{
					// wait some cycles...
					m_counter++;
					if (m_counter > 20) // || !good)
					{
						// move to motion segmentation.
						m_status = VTS_WaitForTarget;
						m_color_segmentation.Reset ();
					}
				}
				else
				{
					m_counter = 0;
				}
			}
			break;

			case VTS_ResetHistograms:
				m_color_segmentation.ResetHistograms ();	// continues aborting the segmentation.
			
			case VTS_AbortSegmentation:
				m_color_segmentation.AbortSegmentation ();
				m_status = VTS_WaitForTarget;
			break;
		}

		m_boxer.Apply (m_storeleft);
		lp.Lp2Cart (m_storeleft, cartesianl);
		m_boxer.DrawBoxes (cartesianl);

		YARPBox *b = m_boxer.GetBoxes();
		YARPBoxCopier::Copy (b[0], target_boxes_l.box1);
		YARPBoxCopier::Copy (b[1], target_boxes_l.box2);
		YARPBoxCopier::Copy (b[2], target_boxes_l.box3);
		YARPBoxCopier::Copy (b[3], target_boxes_l.box4);
		YARPBoxCopier::Copy (b[4], target_boxes_l.box5);
	
		m_boxer.Apply (m_storeright);
		lp.Lp2Cart (m_storeright, cartesianr);
		m_boxer.DrawBoxes (cartesianr);

		YARPBoxCopier::Copy (b[0], target_boxes_r.box1);
		YARPBoxCopier::Copy (b[1], target_boxes_r.box2);
		YARPBoxCopier::Copy (b[2], target_boxes_r.box3);
		YARPBoxCopier::Copy (b[3], target_boxes_r.box4);
		YARPBoxCopier::Copy (b[4], target_boxes_r.box5);

		out_position_l.Content() = target_boxes_l;
		out_position_l.Write();
		out_position_r.Content() = target_boxes_r;
		out_position_r.Write();

		m_color_segmentation.GetMotionHistogramAsImage(m_histo);
		m_color_segmentation.GetWholeHistogramAsImage(m_back_histo);
		
		YARPImageUtils::PasteInto (m_histo, 0, 0, 5, lpImg);
		YARPImageUtils::PasteInto (m_back_histo, 0, 60, 5, lpImg);

		YARPGenericImage& outImgL = outImgPortL.Content(); // Lasts until next Write()
		outImgL.Refer(cartesianl);
		outImgPortL.Write();
		YARPGenericImage& outImgR = outImgPortR.Content(); // Lasts until next Write()
		outImgR.Refer(cartesianr);
		outImgPortR.Write();

		YARPGenericImage& outhImg = histoPort.Content();
		outhImg.Refer(lpImg);
		histoPort.Write();
    }
}

