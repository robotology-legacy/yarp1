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
/// $Id: YARPNameServer.cpp,v 1.14 2003-07-10 13:33:46 babybot Exp $
///
///

// YARPNameServer.cpp: implementation of the NameServer class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPNameServer.h"

#include <fstream>
#include <iostream>

using namespace std;

// general function
char * GetYarpRoot (void)
{
	char * ret = getenv ("YARP_ROOT");
	if (ret == NULL)
	{
		ACE_DEBUG ((LM_DEBUG, "::GetYarpRoot : can't retrieve YARP_ROOT env variable\n"));
	}
	return ret;
}

int YARPNameServer::accept_connection()
{
	ACE_Time_Value timeout (5, 0);
	int ret;
	int one = 1;

	ret = peer_acceptor_.accept (new_stream_, &client_addr_, &timeout);
	new_stream_.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
	return ret;
}

void YARPNameServer::dump_statics()
{
	cout << "\n-Static entries: " << endl;
	
	for(SVC_IT i = ns.statics.begin(); i != ns.statics.end(); i++)
	{
		cout << i->name << "\t\t";
		cout << i->ip << "\tmax ref:" << i->get_max_ref() << endl;
	}
}

void YARPNameServer::dump_names()
{
	cout << "-Names: " << endl;
	
	for(SVC_IT i = ns.names.begin(); i != ns.names.end(); i++)
	{
		cout << i->name << "\t\t";
		cout << i->ip;
		cout << ":";

		///int length = i->ports.size();
		for(PORT_IT j = i->ports.begin(); j != i->ports.end(); j++)
			cout << j->port << "," ;
		cout << "ref:" << (i->get_max_ref()-i->get_ref());
		cout << ",(" << servicetypeConverter(i->type) << ")"<< endl;
	}

	for(QNXSVC_IT j = ns.qnx_names.begin(); j != ns.qnx_names.end(); j++)
	{
		cout << j->getName() << "\t\t";
		cout << j->getNode() << "\t";
		cout << j->getPid() << "\t";
		cout << j->getChan() << "\t";
		cout << "(" << servicetypeConverter(YARP_QNET) << ")";
		cout << endl;
	}
}

void YARPNameServer::dump_resources()
{
	cout << "-Resources: " << endl;
	
	for(IP_IT i = ns.addresses.begin(); i != ns.addresses.end(); i++)
	{
		cout << i->ip << ":" << endl;

		for(PORT_IT j = i->ports.begin(); j != i->ports.end(); j++)
			cout << "\t" << j->port << endl;
	}
}

void YARPNameServer::handle_dump_request()
{
	std::string text;

	for(SVC_IT i = ns.names.begin(); i != ns.names.end(); i++)
	{
		text.append(i->name);
		text.append("\n");
		// cout << i->ip;
		// cout << ":";
		/*
		///int length = i->ports.size();
		for(PORT_IT j = i->ports.begin(); j != i->ports.end(); j++)
			cout << j->port << "," ;
		cout << "ref:" << (i->get_max_ref()-i->get_ref());
		cout << ",(" << servicetypeConverter(i->type) << ")"<< endl;
		*/
	}

	for(QNXSVC_IT j = ns.qnx_names.begin(); j != ns.qnx_names.end(); j++)
	{
		text.append(j->getName());
		text.append("\n");
		/*
		cout << j->getNode() << "\t";
		cout << j->getPid() << "\t";
		cout << j->getChan() << "\t";
		cout << "(" << servicetypeConverter(YARP_QNET) << ")";
		cout << endl;
		*/
	}
	if (text.empty())
		text = "No names registered !\n";

	_handle_reply(text);
}

void YARPNameServer::handle_exdump_request()
{
	std::string text;

	char dummy[255];
	for(SVC_IT i = ns.names.begin(); i != ns.names.end(); i++)
	{
		text.append("-");
		text.append(i->name);
		text.append("\t");
		text.append(i->ip);
		text.append(":");
		int length = i->ports.size();
		for(PORT_IT j = i->ports.begin(); j != i->ports.end(); j++)
		{
			itoa(j->port, dummy, 10);
			text.append (dummy);
			text.append (",");
		}
		text.append (" (");
		text.append (servicetypeConverter(i->type));
		text.append (")");
		text.append ("\n");
		
	}

	for(QNXSVC_IT j = ns.qnx_names.begin(); j != ns.qnx_names.end(); j++)
	{
		text.append("-");
		text.append(j->getName());
		text.append("\t");
		text.append(j->getNode());
		text.append("\t");
		itoa(j->getPid(), dummy, 10);
		text.append(dummy);
		text.append("\t");
		itoa(j->getChan(),dummy, 10);
		text.append(dummy);
		text.append (" (");
		text.append (servicetypeConverter(YARP_QNET));
		text.append (")");
		text.append ("\n");
	}
	if (text.empty())
		text = "No names registered !\n";

	_handle_reply(text);
}


void YARPNameServer::handle_registration(const std::string &service_name, const std::string &ip, int type, int np)
{
	PORT_LIST ports;
	IpEntry tmpEntry;
	tmpEntry.ip = ip;
	ns.registerName(service_name, tmpEntry, type, ports, np);
	
	for(PORT_IT i = ports.begin(); i != ports.end(); i++)
	{
		NAME_SERVER_DEBUG(("Registered %s as %s(%s):%d\n", service_name.c_str(), tmpEntry.ip.c_str(), servicetypeConverter(type), i->port));
	}	
	_handle_reply(tmpEntry.ip, type, ports);
}

void YARPNameServer::handle_query(const std::string &service_name)
{
	std::string ip;
	int port;
	int type;
	ns.queryName(service_name, ip, &type, &port);
	NAME_SERVER_DEBUG(("Reply %s as %s(%s):%d\n", service_name.c_str(), ip.c_str(), servicetypeConverter(type), port));
	_handle_reply(ip, type, port);
}

void YARPNameServer::handle_query_qnx(const std::string &name)
{
	YARPNameQnx entry;
	int type;
	ns.queryNameQnx(name, entry, &type);
	NAME_SERVER_DEBUG(("Reply %s %s(%s) %ld %ld\n", entry.getName(), entry.getNode(), servicetypeConverter(type), entry.getPid(), entry.getChan()));
	_handle_reply(entry, type);
}

void YARPNameServer::handle_registration_dbg(const std::string &service_name, const std::string &ip, int type, int n)
{
	PORT_LIST ports;

	NAME_SERVER_DEBUG(("DBG MODE: registering %s as %s(%s)\n", service_name.c_str(), ip.c_str(), servicetypeConverter(type)));

	IpEntry tmpEntry;
	tmpEntry.ip = ip;

	ns.registerName(service_name, tmpEntry, type, ports, n);

	for(PORT_IT i = ports.begin(); i != ports.end(); i++)
	{
		NAME_SERVER_DEBUG(("Registered %s as %s(%s):%d\n", service_name.c_str(), tmpEntry.ip.c_str(), servicetypeConverter(type), i->port));
	}
}

void YARPNameServer::handle_registration_dip_dbg(const std::string &service_name, int type)
{
	std::string ip;
	int port;

	NAME_SERVER_DEBUG(("DBG MODE: registering %s (%s)\n", service_name.c_str(), servicetypeConverter(type)));

	ns.registerNameDIp(service_name, ip, type, &port);
	NAME_SERVER_DEBUG(("DBG MODE: answering %s(%s):%d\n", ip.c_str(), servicetypeConverter(type), port));
}

void YARPNameServer::handle_registration_dip(const std::string &service_name, int type)
{
	int port;
	std::string ip;
	ns.registerNameDIp(service_name, ip, type, &port);
	NAME_SERVER_DEBUG(("Registered %s as %s(%s):%d\n", service_name.c_str(), ip.c_str(), servicetypeConverter(type), port));
	_handle_reply(ip, type, port);
}

void YARPNameServer::handle_registration_qnx(const YARPNameQnx &entry)
{
	ns.registerNameQnx(entry);
	NAME_SERVER_DEBUG(("Registered %s %s(%s) %ld %ld\n", entry.getName(), entry.getNode(), servicetypeConverter(YARP_QNET), entry.getPid(), entry.getChan()));
	// _handle_reply(entry, type);
}

void YARPNameServer::handle_release_qnx(const string &name)
{
	NAME_SERVER_DEBUG(("QNX name %s no longer used, releasing\n", name.c_str()));
	ns.check_out_qnx(name);
	// _handle_reply(entry, type);
}

void YARPNameServer::_handle_reply(const std::string &text)
{
	YARPNameServiceCmd rplCmd;
	
	int length = text.length()+1;
	rplCmd.cmd = YARPNSDumpRpl;
	rplCmd.length = length;
		
	iovec iov[1];
	// first vector
	iov[0].iov_base = data_buf_;
	memcpy(data_buf_ , &rplCmd, sizeof(YARPNameServiceCmd));
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	new_stream_.sendv_n (iov, 1);
	
	int nVectors = length/SIZE_BUF;
	int lastVectLength = length%SIZE_BUF;

	int i = 0;
	for(i=0; i < nVectors;i++)
	{
		// nth vector
		memcpy(data_buf_, text.c_str()+SIZE_BUF*(i), SIZE_BUF);
		iov[0].iov_len = SIZE_BUF;
		new_stream_.sendv_n (iov, 1);
	}

	// last one ! (if any)
	if (lastVectLength > 0)
	{	
		memcpy(data_buf_, text.c_str()+SIZE_BUF*(nVectors), lastVectLength);
		iov[0].iov_len = lastVectLength;
		new_stream_.sendv_n (iov, 1);
	}
}

void YARPNameServer::_handle_reply(const std::string &ip, int type, int port)
{
	YARPNameServiceCmd rplCmd;
	YARPNameTCP rpl;
	rpl.setAddr(ACE_INET_Addr(port, ip.c_str()));
	rplCmd.cmd = YARPNSRpl;
	rplCmd.type = type;
	rplCmd.length = rpl.length();
		
	memcpy(data_buf_, &rplCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &rpl, rpl.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = rplCmd.length+sizeof(YARPNameServiceCmd);

	///int sent = new_stream_.sendv_n (iov, 1);
	new_stream_.sendv_n (iov, 1);
}

void YARPNameServer::_handle_reply(const std::string &ip, int type, const PORT_LIST &ports)
{
	YARPNameServiceCmd rplCmd;
	YARPNameUDP rpl;

	int j = 0;
	for(CONST_PORT_IT i = ports.begin(); i != ports.end(); i++)
	{
		rpl.setPorts(j, i->port);
		j++;
	}
	rpl.setIp(ip);
	
	rplCmd.cmd = YARPNSRpl;
	rplCmd.length = rpl.length();
	rplCmd.type = type;
		
	memcpy(data_buf_, &rplCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &rpl, rpl.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = rplCmd.length+sizeof(YARPNameServiceCmd);

	///int sent = new_stream_.sendv_n (iov, 1);
	new_stream_.sendv_n (iov, 1);
}

void YARPNameServer::_handle_reply(const YARPNameQnx &entry, int type)
{
	YARPNameServiceCmd rplCmd;
	YARPNameQnx rpl = entry;

	rplCmd.cmd = YARPNSRpl;
	rplCmd.length = rpl.length();
	rplCmd.type = type;
		
	memcpy(data_buf_, &rplCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &rpl, rpl.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = rplCmd.length+sizeof(YARPNameServiceCmd);

	int sent = new_stream_.sendv_n (iov, 1);
	//new_stream_.sendv_n (iov, 1);
}

void YARPNameServer::query_dbg(const std::string &service_name)
{
	std::string ip;
	int port;
	int type;

	NAME_SERVER_DEBUG(("DBG MODE: query %s\n", service_name.c_str()));

	ns.queryName(service_name, ip, &type, &port);

	NAME_SERVER_DEBUG(("DBG MODE: answering %s(%s):%d\n", ip.c_str(), servicetypeConverter(type), port));

}

void YARPNameServer::handle_release(const std::string &service_name)
{
	NAME_SERVER_DEBUG(("Releasing:%s\n", service_name.c_str()));
	ns.check_out(service_name);
}

int YARPNameServer::handle_connection()
{
	// read data from client
	iovec iov[1];
	YARPNameServiceCmd tmpCmd;

	unsigned int byte_count = 0;
	int res = 0;
	memset(data_buf_, 0, SIZE_BUF);	//required !!

	iov[0].iov_base = data_buf_;
	iov[0].iov_len = sizeof(YARPNameServiceCmd);
	res = new_stream_.recvv_n(iov, 1, 0, &byte_count);
		
	tmpCmd = *(YARPNameServiceCmd *) (data_buf_);

	if (tmpCmd.cmd == YARPNSDumpRqs)
	{
		NAME_SERVER_DEBUG(("Handle remote dump\n"));
		if (tmpCmd.type == 0)
			handle_dump_request();
		else if (tmpCmd.type == 1)
			handle_exdump_request();

		if (new_stream_.close() == -1)
			ACE_ERROR ((LM_ERROR, "%p\n", "close"));
		return 0;
	}

	iov[0].iov_base = data_buf_;
	iov[0].iov_len = tmpCmd.length;	//message length
	res = new_stream_.recvv_n(iov, 1, 0, &byte_count);

	if ( (tmpCmd.cmd == YARPNSQuery) &&
		((tmpCmd.type == YARP_UDP) || (tmpCmd.type == YARP_TCP) || (tmpCmd.type == YARP_MCAST)))
	{
		// register a name, ask back ip and port
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_query(tmpRqst->_name);
	}
	else if ( (tmpCmd.cmd == YARPNSQuery) &&
		(tmpCmd.type == YARP_QNET))
	{
		// register a name, ask back ip and port
		YARPNameQnx *tmpRqst = (YARPNameQnx*) (data_buf_);
		handle_query_qnx(std::string(tmpRqst->getName()));
	}
	else if ((tmpCmd.cmd == YARPNSRelease) &&
		((tmpCmd.type == YARP_UDP) || (tmpCmd.type == YARP_TCP) || (tmpCmd.type == YARP_MCAST)))
	{
		// release a name
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_release(tmpRqst->_name);
	}
	else if ((tmpCmd.cmd == YARPNSRelease) &&
		(tmpCmd.type == YARP_QNET) )
	{
		// release a name
		YARPNameQnx *tmpRqst = (YARPNameQnx*) (data_buf_);
		handle_release_qnx(tmpRqst->getName());
	}
	else if ( (tmpCmd.cmd == YARPNSRegister) &&
		(tmpCmd.type == YARP_TCP) )
	{ 
		// register name and ip, ask back port (TCP and UDP)
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_registration(tmpRqst->_name, std::string(tmpRqst->_ip), YARP_TCP);
	}
	else if ( (tmpCmd.cmd == YARPNSRegister) &&
		(tmpCmd.type == YARP_UDP) )
	{
		// register name and ip, ask back multiple ports (UDP)
		YARPNameUDP *tmpRqst = (YARPNameUDP*) (data_buf_);
		handle_registration(tmpRqst->_name, std::string(tmpRqst->_ip), YARP_UDP, tmpRqst->_nPorts);
	}
	else if ((tmpCmd.cmd == YARPNSRegister) &&
		(tmpCmd.type == YARP_MCAST))
	{
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_registration_dip(tmpRqst->_name, YARP_MCAST);
	}
	else if ((tmpCmd.cmd == YARPNSRegister) &&
		(tmpCmd.type == YARP_QNET))
	{
		YARPNameQnx *tmpRqst = (YARPNameQnx*) (data_buf_);
		handle_registration_qnx(*tmpRqst);
	}
	else
		NAME_SERVER_DEBUG(("Sorry: command not recognized\n"));
	
	// close new endpoint
	if (new_stream_.close() == -1)
		ACE_ERROR ((LM_ERROR, "%p\n", "close"));
	return 0;
}
