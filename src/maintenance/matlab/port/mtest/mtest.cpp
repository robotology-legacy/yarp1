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
/// $Id: mtest.cpp,v 1.4 2003-11-20 15:35:54 gmetta Exp $
///
///


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPPort.h>
#include <YARPSemaphore.h>
#include <YARPThread.h>
#include <YARPMath.h>
#include <YARPVectorPortContent.h>
#include <YARPImage.h>
#include <YARPImagePortContent.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPTime.h>

///
///
///
int main (int argc, char *argv[])
{
	YARPOutputPortOf<YARPBottle> port;
	YARPInputPortOf<YARPGenericImage> iport;

	port.Register ("/mtest/o:bottle", "local");
	iport.Register ("/mtest/i:img", "local");

	YarpPixelBGR red(255, 0, 0);
	YarpPixelBGR green(0, 255, 0);

	YARPImageOf<YarpPixelBGR> img;
	img.Resize (128, 128);
	img.Zero();
	img(32, 32) = red;
	img(1, 62) = green;

	YARPBottle bottle;
	bottle.setID("PIPPO_ID");
	YVector a(2);
	a(1) = 10;
	a(2) = 20;

	for (int i = 0;; i++)
	{
		printf ("about to write %d\n", i);

		bottle.rewind();
		bottle.writeInt (i);
		bottle.writeFloat (5.0);
		bottle.writeText ("pippo");
		bottle.writeVocab ("xxxx-xxxx");
		bottle.writeYVector (a);

		port.Content() = bottle;
		port.Write();
		YARPTime::DelayInSeconds(4.0);

///		iport.Read();
///		v2 = iport.Content();
///		printf ("got reply %lf and size %d\n", v2(1), v2.Length());
	}

	return 0;
}