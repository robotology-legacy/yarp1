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
/// $Id: vergence.cpp,v 1.1 2003-11-20 17:46:58 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <YARPImages.h>

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

#include <YARPBabybotHeadKin.h>

///
///
///
YARPInputPortOf<YARPGenericImage> in_img;
YARPInputPortOf<YVector> in_pos;

YARPOutputPortOf<YARPGenericImage> out_img;

const char *DEFAULT_NAME = "/vergence";
const int ISIZE = 128;
const int FULLSIZE = 256;

///
///
///
class _procThread : public YARPThread
{
protected:
	YARPSemaphore _sema;
	bool _getcurrent;

	enum { MAXOBJ = 10 };
	YVector _rays[MAXOBJ];
	int _nextfree;

	YARPBabybotHeadKin _gaze;

public:
	_procThread () : 
		YARPThread(), 
		_sema(1), 
		_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) )
		{}

	virtual ~_procThread () {}

	void Body (void)
	{
		using namespace _logpolarParams;

		_sema.Wait();
		_getcurrent = false;
		_nextfree = 0;
		_sema.Post();

		///
		///
		///
		YARPImageOf<YarpPixelMono> in;
		YARPImageOf<YarpPixelBGR> out;
		YARPImageOf<YarpPixelBGR> colored;
		YARPImageOf<YarpPixelBGR> remapped;

		colored.Resize (_stheta, _srho);
		remapped.Resize (FULLSIZE, FULLSIZE);

		YARPLogpolar mapper;

		while (!IsTerminated())
		{
			in_img.Read();

			in.Refer (in_img.Content());
			mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)in, colored);
			mapper.Logpolar2Cartesian (colored, remapped);

			ACE_ASSERT (in.GetWidth() == _stheta && in.GetHeight() == _srho);
			ACE_ASSERT (remapped.GetWidth() == FULLSIZE && remapped.GetHeight() == FULLSIZE);

			out_img.Content().SetID (YARP_PIXEL_BGR);

			out_img.Content().Resize (ISIZE, ISIZE); 
			out.Refer (out_img.Content());

			/// copy reduced size into output img.
			YARPSimpleOperation::Decimate (remapped, out, 2, 2);

			in_pos.Read();
			YVector& jnts = in_pos.Content();

			_gaze.update (jnts);

			/// does the display.
			for (int i = 0; i < _nextfree; i++)
			{
				int predx = 0, predy = 0;
				_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _rays[i], predx, predy);
				///predx += FULLSIZE/2;
				///predy += FULLSIZE/2;

				///
				YarpPixelBGR green (0, 255, 0);
				AddCircleOutline (out, green, predx/2, predy/2, 5);
				AddCircleOutline (out, green, predx/2, predy/2, 4);
			}


			_sema.Wait();
			bool newpoint = _getcurrent;
			_getcurrent = false;
			_sema.Post();

			/// 
			///
			/// operations, add new vector.
			if (newpoint)
			{
				_rays[_nextfree].Resize(3);
				/// get the current ray.
				_gaze.computeRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _rays[_nextfree], FULLSIZE/2, FULLSIZE/2);
				/// the new ray is also stored.
				_nextfree++;
			}

			out_img.Write ();
		}

		/// returning smoothly.
	}

	void AddPointCurrent (void)
	{
		_sema.Wait();
		_getcurrent = true;
		_sema.Post();
	}
};



///
///
///
///
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

	/// images are coming from the input network.
	sprintf(buf, "%s/i:img", name.c_str());
	in_img.Register(buf, network_i.c_str());

	/// the position of the head gets here from the default network.
	sprintf(buf, "%s/i:headposition", name.c_str());
	in_pos.Register(buf, network_o.c_str());

	sprintf(buf, "%s/o:img", name.c_str());
	out_img.Register(buf, network_i.c_str());

	_procThread thread;
	thread.Begin();

	while (1)
	{
		char c;
		scanf ("%c", &c);

		switch (c)
		{
		case 'h':
		case '?':
			printf ("h,?: help, print this message\n");
			printf ("a: add current fixation point to the set of objects\n");
			printf ("q: quit\n");
			break;

		case 'q':
			goto ReturnSmoothly;
			break;

		case 'a':
			thread.AddPointCurrent ();
			break;
		}
	}


ReturnSmoothly:
	printf ("hopefully returning smoothly\n");

	thread.End();
	
	return 0;
}
