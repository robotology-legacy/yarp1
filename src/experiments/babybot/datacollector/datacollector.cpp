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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: datacollector.cpp,v 1.3 2004-08-02 09:16:15 babybot Exp $
/// 

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPImages.h>
#include <yarp/YARPControlBoardNetworkData.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPLogpolar.h>
#include <yarp/YARPMath.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPLogFile.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageUtils.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPList.h>
#include <yarp/YARPTime.h>

class MyPort
{
public:
	MyPort(){}
	~MyPort(){}
	void setDefault(int n, const double *val)
	{
		_datum = YVector(n,val);
		_howMany = 0;
	}

	void setPath(const YARPString &path, const YARPString &file)
	{
		char tmpName[255];
		sprintf(tmpName, "%s%s", path.c_str(), file.c_str());
		_log.open(tmpName);
	}

	virtual void getData(YVector &v)
	{
		_mutex.Wait();
			v = _datum;
		_mutex.Post();
	}

	virtual void Register(const YARPString &name, const YARPString &net) = 0;

	void dump()
	{
		_log.dump(_datum);
		_log.newLine();
	}

	void update()
	{
		_howMany++;
	}

	int howMany(){ return _howMany; }

	YARPSemaphore _mutex;
	YVector _datum;
	int _howMany;

public:
	YARPLogFile _log;
};

class Port1: public YARPInputPortOf<YVector>, public MyPort
{
	public:
	Port1(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL):YARPInputPortOf<YVector>(n_service_type, n_protocol_type)
	{}
	~Port1(){}

	virtual void OnRead(void)
	{
		Read();
		_datum = Content();
		update();
	}

	virtual void Register(const YARPString &name, const YARPString &net)
	{
		YARPInputPortOf<YVector>::Register(name.c_str(), net.c_str());
	}
};

class Port2: public YARPInputPortOf<YARPControlBoardNetworkData>, public MyPort
{
	public:
	Port2(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL):YARPInputPortOf<YARPControlBoardNetworkData>(n_service_type, n_protocol_type)
	{}
	~Port2(){}

	virtual void OnRead(void)
	{
		Read();
		_datum = Content()._current_position;
		update();
	}

	virtual void Register(const YARPString &name, const YARPString &net)
	{
		YARPInputPortOf<YARPControlBoardNetworkData>::Register(name.c_str(), net.c_str());
	}
};


typedef YARPList<MyPort *> PORT_LIST;
typedef PORT_LIST::iterator PORT_LIST_IT;

PORT_LIST _portList;
YARPString _baseName;
YARPString _path;

// register command macro, for now verbose is equal to VOCABENTRY
void addPort(MyPort *p, int n, const double *def, const YARPString &name, const YARPString &netName, const YARPString &path, const YARPString &filename, PORT_LIST &list = _portList)
{ 
	YARPString portName = _baseName;
	portName.append(name);

	p->Register(portName.c_str(), netName.c_str());
	p->setPath(path, filename);
	p->setDefault(n, def);
	list.push_back(p);
}

// Port1 _retinalPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
Port2 _armPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
Port1 _headPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
Port1 _handPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
Port1 _touchPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);

YARPInputPortOf<YARPGenericImage> _rightImagePort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
YARPInputPortOf<YARPGenericImage> _leftImagePort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);

const double dummy[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

// ADD HERE YOUR PORT
void _fillPortList()
{
	addPort(&_armPort, 6, dummy, "/arm/i", "Net0", _path, "arm.dat");
	addPort(&_headPort, 5, dummy, "/head/i", "Net0", _path, "head.dat");
	addPort(&_handPort, 16, dummy, "/hand/i", "Net0", _path, "hand.dat");
	addPort(&_touchPort, 17, dummy, "/touch/i", "Net0", _path, "touch.dat");
}

///
const char *DEFAULT_NAME = "/datacollector";
const char *DEFAULT_FOLDER = "C:/temp/";

const int DEFAULT_LENGTH = 30;	// seconds
const double timeFrame = 0.04;	// seconds

///
void printFrame(int c, int &time1);

void dumpAll(PORT_LIST &list)
{
	PORT_LIST_IT it(list);

	while(!it.done())
	{
		MyPort *p = (*it);
		p->dump();
		it++;
	}
}

int main(int argc, char *argv[])
{
	using namespace _logpolarParams;
	YARPLogpolar mapper;

	YARPString network_i;
	YARPString network_o;

	int length;

	if (!YARPParseParameters::parse(argc, argv, "-name", _baseName))
	{
		_baseName = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "-neti", network_i))
	{
		network_i = "Net1";
	}

	if (!YARPParseParameters::parse(argc, argv, "-neto", network_o))
	{
		network_o = "default";
	}

	if (!YARPParseParameters::parse(argc, argv, "-dir", _path))
	{
		_path = YARPString(DEFAULT_FOLDER);
	}

	if (!YARPParseParameters::parse(argc, argv, "-length", &length))
	{
		length = DEFAULT_LENGTH;
	}

	// register image ports
	YARPString tmp;
	tmp = _baseName;
	tmp.append("/right/i:img");
	_rightImagePort.Register(tmp.c_str(), "Net1");
	tmp = _baseName;
	tmp.append("/left/i:img");
	_leftImagePort.Register(tmp.c_str(), "Net1");
	
	_fillPortList();
	/////////////////////////////////////////////

	int frameCounter = 0;
	int nFrame = (int) length/timeFrame;

	char tmpRightName[255];
	char tmpLeftName[255];
	
	ACE_OS::printf("Going to save %d frames (estimated time %d) on %s\n", nFrame, length, _path.c_str());
	ACE_OS::printf("Please connect me...\n");

	int time1;
	float deltaT = 0;
	
	time1 = YARPTime::GetTimeAsSeconds ();
	while (frameCounter < nFrame)
	{		
		
		_leftImagePort.Read();
		_rightImagePort.Read();

		sprintf(tmpLeftName, "%sleft%d.pgm", _path.c_str(), frameCounter);
		sprintf(tmpRightName, "%sright%d.pgm", _path.c_str(), frameCounter);
		YARPImageFile::Write(tmpLeftName, _leftImagePort.Content());
		YARPImageFile::Write(tmpRightName, _rightImagePort.Content());

		dumpAll(_portList);

		printFrame(frameCounter, time1);
		frameCounter++;
	}

	return 0;
}

void printFrame(int c, int &time1)
{	
	if (c%100 == 0)
	{
		ACE_OS::printf("Frame #:%d\n", c);
		int time2 = YARPTime::GetTimeAsSeconds ();
		float deltaT = (time2 - time1)/(float) (100);
		ACE_OS::printf("Estimated time frame %lf\n", deltaT);
		time1 = time2;
	}
}
