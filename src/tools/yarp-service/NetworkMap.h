/// $Id: NetworkMap.h,v 1.2 2004-07-09 07:34:53 eshuy Exp $
// 
// July 2003 -- by nat
//////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MAP_HH__
#define __NETWORK_MAP_HH__

#include <ace/config.h>
#include <ace/Log_Msg.h>

#include <yarp/YARPString.h>
#include <yarp/YARPList.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

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
		YARPString netID;
		YARPString nic;
		YARPString ip;
	};
	
	typedef YARPList<tableEntry> NODE_TABLE;
	typedef NODE_TABLE::iterator NODE_TABLE_IT;

	struct networkMapEntry
	{
		YARPString nodeName;
		NODE_TABLE table;
	};

	typedef YARPList<networkMapEntry> NETWORK_MAP;
	typedef NETWORK_MAP::iterator NETWORK_MAP_IT;
	//////////////////////////////////////////////

	NetworkMap(const YARPString &configFile = "");
	virtual ~NetworkMap();

	void findIp(const YARPString &inIp, const YARPString &net, YARPString &outNic, YARPString &outIp);
	
private:
	int _load(const YARPString &filename);
	int _readAndCheck(FILE * fp, char *row);

	NETWORK_MAP _networkMap;
};

#endif // .h
