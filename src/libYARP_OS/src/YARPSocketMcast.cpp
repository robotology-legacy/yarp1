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
/// $Id: YARPSocketMcast.cpp,v 1.8 2004-08-10 17:08:23 gmetta Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocketMcast and YARPOutputSocketMcast
///
///
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
#include <yarp/YARPSocketMcast.h>
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
class OSDataMcast
{
public:
	ACE_INET_Addr _mcast_addr;
	ACE_SOCK_Dgram_Mcast _connector_socket;
	
	ACE_SOCK_Connector _service_socket;

	enum { _max_num_clients = 256 };
	ACE_INET_Addr _clients[_max_num_clients];
	YARPString _client_names[_max_num_clients];
	int _num_connected_clients;

	MyMessageHeader _hdr;

	enum { _iov_buffer_size = 256 };
	iovec _iov[_iov_buffer_size];
	int _num_elements;
	int _overall_msg_size;
};

static OSDataMcast& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSDataMcast*)x);
}

YARPOutputSocketMcast::YARPOutputSocketMcast (void) : YARPNetworkOutputObject ()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSDataMcast;
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_O_SOCKET;

	int i;
	OSDataMcast& d = OSDATA(system_resources);
	for (i = 0; i < d._max_num_clients; i++)
	{
		d._clients[i].set ((u_short)0, INADDR_ANY);
	}

	d._num_elements = 0;
	d._overall_msg_size = 0;
	d._hdr.SetBad();

	d._num_connected_clients = 0;
}

YARPOutputSocketMcast::~YARPOutputSocketMcast (void)
{
	OSDataMcast& d = OSDATA(system_resources);

	if (identifier != ACE_INVALID_HANDLE)
	{
		ACE_DEBUG ((LM_DEBUG, "Closing all connections to port %d on %s\n", 
			d._mcast_addr.get_port_number(), 
			d._mcast_addr.get_host_name()));

		int i;
		for (i = 0; i < d._max_num_clients; i++)
			Close (YARPUniqueNameSock(YARP_NO_SERVICE_AVAILABLE, d._clients[i]));
	}

	d._connector_socket.leave (d._mcast_addr, 0);

	if (system_resources != NULL)
	{
		delete ((OSDataMcast*)system_resources);
		system_resources = NULL;
	}
}

int YARPOutputSocketMcast::GetNumberOfClients (void)
{
	OSDataMcast& d = OSDATA(system_resources);
	return d._num_connected_clients;
}

int YARPOutputSocketMcast::CloseMcastAll (void)
{
	OSDataMcast& d = OSDATA(system_resources);
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 1);

	ACE_SOCK_Stream stream;

	int i;
	for (i = 0; i < d._max_num_clients; i++)
	{
		if (d._clients[i].get_port_number() != 0)
		{
			int r = d._service_socket.connect (stream, d._clients[i], &timeout);
			if (r < 0)
			{
				ACE_DEBUG ((LM_DEBUG, "cannot connect to remote %s:%d\n", d._clients[i].get_host_addr(), d._clients[i].get_port_number()));
			}

			stream.send_n (&hdr, sizeof(hdr), 0);
			
			/// wait response.
			hdr.SetBad ();
			r = stream.recv (&hdr, sizeof(hdr), 0, &timeout);

			if (r < 0)
			{
				ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._clients[i].get_host_addr(), d._clients[i].get_port_number()));
			}
			d._clients[i].set ((u_short)0, INADDR_ANY);

			stream.close ();
		}
	}

	d._connector_socket.close ();
	d._num_connected_clients = 0;
	identifier = ACE_INVALID_HANDLE;

	return YARP_OK;
}

/// closes down a specific connetion (tell the remote thread the connection is going down.).
int YARPOutputSocketMcast::Close (const YARPUniqueNameID& name)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 1);

	int i;
	int j = -1;
	///const char *sname = ((YARPUniqueNameSock&)name).getName().c_str();
	ACE_INET_Addr& nm = ((YARPUniqueNameSock &)name).getAddressRef();

	for (i = 0; i < d._max_num_clients; i++)
	{
		///if (d._client_names[i].compare(sname) == 0)
		if (d._client_names[i] == ((YARPUniqueNameSock&)name).getName())
		{
			j = i;
			///ACE_OS::printf("Returned true\n");
			break;
		}
	}

	if (j == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "the specific name is not connected %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	ACE_SOCK_Stream stream;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

	int r = d._service_socket.connect (stream, d._clients[j], &timeout);

	if (r < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot connect to %s:%d\n", nm.get_host_addr(), nm.get_port_number()));

		d._clients[j].set ((u_short)0, INADDR_ANY);
		d._client_names[j].clear(1);
	
		d._num_connected_clients --;

		/// closes down the socket.
		if (d._num_connected_clients <= 0)
		{
			d._connector_socket.close ();
			identifier = ACE_INVALID_HANDLE;
		}

		return YARP_FAIL;
	}

	stream.send_n (&hdr, sizeof(hdr), 0);

	/// wait response.
	hdr.SetBad ();
	r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);

	if (r < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._clients[j].get_host_addr(), d._clients[j].get_port_number()));

		d._clients[j].set ((u_short)0, INADDR_ANY);
		d._client_names[j].clear(1);

		stream.close ();
		d._num_connected_clients --;

		/// closes down the socket.
		if (d._num_connected_clients <= 0)
		{
			d._connector_socket.close ();
			identifier = ACE_INVALID_HANDLE;
		}

		return YARP_FAIL;
	}

	d._clients[j].set ((u_short)0, INADDR_ANY);
	d._client_names[j].clear(1);

	stream.close ();
	d._num_connected_clients --;

	/// closes down the socket.
	if (d._num_connected_clients <= 0)
	{
		d._connector_socket.close ();
		identifier = ACE_INVALID_HANDLE;
	}

	return YARP_OK;
}


/// this is called only once to create the group.
int YARPOutputSocketMcast::Prepare (const YARPUniqueNameID& name)
{
	OSDataMcast& d = OSDATA(system_resources);
	d._mcast_addr = ((YARPUniqueNameSock&)name).getAddressRef();

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Starting mcast group on port %d and ip %s\n", 
		d._mcast_addr.get_port_number(), 
		d._mcast_addr.get_host_addr()));

	int r = -1;

	if (((YARPUniqueNameSock&)name).getInterfaceName()!=YARPString("default")) {
	  r = d._connector_socket.open (d._mcast_addr, ((YARPUniqueNameSock&)name).getInterfaceName().c_str(), 1);		/// reuse addr on, netif = 0.
	} else {
	  r = d._connector_socket.open (d._mcast_addr, NULL, 1);		/// reuse addr on, netif = 0.
	}
	if (r == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot open mcast socket %s:%d (%s)\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number(),
			    ((YARPUniqueNameSock&)name).getInterfaceName().c_str()));
		return YARP_FAIL;
	}

	int ret = YARPNetworkObject::setSocketBufSize (d._connector_socket, MAX_PACKET);
	if (ret != YARP_OK)
	{
		d._connector_socket.close();
		ACE_DEBUG ((LM_DEBUG, "cannot set buffer size to %d\n", MAX_PACKET));
		return YARP_FAIL;
	}

	identifier = d._connector_socket.get_handle ();

	return YARP_OK;
}

///
///
/// keeping this piece of code for future use. It was needed to smoothly reconnect an MCAST
///	port... semantic has been changed since then.
#if 0
	/// momentarily raise the connection count to prevent closing the thread.
	d._num_connected_clients ++;

	/// tries to shut down the connection first.
	if (Close (name) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't close the mcast connection, it can happen if the server died unexpectedly\n"));

		/// erases the client entry anyway.
		d._clients[i].set ((u_short)0, INADDR_ANY);
		d._client_names[i].erase(d._client_names[i].begin(), d._client_names[i].end());
	}

	/// see comment above.
	d._num_connected_clients --;

	/// 250 ms delay.
	YARPTime::DelayInSeconds (.25);
#endif

///
/// this can be called many many times to ask receivers to join to mcast group.
/// name is the remote we're asking to join.
int YARPOutputSocketMcast::Connect (const YARPUniqueNameID& name, const YARPString& own_name)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// verifies it's a new connection.
	ACE_INET_Addr nm = ((YARPUniqueNameSock&)name).getAddressRef();

	int i, firstempty = -1;
	for (i = 0; i < d._max_num_clients; i++)
	{
		/// don't compare the IP addr because the remote might have already unregistered
		/// in that case, name doesn't contain the IP of the remote.

		///if (d._client_names[i].compare(sname) == 0)
		if (d._client_names[i] == ((YARPUniqueNameSock&)name).getName())
		{
			/// it's already there...
			ACE_DEBUG ((LM_ERROR, "the specific client is already connected %s:%d\n", d._clients[i].get_host_addr(), d._clients[i].get_port_number()));
			///d._service_socket.close();
			return YARP_FAIL;
		}

		if (d._clients[i].get_port_number() == 0 && firstempty < 0)
		{
			firstempty = i;	
		}
	}

	/// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 1 + 128*name.getRequireAck());

	char myhostname[YARP_STRING_LEN];
	getHostname (myhostname, YARP_STRING_LEN);
	ACE_INET_Addr local ((u_short)0, myhostname);
	ACE_SOCK_Stream stream;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

	int r = d._service_socket.connect (stream, nm, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_ERROR, "cannot connect to remote service\n"));
		return YARP_FAIL;
	}

	/// ask for a connection.
	stream.send_n (&hdr, sizeof(hdr), 0);

	/// fine, now send the name of the connection.
	NetInt32 name_len = own_name.length();
	stream.send_n (&name_len, sizeof(NetInt32), 0);
	stream.send_n (own_name.c_str(), name_len, 0);

	/// send mcast ip and port #.
	/// exactly 6 bytes.
	char buf[6];
	int ip = d._mcast_addr.get_ip_address();
	buf[0] = (ip & 0xff000000) >> 24;
	buf[1] = (ip & 0x00ff0000) >> 16;
	buf[2] = (ip & 0x0000ff00) >> 8;
	buf[3] = (ip & 0x000000ff);
	short portn = d._mcast_addr.get_port_number();
	buf[4] = (portn & 0xff00) >> 8;
	buf[5] = (portn & 0x00ff);
	stream.send_n (buf, 6, 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "supposedly sent %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));

	/// wait response.
	hdr.SetBad ();
	r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);

	if (r < 0)
	{
		stream.close ();
		ACE_DEBUG ((LM_ERROR, "cannot handshake with remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	d._clients[firstempty] = nm;

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		d._clients[firstempty].set ((u_short)0, INADDR_ANY);
		///d._client_names[firstempty].erase(d._client_names[firstempty].begin(), d._client_names[firstempty].end());
		d._client_names[firstempty].clear(1);

		/// there might be a real -1 port number -> 65535.
		stream.close ();
		ACE_DEBUG ((LM_ERROR, "got garbage back from remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	/// the connect changes the remote port number to the actual assigned channel.
	d._clients[firstempty].set_port_number (port_number);

	/// stores also the full symbolic name as index.
	/// this can be changed into a string copy since the name now contains a string.
	d._client_names[firstempty] = ((YARPUniqueNameSock&)name).getName();

	stream.close ();
	d._num_connected_clients ++;

	return YARP_OK;
}


int YARPOutputSocketMcast::SendBegin(char *buffer, int buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);
	d._hdr.SetGood ();
	d._hdr.SetLength (buffer_length);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "sending to: %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));

	///d._num_elements = 0;

	d._iov[0].iov_base = (char *)&d._hdr;
	d._iov[0].iov_len = sizeof(d._hdr);
	d._iov[1].iov_base = buffer;
	d._iov[1].iov_len = buffer_length;
	d._num_elements = 2;
	d._overall_msg_size = buffer_length + sizeof(d._hdr);

	return YARP_OK;
}


int YARPOutputSocketMcast::SendContinue(char *buffer, int buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);

	d._iov[d._num_elements].iov_base = buffer;
	d._iov[d._num_elements].iov_len = buffer_length;
	d._num_elements ++;
	d._overall_msg_size += buffer_length;

	return YARP_OK;
}

/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketMcast::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	ACE_OS::memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
}

int YARPOutputSocketMcast::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// the send can fail because the total buf is too big for the IP stack buffer.
	/// assert that situation (use the MAX_PACKET_SIZE as limit for sending msgs).
	/// NOTE: mcast is efficient but should be only used for relatively small messages.
	///	LATER: should set the socket buffer to handle bigger messages, at least the
	///		size of a 128sq color image.

	if (d._overall_msg_size > MAX_PACKET)
	{
		ACE_DEBUG ((LM_ERROR, "Implementation limit, pls, you should refrain from sending big MCAST packets\n"));
		ACE_DEBUG ((LM_ERROR, "Actual size is %d, allowed %d\n", d._overall_msg_size, MAX_PACKET));
		ACE_ASSERT (1 == 0);
	}

	int sent = d._connector_socket.send (d._iov, d._num_elements, 0);
	if (sent < 0)
		return YARP_FAIL;

	ACE_OS::memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
}


ACE_HANDLE YARPOutputSocketMcast::GetIdentifier(void) const
{
	return identifier;
}

#undef THIS_DBG
