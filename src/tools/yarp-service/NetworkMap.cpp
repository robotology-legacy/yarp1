// NetworkMap.cpp: implementation of the NetworkMap class.
//
//////////////////////////////////////////////////////////////////////

#include "NetworkMap.h"
#include <stdio.h>
#include <ace/INET_Addr.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkMap::NetworkMap(const YARPString& configFile,
		       const YARPString& localhost)
{
  myhost = localhost;
	if (configFile != "")
		_load(configFile);
}

NetworkMap::~NetworkMap()
{

}

int NetworkMap::_load(const YARPString &filename)
{
	// open
	FILE *fp = fopen(filename.c_str(), "r");
	if (fp==NULL) {
	  fprintf(stderr,"Failed to open %s (%s:%d)\n", filename.c_str(),
		  __FILE__,__LINE__);
	  exit(1);
	}

	// look for [NETWORK_DESCRIPTION]
	char row[255];
	char row1[255];
	char row2[255];
	char row3[255];

	while (strcmp(row, "[NETWORK_DESCRIPTION]") != 0)
	{
		if (fscanf(fp, "%s", row) == EOF)
		{
			fclose(fp);
			return 0;
		}
	}

	int chk = _readAndCheck(fp, row1);
	if (chk != 1)	//"Node=" is expected
		return 0;

	bool read = true;
	while(read)
	{
		chk = _readAndCheck(fp, row1);
		if (chk != 0)	//node name is expected
			return 0;

		networkMapEntry tmpMapEntry;
		//
		tmpMapEntry.nodeName = YARPString(row1);
		// while
		while (true)
		{
			tableEntry tmpTableEntry;

			// network name
			chk = _readAndCheck(fp, row1);
			if (chk == -1) // [END], OEF
			{
				read = false;
				break;
			}
			else if (chk == 1) // new node
				break;
		
			// nic
			chk = _readAndCheck(fp, row2);
			if (chk == -1) // [END], OEF
			{
				read = false;
				break;
			}
			else if (chk == 1) // new node
				break;

			// ip
			chk = _readAndCheck(fp, row3);
			if (chk == -1) // [END], OEF
			{
				read = false;
				break;
			}
			else if (chk == 1) // new node
				break;

			tmpTableEntry.name = tmpMapEntry.nodeName;
			tmpTableEntry.netID = YARPString(row1);
			tmpTableEntry.nic = YARPString(row2);
			tmpTableEntry.ip = YARPString(row3);

			tmpMapEntry.table.push_back(tmpTableEntry);
		}

		if (!tmpMapEntry.table.empty())
			_networkMap.push_back(tmpMapEntry);
	}

	fclose(fp);
	return 0;
}

int NetworkMap::_readAndCheck(FILE * fp, char *row)
{
	if ( (fscanf(fp, "%s", row) == EOF)
		|| (strcmp(row, "[END]") == 0) )
	{
		///fclose(fp);
		return -1;
	}
	else if (strncmp(row, "Node=", 5) == 0)
		return 1;
	else
		return 0;
}

void NetworkMap::findIp(const YARPString &inIp, const YARPString &net, YARPString &outNic, YARPString &outIp)
{
  printf("Hello good evening and welcome to findIp\n");
  printf("Today we are looking for %s on network %s\n", inIp.c_str(), net.c_str());
	NETWORK_MAP_IT mapIt(_networkMap);
	
	bool foundNode = false;
	bool foundNet = false;
	YARPString tmpIp;
	YARPString tmpNic;

	// for each node
	while (!mapIt.done() && !(foundNet&&foundNode))
	{
		// reset foundNet
		if (!foundNode)
			foundNet = false;

		// for each network
		NODE_TABLE_IT nodeIt((*mapIt).table);
		while (!nodeIt.done() && !(foundNet&&foundNode) )
		{
		  //printf("checking name %s and address %s and network %s\n", (*nodeIt).name.c_str(), (*nodeIt).ip.c_str(), (*nodeIt).netID.c_str());
			if ((*nodeIt).netID == net)
			{
				tmpIp = (*nodeIt).ip;
				tmpNic = (*nodeIt).nic;
				foundNet = true;
			}
			
			if ((*nodeIt).ip == inIp || (*nodeIt).name == inIp.c_str())
				foundNode = true;
				
			nodeIt++;
		}
		mapIt++;
	}

	if ( !(foundNode && foundNet) )
	{
	  // by default, if can do nothing else, just try looking up IP
	  ACE_INET_Addr addr;
	  int r = addr.set((short unsigned int)0,(const char *)inIp.c_str());
	  if (r != -1) {
	    outIp = addr.get_host_addr();
	    outNic = "default";
	  }
	  else {
		outIp = "0.0.0.0";		// nothing was found, return 0.0.0.0
		outNic = "not_found";	// nothing was found, return "not found"
	  }
	}
	else
	{
		outIp = tmpIp;		// node and net were found, return tmpIp 
		outNic = tmpNic;	// node and net were found, return tmpNic
	}

	if (outIp==YARPString("127.0.0.1")) {
	  // localhost is no use at all
	  printf("LocalHost .. let us try to get around it via %s\n",
		 myhost.c_str());
	  ACE_INET_Addr addr;
	  int r = addr.set((short unsigned int)0,(const char *)myhost.c_str());
	  if (r!=-1) {
	    outIp = addr.get_host_addr();
	  }
	}
	if (outIp==YARPString("127.0.0.1")) {
	  // localhost is no use at all
	  // last resort - just hostname
	  outIp = myhost;
	}
	printf("Verdict is %s %s\n", outIp.c_str(), outNic.c_str());
}

