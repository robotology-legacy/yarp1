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
/// $Id: YARPNameServer.h,v 1.4 2003-04-21 21:20:40 natta Exp $
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

#define NAME_SERVER_VERBOSE
#define SIZE_BUF 4096

#include <string>
#include <list>

#ifdef NAME_SERVER_VERBOSE
	#include <stdio.h>
	#define NAME_SERVER_DEBUG(string) \
		do {ACE_OS::printf("NAME_SERVER: "),printf string;} while(0)
#else
	#define NAME_SERVER_DEBUG(string)\
		do {} while(0)
#endif

// various constants (ip and port pools, ret codes...)
const int __startPortPool = 1001;
const int __endPortPool = 1999;
const int __portNotFound = 0;

const int __startDynPortPool = 4000;
const int __endDynPortPool = 4999;

const char __startIpPool[] = {"224.0.0.0"};
const char __endIpPool[] = {"224.255.255.255"};
const char __ipNotFound[] = {"0.0.0.0"};

const int _max_ref = 9999;

// return the ip address right after 'i'
std::string getNextIp(const std::string &i);

class PortEntry
{
public:
	PortEntry()
	{
		port = __portNotFound;
		flag = false;
	}
	PortEntry(int p)
	{
		port = p;
		flag = false;
	}

	bool operator == (const PortEntry &item)
	{
		if (port == item.port)
			return true;
		else
			return false;
	}
	PortEntry operator++()
	{
		port++;
		flag = false;
		return *this;
	}
	const PortEntry &operator = (const PortEntry &i)
	{
		port = i.port;
		flag = i.flag;
		return *this;
	}
	
	int port;
	int flag;
};

typedef std::list<PortEntry> PORT_LIST;
typedef PORT_LIST::iterator PORT_IT;

template <class T, class T_LIST, class T_IT>
class pool
{
public:
	bool findFree(T_LIST &l, T &item)
	{
		T tmp = _min;
		T last = _max;
		++last;		// we want the last item to be used
		while (! (tmp==last)) 
		{
			if (!_check(l,tmp))
			{
				item = tmp;
				return true;
			}
			++tmp;
		} 
		return false;
	}
private:
	bool _check(T_LIST &l, T &item)
	{
		for(T_IT i = l.begin(); i != l.end(); i++)
		{
			if ((*i) == item)
				return true;
		}
		return false;
	}
public:
	T _min;
	T _max;
};

class IpEntry
{
public:
	IpEntry()
	{
		_portPool._min = PortEntry(__startPortPool);
		_portPool._max = PortEntry(__endPortPool);
	}

	IpEntry(const char *i, int startPort, int endPort)
	{
		ip = std::string(i);
		_portPool._min = PortEntry(startPort);
		_portPool._max = PortEntry(endPort);
	}

	std::string	ip;
	PORT_LIST ports;
	pool<PortEntry, PORT_LIST, PORT_IT> _portPool;

	PortEntry ask_new_port();
	bool check_port(PortEntry p);
	int find_port(int port, PORT_IT &it);
		
	void release_port(int port);

	// operators
	bool operator == (const IpEntry &item)
	{
		if (ip == item.ip)
			return true;
		else
			return false;
	}
	IpEntry operator++()
	{
		ip = getNextIp(ip);
		return *this;
	}
	const IpEntry &operator = (const IpEntry &i)
	{
		ip = i.ip;
		_portPool = i._portPool;
		return *this;
	}
};

typedef std::list<IpEntry> IP_LIST;
typedef IP_LIST::iterator IP_IT;

class resources:public IP_LIST
{
public:
	resources()
	{
		_ipPool._min = IpEntry(__startIpPool,__startDynPortPool, __endDynPortPool);
		_ipPool._max = IpEntry(__endIpPool, __startDynPortPool, __endDynPortPool);
	}
	void release (const std::string &ip);
	void release(const std::string &ip, int port);
	bool check_port(const std::string &ip, int port);
	void sign_in(const IpEntry &ip);
	void sign_in(const std::string &ip);
	int ask_new(const std::string &ip, int *port);
	int find_ip(const std::string &ip);
	int find_ip(const std::string &ip, IP_IT &it);

public:
	pool<IpEntry, IP_LIST, IP_IT> _ipPool;
};

struct service
{
	service()
	{
		ref_count = _max_ref;
		max_ref = _max_ref;
	}

	std::string name;
	std::string ip;
	int port;

private:
	int ref_count;
	int max_ref;
		
public:
	void set_max_ref(int mr)
	{
		if (mr == -1)
		{
			ref_count = _max_ref;
			max_ref = _max_ref;
		}
		else
		{
			ref_count = mr;
			max_ref = mr;
		}
	}

	int get_max_ref()
	{return max_ref;}

	int get_ref()
	{return ref_count;}

	bool take_ref()
	{
		ref_count --;

		if (ref_count < 0)
		{
			ref_count++;
			return false;
		}

		return true;
	}

	bool release_ref()
	{
		ref_count ++;

		if (ref_count == max_ref)
			return true;	// resource no longer used
		else	
			return false;	// resource still used
	}
};

typedef std::list<service> SVC_LIST;
typedef SVC_LIST::iterator SVC_IT;

struct services: public SVC_LIST
{
	int find_service(const std::string &n, SVC_IT &it);
	
	// release a name service -- return ip and port
	int check_out(const std::string &name, std::string &ip, int *port);
	
	// check in: increase ref count for a specified port, ip and max_ref
	// Note: service must not exist !
	void check_in(const std::string &name, const std::string &ip, int port, int max_ref);
	
	// check only: return ip, port and current associated with a service name
	bool check(const std::string &name, std::string &ip, int *port)
	{
		int dummy;
		return check(name, ip, port, &dummy);
	}

	// find name and destroy entry, return ip and port
	int destroy(const std::string &name, std::string &ip, int *port)
	{
		SVC_IT it;
		if (find_service(name, it) != -1)
		{
			ip = it->ip;
			*port = it->port;

			erase(it);
			return 1;		// resource destroyed
		}
		else
		{
			NAME_SERVER_DEBUG(("Sorry, cannot find: %s\n", name.c_str()));
			return -1;		// error: resource not found
		}
	}

	// check only: return ip, port and current ref associated with a service name
	bool check(const std::string &name, std::string &ip, int *port, int *ref);
	
	// check if reference exits; return ip associated with a particular name
	// increase reference count returns:
	// > 0, found, resources av.
	// 0, found, resoursec not av.
	// < 0, not found
	int take_ref(const std::string &name, std::string &ip, int *port);
};

class LocalNameServer  
{
public:
	LocalNameServer();
	virtual ~LocalNameServer();

	// sign in a service, specify name/IpEntry get port back
	int registerName(const std::string &name, const IpEntry &entry, int *port);
	// sign in a service, specify name/ip get port back
	int registerName(const std::string &name, const std::string &ip, int *port)
	{
		IpEntry tmpEntry;
		tmpEntry.ip = ip;
		return registerName(name, tmpEntry, port);
	}
	// sign in a service, specify name only, get back ip from pool
	int registerNameDIp(const std::string &name, std::string &ip, int *port);


	// just check
	int queryName(const std::string &name, std::string &ip, int *port);
	
	// release a service
	void check_out(const std::string &name)
	{
		std::string ip;
		int port;

		if (names.check_out(name, ip, &port) == 1){
			//resource no longer used, release it
			addresses.release(ip, port);
		}
	}
	
	int check_static(const std::string &name, std::string &ip, int *max)
	{
		int dummy;
		if (statics.check(name, ip, &dummy, max))
			return 0;
		else
			return -1;
	}

	// load initial resources from file (static entries)
	void init(const std::string &filename);

	resources addresses;
	services  names;
	services  statics;
private:
	// check if name already exists, and remove it
	void _checkAndRemove(const std::string &name)
	{
		std::string sdummy;
		int idummy;
		if (names.check(name, sdummy, &idummy))
		{
			// name already registered, destroy it
			NAME_SERVER_DEBUG(("WARNING: %s already registered as %s:%d\n", name.c_str(), sdummy.c_str(), idummy));
			names.destroy(name, sdummy, &idummy);
			addresses.release(sdummy, idummy);
		}
	}
	int _registerName(const std::string &name, const IpEntry &entry, int *port)
	{
		std::string tmp_ip;
		int tmp_port;
	
		tmp_ip = entry.ip;
		
		int ref = names.take_ref(name, tmp_ip, &tmp_port);
		int max_ref = -1;
	
		if (ref == 0)
		{
			// 0 means found, but ran out of resources (max ref)
			*port = __portNotFound;
			return -1;
		}
		else if (ref <= -1)
		{
			// < -1 means not found
			// find a free resource and assign it
			if (addresses.find_ip(tmp_ip) == -1)
				addresses.sign_in(entry);

			// get new resource name
			addresses.ask_new(tmp_ip, &tmp_port);
			*port = tmp_port;
			if (tmp_port!=__portNotFound){
				// register new resource
				names.check_in(name, tmp_ip, tmp_port, max_ref);
				return -1;
			}
			return 0;
		}
		else
		{
			// found, resources availables
			*port = tmp_port;
			return 0;
		}
	}
};

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
