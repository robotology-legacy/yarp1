//
// skin.cpp
//	- June 2001.
//	- Sept 2001.
//
#include <sys/kernel.h>
#include <iostream.h>

// YARP stuff first.
#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPlogpolar.h"
#include "YARPSkinFilter.h"
#include "YARPBoxer.h"
#include "YARPPort.h"

#include "YARPImagePort.h"

// still some functions to be updated.
#include "temporary/YARPConverter.h"

#include <VisDMatrix.h>

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;
//YARPBasicInputPort<YARPImagePortContent> in;
//YARPBasicOutputPort<YARPImagePortContent> out;
YARPOutputPortOf<FiveBoxesInARow> out_data;

char *my_name = "/yarp/skin";
char in_img_name[256];
char out_img_name[256];
char out_pos_name[256];

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
					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
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

	sprintf (in_img_name, "%s/in/img", my_name); 
	sprintf (out_img_name, "%s/out/img", my_name); 
	sprintf (out_pos_name, "%s/out/boxes", my_name); 
}

const int NECC = 64;
const int NANG = 128;

void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	// cartesian stuff.
	YARPImageOf<YarpPixelBGR> in_image;

	YARPImageOf<YarpPixelRGB> cartesian;
	cartesian.Resize (Size, Size);

	YARPImageOf<YarpPixelRGB> logpolar;
	logpolar.Resize (NECC, NANG);

	YARPImageOf<YarpPixelMono> mono_lp;
	YARPImageOf<YarpPixelMono> mono_tmp;
	mono_lp.Resize (NECC, NANG);
	mono_tmp.Resize (NECC, NANG);

	YARPImageOf<YarpPixelMono> mono_cartesian;
	mono_cartesian.Resize (Size, Size);

	YARPLogPolar	lp (NECC, NANG, rfMin, Size);

	YARPSkinFilter skin_detector;
	YARPLpBoxer boxer (NECC, NANG, rfMin, Size);
	boxer.SetThreshold (50);

	in.Register (in_img_name);
	out.Register (out_img_name);
	out_data.Register (out_pos_name);

	FiveBoxesInARow target_boxes;

	while (1)
	{
		in.Read();
		in_image.Refer (in.Content());

		// takes care of swapping BGR into RGB.
		lp.Cart2LpSwap (in_image, logpolar);

		skin_detector.Apply (logpolar, mono_lp);
		boxer.Apply (mono_lp, mono_tmp);

		lp.Lp2Cart (mono_lp, mono_cartesian);
		boxer.DrawBoxes (mono_cartesian);

		YARPBox *b = boxer.GetBoxes();
		YARPBoxCopier::Copy (b[0], target_boxes.box1);
		YARPBoxCopier::Copy (b[1], target_boxes.box2);
		YARPBoxCopier::Copy (b[2], target_boxes.box3);
		YARPBoxCopier::Copy (b[3], target_boxes.box4);
		YARPBoxCopier::Copy (b[4], target_boxes.box5);

		YARPGenericImage& outImg = out.Content(); // Lasts until next Write()
		outImg.Refer(mono_cartesian);
		out.Write();

		out_data.Content () = target_boxes;
		out_data.Write ();
	}
}
