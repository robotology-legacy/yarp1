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
/// $Id: YARPNameClient.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

// YARPNameClient.cpp: implementation of the YARPNameClient class.
//
//////////////////////////////////////////////////////////////////////

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <yarp/YARPNameClient.h>

#include <ace/OS.h>

//#define NAME_CLIENT_VERBOSE 

#ifdef NAME_CLIENT_VERBOSE
	#define NAME_CLIENT_DEBUG(string) do { ACE_OS::printf("NAME_CLIENT: "), ACE_OS::printf string; } while(0)
#else
	#define NAME_CLIENT_DEBUG(string) do {} while(0)
#endif

//#define YNC if(1) printf
#define YNC if(0) printf

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPNameClient::YARPNameClient(const YARPString& server, int port) : remote_addr_(port, server.c_str()), mutex_(1)
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

YARPString YARPNameClient::dump (int i)
{
	YARPString text;
	
	YARPNameServiceCmd tmpCmd;
	tmpCmd.cmd = YARPNSDumpRqs;
	tmpCmd.type = i;
	tmpCmd.length = sizeof(YARPNameServiceCmd);
	
	if (connect_to_server()!=0)
	{
		text = "Sorry, could not connect to the server.";
		return text;
	}

	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	
	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd);

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
	{
		text = "Sorry, an error accoured while getting server reply.";
		close();
		return text;
	}

	int ret = _handle_reply(text);
	if (ret == YARP_FAIL)
	{
		text = "Sorry, an error accoured while getting server reply.";
		close();
		return text;
	}
	
	// close the connection
	close();
			
	return text;
}

int YARPNameClient::check_in_mcast(const YARPString &s, ACE_INET_Addr &addr)
{
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInMcast(s, addr);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in (const YARPString &s, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr)
{
  YNC("YNC %s:%d --> check in %s\n",__FILE__,__LINE__,s.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ACE_INET_Addr tmpAddr = reg_addr;
	ret = _checkIn(s, tmpAddr);
	addr = tmpAddr;
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in (const YARPString &s, ACE_INET_Addr &addr)
{
  YNC("YNC %s:%d --> check in %s\n",__FILE__,__LINE__,s.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkIn(s, addr);
	mutex_.Post();
	return ret;
}
		
int YARPNameClient::check_in (const YARPString &s, YARPString &ip, NetInt32 *port)
{
  YNC("YNC %s:%d --> check in %s\n",__FILE__,__LINE__,s.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ACE_INET_Addr tmpAddr(ip.c_str());
	ret = _checkIn(s, tmpAddr);
	*port = tmpAddr.get_port_number();
	ip = YARPString(tmpAddr.get_host_name());
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_udp(const YARPString &name, YARPString &addr, NetInt32 *ports, NetInt32 n)
{
  YNC("YNC %s:%d --> check in %s\n",__FILE__,__LINE__,name.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInUdp(name, addr, ports, n);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_udp(const YARPString &name, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr, NetInt32 *ports, NetInt32 n)
{
  YNC("YNC %s:%d --> check in %s\n",__FILE__,__LINE__,name.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	YARPString ip = reg_addr.get_host_addr();
	ret = _checkInUdp(name, ip, ports, n);
	addr.set (ports[0], ip.c_str());
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_in_qnx(const YARPNameQnx &entry)
{
  YNC("YNC %s:%d --> check in \n",__FILE__,__LINE__);
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _checkInQnx(entry);
	mutex_.Post();
	return ret;
}

int YARPNameClient::query (const YARPString &s, ACE_INET_Addr &addr, int *type)
{
  YNC("YNC %s:%d --> query %s\n",__FILE__,__LINE__,s.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _query(s, addr, type);
	mutex_.Post();
	return ret;
}

int YARPNameClient::query_qnx (const YARPString &s, YARPNameQnx &entry, int *type)
{
  YNC("YNC %s:%d --> query %s\n",__FILE__,__LINE__,s.c_str());
	int ret = YARP_FAIL;
	mutex_.Wait();
	ret = _queryQnx(s, entry, type);
	mutex_.Post();
	return ret;
}

int YARPNameClient::query_nic(const YARPString &inIp, const YARPString &netId, YARPString &outNic, YARPString &outIp)
{
  YARPString myIp = inIp;
  YNC("YNC %s:%d --> query_nic %s\n",__FILE__,__LINE__,inIp.c_str());
  if (myIp==YARPString("127.0.0.1")) {
    // localhost is no good, no good at all
    char buf[256];
    gethostname(buf,sizeof(buf));
    myIp = buf;
  }
	int ret = YARP_FAIL;
	mutex_.Wait();
	YARPNSNic tmp;
	tmp.set(myIp, netId);
	ret = _query_nic(tmp, outNic, outIp);
	mutex_.Post();
	return ret;
}

int YARPNameClient::check_out (const YARPString &s)
{
  YNC("YNC %s:%d --> check_out %s\n",__FILE__,__LINE__,s.c_str());
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
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

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

int YARPNameClient::check_out_qnx (const YARPString &s)
{
  YNC("YNC %s:%d --> check_out %s\n",__FILE__,__LINE__,s.c_str());
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
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

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
	  //ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","connection failed"), -1);
	  ACE_DEBUG ((LM_DEBUG, "(%P|%t) %p\n","connection failed"));
	  return YARP_FAIL;
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

int YARPNameClient::_checkIn(const YARPString &s, ACE_INET_Addr &addr)
{
  YNC("YNC %s:%d --> _checkIn %s\n",__FILE__,__LINE__,s.c_str());
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
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
				
	unsigned int byte_count = 0;
	int res = 0;
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
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

int YARPNameClient::_checkInUdp(const YARPString &name, const YARPString &ip, NetInt32 *ports, NetInt32 n)
{
  YNC("YNC %s:%d --> _checkIn %s\n",__FILE__,__LINE__,name.c_str());
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
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
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

int YARPNameClient::_checkInMcast(const YARPString &s, ACE_INET_Addr &addr)
{
  YNC("YNC %s:%d --> _checkIn %s\n",__FILE__,__LINE__,s.c_str());
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
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
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
  YNC("YNC %s:%d --> _checkIn \n",__FILE__,__LINE__);
	YARPNameServiceCmd tmpCmd;
	YARPNameQnx tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
		
	tmpRqst = entry;
	tmpCmd.cmd = YARPNSRegister;
	tmpCmd.type = YARP_QNET;
	tmpCmd.length = tmpRqst.length();
	// send message length
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

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

int YARPNameClient::_query(const YARPString &s, ACE_INET_Addr &addr, int *type)
{
  YNC("YNC %s:%d --> _query %s\n",__FILE__,__LINE__,s.c_str());
	YARPNameServiceCmd tmpCmd;
	YARPNameTCP tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;

	tmpRqst.setName(s);

	tmpCmd.cmd = YARPNSQuery;
	tmpCmd.type = YARP_TCP;	// TCP, UDP, MCAST queries are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	YARPNameServiceCmd *srvCmd = (YARPNameServiceCmd *)data_buf_;
	*type = srvCmd->type;		// get address type
		
	iov[0].iov_len = srvCmd->length;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	YARPNameTCP *srvRpl = (YARPNameTCP *)data_buf_;
	srvRpl->getAddr(addr);
						
	NAME_CLIENT_DEBUG(("underlying ip %s port %d\n",
			   srvRpl->_ip, srvRpl->_port));
	NAME_CLIENT_DEBUG(("Received %s(%s):%d\n", addr.get_host_addr(), servicetypeConverter(*type), addr.get_port_number()));
			
	// close the connection
	close();
				
	return YARP_OK;
}

int YARPNameClient::_query_nic(const YARPNSNic &in, YARPString &outNic, YARPString &outIp)
{
  YNC("YNC %s:%d --> _query_nic\n",__FILE__,__LINE__);
  ACE_DEBUG((LM_DEBUG,"Oi! Nic!\n"));
	YARPString reply;
	YARPNameServiceCmd tmpCmd;
	if (connect_to_server()!=0)
		return YARP_FAIL;
	
	tmpCmd.cmd = YARPNSNicQuery;
	tmpCmd.length = sizeof(YARPNSNic);

	// send message length
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &in, tmpCmd.length);

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpCmd.length;

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	_handle_reply(reply);

	char tmp1[255];
	char tmp2[255];
	sscanf(reply.c_str(), "%s\n%s", tmp1, tmp2);
	outNic = YARPString(tmp1);
	outIp = YARPString(tmp2);

	ACE_DEBUG((LM_DEBUG,"Oi! Nic! got %s // %s\n", outNic.c_str(), outIp.c_str()));

	// close the connection
	close();

	return YARP_OK;
}

int YARPNameClient::_queryQnx(const YARPString &s, YARPNameQnx &entry, int *type)
{
  YNC("YNC %s:%d --> _query\n",__FILE__,__LINE__);
	YARPNameServiceCmd tmpCmd;
	YARPNameQnx tmpRqst;
	
	if (connect_to_server()!=0)
		return YARP_FAIL;
	
	tmpRqst.setName(s);
	tmpCmd.cmd = YARPNSQuery;
	tmpCmd.type = YARP_QNET;	// TCP, UDP, MCAST queries are handled in the same way
	tmpCmd.length = tmpRqst.length();
	// send message length
	ACE_OS::memcpy(data_buf_,&tmpCmd, sizeof(YARPNameServiceCmd));
	ACE_OS::memcpy(data_buf_+sizeof(YARPNameServiceCmd), &tmpRqst, tmpRqst.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd)+tmpRqst.length();

	int sent = client_stream_.sendv_n (iov, 1);

	if (sent == -1)
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);

	//////////////////////////////////////////
			
	unsigned int byte_count = 0;
	int res = 0;
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
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

int YARPNameClient::_handle_reply(YARPString &out)
{
  YNC("YNC %s:%d --> _handle_reply\n",__FILE__,__LINE__);
	unsigned int byte_count = 0;
	int res = 0;
	out = "";
		
	ACE_OS::memset(data_buf_, 0, SIZE_BUF);
	
	// first vector
	iovec iov[1];
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	iov[0].iov_base = data_buf_;
	res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
	
	YARPNameServiceCmd *srvCmd = (YARPNameServiceCmd *)data_buf_;
	int length = srvCmd->length;
	int nVectors = length/SIZE_BUF;
	int lastVectLength = length%SIZE_BUF;
	
	int i = 0;

	for(i=0; i<nVectors;i++)
	{
		// ith vector
		iov[0].iov_base = data_buf_;
		iov[0].iov_len = SIZE_BUF;
		res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
		///out.append(data_buf_,byte_count);
		out += YARPString(data_buf_, byte_count);
	}
	
	if (lastVectLength > 0)
	{
		// last vector
		iov[0].iov_base = data_buf_;
		iov[0].iov_len = lastVectLength;
		res = client_stream_.recvv_n(iov, 1, 0, &byte_count);
		///out.append(data_buf_,byte_count);
		out += YARPString(data_buf_, byte_count);
	}
							
	// close the connection
	close();
				
	return YARP_OK;
}

