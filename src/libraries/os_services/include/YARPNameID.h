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
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNameID.h,v 1.10 2003-07-08 22:04:20 gmetta Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNameID_INC
#define YARPNameID_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/MEM_Addr.h>

#include "YARPAll.h"
#include "YARPNameID_defs.h"

#include <string>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
/// Defines the endpoint name id. encapsulates the socket id and network mode flag.
///	Pasa: moved int variables to protected.
///
class YARPNameID
{
protected:
	int _mode;			// mode socket, Qnet, etc.
	ACE_HANDLE _raw_id;	// OS id, e.g. socket id.

public:
	YARPNameID () { _raw_id = ACE_INVALID_HANDLE;  _mode = YARP_NO_SERVICE_AVAILABLE; }
	YARPNameID (int n_mode, ACE_HANDLE n_raw_id) { _mode = n_mode;  _raw_id = n_raw_id; }
	virtual ~YARPNameID() {}

	int operator == (const YARPNameID& other) { return (_raw_id == other._raw_id) && (_mode == other._mode); }
	int operator != (const YARPNameID& other) { return !(operator == (other)); }

	inline int getServiceType (void) const { return _mode; }
	inline void setServiceType (int type) { _mode = type; }
	inline ACE_HANDLE getRawIdentifier (void) const { return _raw_id; }
	inline void setRawIdentifier (ACE_HANDLE id) { _raw_id = id; }
	inline int isValid(void) const { return (_raw_id != ACE_INVALID_HANDLE) ? 1 : 0; }
	inline int isError(void) const { return (_raw_id == ACE_INVALID_HANDLE); }
	inline int isConsistent(int n_mode) const { return (_mode == n_mode); }
	void invalidate() { _raw_id = ACE_INVALID_HANDLE;  _mode = YARP_NO_SERVICE_AVAILABLE; }
};

#define YARP_NAMEID_NULL YARPNameID()

///
///
///
///
///

class YARPUniqueNameID : public YARPNameID
{
protected:
	std::string _symbolic_name;

public:
	YARPUniqueNameID (void) : YARPNameID() { _symbolic_name = "__null"; }
	YARPUniqueNameID (int service) : YARPNameID(service, ACE_INVALID_HANDLE) { _symbolic_name = "__null"; }
	virtual ~YARPUniqueNameID() {}

	inline YARPNameID& getNameID(void) { return (YARPNameID &)(*this); }
	inline void setName (const char *name) { _symbolic_name = name; }
	inline void setName (const std::string& name) { _symbolic_name = name; }
	inline std::string getName(void) const { return _symbolic_name; }

	YARPUniqueNameID& operator= (const YARPUniqueNameID& other) 
	{
		_symbolic_name = other._symbolic_name;
		YARPNameID::operator= (other);
		return *this;
	}
};



///
///	
///
///

class YARPUniqueNameQnx : public YARPUniqueNameID
{
protected:
	int _pid;
	int _channel_id;
	std::string _node;

public:
	YARPUniqueNameQnx (int service = YARP_QNET) : YARPUniqueNameID(service)
	{
		ACE_ASSERT (service == YARP_QNET || service == YARP_NO_SERVICE_AVAILABLE);
		_pid = -1;
		_channel_id = -1;
	}

	YARPUniqueNameQnx (int service, int pid, int channel, std::string node) : YARPUniqueNameID(service)
	{
		_pid = pid;
		_channel_id = channel;
		_node = node;
	}

	virtual ~YARPUniqueNameQnx() {}

	int getPid() const { return _pid; }
	int getChannelID() const { return _channel_id; }
	void setPid(int pid) { _pid = pid; }
	void setChannelID(int channel) 
	{ 
		_channel_id = channel; 
		setRawIdentifier ((ACE_HANDLE)channel);
	} 
	std::string getNode (void) const { return _node; }
	void setNode (const std::string& node) { _node = node; }
};


class YARPUniqueNameSock : public YARPUniqueNameID
{
protected:
	ACE_INET_Addr _address;
	int _nports;
	int *_ports;

public:
	YARPUniqueNameSock (int service = YARP_MCAST) : YARPUniqueNameID(service), _address(1111) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}
	
	YARPUniqueNameSock (int service, int port, char *hostname) : YARPUniqueNameID(service), _address (port, hostname) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}
	
	YARPUniqueNameSock (int service, const ACE_INET_Addr& addr) : YARPUniqueNameID(service), _address (addr) 
	{
		ACE_ASSERT (service != YARP_QNET);
		_nports = -1;
		_ports = NULL;
	}

	YARPUniqueNameSock (const YARPUniqueNameSock *other) : YARPUniqueNameID (other->getServiceType()), _address (other->_address)
	{
		_nports = -1;
		_ports = NULL;
	}

	virtual ~YARPUniqueNameSock() 
	{
		if (_ports != NULL)	delete[] _ports;
	}

	inline ACE_INET_Addr& getAddressRef (void) { return _address; }

	inline int& getNPorts (void) { return _nports; }
	inline int* getPorts (void) { return _ports; }

	inline int setPorts (int *p, int size) 
	{
		ACE_ASSERT (p != NULL && size > 0);

		if (_nports != size)
			delete[] _ports;
		_nports = size;
		_ports = new int[_nports];
		ACE_ASSERT (_ports != NULL);

		memcpy (_ports, p, sizeof(int) * _nports); 
		return YARP_OK; 
	}
	
	inline int releasePorts (void)
	{
		if (_ports != NULL)
			delete[] _ports;
		_ports = NULL;
		_nports = -1;

		return YARP_OK;
	}

	YARPUniqueNameSock& operator= (const YARPUniqueNameSock& other)
	{
		if (_nports != other._nports)
		{
			delete[] _ports;
			_nports = other._nports;
			if (_nports > 0)
			{
				_ports = new int[_nports];
				ACE_ASSERT (_ports != NULL);
			}
			else
				_ports = NULL;
		}

		if (_nports > 0)
		{
			memcpy (_ports, other._ports, sizeof(int) * _nports);
		}

		_address = other._address;

		YARPUniqueNameID::operator= (other);
		return *this;
	}
};


class YARPUniqueNameMem : public YARPUniqueNameID
{
protected:
	ACE_MEM_Addr _address;

public:
	YARPUniqueNameMem (int service = YARP_SHMEM) : YARPUniqueNameID(service), _address() {}
	YARPUniqueNameMem (int service, int port) : YARPUniqueNameID(service), _address (port) {}
	YARPUniqueNameMem (int service, const ACE_MEM_Addr& addr) : YARPUniqueNameID(service), _address (addr) {}
	YARPUniqueNameMem (const YARPUniqueNameMem *other) : YARPUniqueNameID(YARP_SHMEM), _address(other->_address) {}
	
	virtual ~YARPUniqueNameMem() {}

	inline ACE_MEM_Addr& getAddressRef (void) { return _address; }
};


/// service types.
///
///	YARP_NO_SERVICE_AVAILABLE	= -1,
///	YARP_UDP					= 0,
///	YARP_TCP					= 1,
///	YARP_MCAST					= 2,
///	YARP_QNET					= 3,
/// YARP_SHMEM					= 4,
///


///
/// contains the assigned port/IP address.
///	a bit more generic than the address/port pair only.
///
///

#endif
