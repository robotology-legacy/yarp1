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
/// $Id: LocalNameServer.h,v 1.2 2004-07-09 07:34:53 eshuy Exp $
///
///

// LocalNameServer.h
//
// 
// 
// -- January 2003 -- by nat 
// -- Modified for YARP April 2003 -- by nat 
// -- Moved from YARPNameService.h April 2003 -- by nat
// -- Qnx name support April 2003 -- by nat
//////////////////////////////////////////////////////////////////////
#if !defined __LOCALNAMESERVER__
#define __LOCALNAMESERVER__

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/conf/YARPConfig.h>
#include <yarp_private/wide_nameloc.h>
#include <yarp/YARPString.h>
#include <yarp/YARPList.h>

// #include <list>

#define NAME_SERVER_VERBOSE
#ifdef NAME_SERVER_VERBOSE
	#include <stdio.h>
	#define NAME_SERVER_DEBUG(string) \
		do {ACE_OS::printf("NAME_SERVER: "),printf string;} while(0)
#else
	#define NAME_SERVER_DEBUG(string)\
		do {} while(0)
#endif

// various constants (ip and port pools, ret codes...)
const int __startPortPool = 10001;
const int __endPortPool = 10999;
///const int __portNotFound = 0;

const int __startDynPortPool = 15001;
const int __endDynPortPool = 15999;

const char __startIpPool[] = {"224.1.1.1"};
const char __endIpPool[] = {"224.255.255.255"};

const int _max_ref = 9999;

// return the ip address right after 'i'
YARPString getNextIp(const YARPString &i);
// return the distance between two ip address (count the number of ip)
int getDistance(const YARPString &current, const YARPString &start);

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

	bool operator== (const PortEntry &item)
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

	const PortEntry& operator= (const PortEntry &i)
	{
		port = i.port;
		flag = i.flag;
		return *this;
	}
	
	int port;
	int flag;
};

typedef YARPList<PortEntry> PORT_LIST;
typedef PORT_LIST::const_iterator CONST_PORT_IT;
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
		T_IT i(l);
		for(; !i.done(); i++)
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
		ip = YARPString(i);
		_portPool._min = PortEntry(startPort);
		_portPool._max = PortEntry(endPort);
	}

	IpEntry(const char *i, const char *s)
	{
		ip = YARPString(i);
		int dist = getDistance(ip,s);
		_portPool._min = dist+__startDynPortPool;
		_portPool._max = dist+__startDynPortPool;
	}

	YARPString	ip;
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
		// modify port pool
		int dist = getDistance(ip, __startIpPool);
		_portPool._min = dist+__startDynPortPool;
		_portPool._max = dist+__startDynPortPool;
		return *this;
	}

	const IpEntry &operator = (const IpEntry &i)
	{
		ip = i.ip;
		_portPool = i._portPool;
		return *this;
	}
};

typedef YARPList<IpEntry> IP_LIST;
typedef IP_LIST::iterator IP_IT;

class resources : public IP_LIST
{
public:
	resources()
	{
		// _ipPool._min = IpEntry(__startIpPool, __startDynPortPool, __endDynPortPool);
		// _ipPool._max = IpEntry(__endIpPool, __startDynPortPool, __endDynPortPool);
		_ipPool._min = IpEntry(__startIpPool, __startIpPool);
		_ipPool._max = IpEntry(__endIpPool, __startIpPool);
	}

	void release (const YARPString &ip);
	void release(const YARPString &ip, int port);
	bool check_port(const YARPString &ip, int port);
	void sign_in(const IpEntry &ip);
	void sign_in(const YARPString &ip);
	int ask_new(const YARPString &ip, int *port);
	int ask_new(const YARPString &ip, PORT_LIST &ports, int n);
	int find_ip(const YARPString &ip);
	int find_ip(const YARPString &ip, IP_IT &it);

public:
	pool<IpEntry, IP_LIST, IP_IT> _ipPool;
};

class service
{
public:
	service()
	{
		ref_count = _max_ref;
		max_ref = _max_ref;
	}

	YARPString name;
	// YARPString type;
	int type;
	YARPString ip;
	PORT_LIST ports;

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

typedef YARPList<service> SVC_LIST;
typedef SVC_LIST::iterator SVC_IT;

class services: public SVC_LIST
{
public:
	int find_service(const YARPString &n, SVC_IT &it);
	
	// release a name service -- return ip and port(s)
	int check_out(const YARPString &name, YARPString &ip, PORT_LIST &ports);
	
	// check in: increase ref count for a specified ip, port(s) and max_ref
	// Note: service must not exist !
	void check_in(const YARPString &name, const YARPString &ip, int type, const PORT_LIST &ports, int max_ref);
	
	// check only: return ip, port(s) and type associated with a service name
	bool check(const YARPString &name, YARPString &ip, int *type, PORT_LIST &ports)
	{
		int dummy;
		return check(name, ip, type, ports, &dummy);
	}
	// check only: return ip, port(s), type and current ref associated with a service name
	bool check(const YARPString &name, YARPString &ip, int *type, PORT_LIST &ports, int *ref);

	// find name and destroy entry, return ip and port(s)
	int destroy(const YARPString &name, YARPString &ip, PORT_LIST &ports)
	{
		SVC_IT it(*this);
		if (find_service(name, it) != -1)
		{
			ip = (*it).ip;
			ports = (*it).ports;

			erase(it);
			return 1;		// resource destroyed
		}
		else
		{
			NAME_SERVER_DEBUG(("TCP/UDP/MCAST name %s not found\n", name.c_str()));
			return -1;		// error: resource not found
		}
	}

	// check if reference exits; return ip, ports and type associated with a particular name
	// increase reference count returns:
	// > 0, found, resources av.
	// 0, found, resoursec not av.
	// < 0, not found
	int take_ref(const YARPString &name, YARPString &ip, int *type, PORT_LIST &ports);
};

typedef YARPList<YARPNameQnx> QNXSVC_LIST;
typedef QNXSVC_LIST::iterator QNXSVC_IT;
typedef QNXSVC_LIST::const_iterator  CONST_QNXSVC_IT;

class qnxServices: public QNXSVC_LIST 
{
public:
	int find_service(const YARPString &name, QNXSVC_IT &it);
	// find name and destroy entry
	int destroy(const YARPString &name)
	{
		QNXSVC_IT it(*this);
		if (find_service(name, it) != -1)
		{
			NAME_SERVER_DEBUG(("QNX name %s no longer used, releasing\n", name.c_str()));
			erase(it);
			return 1;		// resource destroyed
		}
		else
		{
			NAME_SERVER_DEBUG(("QNX name %s not found\n", name.c_str()));
			return -1;		// error: resource not found
		}
	}
	// release a name service
	int check_out(const YARPString &name);
	int check_in(const YARPString &name, YARPNameQnx &entry);
	int check_in(const YARPNameQnx &entry);
};

class LocalNameServer  
{
public:
	LocalNameServer(){};
	virtual ~LocalNameServer(){};

	// sign in a service, specify name/IpEntry/type get port back
	int registerName(const YARPString &name, const IpEntry &entry, int type, int *port);
	// sign in a service, specify name/IpEntry get 'n' ports
	int registerName(const YARPString &name, const IpEntry &entry, int type, PORT_LIST &port, int n);
	// sign in a service, specify name/ip get port back
	int registerName(const YARPString &name, const YARPString &ip, int type, int *port)
	{
		IpEntry tmpEntry;
		tmpEntry.ip = ip;
		return registerName(name, tmpEntry, type, port);
	}
	
	// sign in a service, specify name only, get back ip from pool
	int registerNameDIp(const YARPString &name, YARPString &ip, int type, int *port);
	// sign in a qnx service
	int registerNameQnx(const YARPNameQnx &entry);
	// just check TCP, UDP, MCAST
	int queryName(const YARPString &name, YARPString &ip, int *type, int *port);
	// just check QNX
	int queryNameQnx(const YARPString &name, YARPNameQnx &entry, int *type);
	
	// release a service TCP, UDP, MCAST
	void check_out(const YARPString &name)
	{
		YARPString ip;
		PORT_LIST ports;

		if (names.check_out(name, ip, ports) == 1){
			//resource no longer used, release it
			PORT_IT i (ports);
			for(; !i.done(); i++)
					addresses.release(ip, (*i).port);	
		}
	}

	// release a QNX name
	// NOTE: no ref count is implemented for QNX, a single check out
	// remove the name
	void check_out_qnx(const YARPString &name)
	{
		qnx_names.destroy(name);
	}
	
	int check_static(const YARPString &name, YARPString &ip, int *max)
	{
		ACE_UNUSED_ARG (name);
		ACE_UNUSED_ARG (ip);
		ACE_UNUSED_ARG (max);
		return -1;

		/*PORT_LIST dummy;
		if (statics.check(name, ip, dummy, max))
			return 0;
		else
			return -1;*/
	}

	// load initial resources from file (static entries)
	void init(const YARPString &filename);

	resources addresses;
	services  names;
	services  statics;
	qnxServices qnx_names;

private:
	// check if name already exists and remove it, TCP, UDP, MCAST
	void _checkAndRemove(const YARPString &name);
	// check if name already exists and remove it, QNX
	void _checkAndRemoveQnx(const YARPString &name);

	// actual registration routine, TCP, UDP, MCAST
	int _registerName(const YARPString &name, const IpEntry &entry, int type, PORT_LIST &ports, int nPorts);
	// actual registration routine, QNX
	int _registerNameQnx(const YARPNameQnx &entry);
};


#endif //.h
