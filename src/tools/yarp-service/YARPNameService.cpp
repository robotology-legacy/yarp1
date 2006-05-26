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
/// $Id: YARPNameService.cpp,v 2.2 2006-05-26 21:33:07 babybot Exp $
///
///
// YARPNameService.cpp : Defines the entry point for the console application.
//

/// #include "YARPNameServer.h"

#define NAME_SERVER_VERBOSE
#include "YARPNameServer.h"

#include <yarp/YARPString.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPTime.h>
#include <iostream>
#include <fstream>
using namespace std;


// need to be able to make directories
// check how to do this in a portable way...
#include <ace/OS.h>
/// This doesn't compile under ACE 5.3.3, is it needed?
///#include <ace/OS_NS_unistd.h>
#include <ace/INET_Addr.h>


const char *_name_file_path = "conf/namer.conf";
static int _server_port = 1000;

void print_menu(int ok=0)
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
	if (ok) {
	  cout << endl;
	  cout << "*** YARP name service started OK" << endl;
	}
	cout << ":" << flush;
}

int parse(const YARPString &str)
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


extern char * LocalGetYarpRoot (void);

static void SmartHost(YARPString& result) {
  char buf[256] = "localhost";
  result = buf;
  int r = ACE_OS::hostname(buf,sizeof(buf));
  if (r!=-1) {
    result = buf;
    ACE_INET_Addr addr;
    int r = addr.set((short unsigned int)0,(const char *)buf);
    if (r!=-1) {
      YARPString outIp = addr.get_host_addr();
      if (outIp!=YARPString("127.0.0.1")) {
	result = outIp;
      }
    }    
  }
}

static void GetServer(YARPString& fname, YARPString& server, int& port,
		      const char *target = NULL,
		      int target_port = -1, int quiet = 0) {
	int read_ok = 0;

	char buf[256];
	char *root = LocalGetYarpRoot();
	ACE_ASSERT (root != NULL);
	ACE_OS::sprintf (buf, "%s/%s", root, _name_file_path);
	if (!quiet) printf("Using configuration file %s\n", buf);

	{
	ifstream fin(buf);
	if (fin.eof() || fin.fail())
	{
	  if (!quiet) ACE_DEBUG ((LM_DEBUG, "can't read config file, writing a default one\n"));
		fin.close();
		char buf2[1000];
		sprintf(buf2,"%s/conf",root);
		ACE_OS::mkdir(root);
		ACE_OS::mkdir(buf2);
		ofstream fout(buf);
		if (target==NULL) {
		  YARPString host;
		  SmartHost(host);
		  fout << host.c_str() << " 10000" << endl << endl;
		} else {
		  fout << target << " " << target_port << endl << endl;
		}
		fout << "// start network description, don't forget to separate \"Node=\" and names with space" << endl;
		fout << "[NETWORK_DESCRIPTION]" << endl;
		fout << "[END]" << endl;
		fout.close();
	}
	}

	ifstream fin(buf);
	int ok = 1;
	if (fin.eof() || fin.fail())
	  {
	    ok = 0;
	    if (!quiet) {
	      ACE_DEBUG ((LM_WARNING, "can't read or create config file\n"));
	      exit(1);
	    }
	  }

	char hostname[256];
	ACE_OS::memset (hostname, 0, 256);

	if (ok) {
	  fin >> hostname >> _server_port;
	} else {
	  hostname[0] = '\0';
	  _server_port = 10000;
	}

	if (target!=NULL) {
	  if (strcmp(target,hostname)!=0 || _server_port!=target_port) {
	    printf("*** Asked to use name service at [%s %d]\n",
		   target, target_port);
	    printf("*** But [%s] says to use [%s %d]\n",
		   buf, hostname, _server_port);
	    printf("*** Please fix [%s] or delete it\n",
		   buf);
	    ok = 0;
	  }
	}
	if (ok&&!quiet) {
	  ACE_DEBUG ((LM_INFO, "*** Using YARP name server at [%s %d]\n", hostname, _server_port));
	}

	fname = buf;
	server = hostname;
	port = _server_port;
	
	fin.close ();
}


static void info() {
  YARPString config, server;
  int server_port;
  GetServer(config,server,server_port,NULL,-1,1);
  printf("*** configuration file found/created at [%s]\n", config.c_str());
  printf("*** It specifies a server at:\n\t\tMachine [%s]\n\t\tPort number [%d]\n", server.c_str(), server_port);
}

static void diagnose() {
  YARPString config, server;
  int server_port;
  GetServer(config,server,server_port,NULL,-1,1);
  printf("*** Check the configuration file at [%s]\n", config.c_str());
  if (server == YARPString("")) {
    printf("*** It doesn't have a server name in it.\n");
    printf("*** Try deleting it and rerunning this program.\n");
  }
  printf("*** It currently specifies a server at:\n\t\tMachine [%s]\n\t\tPort number [%d]\n", server.c_str(), server_port);
  printf("*** These settings can be changed on the first line of the file.\n");
  int ch = server.c_str()[0];
  if (ch<='0'||ch>='9') {
    printf("*** Perhaps try using an IP address instead of a host name.\n");
  }
  printf("***\n");
  printf("*** If this is the wrong configuration file,\n");
  printf("***    please set the optional YARP_ROOT environment variable\n");
  printf("***    to the directory in which the desired namer.conf resides\n");
  printf("***\n");
}

static void check() {
  // check for a config file
  {
    YARPString config, server;
    int server_port;
    GetServer(config,server,server_port,NULL,-1,1);
  }

  printf("=================================================================\n");
  printf("=== Just in case things go badly wrong,\n");
  printf("=== here is some upfront information on how to get things right:\n");
  diagnose();

  YARPOutputPortOf<NetInt32> p1;
  YARPInputPortOf<NetInt32> p2(YARPInputPort::NO_BUFFERS);
  printf("=================================================================\n");
  printf("=== Trying to register some ports\n");
  p1.Register("/yarp-service/1");
  p2.Register("/yarp-service/2");
  YARPTime::DelayInSeconds(1);

  printf("=================================================================\n");
  printf("=== Trying to connect some ports\n");
  p1.Connect("/yarp-service/2");
  YARPTime::DelayInSeconds(1);

  printf("=================================================================\n");
  printf("=== Trying to write some data\n");
  YARPTime::DelayInSeconds(1);

  p1.Content() = 42;
  printf("*** Writing number %d\n", p1.Content());
  p1.Write();

  printf("=================================================================\n");
  int ok = 0;
  for (int i=0; i<3; i++) {
  printf("=== Trying to read some data\n");
    YARPTime::DelayInSeconds(1);
    if (p2.Read(false)) {
      int x = p2.Content();
      printf("*** Read number %d\n", p2.Content());
      if (x==42) {
	ok = 1;
	break;
      }
    }
  }

  printf("=================================================================\n");
  printf("=== Trying to unregister some ports\n");
  p1.Unregister();
  p2.Unregister();
  YARPTime::DelayInSeconds(1);

  printf("=================================================================\n");
  if (!ok) {
    printf("*** YARP seems broken.\n");
    diagnose();
    exit(1);
  } else {
    printf("*** YARP seems okay!\n");
    //GetServer(buf,server,_server_port,target,port);
  }
}

void interactive(YARPNameServer &dns)
{
  YARPString str;
  print_menu(1);
	
  while (cin >> str)
    {
      int ret = parse(str);

      if (ret == 0)    //return
	break;
      else if (ret == 1)
	{
	  YARPString str1, str2;
	  cin >> str1;
	  cin >> str2;
      dns.handle_registration_dbg(str1,str2, YARP_TCP);
	}
      else if (ret == 2){
	YARPString str1;
	cin >> str1;
			
	dns.handle_release(str1);
      }
      else if (ret == 3){
	YARPString str1;
	cin >> str1;
	dns.query_dbg(str1);
      }
      else if (ret == 4){
	dns.dump();
      }
      else if (ret == 5){
	YARPString str1;
	cin >> str1;
	dns.handle_registration_dip_dbg(str1, YARP_MCAST);
      }
      else if (ret == 6){
	YARPString str1,str2;
	int n;
	cin >> str1;
	cin >> str2;
	cin >> n;
	dns.handle_registration_dbg(str1, str2, YARP_UDP, n);
      }
      print_menu();
    }
}

void run(bool daemon=false) 
{
  YARPString buf, server;
  int _server_port = 10000;
  GetServer(buf,server,_server_port);
  
  YARPNameServer dns(buf,server,_server_port);

  if(!daemon)
    {
      interactive(dns);
    }
  else
    {
      dns.waitForEnd(); //blocks here until quit message is received remotely
    }
}

void help() {
  const char *app = "yarp-service";
  const char *bpp = "            ";
  ACE_OS::printf("\n");
  ACE_OS::printf("To start the YARP name service, type:\n");
  ACE_OS::printf("   %s --run or --daemon\n", app);
  ACE_OS::printf("Then on each other machine that will be using YARP, type:\n");
  ACE_OS::printf("   '%s --remote <host>'\n", app);
  ACE_OS::printf("to configure that machine to refer YARP name queries to <host>\n");
  ACE_OS::printf("\n");
  ACE_OS::printf("Usage information for %s\n\n", app);
  ACE_OS::printf("%s --run   : starts YARP name service running on this machine\n",app);
  ACE_OS::printf("%s --daemon : starts YARP name service as daemon (suppress interactive mode)\n", app);
  ACE_OS::printf("%s --check : checks and troubleshoots YARP name service configuration\n",app);
  ACE_OS::printf("%s --remote <server> [<portnumber>] : \n", app);
  ACE_OS::printf("%s           configures current machine to consult YARP name\n",bpp);
  ACE_OS::printf("%s           running on <server>, with optional <portnumber>\n",bpp);
  ACE_OS::printf("%s --help  : shows this message\n", app);
  ACE_OS::printf("\n");
}

int remote(int argc, char* argv[]) {
  if (argc>0) {
    const char *target = argv[0];
    argc--;
    argv++;
    int port = 10000;
    if (argc>0) {
      port = atoi(argv[0]);
      argc--;
      argv++;
    }
    printf("asked to direct service to another machine, %s:%d\n",target,port);
    YARPString buf, server;
    int _server_port;
    GetServer(buf,server,_server_port,target,port);

  } else {
    help();
  }
  return YARP_OK;
}

int main(int argc, char* argv[])
{
  argc--;
  argv++;

  if (argc>0) {
    if (argv[0][0] == '-') {
      const char *request = argv[0];
      argc--;
      argv++;
      if (strcmp(request,"--check")==0) {
	printf("Checking installation of YARP on this machine.\n");
	check();
	return YARP_OK;
      }
      if (strcmp(request,"--run")==0) {
	printf("Starting YARP name service on this machine.\n");
	run();
	return YARP_OK;
      }
      if (strcmp(request, "--daemon")==0) {
	printf("Starting YARP name service on this machine, run as daemon.\n");
	run(true);
	return YARP_OK;
      }
      if (strcmp(request,"--remote")==0) {
	remote(argc,argv);
	return YARP_OK;
      }
      if (strcmp(request,"--help")==0) {
	help();
	return YARP_OK;
      }
      printf("Option %s not recognized, try yarp-service --help\n", request);
      return YARP_FAIL;
    }
  }

  help();

  return 0;
}


