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
/// $Id: attn_egomap.cpp,v 1.4 2004-01-20 19:31:06 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPBottle.h>
#include <conf/YARPVocab.h>

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

#include <YARPList.h>

///
///
///
const char *DEFAULT_NAME = "/egomap";
const int ISIZE = 128;
const int FULLSIZE = 256;

///
///
///

using namespace _logpolarParams;

struct ObjectListEntry
{
	YARPString key;
	YVector v;
};

class ObjectList: YARPList<ObjectListEntry>
{
	ObjectList():_sema(1)
	{}

	void add(const YARPString &key, YVector &v)
	{
		// LATER
	}
	void remove(const YARPString &key)
	{

	}

	void setCurrent(const YARPString &key)
	{
		sema.Wait();
		// search and replace _current
		// LATER _current = ;
		_sema.Post();
	}

	void getCurrent(YVector curr)
	{
		_sema.Wait();
		curr = _current.v;
		_sema.Post();
	}

private:
	ObjectListEntry _current;
	YARPSemaphore _sema; // ??
};

class EgoMap: public YARPInputPortOf<YARPBottle>
{
	
protected:
	YARPInputPortOf<YVector> _inPortPosition;
	YARPInputPortOf<YVector> _outPortPosition;
	YARPInputPortOf<YARPGenericImage> _inPortImage;
	YARPOutputPortOf<YARPGenericImage> _outPortImage;
	YARPBabybotHeadKin _gaze;

	YARPImageOf<YarpPixelMono> _inImg;
	YARPImageOf<YarpPixelBGR> _outImg;
	YARPImageOf<YarpPixelBGR> _coloredImg;
	YARPImageOf<YarpPixelBGR> _remappedImg;

	YVector _out;

	YARPLogpolar _mapper;
	
public:
	EgoMap(const YARPString &baseName):
	YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
	_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
	_inPortPosition(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
	_inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
	_outPortPosition(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
	_outPortImage(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		Register(YARPString(baseName).append("/cmd/i").c_str());
		_inPortPosition.Register(YARPString(baseName).append("/head/i").c_str());
		_inPortImage.Register(YARPString(baseName).append("/image/i").c_str());
		_outPortPosition.Register(YARPString(baseName).append("/target/o").c_str());
		_outPortImage.Register(YARPString(baseName).append("/image/o").c_str());
		
		_out.Resize(2);
		_out = 0.0;

		_coloredImg.Resize (_stheta, _srho);
		_remappedImg.Resize (FULLSIZE, FULLSIZE);
		_outImg.Resize(FULLSIZE/2, FULLSIZE/2);
	}

	void loop()
	{
		_inPortPosition.Read();
		YVector &pos = _inPortPosition.Content();

		_gaze.update(pos);

		if (_inPortImage.Read(0))
		{
			_inImg.Refer (_inPortImage.Content());
			_mapper.ReconstructColor (_inImg, _coloredImg);
			_mapper.Logpolar2Cartesian (_coloredImg, _remappedImg);

			YARPSimpleOperation::Decimate (_remappedImg, _outImg, 2, 2);
		}
		
		// loop all objects
		{
			int predx = 0, predy = 0;
			YVector v(3);
			// LATER fill v(3)
			_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, v, predx, predy);

			YARPSimpleOperation::DrawCross(_outImg, predx/2, predy/2, YarpPixelBGR(0, 255, 0), 10, 2);

			_out(1) = predx-128;
			_out(2) = predy-128;
			// LATER: plot cross for each obj
		}
		
		// write position
		_outPortPosition.Content() = _out;


		// send image
		_outPortImage.Content().Refer(_outImg);
		_outPortImage.Write();
	}

	void OnRead()
	{
		Read();
		YARPBottle &tmpBottle = Content();

		YBVocab tmp;
		if (!tmpBottle.tryReadVocab(tmp))
			return;	
			
		tmpBottle.moveOn();
		
		if (tmp == YBVEgoMapAdd)
		{
			char key[80];
			int x,y;
			tmpBottle.readText(key);
			tmpBottle.readInt(&x);
			tmpBottle.readInt(&y);
			_handleAdd(key, x, y);
		}
		if (tmp == YBVEgoMapRemove)
		{
			char key[80];
			tmpBottle.readText(key);
			_handleRemove(key);
		}
		if (tmp == YBVEgoMapSetCurrent)
		{
			char key[80];
			tmpBottle.readText(key);
			_handleSetCurrent(key);
		}
	}

private:
	void _handleAdd(const YARPString &key, int x, int y)
	{
		// LATER ADD
		// convert x,y, into v and add it
	}
	void _handleRemove(const YARPString &key)
	{
		// LATER
	}
	void _handleSetCurrent(const YARPString &key)
	{
		// LATER
	}
};


int main(int argc, char *argv[])
{
	EgoMap _map("/egomap");

	while(1)
	{
		_map.loop();
	}

	return 0;
}
