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
///                    #pasa, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocketDgram.h,v 1.1 2003-04-22 09:06:36 gmetta Exp $
///
///

#ifndef __YARPSocketDgramh__
#define __YARPSocketDgramh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/Synch.h>

#include "YARPAll.h"
#include "YARPNameID.h"
#include "YARPSocket.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "YARPNetworkTypes.h" // not strictly necessary here

/// SocketTypes
///{
///	YARP_NO_SOCKET = 0,
///	YARP_I_SOCKET = 1,
///	YARP_O_SOCKET = 2,
///};

/*
	same as above.

class YARPNetworkObject
{
protected:
	int _socktype;

public:
	static int getHostname(char *buffer, int buffer_length);
	int getSocketType (void) const { return _socktype; }
};
*/

///
///
///
class YARPInputSocketDgram : public YARPNetworkObject
{
protected:
	void *system_resources;
///	int identifier;
///	int assigned_port;

public:
	YARPInputSocketDgram();
	virtual ~YARPInputSocketDgram();

	int Close(ACE_HANDLE reply_id);
	int CloseAll(void);

	int Prepare (const YARPUniqueNameID& name, int port1, int number_o_ports);

	int PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id = NULL);
	int ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length);
	int ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);
	int ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length);

	ACE_HANDLE GetIdentifier(void) const;		/// { return identifier; }
	int GetAssignedPort(void) const;	/// { return assigned_port; }
};


///
///
///
class YARPOutputSocketDgram : public YARPNetworkObject
{
protected:
	void *system_resources;
	ACE_HANDLE identifier;

public:
	YARPOutputSocketDgram();
	virtual ~YARPOutputSocketDgram();

	int Prepare (int local_port, const YARPUniqueNameID& name);
	int Close(void);
	int Connect(void);
	
	int SendBegin(char *buffer, int buffer_length);
	int SendContinue(char *buffer, int buffer_length);
	int SendReceivingReply(char *reply_buffer, int reply_buffer_length);
	int SendEnd(char *reply_buffer, int reply_buffer_length);

	/// what is it for?
	ACE_HANDLE GetIdentifier();
	void SetIdentifier(int n_identifier);
};

#endif
