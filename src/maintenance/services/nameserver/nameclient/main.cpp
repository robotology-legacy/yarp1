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
/// $Id: main.cpp,v 1.10 2003-07-16 13:25:40 babybot Exp $
///
///

// NameClient.cpp: YARPNameService test program.
//

#define NAME_CLIENT_VERBOSE

#include <conf/YARPConfig.h>
#include <ace/config.h>

#include <YARPAll.h>
#include <YARPNameService.h>
#include <YARPNameClient.h>
#include <YARPParseParameters.h>

#include <string>

using namespace std;

void printHelp (void);
void interactive(YARPNameClient& nc);
void commandLine(YARPNameClient& nc, int argc, char* argv[]);

void print_menu (void)
{
	cout << "\n-----------------------\n";
	cout << "\nName client menu\n";
	cout << "\n";
	cout << "reg name ip: register name ip\n";
	cout << "regn name ip n: register name ip, ask n ports (UDP)\n";
	cout << "regmc name: register name, multicast\n";
	cout << "query name: query name\n";
	cout << "rel name: release name\n";
	cout << "queryqnx name: query a QNX name\n";
	cout << "regqnx name, node, pid, channel: register QNX name\n";
	cout << "releaseqnx name: release a QNX name\n";
	cout << "dump: remote dump\n";
	cout << "xdump: extended dump\n";
	cout << "any key: this menu\n";
	cout << "q!: exit\n";
	cout << "-----------------------\n";
	cout << ":" << flush;
}

int parse(const std::string &str)
{
	if (str == "q!")
		return 0;
	else if (str == "reg")
		return 1;
	else if (str == "rel")
		return 2;
	else if (str == "query")
		return 3;
	else if (str == "regmc")
		return 4;
	else if (str == "regn")
		return 5;
	else if (str == "queryqnx")
		return 6;
	else if (str == "regqnx")
		return 7;
	else if (str == "releaseqnx")
		return 8;
	else if (str == "dump")
		return 9;
	else if (str == "xdump")
		return 10;
	else
		return -1;
}

///
///
///
int main(int argc, char* argv[])
{
	char buf[YARP_STRING_LEN];
	ACE_ASSERT (GetYarpRoot() != NULL);
#ifdef __WIN32__
	ACE_OS::sprintf (buf, "%s\\%s\0", GetYarpRoot(), NAMER_CONFIG_FILE);
#else
	ACE_OS::sprintf (buf, "%s/%s\0", GetYarpRoot(), NAMER_CONFIG_FILE);
#endif

	ifstream fin(buf);
	if (fin.eof() || fin.fail())
	{
		return YARP_FAIL;
	}

	char hostname[YARP_STRING_LEN];
	ACE_OS::memset (hostname, 0, YARP_STRING_LEN);
	int portnumber = -1;

	fin >> hostname >> portnumber;
	fin.close ();

	YARPNameClient nc(hostname, portnumber);

	if (YARPParseParameters::parse(argc, argv, "i"))
		interactive(nc);
	else
		commandLine(nc, argc, argv);
		
	return YARP_OK;
}

void commandLine(YARPNameClient& nc, int argc, char* argv[])
{
	bool ok = false;
	if (YARPParseParameters::parse(argc, argv, "d"))
	{
		cout << nc.dump();
		ok = true;
	}
	
	if (YARPParseParameters::parse(argc, argv, "dx") || YARPParseParameters::parse(argc, argv, "xd"))
	{
		cout << nc.dump(1);
		ok = true;
	}
	
	std::string str;
	if (YARPParseParameters::parse(argc, argv, "rel", str))
	{
		if (nc.check_out(str) != 0)
			cout << "Error connecting to the server\n";
		ok = true;
	}
	
	if (!ok)
		printHelp();
}

void interactive(YARPNameClient& nc)
{
	string str;
	print_menu();
	
	while (cin >> str)
	{
		int ret = parse(str);

		if (ret == 0)
			break;
		else if (ret == 1)
		{
			string str1,str2;
			cin >> str1;
			cin >> str2;

			ACE_INET_Addr addr;
			ACE_INET_Addr reg_addr;

			reg_addr.set((unsigned short)0, str2.c_str());
			
			if (nc.check_in(str1, reg_addr, addr) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 2){
			string str1;
			cin >> str1;
						
			if (nc.check_out(str1) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 3){
			string str1;
			cin >> str1;
			ACE_INET_Addr tmpAddr;
			int tmpType;
			if (nc.query(str1, tmpAddr, &tmpType) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 4) {
			string str1;
			cin >> str1;
			ACE_INET_Addr tmpAddr;
			if (nc.check_in_mcast(str1, tmpAddr) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 5) {
			string str1,str2;
			int n;
			cin >> str1;	// name
			cin >> str2;	// ip
			cin >> n;		// num of ports
			NetInt32 *ports = new NetInt32 [n];
			if (nc.check_in_udp(str1,str2,ports,n) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 6) {
			string str1;
			cin >> str1;	// name
			YARPNameQnx tmp;
			int type;
			if (nc.query_qnx(str1,tmp,&type) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 7) {
			string name,node;
			int pid, chan;
			cin >> name;
			cin >> node;	// ip
			cin >> pid;		// num of ports
			cin >> chan;
			YARPNameQnx tmp;
			tmp.setName(name);
			tmp.setAddr(node, pid, chan);
			if (nc.check_in_qnx(tmp) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 8) {
			string name;
			cin >> name;
			if (nc.check_out_qnx(name) != 0)
				cout << "Error connecting to the server\n";
		}
		else if (ret == 9) {
			cout << "-------------------\n";
			cout << "Name server dump:\n";
			cout << nc.dump();
			cout << "-------------------";
		}
		else if (ret == 10) {
			cout << "-------------------\n";
			cout << "Extended dump:\n";
			cout << nc.dump(1);
			cout << "-------------------";
		}
		print_menu();
	}
}

void printHelp()
{
	cout << "Use: \n";
	cout << "-i (interactive mode)\n";
	cout << "-d (dump) -xd (extended dump)\n";
	cout << "-rel name (release name)\n";
}
