// NetworkMap.cpp: implementation of the NetworkMap class.
//
//////////////////////////////////////////////////////////////////////

#include "NetworkMap.h"
#include <stdio.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkMap::NetworkMap(const std::string &configFile)
{
	if (configFile != "")
		_load(configFile);
}

NetworkMap::~NetworkMap()
{

}

int NetworkMap::_load(const std::string &filename)
{
	// open
	FILE *fp = fopen(filename.c_str(), "r");

	// look for [NETWORK_DESCRIPTION]
	char row[255];
	char row1[255];
	char row2[255];

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
		tmpMapEntry.nodeName = std::string(row1);
		// while
		while (true)
		{
			tableEntry tmpTableEntry;

			chk = _readAndCheck(fp, row1);
			if (chk == -1) // [END], OEF
			{
				read = false;
				break;
			}
			else if (chk == 1) // new node
				break;
		
			chk = _readAndCheck(fp, row2);
			if (chk == -1) // [END], OEF
			{
				read = false;
				break;
			}
			else if (chk == 1) // new node
				break;

			tmpTableEntry.netID = string(row1);
			tmpTableEntry.ip = string(row2);

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
		fclose(fp);
		return -1;
	}
	else if (strncmp(row, "Node=", 5) == 0)
		return 1;
	else
		return 0;
}

void NetworkMap::findIp(const std::string &inIp, const std::string &net, std::string &outIp)
{
	NETWORK_MAP_IT mapIt;
	NODE_TABLE_IT nodeIt;

	mapIt = _networkMap.begin();

	bool foundNode = false;
	bool foundNet = false;
	string tmpIp;
	// for each node
	while (mapIt != _networkMap.end() && !(foundNet&&foundNode))
	{
		// reset foundNet
		if (!foundNode)
			foundNet = false;

		// for each network
		nodeIt = mapIt->table.begin();
		while (nodeIt != mapIt->table.end() && !(foundNet&&foundNode) )
		{
			if (nodeIt->netID == net)
			{
				tmpIp = nodeIt->ip;
				foundNet = true;
			}
			
			if (nodeIt->ip == inIp)
				foundNode = true;
				
			nodeIt++;
		}
		mapIt++;
	}

	if ( !(foundNode && foundNet) )
		outIp = inIp;  // nothing was found, return ip
	else
		outIp = tmpIp; // node and net were found, return tmpIp 
	
}