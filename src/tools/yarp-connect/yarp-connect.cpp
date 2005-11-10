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
/// $Id: yarp-connect.cpp,v 2.0 2005-11-10 17:36:29 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <yarp/YARPPort.h>

extern int __debug_level;

int main(int argc, char *argv[])
{
  int reconnect = 0;
  if (argc>1) {
    if (strcmp(argv[1],"-f")==0) {
      reconnect = 1;
      argc--;
      argv++;
    }
  }
	if (argc == 3)
	{
	  if (reconnect) {
	    // try to disconnect first
	    char buf[256];
	    sprintf(buf,"!%s",argv[2]);
	    YARPPort::Connect (argv[1], buf);
	  }
		YARPPort::Connect (argv[1], argv[2]);
	}
	else
	if (argc == 2)
	{
		YARPPort::Connect (argv[1], "__null");
	}
	else
	{
		ACE_DEBUG ((LM_INFO, "Use: yarp-connect <name1> <name2>\n"));
		ACE_DEBUG ((LM_INFO, "where <name1> is an output port name (source of messages)\n"));
		ACE_DEBUG ((LM_INFO, "and <name2> is an input port name (destination of messages)\n"));
		ACE_DEBUG ((LM_INFO, "prefix <name2> with '!' to detach port <name2> from <name1>\n"));
		ACE_DEBUG ((LM_INFO, "Use: yarp-connect -f <name1> <name2>\n"));
		ACE_DEBUG ((LM_INFO, "to refresh a connection when the target is restarted.\n"));
		ACE_DEBUG ((LM_INFO, "prefix <name1> with '*' to ask the port to display its connections\n"));
		ACE_DEBUG ((LM_INFO, "note that the display is simply made on the stdout of the port\n"));
		ACE_DEBUG ((LM_INFO, "that receives the request and not on the terminal where \nyarp-connect is called.\n"));
		ACE_DEBUG ((LM_INFO, "Have a nice day!\n"));
	}

	return YARP_OK;
}
