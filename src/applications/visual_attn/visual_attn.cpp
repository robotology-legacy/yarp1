// new version. partially YARP based.
//
// June 2001.

#include <sys/kernel.h>
#include <iostream.h>

// new stuff first.
#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPBlobFinder.h"
#include "YARPlogpolar.h"

#include "YARPPort.h"
#include "YARPImagePort.h"

#include "temporary/YARPConverter.h"	// YARP conversion (old)

// Remember to include the new version first!
#include <VisDMatrix.h>

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

// The old port software.
YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;
YARPOutputPortOf<FiveBoxesInARow> out_data;

char *my_name = "/yarp/visual_attn";
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
	//YARPImageOf<YarpPixelRGB> out_image;

	//out_image.Resize (Size, Size);

	YARPImageOf<YarpPixelRGB> cartesian;
	YARPImageOf<YarpPixelRGB> logpolar;
	cartesian.Resize (Size, Size);
	logpolar.Resize (NECC, NANG);

	//YARPLpConverter lp  (nEcc, nAng, rfMin, Size);
	YARPLogPolar	lp (NECC, NANG, rfMin, Size);
	YARPBlobFinder	m_blobber (NECC, NANG, rfMin, Size);
	
	m_blobber.SetSaturationThreshold (50);	// this is about 20% of the max
	m_blobber.SetHueThreshold (3 /*5*/);			// it was 5. (this was about 18 deg)
	m_blobber.SetSaliencyThreshold (50);	// this is about 20% of the max
	m_blobber.SetLumaSaliencyThreshold (75);

	in.Register (in_img_name);
	out.Register (out_img_name);
	out_data.Register (out_pos_name);

	FiveBoxesInARow target_boxes;

	while (1)
	{
		in.Read();
		in_image.Refer (in.Content());

		// this call converts also from the old to the new format.
		lp.Cart2LpSwap (in_image, logpolar);

		m_blobber.Apply (logpolar, logpolar);

		lp.Lp2Cart (logpolar, cartesian);
		m_blobber.DrawBoxes (cartesian);

		// I should probably write a channel swapping function here.
		//YARPConverter::ConvertFromYARPToCog (cartesian, out_image);

		YARPBox *b = m_blobber.GetBoxes();
		YARPBoxCopier::Copy (b[0], target_boxes.box1);
		YARPBoxCopier::Copy (b[1], target_boxes.box2);
		YARPBoxCopier::Copy (b[2], target_boxes.box3);
		YARPBoxCopier::Copy (b[3], target_boxes.box4);
		YARPBoxCopier::Copy (b[4], target_boxes.box5);
		
		YARPGenericImage& outImg = out.Content(); // Lasts until next Write()
		outImg.Refer(cartesian);
		out.Write();

		out_data.Content () = target_boxes;
		out_data.Write ();

//		char c;
//		scanf ("%c", &c);
	}
}


