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
/// $Id: YARPSocketMulti.h,v 1.5 2004-08-10 13:42:07 babybot Exp $
///
///

#ifndef __YARPSocketMultih__
#define __YARPSocketMultih__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/MEM_Acceptor.h>
#include <ace/MEM_Connector.h>
#include <ace/Synch.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPSocket.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPNetworkTypes.h> // not strictly necessary here

/// SocketTypes
///{
///	YARP_NO_SOCKET = 0,
///	YARP_I_SOCKET = 1,
///	YARP_O_SOCKET = 2,
///};

///
/// still requires the fancy mthread stuff to read from multiple groups.
///
class YARPInputSocketMulti : public YARPNetworkInputObject
{
protected:
	void *system_resources;

public:
	YARPInputSocketMulti();
	virtual ~YARPInputSocketMulti();

	/// virtual override.
	int Close(ACE_HANDLE reply_id);
	int CloseByName(const YARPString& name);
	int CloseAll(void);

	int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length);
	int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);
	int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;
	int GetServiceType (void) { return YARP_MULTI; }

	/// specific.
	int Prepare (const YARPUniqueNameID& name, int *ports, int number_o_ports);
	int GetAssignedPort(void) const;
	int PrintThreadList(void);
};

///
///
/// this handles only the SHMEM out connection, UDP and MCAST use the protocol-specific class.
class YARPOutputSocketMulti : public YARPNetworkOutputObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;

public:
	YARPOutputSocketMulti();
	virtual ~YARPOutputSocketMulti();

	/// virtual override.
	int Close(const YARPUniqueNameID& name);
	int Connect(const YARPUniqueNameID& name, const YARPString& own_name);
	
	int SendBegin(char *buffer, int buffer_length);
	int SendContinue(char *buffer, int buffer_length);
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;
	int GetServiceType (void) { return YARP_SHMEM; }

	/// specific.
	int Prepare (const YARPUniqueNameID& name);
};

#endif

