// $Id: LocalNameServer.cpp,v 1.3 2003-04-23 17:40:01 natta Exp $

#include "LocalNameServer.h"

using namespace std;

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

///////////////////////////////////////////////
// IpEntry class

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

///////////////////////////////////////////////
// Resources class

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

int resources::ask_new(const std::string &ip, PORT_LIST &ports, int n)
{
	IP_IT it;
	PortEntry temp;
		
	if (find_ip(ip, it) != -1)
	{
		for(int i = 0; i < n; i++) {
			temp = it->ask_new_port();
			if (temp.port == __portNotFound)
			{
				// port not found: destroy list, add first entry as __PortNotFound, return -1
				ports.clear();
				ports.push_back(temp);
				return -1;
			}
			else
				ports.push_back(temp);
		}
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

///////////////////////////////////////////////
// Services class

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

int services::check_out(const std::string &name, std::string &ip, PORT_LIST &ports)
{
	SVC_IT it;
	if (find_service(name, it) != -1)
	{
		ip = it->ip;
		ports = it->ports;

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
void services::check_in(const std::string &name, const std::string &ip, int type, const PORT_LIST &ports, int max_ref)
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
		tmp.ports = ports;
		tmp.name = name;
		tmp.type = type;
		tmp.set_max_ref(max_ref);
		tmp.take_ref();
		push_back(tmp);
	}
}

bool services::check(const std::string &name, std::string &ip, int *type, PORT_LIST &ports, int *max)
{
	SVC_IT it;
	if(find_service(name, it) != -1)
	{
		ip = it->ip;
		ports = it->ports;
		*type = it->type;
		*max = it->get_max_ref();
		return true;
	}
	else
	{
		ports.push_back(PortEntry(__portNotFound));	//port not found !
		NAME_SERVER_DEBUG(("%s not found\n", name.c_str()));
		return false;
	}

}

// check if reference exits; return ip and ports associated with a particular name
// increase reference count returns:
// > 0, found, resources av.
// 0, found, resoursec not av.
// < 0, not found
int services::take_ref(const std::string &name, std::string &ip, int *type, PORT_LIST &ports)
{
	SVC_IT it;
	if(find_service(name, it) != -1)
	{
		bool ref = it->take_ref();
		ip = it->ip;
		ports = it->ports;
		*type = it->type;

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

///////////////////////////////////////////////
// LocalNameServer class

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
	//		statics.check_in(name, ip, 0, n);
		}
	}
	input.close();
}

int LocalNameServer::queryName(const std::string &name, std::string &ip, int *type, int *port)
{
	//  if name is already registered
	std::string tmp_ip;
	PORT_LIST tmp_ports;
	int protocol;
	int max_ref = 0;
	if (names.check(name, tmp_ip, &protocol, tmp_ports))
	{
		int ref = names.take_ref(name, tmp_ip, &protocol, tmp_ports);
		if (ref == 0)
		{
			// 0 means found, but ran out of resources
			ip = "0.0.0.0";
			*port = __portNotFound;
			*type = YARP_NO_SERVICE_AVAILABLE;
			return -1;
		}
		else
		{
			// found, resources availables
			ip = tmp_ip;
			*port = tmp_ports.begin()->port;
			*type = protocol;
			return 0;
		}
	}
	else
	{
		// not found
		ip = "0.0.0.0";
		*port = __portNotFound;
		*type = YARP_NO_SERVICE_AVAILABLE;
		return 0;
	}
}

int LocalNameServer::registerName(const std::string &name, const IpEntry &entry, int type, int *port)
{
	PORT_LIST new_ports;
	int ret;
	//  if name is already registered
	_checkAndRemove(name);
	ret = _registerName(name, entry, type, new_ports, 1);
	*port = new_ports.begin()->port;
	return ret;
}	

int LocalNameServer::registerName(const std::string &name, const IpEntry &entry, int type, PORT_LIST &ports, int n)
{
	int ret;
	//  if name is already registered
	_checkAndRemove(name);
	ret = _registerName(name, entry, type, ports, n);
	return ret;
}	

int LocalNameServer::registerNameDIp(const std::string &name, std::string &ip, int type, int *port)
{
	_checkAndRemove(name);

	IpEntry new_ip;
	PORT_LIST new_ports;
	int ret;

	if (!addresses._ipPool.findFree(addresses, new_ip)) {
		NAME_SERVER_DEBUG(("%s : no more ip available from pool\n", name.c_str()));
		*port = __portNotFound;
		ip = std::string(__ipNotFound);
		ret = -1;
	}
	else {
		ret = _registerName(name, new_ip, type, new_ports, 1);
		ip = new_ip.ip;
		*port = new_ports.begin()->port;
	}
	return ret;
}

void LocalNameServer::_checkAndRemove(const std::string &name)
{
	std::string sdummy;
	PORT_LIST pdummy;
	int dtype;
	if (names.check(name, sdummy, &dtype, pdummy))
	{
		// name already registered, destroy it
		NAME_SERVER_DEBUG(("WARNING: %s already registered as %s(%s):%d\n", name.c_str(), sdummy.c_str(), servicetypeConverter(dtype), pdummy.begin()->port));
		names.destroy(name, sdummy, pdummy);
		for(PORT_IT i = pdummy.begin(); i != pdummy.end(); i++)
			addresses.release(sdummy, i->port);
	}
}

int LocalNameServer::_registerName(const std::string &name, const IpEntry &entry, int type, PORT_LIST &ports, int nPorts)
{
	std::string tmp_ip;
	PORT_LIST tmp_ports;
	
	tmp_ip = entry.ip;
	int dtype;
	int ref = names.take_ref(name, tmp_ip, &dtype, tmp_ports);
	int max_ref = -1;
	
	if (ref == 0)
	{
		// 0 means found, but ran out of resources (max ref)
		ports.push_back(PortEntry(__portNotFound));
		return -1;
	}
	else if (ref <= -1)
	{
		// < -1 means not found
		// find a free resource and assign it
		if (addresses.find_ip(tmp_ip) == -1)
			addresses.sign_in(entry);

		// get new resources
		addresses.ask_new(tmp_ip, ports, nPorts);
		if (ports.begin() !=__portNotFound){
			// register new resource
			names.check_in(name, tmp_ip, type, ports, max_ref);
			return -1;
		}
		return 0;
	}
	else
	{
		// check this, is it possible to get here ?
		// found, resources availables
		ports = tmp_ports;
		return 0;
	}
}
