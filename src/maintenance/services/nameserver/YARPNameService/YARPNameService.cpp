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
/// $Id: YARPNameService.cpp,v 1.7 2003-06-20 17:38:12 beltran Exp $
///
///
// YARPNameService.cpp : Defines the entry point for the console application.
//

/// #include "YARPNameServer.h"

#define NAME_SERVER_VERBOSE
#include "YARPNameServer.h"

#include <iostream>
#include <string>

using namespace std;

#ifdef __WIN32__
const char *_name_file_path = "conf\\namer.conf";
const char *_name_static_init = "conf\\namer.static.conf";
#else
const char *_name_file_path = "conf/namer.conf";
const char *_name_static_init = "conf/namer.static.conf";
#endif

static int _server_port = 1000;

void print_menu()
{
	cout << "\n-----------------------\n";
	cout << "\nName server menu\n";
	cout << "\n";
	cout << "reg name ip: dbg mode, register name-ip get a port\n";
	cout << "regmc name: dbg mode, register name, get a dynamic ip\n";
	cout << "regn name ip n: dbg mode, register name-ip get n ports\n";
	cout << "rel name: dbg mode, release name\n";
	cout << "query name: dbg mode, query name\n";
	cout << "d: dump resource status\n";
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
	else if (str == "d")
		return 4;
	else if (str == "query")
		return 3;
	else if (str == "regmc")
		return 5;
	else if (str == "regn")
		return 6;
	else
		return -1;
}

int main(int argc, char* argv[])
{
	/// This is NT only, usual idiosincrasy between // and \\ Unix/NT clash.
	/// handle the connection w/ the remote name server.
	char buf[256];
	ACE_ASSERT (GetYarpRoot() != NULL);
#ifdef __WIN32__
	ACE_OS::sprintf (buf, "%s\\%s\0", GetYarpRoot(), _name_file_path);
#else
	ACE_OS::sprintf (buf, "%s/%s\0", GetYarpRoot(), _name_file_path);
#endif

	ifstream fin(buf);
	if (fin.eof() || fin.fail())
	{
		ACE_DEBUG ((LM_DEBUG, "can't read config file\n"));
		return YARP_FAIL;
	}

	char hostname[256];
	ACE_OS::memset (hostname, 0, 256);

	fin >> hostname >> _server_port;
	ACE_DEBUG ((LM_DEBUG, "name server at %s port %d\n", hostname, _server_port));

	///
#ifdef __WIN32__
	ACE_OS::sprintf (buf, "%s\\%s\0", GetYarpRoot(), _name_static_init);
#else
	ACE_OS::sprintf (buf, "%s/%s\0", GetYarpRoot(), _name_static_init);
#endif
	YARPNameServer dns(buf, _server_port);

	fin.close ();

	string str;
	print_menu();
	
	while (cin >> str)
	{
		int ret = parse(str);

		if (ret == 0)
			break;
		else if (ret == 1)
		{
			string str1, str2;
			cin >> str1;
			cin >> str2;
			dns.handle_registration_dbg(str1,str2, YARP_TCP);
		}
		else if (ret == 2){
			string str1;
			cin >> str1;
			
			dns.handle_release(str1);
		}
		else if (ret == 3){
			string str1;
			cin >> str1;
			dns.query_dbg(str1);
		}
		else if (ret == 4){
			dns.dump();
		}
		else if (ret == 5){
			string str1;
			cin >> str1;
			dns.handle_registration_dip_dbg(str1, YARP_MCAST);
		}
		else if (ret == 6){
			string str1,str2;
			int n;
			cin >> str1;
			cin >> str2;
			cin >> n;
			dns.handle_registration_dbg(str1, str2, YARP_UDP, n);
		}

		print_menu();
	}

	return 0;
}


