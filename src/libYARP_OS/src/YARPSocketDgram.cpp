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
/// $Id: YARPSocketDgram.cpp,v 1.6 2004-08-05 17:11:56 babybot Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocketDgram and YARPOutputSocketDgram
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
///
///

///
/// name server allocation policy for dgrams.
///	1) the input socket is created with a max number of acceptable connections.
///	2) the name server allocates a pool of ports for each IP asking a registration in UDP mode.
/// 3) the port simply sends back a port number for each connection attempt.
///	4) the port complains if a given remote is already connected.
///	5) this structure working in UDP mimics the corresponding TCP accept with less
///		exception handling though.
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Handle_Set.h>
#include <ace/Time_Value.h>
#include <ace/Sched_Params.h>

#if defined(__QNX6__) || defined(__LINUX__)
#include <signal.h>
#endif

#ifndef __QNX__
/// WIN32, Linux

#ifndef __WIN_MSVC__
#	include <unistd.h>  // just for gethostname
#endif

#else

#	include <unix.h>  // just for gethostname

#endif

#include <yarp/YARPSocket.h>
#include <yarp/YARPSocketDgram.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPString.h>

///
#define THIS_DBG 80


///
/// yarp message header.
///
#include <yarp/begin_pack_for_net.h>

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
/// Output socket + stream incapsulation.
///
///
class OSDataDgram
{
public:
	ACE_INET_Addr _remote_acceptor_store;
	ACE_INET_Addr _remote_addr;

	ACE_INET_Addr _local_addr;
	ACE_SOCK_Dgram _connector_socket;

	ACE_SOCK_Connector _service_socket;

	MyMessageHeader _hdr;

	enum { _iov_buffer_size = 256 };
	iovec _iov[_iov_buffer_size];
	int _num_elements;
	int _overall_msg_size;

	char _buffer[MAX_PACKET];
};

static OSDataDgram& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSDataDgram*)x);
}

YARPOutputSocketDgram::YARPOutputSocketDgram (void) : YARPNetworkOutputObject ()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSDataDgram;
	ACE_ASSERT(system_resources!=NULL);
	OSDataDgram& d = OSDATA(system_resources);

	_socktype = YARP_O_SOCKET;

	memset (d._iov, 0, sizeof(iovec) * d._iov_buffer_size);
	d._num_elements = 0;
	d._overall_msg_size = 0;

	d._hdr.SetBad();

	/// I hate this but there's no much choice unless the protocol is changed to allow
	/// for nice iovec style sync comm.
	memset (d._buffer, 0, MAX_PACKET);
}

YARPOutputSocketDgram::~YARPOutputSocketDgram (void)
{
	if (identifier != ACE_INVALID_HANDLE)
		Close (YARPUniqueNameSock());

	if (system_resources != NULL)
	{
		delete ((OSDataDgram*)system_resources);
		system_resources = NULL;
	}
}


int YARPOutputSocketDgram::Close (const YARPUniqueNameID& name)
{
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_UNUSED_ARG (name);

	OSDataDgram& d = OSDATA(system_resources);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Pretending to close a connection to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 1);

	ACE_SOCK_Stream stream;
	int r = d._service_socket.connect (stream, d._remote_addr, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot connect to remote peer %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		ACE_DEBUG ((LM_DEBUG, "close will complete anyway\n"));
		d._connector_socket.close ();
		identifier = ACE_INVALID_HANDLE;
		return YARP_FAIL;
	}

	r = stream.send_n (&hdr, sizeof(hdr), 0);

	/// wait response.
	/// need a timeout here!
	hdr.SetBad ();

	r = stream.recv (&hdr, sizeof(hdr), 0, &timeout);
	if (r < 0)
	{
		stream.close ();
		d._connector_socket.close ();
		identifier = ACE_INVALID_HANDLE;
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	hdr.GetLength();
	d._remote_addr.set ((u_short)0);
	d._remote_acceptor_store.set ((u_short)0);

	stream.close ();
	d._connector_socket.close ();
	identifier = ACE_INVALID_HANDLE;

	return YARP_OK;
}


int YARPOutputSocketDgram::Prepare (const YARPUniqueNameID& name)
{
	OSDataDgram& d = OSDATA(system_resources);

	/// local_port might not be needed by the socket layer.
	char buf[YARP_STRING_LEN];
	getHostname (buf, YARP_STRING_LEN);
	d._local_addr.set ((u_short)0, buf);
	d._remote_addr = ((YARPUniqueNameSock&)name).getAddressRef();

	int r = d._connector_socket.open (d._local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);
	if (r < 0)
	{
		identifier = ACE_INVALID_HANDLE;
		return YARP_FAIL;
	}

	r = YARPNetworkObject::setSocketBufSize (d._connector_socket, MAX_PACKET);
	if (r < 0)
	{
		d._connector_socket.close();
		ACE_DEBUG ((LM_DEBUG, "cannot set buffer size to %d\n", MAX_PACKET));
		return YARP_FAIL;
	}

	identifier = d._connector_socket.get_handle();

	return YARP_OK;
}

///
/// pretend a connection.
int YARPOutputSocketDgram::Connect (const YARPUniqueNameID& name)
{
	ACE_UNUSED_ARG (name);

	OSDataDgram& d = OSDATA(system_resources);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Pretending a connection to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_SOCK_Stream stream;

	int r = d._service_socket.connect (stream, d._remote_addr, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_ERROR, "cannot connect to remote peer %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 128*name.getRequireAck());
	stream.send_n (&hdr, sizeof(hdr), 0);

	/// wait response.
	hdr.SetBad ();
	r = stream.recv (&hdr, sizeof(hdr), 0, &timeout);
	if (r < 0)
	{
		stream.close ();
		ACE_DEBUG ((LM_ERROR, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// stores the remote acceptor address for future use (e.g. closing the connection).
	d._remote_acceptor_store = d._remote_addr;

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		/// there might be a real -1 port number -> 65535.
		stream.close ();
		ACE_DEBUG ((LM_ERROR, "*** error, got garbage back from remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// the connect changes the remote port number to the actual assigned channel.
	d._remote_addr.set_port_number (port_number);
	stream.close ();

	return YARP_OK;
}


int YARPOutputSocketDgram::SendBegin(char *buffer, int buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);
	d._hdr.SetGood ();
    d._hdr.SetLength (buffer_length);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "sending to: %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));

	d._iov[0].iov_base = (char *)&d._hdr;
	d._iov[0].iov_len = sizeof(d._hdr);
	d._iov[1].iov_base = buffer;
	d._iov[1].iov_len = buffer_length;
	d._num_elements = 2;
	d._overall_msg_size = buffer_length + sizeof(d._hdr);

	return YARP_OK;
}


int YARPOutputSocketDgram::SendContinue(char *buffer, int buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);

	d._iov[d._num_elements].iov_base = buffer;
	d._iov[d._num_elements].iov_len = buffer_length;
	d._num_elements ++;
	d._overall_msg_size += buffer_length;
		
	return YARP_OK;
}


/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketDgram::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
}


int YARPOutputSocketDgram::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);

	/// it needs to send the message first.
	if (d._overall_msg_size > MAX_PACKET)
	{
		ACE_DEBUG ((LM_DEBUG, "Implementation limit, pls, you should refrain from sending big MCAST packets\n"));
		ACE_DEBUG ((LM_DEBUG, "Actual size is %d, allowed %d\n", d._overall_msg_size, MAX_PACKET));
		ACE_ASSERT (1 == 0);
	}

	int sent = d._connector_socket.send (d._iov, d._num_elements, d._remote_addr, 0);
	if (sent < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "Actual send to %s:%d failed\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
}


ACE_HANDLE YARPOutputSocketDgram::GetIdentifier(void) const
{
	return identifier;
}

#undef THIS_DBG
