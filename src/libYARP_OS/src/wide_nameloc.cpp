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
/// $Id: wide_nameloc.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <yarp/wide_nameloc.h>

///
///
///
void YARPNameQnx::set(const YARPString &str, const YARPString &node, NetInt32 pid, NetInt32 ch)
{
	setName(str);
	setAddr(node, pid, ch);
}

void YARPNameQnx::setName(const YARPString &str)
{	
	int len = ACE_OS::strlen(str.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_name, str.c_str());
	_name[len] = 0;
}

void YARPNameQnx::setAddr(const YARPString &node, NetInt32 pid, NetInt32 ch)
{
	int len = ACE_OS::strlen(node.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_node, node.c_str());
	_node[len] = 0;
	_pid = pid;
	_chan = ch;
}

void YARPNameQnx::getAddr(YARPString &node, NetInt32 *pid, NetInt32 *ch)
{
	node = YARPString(_node);
	*pid = _pid;
	*ch = _chan;
}

///
///
///
void YARPNameTCP::set(const YARPString &str, const ACE_INET_Addr &addr)
{
	setName(str);
	setAddr(addr);
}

void YARPNameTCP::setName(const YARPString &str)
{
	int len = ACE_OS::strlen (str.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_name, str.c_str());
	_name[len] = 0;
}

void YARPNameTCP::setAddr(const ACE_INET_Addr &addr)
{
	setIp(addr.get_host_addr());
	setPort(addr.get_port_number());
}

void YARPNameTCP::setIp(const YARPString &ip)
{
	int len = ACE_OS::strlen(ip.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_ip, ip.c_str());
	_ip[len] = 0;
}

void YARPNameTCP::setPort(NetInt32 p)
{
	_port = p;
}

void YARPNameTCP::getAddr(ACE_INET_Addr &addr)
{
	addr.set(_port, _ip);
}

///
///
///
void YARPNameUDP::set(const YARPString &str, const ACE_INET_Addr &addr)
{
	setName(str);
	setAddr(addr);
}

void YARPNameUDP::setName(const YARPString &str)
{
	int len = ACE_OS::strlen(str.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_name, str.c_str());
	_name[len] = 0;
}

void YARPNameUDP::setAddr(const ACE_INET_Addr &addr)
{
	setIp(addr.get_host_addr());
	setPorts(0, addr.get_port_number());
}

void YARPNameUDP::setIp(const YARPString &ip)
{
	int len = ACE_OS::strlen(ip.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_ip, ip.c_str());
	_ip[len] = 0;
}

void YARPNameUDP::setPorts(NetInt32 index, NetInt32 p)
{
	ACE_ASSERT( (index>=0) && (index<__YARP_NAMESERVICE_UDP_MAX_PORTS) );
	_ports[index] = p;
}

void YARPNameUDP::setNPorts(NetInt32 n)
{
	ACE_ASSERT( (n>=1) && (n<=__YARP_NAMESERVICE_UDP_MAX_PORTS) );
	_nPorts = n;
}

void YARPNameUDP::getAddr(ACE_INET_Addr &addr)
{
	addr.set(_ports[0], _ip);
}

NetInt32 YARPNameUDP::getPorts(NetInt32 index) 
{
	ACE_ASSERT( (index>=0) && (index<__YARP_NAMESERVICE_UDP_MAX_PORTS) );
	return _ports[index];
}

//
void YARPNSNic::set(const YARPString &ip, const YARPString &netId)
{
	int len = ACE_OS::strlen (ip.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_ip, ip.c_str());
	_ip[len] = 0;

	len = ACE_OS::strlen (netId.c_str());
	ACE_ASSERT (len < __YARP_NAMESERVICE_STRING_LEN);
	ACE_OS::strcpy(_netId, netId.c_str());
	_netId[len] = 0;
}
