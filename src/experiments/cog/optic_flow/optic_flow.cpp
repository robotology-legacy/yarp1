//
// optic_flow.cpp
//	- optic flow based segmentation June 2001.
//
#include <sys/kernel.h>
#include <iostream.h>

#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPFirstOrderFlow.h"
#include "YARPFlowVerify.h"
#include "YARPBoxer.h"

#include "YARPPort.h"
#include "YARPImagePort.h"

#include "temporary/YARPConverter.h"	// YARP conversion (old)

#include <VisDMatrix.h>
#include <Models.h>

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

YARPInputPortOf<YARPGenericImage> inImgPort;
YARPInputPortOf<JointSpeed> inJointSpeed;

YARPOutputPortOf<YARPGenericImage> outImgPort;
YARPOutputPortOf<FiveBoxesInARow> outBoxes;

char *my_name = "/yarp/optic_flow";

char in_channel[256];
char vel_channel[256];
char out_channel[256];
char data_channel[256];

bool prediction_mode = false;

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
					case 'p':
						prediction_mode = true;
						break;

					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
						cout << "-p prediction mode" << endl;
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

	sprintf (in_channel, "%s/in/img", my_name); 
	sprintf (vel_channel, "%s/in/velocity", my_name);
	sprintf (out_channel, "%s/out/img", my_name); 
	sprintf (data_channel, "%s/out/boxes", my_name); 
}

// this is just a test for a logpolar display.
void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	YARPImageOf<YarpPixelBGR> workImg;	// just a reference.
	
	//YARPImageOf<YarpPixelMono> outWorkImg;
	//outWorkImg.Resize (Size, Size);

    YARPImageOf<YarpPixelMono> lpImg;
	lpImg.Resize (Size, Size);

	//
	//YARPImageOf<YarpPixelRGB> in_cart;		// in image.
	YARPImageOf<YarpPixelMono> cartesian;	// mono version.
	YARPImageOf<YarpPixelMono> logpolar;	// sampled.

	//in_cart.Resize (Size, Size);
	cartesian.Resize (Size, Size);
	logpolar.Resize (nEcc, nAng);

	YARPImageOf<YarpPixelMono> mask;		// of mask.
	mask.Resize (nEcc, nAng);
	YARPImageOf<YarpPixelMono> tmp_mask;
	tmp_mask.Resize (nEcc, nAng);

	// prepares mask.
	mask.Zero();
	for (int i = 0; i < nAng; i++)
	{
		unsigned char *mptr = (unsigned char *)mask.GetArray()[i] + nEcc/3;
		for (int j = 0; j < nEcc/3; j++)
			*mptr++ = 255;
	}
	tmp_mask = mask;

	YARPLogPolar lp (nEcc, nAng, rfMin, Size);

	// if this is being used in prediction mode.
	if (prediction_mode)
	{
		YARPLpFlowVerify fo_verify (nEcc, nAng, rfMin, Size);
		YARPLpBoxer boxer (nEcc, nAng, rfMin, Size);
		boxer.SetExact (255);

		inImgPort.Register(in_channel);
		inJointSpeed.Register (vel_channel);
		outImgPort.Register (out_channel);
		outBoxes.Register (data_channel);

		bool first_frame = true;
		CVisDVector result (6);
		result = 0;

		// least square prediction.
		const int num_params = 12;
		bool valid_speed = false;

		CVisDVector theta (num_params);
		//theta(1) = -6.43817e-06;
		//theta(2) = -7.43023e-05;
		//theta(3) = -7.53038e-05;
		//theta(4) = 0.000286664;
		//theta(5) = 0.000109773;
		//theta(6) = -7.83885e-05;
		//theta(7) = 0.000175181;
		//theta(8) = -2.42158e-06;
		//theta(9) = 0.000129027;
		//theta(10) = 8.68172e-05;
		//theta(11) = 3.80287e-05;
		//theta(12) = 7.20796e-05;

		// thresholded version.
		theta(1) = 0.0;
		theta(2) = -7.43023e-05;
		theta(3) = 0.0;
		theta(4) = 0.0;
		theta(5) = 0.0;
		theta(6) = 0.0;
		theta(7) = 0.000175181;
		theta(8) = 0.0;
		theta(9) = 0.0;
		theta(10) = 0.0;
		theta(11) = 0.0;
		theta(12) = 0.0;

		CVisDVector prediction(6);
		prediction = 0;

		// delay buffer.
		const int delay = 3;
		JointSpeed speed_array[delay];
		memset (speed_array, 0, sizeof(JointSpeed) * delay);
		int index_array = 0;
		bool array_full = false;

		while (1)
		{
			inImgPort.Read();
			YARPGenericImage& inImg = inImgPort.Content(); // Lifetime until next Read()
			workImg.Refer(inImg);

			inJointSpeed.Read();
			JointSpeed new_speed = inJointSpeed.Content();
			JointSpeed speed;
			memset (&speed, 0, sizeof(JointSpeed));

			if (array_full)
			{
				index_array = (index_array + 1) % delay;

				speed = speed_array[index_array];

				// check motion.
				double sq = speed.j1 * speed.j1;
				sq += speed.j2 * speed.j2;
				sq += speed.j4 * speed.j4;
				sq += speed.j5 * speed.j5;
				sq += speed.j6 * speed.j6;
				sq += speed.j7 * speed.j7;
				valid_speed = (sq > 10000) ? true : false;
				
				speed_array[index_array] = new_speed;
			}
			else
			{
				// just fill the buffer.
				speed_array [index_array] = new_speed;
				index_array ++;
				if (index_array > delay-1)
				{
					array_full = true;
					index_array = delay-1;
				}
				valid_speed = false;
			}

			// there's a BGR 2 RGB conversion missing here.
			//YARPConverter::ConvertFromCogToYARP (workImg, in_cart);

			YARPColorConverter::RGB2Grayscale (workImg, cartesian);
			lp.Cart2LpAverage (cartesian, logpolar);

			if (!valid_speed)
				prediction = 0;
			else
			{
				// prediction = theta * speed;
				prediction (1) = 0;
				prediction (1) += theta(1) * speed.j1;
				prediction (1) += theta(2) * (speed.j2 + speed.j3) / 2;
				prediction (1) += theta(3) * speed.j4;
				prediction (1) += theta(4) * speed.j5;
				prediction (1) += theta(5) * speed.j6;
				prediction (1) += theta(6) * speed.j7;

				prediction (2) = 0;
				prediction (2) += theta(7) * speed.j1;
				prediction (2) += theta(8) * (speed.j2 + speed.j3) / 2;
				prediction (2) += theta(9) * speed.j4;
				prediction (2) += theta(10) * speed.j5;
				prediction (2) += theta(11) * speed.j6;
				prediction (2) += theta(12) * speed.j7;
			}

			//cout << "flow : " << prediction(1) << " " << prediction(2) << endl;

			if (!first_frame)
			{
				tmp_mask = mask;
				fo_verify.Apply (logpolar, tmp_mask, prediction);
				boxer.Apply (tmp_mask);
			}
			else
			{
				// first frame.
				fo_verify.Init (logpolar);
				first_frame = false;
			}

			// write out image.
			lp.Lp2Cart (tmp_mask, cartesian);
			boxer.DrawBoxes (cartesian);
			//YARPConverter::ConvertFromYARPToCog (cartesian, outWorkImg);

			FiveBoxesInARow& target_boxes = outBoxes.Content();
			YARPBox *b = boxer.GetBoxes();
			YARPBoxCopier::Copy (b[0], target_boxes.box1);
			YARPBoxCopier::Copy (b[1], target_boxes.box2);
			YARPBoxCopier::Copy (b[2], target_boxes.box3);
			YARPBoxCopier::Copy (b[3], target_boxes.box4);
			YARPBoxCopier::Copy (b[4], target_boxes.box5);

			YARPGenericImage& outImg = outImgPort.Content(); // Lasts until next Write()
			outImg.Refer(cartesian);
			outImgPort.Write();

			outBoxes.Write ();
		}
	}
	else
	{
		YARPLpFirstOrderFlow fo_flow (nEcc, nAng, rfMin, Size);

		inImgPort.Register(in_channel);
		inJointSpeed.Register (vel_channel);

		bool first_frame = true;
		CVisDVector result (6);
		result = 0;

		// least square.
		const int num_params = 12;
		CRecursiveLS estimator (num_params, 1.0);
		CVisDVector theta (num_params);
		theta = 0;
		CVisDVector psi (num_params);
		psi = 0;
		CVisDMatrix p0 (num_params, num_params);

		p0 = 0;
		for (i = 1; i <= num_params; i++)
			p0(i,i) = 1;

		estimator.SetInitialState (p0, theta);
		bool valid_speed = false;

		const int delay = 3;
		JointSpeed speed_array[delay];
		memset (speed_array, 0, sizeof(JointSpeed) * delay);
		int index_array = 0;
		bool array_full = false;

		while (1)
		{
			inImgPort.Read();
			YARPGenericImage& inImg = inImgPort.Content(); // Lifetime until next Read()
			workImg.Refer(inImg);

			inJointSpeed.Read();
			JointSpeed new_speed = inJointSpeed.Content();
			JointSpeed speed;
			memset (&speed, 0, sizeof(JointSpeed));

			if (array_full)
			{
				index_array = (index_array + 1) % delay;

				speed = speed_array[index_array];

				// check motion.
				double sq = speed.j1 * speed.j1;
				sq += speed.j2 * speed.j2;
				sq += speed.j4 * speed.j4;
				sq += speed.j5 * speed.j5;
				sq += speed.j6 * speed.j6;
				sq += speed.j7 * speed.j7;
				valid_speed = (sq > 10000) ? true : false;
				
				speed_array[index_array] = new_speed;
			}
			else
			{
				// just fill the buffer.
				speed_array [index_array] = new_speed;
				index_array ++;
				if (index_array > delay-1)
				{
					array_full = true;
					index_array = delay-1;
				}
				valid_speed = false;
			}

			cout << "speed : ";
			cout << speed.j1 << " ";
			cout << speed.j2 << " ";
			cout << speed.j3 << " ";
			cout << speed.j4 << " ";
			cout << speed.j5 << " ";
			cout << speed.j6 << " ";
			cout << speed.j7 << endl;

			//YARPConverter::ConvertFromCogToYARP (workImg, in_cart);
			YARPColorConverter::RGB2Grayscale (workImg, cartesian);
			lp.Cart2LpAverage (cartesian, logpolar);

			if (!first_frame)
			{
				fo_flow.Apply (logpolar, mask);
				result = fo_flow.GetFlowComp();

				cout << "flow : " << result(1) << " " << result(2) << endl;

				if (valid_speed)
				{
					psi = 0;
					psi(1) = speed.j1;
					psi(2) = speed.j2;
					psi(3) = speed.j4;
					psi(4) = speed.j5;
					psi(5) = speed.j6;
					psi(6) = speed.j7;

					theta = estimator.Estimate (result(1), psi);

					psi = 0;
					psi(7) = speed.j1;
					psi(8) = speed.j2;
					psi(9) = speed.j4;
					psi(10) = speed.j5;
					psi(11) = speed.j6;
					psi(12) = speed.j7;

					theta = estimator.Estimate (result(2), psi);
				}

				cout << "theta : ";
				for (int i = 1; i <= num_params; i++)
				{
					cout << theta(i) << " ";
				}
				cout << endl;
			}
			else
			{
				// first frame.
				fo_flow.Init (logpolar);
				first_frame = false;
			}
		}
	}
}

