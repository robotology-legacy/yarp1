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
/// $Id: YARPNameServer.h,v 1.5 2003-04-21 21:41:28 natta Exp $
///
///

// YARPNameServer.h: interface for the YARPNameServer class.
//
// 
// 
// -- January 2003 -- by nat 
// -- Modified for YARP April 2003 -- by nat 
//////////////////////////////////////////////////////////////////////

#if !defined __YARPNAMESERVER__
#define __YARPNAMESERVER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <conf/YARPConfig.h>
#include "wide_nameloc.h"

#include "ace\config.h"
#include "ace\SOCK_Acceptor.h"
#include "ace\SOCK_Connector.h"
#include "ace\SOCK_Stream.h"
#include "ace\Log_Msg.h"

#include "CThreadImpl.h"
#include "LocalNameServer.h"

#define SIZE_BUF 4096

class YARPNameServer: public CThreadImpl
{
public:
	YARPNameServer(const std::string &file, int port):
	  CThreadImpl("name server thread",0),
	  server_addr_(port), peer_acceptor_(server_addr_)
	{
		  ns.init(file);
		  data_buf_ = new char [SIZE_BUF];

		  start();
	}
	~YARPNameServer()
	{
		///peer_acceptor_.close();
		terminate();
		delete [] data_buf_;
	}

	int accept_connection();
	int handle_connection();

	// dump current status
	void dump_resources();
	void dump_names();
	void dump_statics();
	void dump()
	{
		cout << "\n\n-Dumping current status:";
		dump_statics();
		cout << "\n";
		dump_names();
		cout << "\n";
		dump_resources();
		cout << "-End";
	};
	
	void handle_registration(const std::string &service_name, const std::string &ip);
	void handle_query(const std::string &service_name);
	void handle_registration_dip(const std::string &service_name);
	void handle_registration_dip_dbg(const std::string &service_name);
	void handle_registration_dbg(const std::string &service_name, const std::string &ip);
	void query_dbg(const std::string &service_name);
	void handle_release(const std::string &service_name);

	// usual thread methods
	virtual void doInit()
	{
		// wait 0.5 sec...
		ACE_OS::sleep(ACE_Time_Value(0,500000));
		// just print port used
		if (peer_acceptor_.get_local_addr (server_addr_) != -1)
			NAME_SERVER_DEBUG (("Starting server at port %d\n", server_addr_.get_port_number ()));
		else
			NAME_SERVER_DEBUG (("Error: cannot get local address\n"));

	}
	virtual void doLoop()
	{
		if (accept_connection() != -1)
			handle_connection();
	}
	virtual void doRelease()
			{/* release, if any */}

private:
	void _handle_reply(const std::string &ip, int port);
	LocalNameServer ns;

	ACE_INET_Addr		server_addr_;
	ACE_INET_Addr		client_addr_;
	ACE_SOCK_Acceptor	peer_acceptor_;
	ACE_SOCK_Stream		new_stream_;

	char *data_buf_;
};

#endif // 
