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
///                    #pasa, paulfitz, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: wide_nameloc.h,v 1.6 2003-04-27 16:54:35 natta Exp $
///
///

#ifndef wide_nameloc_INC
#define wide_nameloc_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#include "YARPNetworkTypes.h"
#include "YARPNameID_defs.h"

#include <string>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif


///
///
///
#define __YARP_NAMESERVICE_STRING_LEN 256
#define __YARP_NAMESERVICE_UDP_MAX_PORTS 21

const int __portNotFound = 0;
const char __ipNotFound[] = {"0.0.0.0"};


///
///
///
enum
{
	YARPNSRegister = 0,
	YARPNSRelease = 1,
	YARPNSQuery = 2,
	YARPNSRpl = 3
};

#include "begin_pack_for_net.h"
struct YARPNameServiceCmd
{
	NetInt32 cmd;
	NetInt32 length;
	NetInt32 type;
} PACKED_FOR_NET;

class YARPNameQnx
{
public:
	void set(const std::string &str, const std::string &node, NetInt32 pid, NetInt32 ch)
	{
		setName(str);
		setAddr(node, pid, ch);
	}

	void setName(const std::string &str)
	{
		strncpy(_name, str.c_str(), __YARP_NAMESERVICE_STRING_LEN);
	}

	void setAddr(const std::string &node, NetInt32 pid, NetInt32 ch)
	{
		strncpy(_node, node.c_str(), __YARP_NAMESERVICE_STRING_LEN);
		_pid = pid;
		_chan = ch;
	}

	void getAddr(std::string &node, NetInt32 *pid, NetInt32 *ch)
	{
		node = std::string(_node);
		*pid = _pid;
		*ch = _chan;
	}

	const char *getName() const
	{return _name;}

	const char *getNode() const
	{return _node;}

	NetInt32 getPid() const
	{return _pid;}
	
	NetInt32 getChan() const
	{return _chan;}

	int length() const
	{return sizeof(YARPNameQnx);}

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _node[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _pid;
	NetInt32 _chan;
} PACKED_FOR_NET;

class YARPNameTCP
{
public:
	void set(const std::string &str, const ACE_INET_Addr &addr)
	{
		setName(str);
		setAddr(addr);
	}

	void setName(const std::string &str)
	{
		strncpy(_name, str.c_str(), __YARP_NAMESERVICE_STRING_LEN);
	}

	void setAddr(const ACE_INET_Addr &addr)
	{
		setIp(addr.get_host_addr());
		setPort(addr.get_port_number());
	}

	void setIp(const std::string &ip)
	{
		strncpy(_ip, ip.c_str(), __YARP_NAMESERVICE_STRING_LEN);
	}

	void setPort(NetInt32 p)
	{
		_port = p;
	}

	void getAddr(ACE_INET_Addr &addr)
	{
		addr.set(_port, _ip);
	}

    const char *getName() const
	{return _name;}

	const char *getIp() const
	{return _ip;}

	NetInt32 getPort()
	{return _port;}

	int length()
	{return sizeof(YARPNameTCP);}

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _ip[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _port;
} PACKED_FOR_NET;

class YARPNameUDP
{
public:
	void set(const std::string &str, const ACE_INET_Addr &addr)
	{
		setName(str);
		setAddr(addr);
	}

	void setName(const std::string &str)
	{
		strncpy(_name, str.c_str(), __YARP_NAMESERVICE_STRING_LEN);
	}

	void setAddr(const ACE_INET_Addr &addr)
	{
		setIp(addr.get_host_addr());
		setPorts(0, addr.get_port_number());
	}

	void setIp(const std::string &ip)
	{
		strncpy(_ip, ip.c_str(), __YARP_NAMESERVICE_STRING_LEN);
	}

	void setPorts(NetInt32 index, NetInt32 p)
	{
		ACE_ASSERT( (index>=0) && (index<__YARP_NAMESERVICE_UDP_MAX_PORTS) );
		_ports[index] = p;
	}

	void setNPorts(NetInt32 n)
	{
		ACE_ASSERT( (n>=1) && (n<=__YARP_NAMESERVICE_UDP_MAX_PORTS) );
		_nPorts = n;
	}

	void getAddr(ACE_INET_Addr &addr)
	{
		addr.set(_ports[0], _ip);
	}

	const char *getName() const
	{return _name;}

	const char *getIp() const
	{return _ip;}

	NetInt32 getPorts(NetInt32 index) {
		ACE_ASSERT( (index>=0) && (index<__YARP_NAMESERVICE_UDP_MAX_PORTS) );
		return _ports[index];
	}

	int length()
	{return sizeof(YARPNameUDP);}

	char _name[__YARP_NAMESERVICE_STRING_LEN];
	char _ip[__YARP_NAMESERVICE_STRING_LEN];
	NetInt32 _ports[__YARP_NAMESERVICE_UDP_MAX_PORTS];
	NetInt32 _nPorts;
} PACKED_FOR_NET;
#include "end_pack_for_net.h"

///
///
///
///enum NameServiceRequest
///{
///	NAME_NO_ACTION,
///	NAME_REGISTER,
///	NAME_LOOKUP,
///};

///
///
///
#if 0
#include "begin_pack_for_net.h"
struct NameServiceHeader
{
	NetInt32 port;
	NetInt32 key_length;
	NetInt32 machine_length;
	NetInt32 spare1;
	NetInt32 spare2;
	char request_type;
} PACKED_FOR_NET;
#include "end_pack_for_net.h"
#endif




#endif