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
/// $Id: YARPSocket.cpp,v 1.11 2004-08-10 17:08:23 gmetta Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocket and YARPOutputSocket
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Handle_Set.h>

#if defined(__QNX6__) || defined(__LINUX__)
#include <signal.h>
#endif

#ifndef __QNX__
	/// WINDOWS/LINUX

#ifndef __WIN_MSVC__
#	include <unistd.h>  // just for gethostname
#else
#	include <winsock2.h>
#endif

#else
	/// QNX4, and QNX6

#	include <unix.h>  // just for gethostname

#endif

#include <yarp/YARPSocket.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPString.h>

#define THIS_DBG 80

///
/// yarp message header.
///
#include <yarp/begin_pack_for_net.h>

#define DONT_WAIT_UP


struct MyMessageHeader
{
public:
	char key[2];
	NetInt32 len;
	char key2[2];

	MyMessageHeader()
	{
		len = 0;
		SetBad();
	}

	void SetGood()
    {
		key[0] = 'Y';
		key[1] = 'A';
		key2[0] = 'R';
		key2[1] = 'P';
    }
  
	void SetBad()
    {
		key[0] = 'y';
		key[1] = 'a';
		key2[0] = 'r';
		key2[1] = 'p';
    }

	void SetLength(int n_len)
	{
		len = n_len;
	}
  
	int GetLength()
	{
		if (key[0] == 'Y' && key[1] == 'A' && key2[0] == 'R' && key2[1] == 'P')
		{
			return len;
		}
		else
		{
			//	  printf("*** Bad header\n");
			return -1;
		}
	}
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>

///
/// sets recv and send <sock> buffers to <size>
int YARPNetworkObject::setSocketBufSize (ACE_SOCK& sock, int size)
{
	int ret = YARP_OK;
	if (sock.set_option (SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)) < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "can't change recv buffer size to %d\n", MAX_PACKET));
		ret = YARP_FAIL;
	}
	if (sock.set_option (SOL_SOCKET, SO_SNDBUF, &size, sizeof(int)) < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "can't change send buffer size to %d\n", MAX_PACKET));
		ret = YARP_FAIL;
	}
	return ret;
}

///
///
///
int getHostname(char *buffer, int buffer_length)
{
	int result = gethostname (buffer, buffer_length);
	// just who wants the following to actually happen?
	// breaks shmem endpoint comparison if no domain set.
	// interesting to see what happens across different domains...
#if 0
#ifndef __QNX4__
#ifndef __WIN32__
#ifndef __QNX6__
	// QNX4 just doesn't have getdomainname or any obvious equivalent
	// cygwin version doesn't have getdomainname or any obvious equivalent
	if (result==0)
	{
		if (ACE_OS::strchr(buffer,'.')==NULL)
		{
			int delta = ACE_OS::strlen(buffer);
			buffer += delta;
			buffer_length -= delta;
			if (buffer_length>=1)
			{
				*buffer = '.';
				buffer++;
				buffer_length--;
			}
			result = getdomainname(buffer,buffer_length);
		}
	}
#endif
#endif
#endif
#endif
	return result;
}


///
/// Output socket + stream incapsulation.
///
///
class OSData
{
public:
	ACE_INET_Addr _remote_addr;
	ACE_SOCK_Connector _connector;
	ACE_SOCK_Stream _stream;
};

static OSData& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSData*)x);
}

YARPOutputSocket::YARPOutputSocket() : YARPNetworkOutputObject ()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSData;
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_O_SOCKET;
}

YARPOutputSocket::~YARPOutputSocket()
{
	if (identifier != ACE_INVALID_HANDLE)
		Close (YARPUniqueNameSock());

	if (system_resources != NULL)
	{
		delete ((OSData*)system_resources);
		system_resources = NULL;
	}
}


int YARPOutputSocket::Close (const YARPUniqueNameID& name)
{
	ACE_UNUSED_ARG (name);
	identifier = ACE_INVALID_HANDLE;
	return OSDATA(system_resources)._stream.close ();
}


int YARPOutputSocket::Prepare (const YARPUniqueNameID& name)
{
	OSDATA(system_resources)._remote_addr = ((YARPUniqueNameSock&)name).getAddressRef();
	return YARP_OK;
}

int YARPOutputSocket::SetTCPNoDelay (void)
{
	OSData& d = OSDATA(system_resources);
	int one = 1;
	return d._stream.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
}

int YARPOutputSocket::Connect (const YARPUniqueNameID& name, const YARPString& own_name)
{
	ACE_UNUSED_ARG (name);

	OSData& d = OSDATA(system_resources);
	ACE_DEBUG ((LM_DEBUG, "Connecting to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	d._connector.connect (d._stream, d._remote_addr);

	MyMessageHeader hdr;
	hdr.SetGood();
	hdr.SetLength(YARP_MAGIC_NUMBER+3 + 128*name.getRequireAck());

	d._stream.send_n (&hdr, sizeof(hdr), 0);

	/// fine, now send the name of the connection.
	NetInt32 name_len = own_name.length();
	d._stream.send_n (&name_len, sizeof(NetInt32), 0);
	d._stream.send_n (own_name.c_str(), name_len, 0);

	/// ...and wait for a reply.
	hdr.SetBad();
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	int r = d._stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);
	if (r < 0)
	{
		///
		ACE_DEBUG ((LM_ERROR, "troubles connecting to TCP, this shouldn't really happen, maybe a bug?\n"));

		d._stream.close();
		identifier = ACE_INVALID_HANDLE;
		return YARP_FAIL;
	}

	identifier = d._stream.get_handle ();

	YARPNetworkObject::setSocketBufSize (d._stream, MAX_PACKET);

	return YARP_OK;
}


int YARPOutputSocket::SendBegin(char *buffer, int buffer_length)
{
	MyMessageHeader hdr;
	hdr.SetGood ();
    hdr.SetLength (buffer_length);

	int sent = -1;
	sent = OSDATA(system_resources)._stream.send_n ((const void *)(&hdr), sizeof(hdr), 0);
	if (sent != sizeof(hdr))
		return YARP_FAIL;

	sent = OSDATA(system_resources)._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;
}


int YARPOutputSocket::SendContinue(char *buffer, int buffer_length)
{
	/// without header.
	int sent = OSDATA(system_resources)._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;
}

/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocket::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
  //printf("RequireAck state is %d\n", getRequireAck());
#ifdef DONT_WAIT_UP
  if (getRequireAck()) {
#endif
	MyMessageHeader hdr2;
	hdr2.SetBad ();

	int result = -1;
	int r = OSDATA(system_resources)._stream.recv_n ((void *)(&hdr2), sizeof(hdr2), 0);
	if (r == sizeof(hdr2))
	{
		int len2 = hdr2.GetLength();
		if (len2 > 0)
		{
			if (len2 < reply_buffer_length)
			{
				reply_buffer_length = len2;
			}

			result = OSDATA(system_resources)._stream.recv_n ((void *)reply_buffer, reply_buffer_length, 0);
		}
		else
		{
			if (len2 == 0) { result = 0; }
		}
	}
	return result;
#ifdef DONT_WAIT_UP
  } else {
	ACE_OS::memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
  }
#endif
}


int YARPOutputSocket::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	return SendReceivingReply (reply_buffer, reply_buffer_length);
}

ACE_HANDLE YARPOutputSocket::GetIdentifier(void) const
{
	return identifier;
}


