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
/// $Id: YARPSocketSyncComm.cpp,v 1.5 2004-07-09 16:10:13 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

//#include <assert.h>
//#include <stdio.h>

#include <yarp/YARPSocketSyncComm.h>
#include <yarp/YARPSocket.h>
#include <yarp/YARPSocketDgram.h>
#include <yarp/YARPSocketNameService.h>
#include <yarp/YARPScheduler.h>

#include <yarp/YARPNameID_defs.h>
#include "yarp_private/BlockPrefix.h"

#include <yarp/debug.h>

#define THIS_DBG 50

/*
  Code sends along a preamble to permit transparent bridging.  Preamble
  is otherwise ignored completely. since the QNX spec doesn't have it.
 */

///const int _bufsize = 256;
///NetInt32 YARPSocketSyncComm::_buffer[_bufsize];

int YARPSocketSyncComm::Send(const YARPNameID& dest, char *buffer, int buffer_length, char *return_buffer, int return_buffer_length)
{
	YARPNetworkOutputObject *os = (YARPNetworkOutputObject *)YARPSocketEndpointManager::GetThreadSocket ();
	ACE_ASSERT (!dest.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (os != NULL);

	BlockPrefix prefix;
	prefix.total_blocks = 1;
	prefix.reply_blocks = 1;
	prefix.size = buffer_length;
	prefix.reply_size = return_buffer_length;

	os->SendBegin ((char*)(&prefix), sizeof(prefix));
	os->SendContinue (buffer, buffer_length);
	
	char ch = -1;
	os->SendReceivingReply (&ch, 1);
	if (ch == 0)
	{
		return os->SendEnd (return_buffer, return_buffer_length);
	}

	return YARP_FAIL;
}


YARPNameID YARPSocketSyncComm::BlockingReceive(const YARPNameID& src, char *buffer, int buffer_length)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	const int _bufsize = 64;
	NetInt32 _buffer[_bufsize];

	BlockPrefix prefix;
	ACE_HANDLE id = ACE_INVALID_HANDLE;
	int ct = ts->ReceiveBegin ((char*)(&prefix), sizeof(prefix), &id);
	if (ct >= 0)
	{
		if (prefix.size < 0)
		{
			ACE_ASSERT (prefix.total_blocks + prefix.reply_blocks <= _bufsize);
			ct = ts->ReceiveContinue (id, (char *)_buffer, sizeof(NetInt32) * (prefix.total_blocks + prefix.reply_blocks));
		}

		ct = ts->ReceiveContinue (id, buffer, buffer_length);
	}

	if (ct < 0) 
	{
		//printf("Failed in YARPSocketSyncComm::BlockingReceive\n");
		id = ACE_INVALID_HANDLE;
	}

	return YARPNameID (src.getServiceType(), id);
}


YARPNameID YARPSocketSyncComm::PollingReceive(const YARPNameID& src, char *buffer, int buffer_length)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	const int _bufsize = 64;
	NetInt32 _buffer[_bufsize];

	BlockPrefix prefix;
	ACE_HANDLE id = ACE_INVALID_HANDLE;
	int ct = ts->PollingReceiveBegin ((char*)(&prefix), sizeof(prefix), &id);
	if (ct >= 0)
	{
		if (prefix.size < 0)
		{
			ACE_ASSERT (prefix.total_blocks + prefix.reply_blocks <= _bufsize);
			ct = ts->ReceiveContinue (id, (char *)_buffer, sizeof(NetInt32) * (prefix.total_blocks + prefix.reply_blocks));
		}

		ct = ts->ReceiveContinue (id, buffer, buffer_length);
	}
	
	if (ct < 0) id = ACE_INVALID_HANDLE;

	return YARPNameID (src.getServiceType(), id);
}


int YARPSocketSyncComm::ContinuedReceive(const YARPNameID& src, char *buffer, int buffer_length)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (ts != NULL);

	int ct = YARP_FAIL;
	if (src.isValid())
	{
		ct = ts->ReceiveContinue (src.getRawIdentifier(), buffer, buffer_length);
	}
	return ct;
}


int YARPSocketSyncComm::Reply(const YARPNameID& src, char *buffer, int buffer_length)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	char ch = 0;
	ts->ReceiveReplying (src.getRawIdentifier(), &ch, 1);
	return ts->ReceiveEnd (src.getRawIdentifier(), buffer, buffer_length);
}


int YARPSocketSyncComm::InvalidReply(const YARPNameID& src)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	char ch = 0;
	return ts->ReceiveEnd (src.getRawIdentifier(), &ch, 1);
}


int YARPSocketSyncComm::Send(const YARPNameID& dest, YARPMultipartMessage& msg, YARPMultipartMessage& return_msg)
{
	YARPNetworkOutputObject *os = (YARPNetworkOutputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!dest.isConsistent(YARP_NO_SERVICE_AVAILABLE));

	if (os == NULL) 
	{
		ACE_DEBUG ((LM_DEBUG, "Just a warning: attempted a send on a non-existing (NULL) socket\n"));
		return YARP_FAIL;
	}

	int send_parts = msg.GetParts();
	int return_parts = return_msg.GetParts();
	ACE_ASSERT (send_parts >= 1);
	ACE_ASSERT (return_parts >= 1);
	int i;

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Get %d send_parts, %d return_parts\n", send_parts, return_parts));
	const int _bufsize = 64;

	ACE_ASSERT (send_parts + return_parts <= _bufsize);

	/* preamble code begins */
	BlockPrefix prefix;
	prefix.total_blocks = send_parts;
	prefix.reply_blocks = return_parts;
	prefix.size = -1;
	prefix.reply_size = -1;

	NetInt32 _buffer[_bufsize];

	os->SendBegin ((char*)(&prefix), sizeof(prefix));
	
	for (i = 0; i < send_parts; i++)
	{
		_buffer[i] = msg.GetBufferLength(i);
	}
	for (i = 0; i < return_parts; i++)
	{
		_buffer[i+send_parts] = return_msg.GetBufferLength(i);
	}

	os->SendContinue ((char *)_buffer, sizeof(NetInt32) * (send_parts + return_parts));

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "about to send buf 0 %d bytes\n", msg.GetBufferLength(0)));
	os->SendContinue (msg.GetBuffer(0), msg.GetBufferLength(0));

	/* preamble code ends */
	for (i = 1; i < send_parts; i++)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "about to send buf %d %d bytes\n", i, msg.GetBufferLength(i)));
		os->SendContinue (msg.GetBuffer(i), msg.GetBufferLength(i));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "sent buf %d %d bytes\n", i, msg.GetBufferLength(i)));
	}
	
	char ch = -1;
	os->SendReceivingReply (&ch, 1);
	
	if (ch == 0)
	{
		for (i = 0; i < return_parts-1; i++)
		{
			os->SendReceivingReply (return_msg.GetBuffer(i), return_msg.GetBufferLength(i));
		}
		int result = os->SendEnd (return_msg.GetBuffer(return_parts-1), return_msg.GetBufferLength(return_parts-1));
		return result;
	}

	return YARP_FAIL;
}


YARPNameID YARPSocketSyncComm::BlockingReceive(const YARPNameID& src, YARPMultipartMessage& msg)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (ts != NULL);
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));

	ACE_HANDLE id = ACE_INVALID_HANDLE;
	int receive_parts = msg.GetParts();
	ACE_ASSERT (receive_parts >= 1);

	/* preamble code begins */
	const int _bufsize = 64;
	NetInt32 _buffer[_bufsize];

	BlockPrefix prefix;
	ts->ReceiveBegin ((char*)(&prefix), sizeof(prefix), &id);

	if (id != ACE_INVALID_HANDLE)
	{
		if (prefix.size < 0)
		{
			ACE_ASSERT (prefix.total_blocks + prefix.reply_blocks <= _bufsize);
			ts->ReceiveContinue (id, (char *)_buffer, sizeof(NetInt32) * (prefix.total_blocks+prefix.reply_blocks));
		}

		ts->ReceiveContinue (id, msg.GetBuffer(0), msg.GetBufferLength(0));
	}
	/* preamble code ends */

	/// no proper error check/handling here.
	if (id != ACE_INVALID_HANDLE)
	{
		for (int i = 1; i < receive_parts; i++)
		{
			int ct2 = ts->ReceiveContinue (id, msg.GetBuffer(i), msg.GetBufferLength(i));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^ additional receive of %d bytes\n", ct2));
		}
	}

	return YARPNameID (src.getServiceType(),id);
}


YARPNameID YARPSocketSyncComm::PollingReceive(const YARPNameID& src, YARPMultipartMessage& msg)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	ACE_HANDLE id = ACE_INVALID_HANDLE;
	const int _bufsize = 64;
	NetInt32 _buffer[_bufsize];

	int receive_parts = msg.GetParts();
	ACE_ASSERT(receive_parts>=1);

	/* preamble code begins */
	BlockPrefix prefix;
	ts->PollingReceiveBegin((char*)(&prefix),sizeof(prefix), &id);

	if (id != ACE_INVALID_HANDLE)
	{
		if (prefix.size < 0)
		{
			ACE_ASSERT (prefix.total_blocks + prefix.reply_blocks <= _bufsize);
			ts->ReceiveContinue (id, (char *)_buffer, sizeof(NetInt32) * (prefix.total_blocks+prefix.reply_blocks));
		}

		ts->ReceiveContinue (id, msg.GetBuffer(0), msg.GetBufferLength(0));
	}

	/* preamble code ends */
	if (id != ACE_INVALID_HANDLE)
	{
		for (int i = 1; i < receive_parts; i++)
		{
			ts->ReceiveContinue (id, msg.GetBuffer(i), msg.GetBufferLength(i));
		}
	}

	return YARPNameID (src.getServiceType(),id);
}



int YARPSocketSyncComm::Reply(const YARPNameID& src, YARPMultipartMessage& msg)
{
	YARPNetworkInputObject *ts = (YARPNetworkInputObject *)YARPSocketEndpointManager::GetThreadSocket();
	ACE_ASSERT (!src.isConsistent(YARP_NO_SERVICE_AVAILABLE));
	ACE_ASSERT (ts != NULL);

	int reply_parts = msg.GetParts();
	ACE_ASSERT (reply_parts >= 1);

	char ch = 0;

	ts->ReceiveReplying (src.getRawIdentifier(), &ch, 1);
	for (int i = 0; i < reply_parts-1; i++)
	{
		ts->ReceiveReplying (src.getRawIdentifier(), msg.GetBuffer(i), msg.GetBufferLength(i));
	}

	return ts->ReceiveEnd (src.getRawIdentifier(), msg.GetBuffer(reply_parts-1), msg.GetBufferLength(reply_parts-1));
}


#undef THIS_DBG
