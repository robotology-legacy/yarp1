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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNameClient.cpp,v 1.5 2003-06-30 09:30:05 babybot Exp $
///
///

// YARPNameClient.cpp: implementation of the YARPNameClient class.
//
//////////////////////////////////////////////////////////////////////

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include "YARPNameClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPNameClient::YARPNameClient(const std::string server, int port) : remote_addr_(port, server.c_str()), mutex_(1)
{
	data_buf_ = new char [SIZE_BUF];
}

YARPNameClient::YARPNameClient(const ACE_INET_Addr &addr) : remote_addr_(addr), mutex_(1)
{
	data_buf_ = new char [SIZE_BUF];
}

YARPNameClient::~YARPNameClient()
{
	delete [] data_buf_;
}

int YARPNameClient::check_in_mcast(const std::string &s, ACE_INET_Addr &addr)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInMcast(s, addr);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in (const std::string &s, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ACE_INET_Addr tmpAddr = reg_addr;
	ret = _checkIn(s, tmpAddr);
	addr = tmpAddr;
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in (const std::string &s, ACE_INET_Addr &addr)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkIn(s, addr);
	mutex_.Post();
	return ret;
}
		
int YARPNameClient::check_in (const std::string &s, std::string &ip, NetInt32 *port)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ACE_INET_Addr tmpAddr(ip.c_str());
	ret = _checkIn(s, tmpAddr);
	*port = tmpAddr.get_port_number();
	ip = std::string(tmpAddr.get_host_name());
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_udp(const std::string &name, std::string &addr, NetInt32 *ports, NetInt32 n)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInUdp(name, addr, ports, n);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_udp(const std::string &name, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr, NetInt32 *ports, NetInt32 n)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	std::string ip = reg_addr.get_host_addr();
	ret = _checkInUdp(name, ip, ports, n);
	addr.set (ports[0], ip.c_str());
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_qnx(const YARPNameQnx &entry)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInQnx(entry);
	mutex_.Post();
	return ret;
}

int YARPNameClient::query (const std::string &s, ACE_INET_Addr &addr, int *type)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _query(s, addr, type);
	mutex_.Post();
	return ret;
}

int YARPNameClient::query_qnx (const std::string &s, YARPNameQnx &entry, int *type)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _queryQnx(s, entry, type);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_out (const std::string &s)
{
	mutex_.Wait();
	// send data to server
	YARPNameServiceCmd tmpCmd;
	YARPNameTCP tmpRqst;
		
	if (connect_to_server()!=0)
	{
		mutex_.Post();
		return YARP_FAIL;
	}

	tmpRqst.setName(s);
	tmpCmd.cmd = YARPNSRelease;
	tmpCmd.type = YARP_TCP;	// MCAST, TCP and UDP releases are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
	{
		mutex_.Post();
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"), YARP_FAIL);
	}

	// close the connection
	close();
			
	mutex_.Post();
	return YARP_OK;
}

int YARPNameClient::check_out_qnx (const std::string &s)
{
	mutex_.Wait();
	// send data to server
	YARPNameServiceCmd tmpCmd;
	YARPNameQnx tmpRqst;
	
	if (connect_to_server()!=0)
	{
		mutex_.Post();
		return YARP_FAIL;
	}

	tmpRqst.setName(s);
	tmpCmd.cmd = YARPNSRelease;
	tmpCmd.type = YARP_QNET;	// MCAST, TCP and UDP releases are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
	{
		mutex_.Post();
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"), YARP_FAIL);
	}

	// close the connection
	close();
	
	mutex_.Post();
	return YARP_OK;
}

// connect to server
int YARPNameClient::connect_to_server ()
{
	if (connector_.connect (client_stream_, remote_addr_) == -1)
	{
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","connection failed"), -1);
	}

	int one = 1;
	client_stream_.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));

	return YARP_OK;
}

// close down the connection properly
int YARPNameClient::close()
{
	if (client_stream_.close() == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "%P|%t) %p\n", "close"), -1);
	else
		return YARP_OK;
}

int YARPNameClient::_checkIn(const std::string &s, ACE_INET_Addr &addr)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameTCP tmpRqst;
	
	if (connect_to_server() != 0)
		return YARP_FAIL;
			
	tmpRqst.setName(s);
	tmpRqst.setIp(addr.get_host_addr());
	tmpCmd.cmd = YARPNSRegister;
	tmpCmd.type = YARP_TCP;
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
				
	unsigned int byte_count = 0;
	int res = 0;
		
	memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	iov[0].iov_len = tmpCmd.length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	YARPNameTCP *tmpRpl = (YARPNameTCP *)data_buf_;
	tmpRpl->getAddr(addr);
					
	NAME_CLIENT_DEBUG(("Received %s(%s):%d\n", addr.get_host_addr(), servicetypeConverter(tmpCmd.type), addr.get_port_number()));
			
	// close the connection
	close();
			
	return YARP_OK;
}

int YARPNameClient::_checkInUdp(const std::string &name, const std::string &ip, NetInt32 *ports, NetInt32 n)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameUDP tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
			
	tmpRqst.setName(name);
	tmpRqst.setIp(ip);
	tmpRqst.setNPorts(n);
	tmpCmd.cmd = YARPNSRegister;
	tmpCmd.type = YARP_UDP;
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	iov[0].iov_len = tmpCmd.length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	YARPNameUDP *tmpRpl = (YARPNameUDP *)data_buf_;

	for (int i = 0; i < n; i++)
	{
		ports[i] = tmpRpl->getPorts(i);
		NAME_CLIENT_DEBUG(("Received %s(%d):%d\n", ip.c_str(), (int)tmpCmd.type, (int)ports[i]));
	}
						
	// close the connection
	close();
			
	return YARP_OK;
}

int YARPNameClient::_checkInMcast(const std::string &s, ACE_INET_Addr &addr)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameTCP tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
			
	tmpRqst.setName(s);
	tmpRqst.setIp(addr.get_host_addr());
	tmpCmd.cmd = YARPNSRegister;
	tmpCmd.type = YARP_MCAST;
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	iov[0].iov_len = tmpCmd.length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
		
	YARPNameTCP *tmpRpl = (YARPNameTCP *)data_buf_;
	tmpRpl->getAddr(addr);
					
	NAME_CLIENT_DEBUG(("Received %s(%s):%d\n", addr.get_host_addr(), servicetypeConverter(tmpCmd.type), addr.get_port_number()));
			
	// close the connection
	close();
				
	return YARP_OK;
}

int YARPNameClient::_checkInQnx(const YARPNameQnx &entry)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameQnx tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
		
	tmpRqst = entry;
	tmpCmd.cmd = YARPNSRegister;
	tmpCmd.type = YARP_QNET;
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	// no reply here, just close and exit
	close();
			
	return YARP_OK;
}

int YARPNameClient::_query(const std::string &s, ACE_INET_Addr &addr, int *type)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameTCP tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;

	tmpRqst.setName(s);
	ACE_OS::printf ("----- DAMN include files code! ----- %s\n", tmpRqst.getName());

	tmpCmd.cmd = YARPNSQuery;
	tmpCmd.type = YARP_TCP;	// TCP, UDP, MCAST queries are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	YARPNameServiceCmd *srvCmd = (YARPNameServiceCmd *)data_buf_;
	*type = srvCmd->type;		// get address type
		
	iov[0].iov_len = srvCmd->length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	YARPNameTCP *srvRpl = (YARPNameTCP *)data_buf_;
	srvRpl->getAddr(addr);
						
	NAME_CLIENT_DEBUG(("Received %s(%s):%d\n", addr.get_host_addr(), servicetypeConverter(*type), addr.get_port_number()));
			
	// close the connection
	close();
				
	return YARP_OK;
}

int YARPNameClient::_queryQnx(const std::string &s, YARPNameQnx &entry, int *type)
{
	YARPNameServiceCmd tmpCmd;
	YARPNameQnx tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
	
	tmpRqst.setName(s);
	tmpCmd.cmd = YARPNSQuery;
	tmpCmd.type = YARP_QNET;	// TCP, UDP, MCAST queries are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	YARPNameServiceCmd *srvCmd = (YARPNameServiceCmd *)data_buf_;
	*type = srvCmd->type;		// get address type
	
	iov[0].iov_len = srvCmd->length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	entry = *(YARPNameQnx *)data_buf_;
									
	NAME_CLIENT_DEBUG(("Received %s %s(%s) %d %d\n", entry.getName(), entry.getNode(), servicetypeConverter(*type), (int)entry.getPid(), (int)entry.getChan()));
			
	// close the connection
	close();
			
	return YARP_OK;
}
