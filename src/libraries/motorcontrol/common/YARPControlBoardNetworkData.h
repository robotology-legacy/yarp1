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

#include <conf/YARPConfig.h>
#include <ace/config.h>

#include "YARPPort.h"
#include "YARPPortContent.h"

#include <YARPRobotMath.h>
#include <YARPConfigFile.h>

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

	virtual ~YARPInputPortOf<YARPControlBoardNetworkData> () { ((Port *)system_resource)->End(); }
};

class YARPOutputPortOf<YARPControlBoardNetworkData> : public YARPBasicOutputPort<YARPControlBoardNetworkPortContent>
{
public:
	YARPOutputPortOf<YARPControlBoardNetworkData>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPControlBoardNetworkPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YARPControlBoardNetworkData> () { ((Port *)system_resource)->End(); }
};

#endif //.h