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
/// $Id: YARPNameServer.cpp,v 1.5 2003-04-21 21:20:40 natta Exp $
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

std::string getNextIp(const std::string &i)
{
	char tmp[255];
	int a,b,c,d;
	sscanf(i.c_str(),"%d.%d.%d.%d", &a,&b,&c,&d);
	d++;
	if (d == 256)
	{
		d = 0;
		c++;
		if (c == 256)
		{
			c = 0;
			b++;
			if (b == 256)
			{
				b = 0;
				a++;
				if (a = 256)
					a = 0;
			}
		}
	}
	sprintf(tmp, "%d.%d.%d.%d", a,b,c,d);
	return std::string(tmp);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PortEntry IpEntry::ask_new_port()
{
	PortEntry new_port;

	if (!_portPool.findFree(ports, new_port))
	{
		NAME_SERVER_DEBUG(("%s : no more ports available\n", ip.c_str()));
	}
	else
		ports.push_back(new_port);

	return new_port;
}

bool IpEntry::check_port(PortEntry p)
{
	PORT_IT it;
	if (find_port(p.port, it) != -1)
		return true;
	else
		return false;
}

int IpEntry::find_port(int port, PORT_IT &it)
{
	for(PORT_IT i = ports.begin(); i != ports.end(); i++)
	{
		if ((*i).port == port)
		{
			it = i;
			return 0;
		}
	}
	return -1;
}

void IpEntry::release_port(int port)
{
	PortEntry port_entry;
	port_entry.port = port;

	PORT_IT it;
	if (find_port(port, it) != -1)
	{
		ports.erase(it);		
	}
	else		
		NAME_SERVER_DEBUG(("Sorry, cannot find %s: %d\n", ip.c_str() ,port));
}

void resources::release (const std::string &ip)
{
	IP_IT it;
	if (find_ip(ip, it) != -1)
	{
		erase(it);
		NAME_SERVER_DEBUG(("Released %s\n", ip.c_str()));
	}
	else
		NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", ip.c_str()));
}

void resources::release(const std::string &ip, int port)
{
	IP_IT it;
	if (find_ip(ip, it) != -1)
	{
		it->release_port(port);
		NAME_SERVER_DEBUG(("%s:%d no longer used, releasing\n", ip.c_str(), port));
		if (it->ports.empty())
			erase(it);
	}
	else
		NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", ip.c_str()));
}

bool resources::check_port(const std::string &ip, int port)
{
	IP_IT it;
	PortEntry temp;
	temp.port = port;

	if (find_ip(ip, it) != -1)
	{
		it->check_port(temp);
		return true;
	}
	else
	{
		NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", ip.c_str()));
		return false;
	}
}

void resources::sign_in(const IpEntry &ip)
{
	IpEntry new_ip;
	new_ip = ip;

	push_back(new_ip);
}

void resources::sign_in(const std::string &ip)
{
	IpEntry new_ip;
	new_ip.ip = ip;

	push_back(new_ip);
}

int resources::ask_new(const std::string &ip, int *port)
{
	IP_IT it;
	PortEntry temp;
		
	if (find_ip(ip, it) != -1)
	{
		temp = it->ask_new_port();
		*port = temp.port;
		return 0;
	}
	else
	{
		NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", ip.c_str()));
		return -1;
	}
}
	
int resources::find_ip(const std::string &ip)
{
	IP_IT dummy;

	return find_ip(ip, dummy);
}

int resources::find_ip(const std::string &ip, IP_IT &it)
{
	for(IP_IT i = begin(); i != end(); i++)
	{
		if ((*i).ip == ip)
		{
			it = i;
			return 0;
		}
	}
		
	return -1;
}

int services::find_service(const std::string &n, SVC_IT &it)
{
	for(SVC_IT i = begin(); i != end(); i++)
	{
		if (i->name == n)
		{
			it = i;
			return 0;
		}
	}
	return -1;
}

int services::check_out(const std::string &name, std::string &ip, int *port)
{
	SVC_IT it;
	if (find_service(name, it) != -1)
	{
		ip = it->ip;
		*port = it->port;

		if (it->release_ref())
		{
			erase(it);
			return 1;	// resource no longer used
		}
		return 0;		// resource found, but still used
	}
	else
	{
		NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", name.c_str()));
		return -1;		// error: resource not found
	}
}

// check in: increase ref count for a specified port, ip and max_ref
// Note: service must not exist !
void services::check_in(const std::string &name, const std::string &ip, int port, int max_ref)
{
	SVC_IT it;
	if (find_service(name, it) != -1)
	{
		NAME_SERVER_DEBUG(("Error, %s already registered\n", name.c_str()));
	}
	else
	{
		service tmp;
		tmp.ip = ip;
		tmp.port = port;
		tmp.name = name;
		tmp.set_max_ref(max_ref);
		tmp.take_ref();
		push_back(tmp);
	}
}

bool services::check(const std::string &name, std::string &ip, int *port, int *max)
{
	SVC_IT it;
	if(find_service(name, it) != -1)
	{
		ip = it->ip;
		*port = it->port;
		*max = it->get_max_ref();
		return true;
	}
	else
	{
		*port = __portNotFound;	//port not found !
		NAME_SERVER_DEBUG(("%s not found\n", name.c_str()));
		return false;
	}

}

// check if reference exits; return ip associated with a particular name
// increase reference count returns:
// > 0, found, resources av.
// 0, found, resoursec not av.
// < 0, not found
int services::take_ref(const std::string &name, std::string &ip, int *port)
{
	SVC_IT it;
	if(find_service(name, it) != -1)
	{
		bool ref = it->take_ref();
		ip = it->ip;
		*port = it->port;

		if (ref)
			return 1;	// found, resources avaiable
		else
			return 0;	// found, no more resources
	}
	else
	{
		return -1;		// not found
	}
	
}

LocalNameServer::LocalNameServer()
{

}

LocalNameServer::~LocalNameServer()
{

}

void LocalNameServer::init(const std::string &filename)
{
	ifstream input;
	input.open(filename.c_str());

	while (!input.eof() && input.is_open())
	{
		string name;
		string ip;

		int n;

		input >> name;	// name
		input >> ip;	// ip address
		input >> n;		// max_references

		// just check to be sure both name and size are not empty
		if ((name.size != 0) && (ip.size != 0) )
		{
			statics.check_in(name, ip, 0, n);
		}
	}
	input.close();
}

int LocalNameServer::queryName(const std::string &name, std::string &ip, int *port)
{
	//  if name is already registered
	std::string tmp_ip;
	int tmp_port;
	int max_ref = 0;
	if (names.check(name, tmp_ip, &tmp_port))
	{
		int ref = names.take_ref(name, tmp_ip, &tmp_port);
		if (ref == 0)
		{
			// 0 means found, but ran out of resources
			ip = "0.0.0.0";
			*port = __portNotFound;
			return -1;
		}
		else
		{
			// found, resources availables
			ip = tmp_ip;
			*port = tmp_port;
			return 0;
		}
	}
	else
	{
		// not found
		ip = "0.0.0.0";
		*port = __portNotFound;
		return 0;
	}
}

int LocalNameServer::registerName(const std::string &name, const IpEntry &entry, int *port)
{
	//  if name is already registered
	_checkAndRemove(name);
	return _registerName(name, entry, port);
}	

int LocalNameServer::registerNameDIp(const std::string &name, std::string &ip, int *port)
{
	_checkAndRemove(name);

	IpEntry new_ip;
	int new_port;
	int ret;

	if (!addresses._ipPool.findFree(addresses, new_ip)) {
		NAME_SERVER_DEBUG(("%s : no more ip available from pool\n", name.c_str()));
		*port = __portNotFound;
		ip = std::string(__ipNotFound);
		ret = -1;
	}
	else {
		ret = _registerName(name, new_ip, &new_port);
		ip = new_ip.ip;
		*port = new_port;
	}
	return ret;
}

int YARPNameServer::accept_connection()
{
	ACE_Time_Value timeout (5, 0);
	return peer_acceptor_.accept(new_stream_, &client_addr_, &timeout);
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
		cout << i->ip <<":" << i->port;
		cout << "\tref:" << (i->get_max_ref()-i->get_ref()) << endl;
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

void YARPNameServer::handle_registration(const std::string &service_name, const std::string &ip)
{
	int port;
	ns.registerName(service_name, ip, &port);
	NAME_SERVER_DEBUG(("Reply %s as %s:%d\n", service_name.c_str(), ip.c_str(), port));
	_handle_reply(ip, port);
}

void YARPNameServer::handle_query(const std::string &service_name)
{
	std::string ip;
	int port;
	ns.queryName(service_name, ip, &port);
	NAME_SERVER_DEBUG(("Reply %s as %s:%d\n", service_name.c_str(), ip.c_str(), port));
	_handle_reply(ip, port);
}

void YARPNameServer::handle_registration_dbg(const std::string &service_name, const std::string &ip)
{
	int port;

	NAME_SERVER_DEBUG(("DBG MODE: registering %s as %s\n", service_name.c_str(), ip.c_str()));

	ns.registerName(service_name, ip, &port);
	NAME_SERVER_DEBUG(("Reply %s as %s:%d\n", service_name.c_str(), ip.c_str(), port));
}

void YARPNameServer::handle_registration_dip_dbg(const std::string &service_name)
{
	std::string ip;
	int port;

	NAME_SERVER_DEBUG(("DBG MODE: registering %s\n", service_name.c_str()));

	ns.registerNameDIp(service_name, ip, &port);
	NAME_SERVER_DEBUG(("DBG MODE: answering %s:%d\n", ip.c_str(), port));
}

void YARPNameServer::handle_registration_dip(const std::string &service_name)
{
	int port;
	std::string ip;
	ns.registerNameDIp(service_name, ip, &port);
	NAME_SERVER_DEBUG(("Reply %s as %s:%d\n", service_name.c_str(), ip.c_str(), port));
	_handle_reply(ip, port);
}

void YARPNameServer::_handle_reply(const std::string &ip, int port)
{
	YARPNameServiceCmd rplCmd;
	YARPNameTCP rpl;
	rpl.setAddr(ACE_INET_Addr(port, ip.c_str()));
	rplCmd.cmd = YARPNSRpl;
	rplCmd.length = rpl.length();
		
	memcpy(data_buf_, &rplCmd, sizeof(YARPNameServiceCmd));
	memcpy(data_buf_+sizeof(YARPNameServiceCmd), &rpl, rpl.length());

	iovec iov[1];
	iov[0].iov_base = data_buf_;
	iov[0].iov_len = rplCmd.length+sizeof(YARPNameServiceCmd);

	int sent = new_stream_.sendv_n (iov, 1);
}

void YARPNameServer::query_dbg(const std::string &service_name)
{
	std::string ip;
	int port;

	NAME_SERVER_DEBUG(("DBG MODE: query %s\n", service_name.c_str()));

	ns.queryName(service_name, ip, &port);

	NAME_SERVER_DEBUG(("DBG MODE: answering %s:%d\n", ip.c_str(), port));

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
	else if ((tmpCmd.cmd == YARPNSRelease) &&
		((tmpCmd.type == YARP_UDP) || (tmpCmd.type == YARP_TCP) || (tmpCmd.type == YARP_MCAST)))
	{
		// release a name
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_release(tmpRqst->_name);
	}
	else if ((tmpCmd.cmd == YARPNSRegister) &&
		((tmpCmd.type == YARP_UDP) || (tmpCmd.type == YARP_TCP)))
	{ 
		// register name and ip, ask back port (TCP and UDP)
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_registration(tmpRqst->_name, std::string(tmpRqst->_ip));
	}
	else if ((tmpCmd.cmd == YARPNSRegister) &&
		(tmpCmd.type == YARP_MCAST))
	{
		YARPNameTCP *tmpRqst = (YARPNameTCP*) (data_buf_);
		handle_registration_dip(tmpRqst->_name);
	}
	else
		NAME_SERVER_DEBUG(("Sorry: command not recognized\n"));
	
	// close new endpoint
	if (new_stream_.close() == -1)
		ACE_ERROR ((LM_ERROR, "%p\n", "close"));
	return 0;
}