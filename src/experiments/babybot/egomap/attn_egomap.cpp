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
/// $Id: attn_egomap.cpp,v 1.7 2004-05-05 17:55:36 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
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
const char *__imageNetName = "Net1";
const int DECIMATE = 2;

using namespace _logpolarParams;

struct ObjectListEntry
{
	YARPString key;
	YVector v;
};

typedef YARPList<ObjectListEntry> OBJECT_LIST;
typedef OBJECT_LIST::iterator OBJECT_LIST_IT;

class ObjectList: public OBJECT_LIST
{
public:
	ObjectList()
	{
		_current.v.Resize(3);
		_current.key = "Null";
	}

	void add(const YARPString &key, const YVector &v)
	{
		remove(key);
		ObjectListEntry newObj;
		newObj.key = key;
		newObj.v = v;

		push_back(newObj);
	}
	void remove(const YARPString &key)
	{
		OBJECT_LIST_IT it(*this);
		while(!it.done())
		{
			if ((*it).key == key)
				it.remove();
			it++;
		}
	}

	void setCurrent(const YARPString &key)
	{
		OBJECT_LIST_IT it(*this);
		while(!it.done())
		{
			if ((*it).key == key)
			{
				_current.key = (*it).key;
				_current.v = (*it).v;
				break;
			}
			it++;
		}
	}

	void getCurrent(YVector &curr)
	{ curr = _current.v; }

private:
	ObjectListEntry _current;
};

class EgoMap: public YARPInputPortOf<YARPBottle>
{
protected:
	YARPInputPortOf<YVector> _inPortPosition;
	YARPOutputPortOf<YVector> _outPortPosition;
	YARPInputPortOf<YARPGenericImage> _inPortImage;
	YARPOutputPortOf<YARPGenericImage> _outPortImage;
	YARPBabybotHeadKin _gaze;

	YARPImageOf<YarpPixelMono> _inImg;
	YARPImageOf<YarpPixelBGR> _outImg;
	YARPImageOf<YarpPixelBGR> _coloredImg;
	YARPImageOf<YarpPixelBGR> _remappedImg;

	YVector _out;
	YARPLogpolar _mapper;
	ObjectList _objList;
	
	YARPSemaphore _sema;
	
public:
	EgoMap(const YARPString &baseName):
	YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	_sema(1),
	_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
	_inPortPosition(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
	_inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
	_outPortPosition(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
	_outPortImage(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		Register(YARPString(baseName).append("/cmd/i").c_str());
		_inPortPosition.Register(YARPString(baseName).append("/head/i").c_str());
		_inPortImage.Register(YARPString(baseName).append("/image/i").c_str(), __imageNetName);
		_outPortPosition.Register(YARPString(baseName).append("/target/o").c_str());
		_outPortImage.Register(YARPString(baseName).append("/image/o").c_str());
		
		_out.Resize(2);
		_out = 0.0;

		_coloredImg.Resize (_stheta, _srho);
		_remappedImg.Resize (_xsize, _ysize);
		_outImg.Resize(_xsize/DECIMATE, _ysize/DECIMATE);
	}

	void loop()
	{
		_inPortPosition.Read();
		YVector &pos = _inPortPosition.Content();

		_lock();
			_gaze.update(pos);
		_unlock();

		if (_inPortImage.Read(0))
		{
			_inImg.Refer (_inPortImage.Content());
			_mapper.ReconstructColor (_inImg, _coloredImg);
			_mapper.Logpolar2Cartesian (_coloredImg, _remappedImg);
		}
		
		// loop all objects

		_lock();
		OBJECT_LIST_IT it(_objList);
		while(!it.done())
		{
			int predx = 0, predy = 0;
			YVector &v = (*it).v;
			_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, v, predx, predy);

			YARPSimpleOperation::DrawCross(_remappedImg, predx, predy, YarpPixelBGR(200, 0, 0), 8, 3);

			it++;
		}

		YARPSimpleOperations::Decimate (_remappedImg, _outImg, DECIMATE, DECIMATE);

		YVector v(3);
		_objList.getCurrent(v);
		int tmpx, tmpy;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, v, tmpx, tmpy);
		_out(1) = tmpx-_xsize/2;
		_out(2) = tmpy-_ysize/2;

		_unlock();
		// write position
		_outPortPosition.Content() = _out;
		_outPortPosition.Write();
				
		// send image
		_outPortImage.Content().Refer(_outImg);
		_outPortImage.Write();
	}

	virtual void OnRead(void)
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
			if (!tmpBottle.tryReadText(key))
				return;
			tmpBottle.moveOn();
			tmpBottle.readInt(&x);
			tmpBottle.readInt(&y);

			ACE_OS::printf("EgoMap: adding %s %d,%d\n", key, x, y);
			_handleAdd(key, x, y);
		}
		if (tmp == YBVEgoMapRemove)
		{
			char key[80];
			if (!tmpBottle.tryReadText(key))
				return;
			tmpBottle.moveOn();
			ACE_OS::printf("EgoMap: removing %s\n", key);
			_handleRemove(key);
		}
		if (tmp == YBVEgoMapSetCurrent)
		{
			char key[80];
			if (!tmpBottle.tryReadText(key))
				return;
			tmpBottle.moveOn();
			ACE_OS::printf("EgoMap: setting current as %s\n", key);
			_handleSetCurrent(key);
		}
	}

private:
	void inline _lock()
	{ _sema.Wait(); }

	void inline _unlock()
	{ _sema.Post(); }

	void _handleAdd(const YARPString &key, int x, int y)
	{
		_lock();
			YVector v(3);
			_gaze.computeRay (YARPBabybotHeadKin::KIN_LEFT_PERI, v, x, y);
			_objList.add(key, v);
		_unlock();
	}
	void _handleRemove(const YARPString &key)
	{
		_lock();
			_objList.remove(key);
		_unlock();
	}
	void _handleSetCurrent(const YARPString &key)
	{
		_lock();
			_objList.setCurrent(key);
		_unlock();
	}
};


int main(int argc, char *argv[])
{
	EgoMap _map(DEFAULT_NAME);

	while(1)
	{
		_map.loop();
	}

	return 0;
}
