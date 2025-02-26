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
///                    #original paulfitz, changed pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: attn_trackercloned.cpp,v 1.5 2005-06-17 14:27:45 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPBottle.h>

#include <yarp/YARPFineOrientation.h>

#include "ImgTrack.h"
#include "find_flipper.h"

// stupid windows
#define for if(1) for

///#define BYPASS

///
///
///
const int __numberOfTrials = 3;
const int __numOfSaccades = 3;
const int __delay = 10;

YARPInputPortOf<YARPGenericImage> in_img; //YARPInputPort::NO_BUFFERS);
YARPOutputPortOf<YARPGenericImage> out_img;

static int global_grasping = 0;
static YARPSemaphore global_mutex(1);
static int global_search = 1;
static int global_trials = __numberOfTrials;
static int global_saccading = 1;
static int global_delay = __delay;

#define TRACKER_CLONED_SILENT

class MyBot : public YARPInputPortOf<YARPBottle> 
{
public:
	MyBot():YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP){}
public:
	virtual void OnRead() 
	{
		if (Read()) 
		{
			YARPBottle& bot = Content();
			// interested in:
			// KFStart
			// KFStop
			// ReachingAbort
			// ReachingAck
			YBVocab vocab;
			if (bot.tryReadVocab(vocab)) 
			{
				// we have a vocab
				if (vocab == YBVKFStart)
				{
					printf("Cool... I have a new object\n");
					global_search = 0;	// inhibit search
					global_grasping = 0;
				}
				if (vocab == YBVKFTrainStart) 
				{
					printf("Beginning train\n");
					global_mutex.Wait();
					global_grasping = 1;
					global_search = 0;
					global_mutex.Post();
				}
				if (vocab == YBVKFTrainStop) 
				{
					global_mutex.Wait();
					global_grasping = 0;
					global_search = 0;
					global_mutex.Post();
					printf("Ending training\n");
				}
				if (vocab == YBVReachingAbort)
				{
					printf("Reaching was aborted\n");
					global_mutex.Wait();
					printf("Trials: %d (0 means stop)\n", global_trials);
					if (global_trials > 0)
					{
						printf("... still willing to try again: begin searching\n");
						global_search = __numOfSaccades;	// start searching
					}
					else
						printf("I'm bored, may I ask another object ?\n");
					global_mutex.Post();
				}
				if (vocab == YBVReachingAck)
				{
					printf("Reaching was started, waiting\n");
					global_mutex.Wait();
					global_search = 0;
					global_trials--;
					printf("Trials: %d (0 means stop)\n", global_trials);
					global_mutex.Post();
				}
				if (vocab == YBVKFStop) 
				{
					printf("Beginning searching for a target\n");
					global_mutex.Wait();
					global_grasping = 0;
					global_search = __numOfSaccades;
					global_delay = __delay;
					global_trials = __numberOfTrials;
					printf("Trials: %d (0 means stop)\n", global_trials);
					global_mutex.Post();
					printf("Ending exploration\n");
				}
				if (vocab == YBVReachingSuccess)
				{
					printf("Good news ! I was told reaching was successful, we can stop searching\n");
					global_mutex.Wait();
					global_search = 0;	// inhibit search
					global_grasping = 0;
					global_trials = 0;
					global_mutex.Post();
				}
				if (vocab == YBVReachingFailure)
				{
					printf("Doh ! I was told reaching failed, searching again\n");
					global_mutex.Wait();
					if (global_trials > 0)
						global_search = __numOfSaccades;	// start searching
					else
					{
						printf("On second thought... I give up\n");
						printf("May I ask another object ?\n");
					}

					global_grasping = 0;
					global_mutex.Post();
				}
				if (vocab == YBVSaccadeFixated)
				{
					global_mutex.Wait();
						global_saccading = 0;
						printf("Fixated (saccading = %d)\n", global_saccading);
					global_mutex.Post();
				}
			}
		}
	}
} in_bot;

YARPOutputPortOf<YARPBottle> out_bot;

const char *DEFAULT_NAME = "/trackercloned";

///
///
///
///
extern int flipper_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	//set_yarp_debug(100,100);
	printf("Greetings; it begins\n");
	YARPFineOrientation fine;
	printf("Greetings; it begins 2\n");
	fine.Init("y:/conf/babybot/orient.txt");
	printf("Greetings; it begins 3\n");
	
	int ac = 1;
	char *av[] = { "main", "foo.txt", NULL };

	//flipper_main(argc,argv);
	flipper_main(ac,av);
	int use_wide = 1;

	YARPString name;
	YARPString network_i;
	YARPString network_o;
	char buf[256];

	if (!YARPParseParameters::parse(argc, argv, "-name", name))
	{
		name = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "-neti", network_i))
	{
		network_i = "default";
	}

	if (!YARPParseParameters::parse(argc, argv, "-neto", network_o))
	{
		network_o = "default";
	}

	/// images are coming from the input network.
	sprintf(buf, "%s/i:img", name.c_str());
	in_img.Register(buf, network_i.c_str());

	sprintf(buf, "%s/o:img", name.c_str());
	out_img.Register(buf, network_i.c_str());
	out_img.SetRequireAck(1);

	sprintf(buf, "%s/i:bot", name.c_str());
	in_bot.Register(buf, "default");

	sprintf(buf, "%s/o:bot", name.c_str());
	out_bot.Register(buf, "default");

	///
	///
	///
	YARPImageOf<YarpPixelBGR> in;
	YARPImageOf<YarpPixelBGR> out;
	YARPImageOf<YarpPixelBGR> remapped;

	static double train_time = -10000;
	static double down_time = -10000;
	while(1)
    {
		double now = YARPTime::GetTimeAsSeconds();
		static double first_time = now;
		int tick = (int)(now-first_time)/10;
		use_wide = (tick%2);
		use_wide = 1;

		static int msg0 = 1;
		if (msg0) { printf("Waiting for an image...\n"); msg0=0; }
		
		global_mutex.Wait();
		int grasping = global_grasping;
		int trials = global_trials;
		global_delay--;
		if (global_delay<0)
			global_delay = 0;
		global_mutex.Post();

		in_img.Read();
		if (!grasping) 
		{
			in_img.Read();
			in_img.Read();
			in_img.Read();

			static int msg1 = 1;
			in.CastCopy (in_img.Content());
			IMGFOR(in,x,y) 
			{
				YarpPixelBGR& pix = in(x,y);
				if (0) 
				{ //(y<64) { //pix.r>250 && pix.g>250 && pix.b>250) {
					pix.r = pix.g = pix.b = 0;
				}
			}
#ifndef TRACKER_CLONED_SILENT
			if (msg1) { printf("Got an image %dx%d\n",in.GetWidth(),in.GetHeight()); msg1 = 0; }
#endif

			SatisfySize (in, remapped);
			remapped.CastCopy(in);

			out_img.Content().SetID (YARP_PIXEL_BGR);
			SatisfySize (remapped, out_img.Content());

			int xx = 128, yy = 128, ff = 0;
#ifndef BYPASS
			flipper_apply(remapped,out, xx, yy, ff);
			#ifndef TRACKER_CLONED_SILENT
				printf("Done with flipper_apply()\n");
			#endif
			out_img.Content().PeerCopy(out);
#else
			out = remapped;
			out_img.Content().PeerCopy(out);
#endif


			static int at = 0;
			at = (at+1)%out.GetHeight();

			#ifndef TRACKER_CLONED_SILENT
				printf("Writing image of size %d %d\n",
						out_img.Content().GetWidth(),
						out_img.Content().GetHeight());
			#endif
			out_img.Write ();

			double now = YARPTime::GetTimeAsSeconds();
			static int fixct = 0;
			if (now-train_time<3) 
			{
				train_time = -100000;
				down_time = now;
			}

			global_mutex.Wait();
		//	printf("%d %d search:%d (search = 1 means grasp)\n", xx, yy, global_search);
			if ((global_search>=1)&&(global_search<=__numOfSaccades) && (xx>=0 && yy>=0 && xx<256 && yy<256) &&
				(global_delay <= 0)&&
				(global_saccading != 1) )
			{
				// if (now-down_time<20 && now-down_time>3) 
				global_saccading = 1;
				
				int search = global_search;
				global_search--;
				if (global_search <= 0)
					global_search = 0;
				global_mutex.Post();
							
				down_time = now-1.5;
				printf("Saccading to %d %d search:%d (search = 1 means grasp)\n", xx, yy, search);
				out_bot.Content().rewind();
				out_bot.Content().writeInt(xx);
				out_bot.Content().writeInt(yy);

				if (search != 1)
					out_bot.Content().writeInt(255);		// no grasping

				out_bot.Write();
			}
			else
				global_mutex.Post();
		} 
		else 
		{
			in.Refer (in_img.Content());
			out_img.Content().SetID (YARP_PIXEL_BGR);
			out_img.Content().CastCopy(in);
			out.Refer(out_img.Content());
			static int phase = 0;
			if (phase) 
			{
				for (int j=-10; j<=10; j++) 
				{
					for (int i=0; i<out.GetWidth(); i++) 
					{
						out.SafePixel(i,j+out.GetHeight()/2).r = 255;
					}
				}
			} 
			else 
			{
				for (int j=-10; j<=10; j++) 
				{
					for (int i=0; i<out.GetHeight(); i++) 
					{
						out.SafePixel(j+out.GetWidth()/2,i).r = 255;
					}
				}
			}

			phase = !phase;
			// pick a foveal region, any foveal region
			// prep a mask, leave some of the boundary so orientations
			// can be computed reasonably

			int xc = in.GetWidth()/2;
			int yc = in.GetHeight()/2;
			int dx = in.GetWidth()/8;
			int dy = in.GetHeight()/8;
			int dx2 = dx*2;
			int dy2 = dy*2;

			YarpPixelBGR pix1(255,255,0);
			YarpPixelBGR pix2(0,255,255);
			AddRectangle(out,pix2,xc,yc,dx+1,dy+1);
			AddRectangle(out,pix2,xc,yc,dx-1,dy-1);
			AddRectangle(out,pix2,xc,yc,dx2,dy2);

			static YARPImageOf<YarpPixelBGR> img,img2,fake_dest;
			static YARPImageOf<YarpPixelMono> mask,mask2;
			img.Resize(dx2*2,dy2*2);
			mask.Resize(dx2*2,dy2*2);
			img2.Resize(dx2*2,dy2*2);
			mask2.Resize(dx2*2,dy2*2);
			in.Crop(img,xc-dx2,yc-dy2);
			in.Crop(img2,xc-dx2,0);
			mask.Zero();
			mask2.Zero();
			
			for (int x=dx2-dx; x<=dx2+dx; x++) 
			{
				for (int y=dy2-dy; y<=dy2+dy; y++) 
				{
					mask(x,y) = 255;
				}
			}
			
			IMGFOR(mask2,x,y) 
			{
				if (y<mask2.GetHeight()*0.75) 
				{
					mask2(x,y) = 255;
				}
			}
			
			static int ct = 0;
			ct++;

			flipper_apply(img,mask,fake_dest);
			flipper_apply(img2,mask2,fake_dest,1);
			out_img.Write();
			train_time = YARPTime::GetTimeAsSeconds();

		} /* else grasping */

    } /* end while(1) */

	flipper_end();

	return 0;
}
