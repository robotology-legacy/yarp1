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
/// $Id: YARPNameClient.h,v 1.4 2004-07-09 16:10:13 eshuy Exp $
///
///

// YARPNameClient.h: interface for the YARPNameClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __NAME_CLIENT__
#define __NAME_CLIENT__

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>
#include <ace/config.h>

#include <yarp/wide_nameloc.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSemaphore.h>

#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPString.h>

///#define NAME_CLIENT_VERBOSE 

#ifdef NAME_CLIENT_VERBOSE
	#define NAME_CLIENT_DEBUG(string) do { ACE_OS::printf("NAME_CLIENT: "), ACE_OS::printf string; } while(0)
#else
	#define NAME_CLIENT_DEBUG(string) do {} while(0)
#endif

#ifndef SIZE_BUF
	#define SIZE_BUF 4096
#endif

class YARPNameClient  
{
public:
	YARPNameClient(const YARPString& server, int port);
	YARPNameClient(const ACE_INET_Addr &addr);
	virtual ~YARPNameClient();

	int check_in_mcast(const YARPString &s, ACE_INET_Addr &addr);
	int check_in (const YARPString &s, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr);
	int check_in (const YARPString &s, ACE_INET_Addr &addr);
	int check_in (const YARPString &s, YARPString &ip, NetInt32 *port);
	int check_in_udp(const YARPString &name, YARPString &addr, NetInt32 *ports, NetInt32 n);
	int check_in_udp(const YARPString &name, const ACE_INET_Addr &reg_addr, ACE_INET_Addr &addr, NetInt32 *ports, NetInt32 n);
	int check_in_qnx(const YARPNameQnx &entry);
	int query (const YARPString &s, ACE_INET_Addr &addr, int *type);
	int query_qnx (const YARPString &s, YARPNameQnx &entry, int *type);
	int query_nic(const YARPString &inIp, const YARPString &netId, YARPString &outNic, YARPString &outIp);
	int check_out (const YARPString &s);
	int check_out_qnx (const YARPString &s);
	YARPString dump(int i = 0);	// 0 short, 1 extended
	int _handle_reply(YARPString &out);

private:
	// connect to server
	int connect_to_server ();
	// close down the connection properly
	int close();
	int _checkIn(const YARPString &s, ACE_INET_Addr &addr);
	int _checkInUdp(const YARPString &name, const YARPString &ip, NetInt32 *ports, NetInt32 n);
	int _checkInMcast(const YARPString &s, ACE_INET_Addr &addr);
	int _checkInQnx(const YARPNameQnx &entry);
	int _query(const YARPString &s, ACE_INET_Addr &addr, int *type);
	int _queryQnx(const YARPString &s, YARPNameQnx &entry, int *type);
	int _query_nic(const YARPNSNic &in, YARPString &outNic, YARPString &outIp);

private:
	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
	YARPSemaphore mutex_;
	
	char *data_buf_;
	char *reply_buf_;
};

#endif // .h
