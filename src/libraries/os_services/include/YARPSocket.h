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
/// $Id: YARPSocket.h,v 1.12 2003-08-02 07:46:14 gmetta Exp $
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
#include "YARPNetworkTypes.h" // not strictly necessary here

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

enum
{
	YARP_NO_SOCKET = 0,
	YARP_I_SOCKET = 1,
	YARP_O_SOCKET = 2,
};

///
/// base class of all socket communication classes.
///	- not pure virtual because this is base of both In and Out sockets.
///
class YARPNetworkObject
{
private:
	YARPNetworkObject (const YARPNetworkObject&);
	YARPNetworkObject& operator= (const YARPNetworkObject&);

protected:
	int _socktype;
	enum { NOT_IMPLEMENTED = 0 };

public:
	YARPNetworkObject () { _socktype = YARP_NO_SOCKET; }
	virtual ~YARPNetworkObject() {}

	static int setSocketBufSize(ACE_SOCK& sock, int size);

	virtual int getSocketType (void) const { return _socktype; }
	virtual ACE_HANDLE GetIdentifier(void) const = 0;
	virtual int GetServiceType (void) = 0;
};

class YARPNetworkInputObject : public YARPNetworkObject
{
private:
	YARPNetworkInputObject (const YARPNetworkInputObject&);
	YARPNetworkInputObject& operator=(const YARPNetworkInputObject&);

public:
	YARPNetworkInputObject() : YARPNetworkObject() { _socktype = YARP_I_SOCKET; }
	virtual ~YARPNetworkInputObject() {}

	virtual int Close(ACE_HANDLE reply_id) = 0;
	virtual int CloseAll(void) = 0;

	virtual int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) = 0;
	virtual int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL) = 0;
	virtual int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length) = 0;
	virtual int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) = 0;
	virtual int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length) = 0;
};


class YARPNetworkOutputObject : public YARPNetworkObject
{
private:
	YARPNetworkOutputObject (const YARPNetworkOutputObject&);
	YARPNetworkOutputObject& operator=(const YARPNetworkOutputObject&);

public:
	YARPNetworkOutputObject (void) : YARPNetworkObject() { _socktype = YARP_O_SOCKET; }
	virtual ~YARPNetworkOutputObject () {}

	virtual int Close (const YARPUniqueNameID& name) = 0;
	virtual int Connect (const YARPUniqueNameID& name) = 0;
	virtual int SendBegin(char *buffer, int buffer_length) = 0;
	virtual int SendContinue(char *buffer, int buffer_length) = 0;
	virtual int SendReceivingReply(char *reply_buffer, int reply_buffer_length) = 0;
	virtual int SendEnd(char *reply_buffer, int reply_buffer_length) = 0;
};

///
///
///
class YARPOutputSocket : public YARPNetworkOutputObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;

public:
	YARPOutputSocket();
	virtual ~YARPOutputSocket();

	/// virtual override.
	int Close(const YARPUniqueNameID& name);
	int Connect(const YARPUniqueNameID& name);
	
	int SendBegin(char *buffer, int buffer_length);
	int SendContinue(char *buffer, int buffer_length);
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;
	int GetServiceType (void) { return YARP_TCP; }

	/// specific stuff.
	int Prepare (const YARPUniqueNameID& name);
	int SetTCPNoDelay (void);
};

#endif
