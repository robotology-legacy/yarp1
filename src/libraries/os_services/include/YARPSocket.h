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
/// $Id: YARPSocket.h,v 1.8 2003-07-01 12:49:57 gmetta Exp $
///
///

#ifndef YARPSOCKET_INC
#define YARPSOCKET_INC

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocket and YARPOutputSocket
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
/// SEE same comment on .cpp file.
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Synch.h>

#include "YARPAll.h"
#include "YARPNameID.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

//#include <stdlib.h>

/// LATER: requires error handling for broken connections.
///

#include "YARPNetworkTypes.h" // not strictly necessary here

enum
{
	YARP_NO_SOCKET = 0,
	YARP_I_SOCKET = 1,
	YARP_O_SOCKET = 2,
};

class YARPNetworkObject
{
protected:
	int _socktype;
	enum { NOT_IMPLEMENTED = 0 };

public:
	static int getHostname(char *buffer, int buffer_length);
	static int setSocketBufSize(ACE_SOCK& sock, int size);
	int getSocketType (void) const { return _socktype; }

	virtual int Close(void) { ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
	virtual int Close(ACE_HANDLE reply_id) { ACE_UNUSED_ARG(reply_id); ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
	virtual int Close (const YARPUniqueNameID& name) { ACE_UNUSED_ARG(name); ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
	virtual int CloseAll(void) { ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }

	virtual int Prepare (const YARPUniqueNameID& name) { ACE_UNUSED_ARG(name); ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
	virtual int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) 
	{ 
		ACE_UNUSED_ARG(buffer); 
		ACE_UNUSED_ARG(buffer_length);
		ACE_UNUSED_ARG(reply_id);
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) 
	{
		ACE_UNUSED_ARG(buffer);
		ACE_UNUSED_ARG(buffer_length);
		ACE_UNUSED_ARG(reply_id); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length) 
	{
		ACE_UNUSED_ARG(reply_id);
		ACE_UNUSED_ARG(buffer);
		ACE_UNUSED_ARG(buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) 
	{
		ACE_UNUSED_ARG(reply_id);
		ACE_UNUSED_ARG(reply_buffer);
		ACE_UNUSED_ARG(reply_buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) 
	{
		ACE_UNUSED_ARG(reply_id);
		ACE_UNUSED_ARG(reply_buffer);
		ACE_UNUSED_ARG(reply_buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}

	virtual ACE_HANDLE GetIdentifier(void) const { ACE_ASSERT (NOT_IMPLEMENTED); return ACE_INVALID_HANDLE; }
	virtual void SetIdentifier(int n_identifier) { ACE_UNUSED_ARG(n_identifier); ACE_ASSERT (NOT_IMPLEMENTED); }
	virtual int GetAssignedPort(void) const { ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }

	virtual int Connect(void) { ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
	virtual int Connect (const YARPUniqueNameID& name) 
	{
		ACE_UNUSED_ARG(name);
		ACE_ASSERT (NOT_IMPLEMENTED);
		return YARP_FAIL;
	}

	virtual int SendBegin(char *buffer, int buffer_length) 
	{
		ACE_UNUSED_ARG(buffer);
		ACE_UNUSED_ARG(buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int SendContinue(char *buffer, int buffer_length) 
	{
		ACE_UNUSED_ARG(buffer);
		ACE_UNUSED_ARG(buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
///	virtual int SendReceivingReply(YARPMultipartMessage& return_msg)
///	{
///		ACE_UNUSED_ARG(return_msg);
///		return YARP_FAIL;
///	}
	virtual int SendReceivingReply(char *reply_buffer, int reply_buffer_length) 
	{
		ACE_UNUSED_ARG(reply_buffer);
		ACE_UNUSED_ARG(reply_buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}
	virtual int SendEnd(char *reply_buffer, int reply_buffer_length) 
	{
		ACE_UNUSED_ARG(reply_buffer);
		ACE_UNUSED_ARG(reply_buffer_length); 
		ACE_ASSERT (NOT_IMPLEMENTED); 
		return YARP_FAIL; 
	}

	virtual int GetServiceType (void) { ACE_ASSERT (NOT_IMPLEMENTED); return YARP_FAIL; }
};


///
///
///
class YARPInputSocket : public YARPNetworkObject
{
protected:
	void *system_resources;

public:
	YARPInputSocket();
	virtual ~YARPInputSocket();

	int Close(ACE_HANDLE reply_id);
	int CloseAll(void);

	int Prepare (const YARPUniqueNameID& name);

	int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length);
	int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);
	int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;		/// { return identifier; }
	int GetAssignedPort(void) const;	/// { return assigned_port; }
	
	int GetServiceType (void) { return YARP_TCP; }
};


///
///
///
class YARPOutputSocket : public YARPNetworkObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;		/// still unclear how to get this.

public:
	YARPOutputSocket();
	virtual ~YARPOutputSocket();

	int Prepare (const YARPUniqueNameID& name);
	int Close(void);
	int Connect(void);
	
	int SendBegin(char *buffer, int buffer_length);
	int SendContinue(char *buffer, int buffer_length);
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;

	int SetTCPNoDelay (void);

	int GetServiceType (void) { return YARP_TCP; }
};

#endif
