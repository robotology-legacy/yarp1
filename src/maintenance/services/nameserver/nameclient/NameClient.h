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
/// $Id: NameClient.h,v 1.1 2003-04-18 08:52:33 gmetta Exp $
///
///

// NameClient.h: interface for the NameClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __NAME_CLIENT__
#define __NAME_CLIENT__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <conf/YARPConfig.h>
#include <YARPAll.h>
#include <ace/config.h>
#include <wide_nameloc.h>

#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

///#include "..\YARPNameService\YARPNameServer.h"

#include <string>

#ifdef NAME_CLIENT_VERBOSE
	#include <stdio.h>
	#define NAME_CLIENT_DEBUG(string) \
		do {printf("NAME_CLIENT: "),printf string;} while(0)
#else
	#define NAME_CLIENT_DEBUG(string)\
		do {} while(0)
#endif

#ifndef SIZE_BUF
	#define SIZE_BUF 4096
#endif

class NameClient  
{
public:
	NameClient(const std::string server, int port);
	NameClient(const ACE_INET_Addr &addr);
	virtual ~NameClient();

	int check_in (const std::string &s, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr)
	{
		int ret;
		ACE_INET_Addr tmpAddr = reg_addr;
		ret = _check_in(s, tmpAddr);
		addr = tmpAddr;
		return ret;
	}

	int check_in (const std::string &s, ACE_INET_Addr &addr)
	{
		int ret;
		ret = _check_in(s, addr);
		return ret;
	}
		
	int check_in (const std::string &s, std::string &ip, NetInt32 *port)
	{
		int ret;
		ACE_INET_Addr tmpAddr(ip.c_str());
		ret = _check_in(s, tmpAddr);
		*port = tmpAddr.get_port_number();
		ip = std::string(tmpAddr.get_host_name());
		return ret;
	}

	int query (const std::string &s, ACE_INET_Addr &addr)
	{
		int ret = _query(s,addr);
		return ret;
	}

	int check_out (const std::string &s)
	{
		// send data to server
		YARPNameServiceCmd tmpCmd;
		YARPNameTCP tmpRqst;
		
		if (connect_to_server()!=0)
			return YARP_FAIL;
		
		tmpRqst.setName(s);
		tmpCmd.cmd = YARPNSRelease;
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

		// close the connection
		close();
				
		return YARP_OK;
	}

private:
	// connect to server
	int connect_to_server ()
	{
		// initiate blocking connection with server
		// ACE_DEBUG((LM_DEBUG, "(%P|%t) starting connect to %s:%d\n"));
			
		// remote_addr_.get_host_name(), remote_addr_.get_port_number()));

		if (connector_.connect (client_stream_, remote_addr_) == -1)
			ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","connection failed"), -1);
		// else
		//	ACE_DEBUG ((LM_DEBUG, "(%P|%t) connect to %s\n",
		//	remote_addr_.get_host_name () ));
		
		return 0;
	}

	// close down the connection properly
	int close()
	{
		if (client_stream_.close() == -1)
			ACE_ERROR_RETURN ((LM_ERROR, "%P|%t) %p\n", "close"), -1);
		else
			return YARP_OK;
	}

	int _check_in(const std::string &s, ACE_INET_Addr &addr)
	{
		YARPNameServiceCmd tmpCmd;
		YARPNameTCP tmpRqst;
		
		if (connect_to_server()!=0)
			return YARP_FAIL;
				
		tmpRqst.setName(s);
		tmpRqst.setIp(addr.get_host_addr());
		tmpCmd.cmd = YARPNSRegister;
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
						
		NAME_CLIENT_DEBUG(("Received %s:%d\n", addr.get_host_addr(), addr.get_port_number()));
				
		// close the connection
		close();
				
		return YARP_OK;
	}

	int _query(const std::string &s, ACE_INET_Addr &addr)
	{
		YARPNameServiceCmd tmpCmd;
		YARPNameTCP tmpRqst;
		
		if (connect_to_server()!=0)
			return YARP_FAIL;
		
		tmpRqst.setName(s);
		tmpCmd.cmd = YARPNSQuery;
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
						
		NAME_CLIENT_DEBUG(("Received %s:%d\n", addr.get_host_addr(), addr.get_port_number()));
				
		// close the connection
		close();
				
		return YARP_OK;
	}

private:
	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
	
	char *data_buf_;
	char *reply_buf_;
};

#endif // .h
