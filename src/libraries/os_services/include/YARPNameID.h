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
/// $Id: YARPNameID.h,v 1.4 2003-04-22 17:01:18 gmetta Exp $
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

#include "YARPAll.h"
#include "YARPNameID_defs.h"

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

	int operator == (const YARPNameID& other) { return (_raw_id == other._raw_id) && (_mode == other._mode); }
	int operator != (const YARPNameID& other) { return !(operator == (other)); }

	inline int getServiceType (void) const { return _mode; }
	inline ACE_HANDLE getRawIdentifier (void) const { return _raw_id; }
	inline void setRawIdentifier (ACE_HANDLE id) { _raw_id = id; }
	inline int isValid(void) const { return (_raw_id != ACE_INVALID_HANDLE) ? 1 : 0; }
	///int IsGeneric() { return raw_id == 0; } buggy maybe never used
	inline int isError(void) const { return (_raw_id == ACE_INVALID_HANDLE); }
	inline int isConsistent(int n_mode) const { return (_mode == n_mode) || (_mode == YARP_NO_SERVICE_AVAILABLE); }
	void invalidate() { _raw_id = ACE_INVALID_HANDLE;  _mode = YARP_NO_SERVICE_AVAILABLE; }
};

#define YARP_NAMEID_NULL YARPNameID()

///
/// contains the assigned port/IP address.
///	a bit more generic than the address/port pair only.
///
class YARPUniqueNameID : public YARPNameID
{
protected:
	ACE_INET_Addr _address;
	int _p1, _p2;

public:
	YARPUniqueNameID () : YARPNameID(), _address(1111, "localhost") { _p1 = _p2 = 0; }
	YARPUniqueNameID (int service, int port, char *hostname) : YARPNameID(service, ACE_INVALID_HANDLE), _address (port, hostname) { _p1 = _p2 = 0; }
	YARPUniqueNameID (int service, const ACE_INET_Addr& addr) : YARPNameID(service, ACE_INVALID_HANDLE), _address (addr) { _p1 = _p2 = 0; }

	inline ACE_INET_Addr& getAddressRef (void) { return _address; }
	inline YARPNameID& getNameID(void) { return (YARPNameID &)(*this); }
	inline int& getP1Ref (void) { return _p1; }
	inline int& getP2Ref (void) { return _p2; }

	YARPUniqueNameID& operator= (const YARPUniqueNameID& other) 
	{ 
		_address = other._address; 
		_mode = other._mode;
		_raw_id = other._raw_id;

		_p1 = other._p1;
		_p2 = other._p2;

		return (*this);
	}
};


/// service types.
///
///	YARP_NO_SERVICE_AVAILABLE	= -1,
///	YARP_UDP					= 0,
///	YARP_TCP					= 1,
///	YARP_MCAST					= 2,
///	YARP_QNET					= 3,
///

#endif
