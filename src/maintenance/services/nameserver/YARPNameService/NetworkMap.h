/// $Id: NetworkMap.h,v 1.1 2003-07-17 10:56:40 babybot Exp $
// 
// July 2003 -- by nat
//////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MAP_HH__
#define __NETWORK_MAP_HH__

#include <ace/config.h>
#include <ace/Log_Msg.h>
#include <string>
#include <list>

#define NETWORK_MAP_VERBOSE
#ifdef NETWORK_MAP_VERBOSE
	#include <stdio.h>
	#define NETWORK_MAP_DEBUG(string) \
		do {ACE_OS::printf("NETWORK_MAP: "),printf string;} while(0)
#else
	#define NETWORK_MAP_DEBUG(string)\
		do {} while(0)
#endif

class NetworkMap  
{
public:
	// typedef
	struct tableEntry
	{
		std::string netID;
		std::string ip;
	};
	
	typedef std::list<tableEntry> NODE_TABLE;
	typedef NODE_TABLE::iterator NODE_TABLE_IT;

	struct networkMapEntry
	{
		std::string nodeName;
		NODE_TABLE table;
	};

	typedef std::list<networkMapEntry> NETWORK_MAP;
	typedef NETWORK_MAP::iterator NETWORK_MAP_IT;
	//////////////////////////////////////////////

	NetworkMap(const std::string &configFile = "");
	virtual ~NetworkMap();

	void findIp(const std::string &inIp, const std::string &net, std::string &outIp);
	
private:
	int _load(const std::string &filename);
	int _readAndCheck(FILE * fp, char *row);

	NETWORK_MAP _networkMap;
};

#endif // .h
