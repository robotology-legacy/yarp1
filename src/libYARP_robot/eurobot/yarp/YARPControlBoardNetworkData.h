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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPControlBoardNetworkData.h,v 1.1 2004-12-23 14:17:53 beltran Exp $
///
///
// ArmStatus class
// this class stores the status of the arm and should be used for transmission.
// 
//  
// June 2002 -- by nat
// Ported to YARP -- September 2003

#ifndef __yarpcontrolboardnetorkdata__
#define __yarpcontrolboardnetorkdata__

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>

#include <yarp/YARPRobotMath.h>
#include <yarp/YARPConfigFile.h>

class YARPControlBoardNetworkData
{
public:
	YARPControlBoardNetworkData(){};
	YARPControlBoardNetworkData(const YARPString &path, const YARPString &confFile)
	{ resize(path, confFile); };
	YARPControlBoardNetworkData(int rate, int njoints)
	{ resize(rate, njoints);	}
	
	~YARPControlBoardNetworkData(){}

	// copy function
	YARPControlBoardNetworkData &operator=(const YARPControlBoardNetworkData &d)
	{
		if (_nj != d._nj)
			resize((int) d._deltaT*1000, d._nj);

		_current_position = d._current_position;
		_torques = d._torques;
		_old_position = d._old_position;
		_final_position = d._final_position;
		_velocity = d._velocity;
		_time = d._time;
		return *this;
	}

	void resize(const YARPString &path, const YARPString &confFile)
	{
		YARPConfigFile file;
		int nj;
		int rate;
		file.set(path, confFile);
		file.get("[GENERAL]", "Joints", &nj, 1);
		file.get("[THREAD]", "Rate", &rate, 1);
		resize(rate, nj);
	}
	void resize(int rate, int njoints)
	{
		_pidStatus = 0;
		_nj = njoints;
		_time = 0;	//reset time
		_deltaT = (double) rate/1000;
		
		_current_position.Resize(_nj);
		_old_position.Resize(_nj);
		_velocity.Resize(_nj);
		_final_position.Resize(_nj);
		_torques.Resize(_nj);

		length = _compute_size();
	}

	inline void tick() { _time++; }
	
	int size(void) const { return length; }
	
	////////////////////////////
	// these need to be sent !
	YVector _current_position;	// current position as read by the mei board
	YVector _torques;			// current torques
	YVector _old_position;		// old stored position
	YVector _final_position;	// final position
	YVector _velocity;			// current speed, as read by the mei board
	int _state;					// state
	unsigned int _time;			// very simple timer, maybe it will be useful
	////////////////////////////
	double _deltaT;
	int _nj;

	int _pidStatus;

	bool isPIDHigh()
	{
		if (_pidStatus == 1)
			return true;
		else
			return false;
	}

private:
	inline int _compute_size() const;
	int length;
};

inline int YARPControlBoardNetworkData::_compute_size() const {
	int tmp;
	tmp = 5*_nj*sizeof(double);		// current position, error, old_position, velocity
	
	// time
	tmp = tmp + sizeof(int);
	// state
	tmp += sizeof(int);
		
	return tmp;
}

#include "begin_pack_for_net.h"
class YARPCBNetPortContentHeader
{
public:
  NetInt32 njoints;
  NetInt32 rate;
} PACKED_FOR_NET;
#include "end_pack_for_net.h"

class YARPControlBoardNetworkPortContent : public YARPControlBoardNetworkData, public YARPPortContent
{
public:
	YARPCBNetPortContentHeader header;

	virtual ~YARPControlBoardNetworkPortContent () {}

	//// of course this only works on machines w/ the same floating point representation.
	virtual int Read (YARPPortReader& reader)
	{
		if (reader.Read ((char*)(&header), sizeof(header)))
		{
			if (header.njoints != _nj) 
				resize (header.rate, header.njoints);

			_deserialize(reader);
		}
		return 1;
	}

	virtual int Write (YARPPortWriter& writer)
	{
		header.njoints = _nj;
		header.rate = (int) _deltaT*1000;
		writer.Write ((char*)(&header), sizeof(header));

		_serialize(writer);
		return 1;
	}

	void _deserialize(YARPPortReader& reader)
	{
		//current position
		reader.Read((char *)_current_position.data(), sizeof(double)*_nj);
		//old position
		reader.Read((char *)_old_position.data(), sizeof(double)*_nj);
		//velocity
		reader.Read((char *)_final_position.data(), sizeof(double)*_nj);
		// error
		reader.Read((char *)_velocity.data(), sizeof(double)*_nj);
		// torques
		reader.Read((char *)_torques.data(), sizeof(double)*_nj);
		// time
		reader.Read((char *)&_time, sizeof(int));
		// state
		reader.Read((char *)&_state, sizeof(int));
	}

	void _serialize(YARPPortWriter& writer)
	{
		int offset = 0;
		//current position
		writer.Write((char *)_current_position.data(), sizeof(double)*_nj);
		//old position
		writer.Write((char *)_old_position.data(), sizeof(double)*_nj);
		//velocity
		writer.Write((char *)_final_position.data(), sizeof(double)*_nj);
		// error
		writer.Write((char *)_velocity.data(), sizeof(double)*_nj);
		// torques
		writer.Write((char *)_torques.data(), sizeof(double)*_nj);
		// time
		writer.Write((char *)&_time, sizeof(int));
		// state
		writer.Write((char *)&_state, sizeof(int));
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }

    YARPControlBoardNetworkData& operator=(const YARPControlBoardNetworkData &d)
	{return YARPControlBoardNetworkData::operator= (d); }
};

class YARPInputPortOf<YARPControlBoardNetworkData> : public YARPBasicInputPort<YARPControlBoardNetworkPortContent>
{
public:
	YARPInputPortOf<YARPControlBoardNetworkData>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPControlBoardNetworkPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YARPControlBoardNetworkData> () { YARPPort::End(); }
};

class YARPOutputPortOf<YARPControlBoardNetworkData> : public YARPBasicOutputPort<YARPControlBoardNetworkPortContent>
{
public:
	YARPOutputPortOf<YARPControlBoardNetworkData>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPControlBoardNetworkPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YARPControlBoardNetworkData> () { YARPPort::End(); }
};

#endif //.h