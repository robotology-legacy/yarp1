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
///                    #paulfitz, pasa, carlos#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNativeNameService.cpp,v 1.3 2003-05-02 22:56:11 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <sys/dispatch.h> //+QNX6+
#include <sys/iofunc.h>
#include <sys/netmgr.h>

#include "YARPTime.h"
#include "YARPNativeNameService.h"
#include "YARPNameID_defs.h"
#include "YARPSemaphore.h"

///
/// need to create a channel first, then register into the name server.
///
int YARPNativeEndpointManager::CreateQnetChannel (void)
{
	int chid;
	if ((chid = ChannelCreate(0)) != -1)
		return chid;
	
	return YARP_FAIL;
}

///
///
///
YARPNameID YARPNativeEndpointManager::CreateInputEndpoint (YARPUniqueNameID& name)
{
	return name.getNameID();
}



YARPNameID YARPNativeEndpointManager::CreateOutputEndpoint(YARPUniqueNameID& name)
{
	int coid = ConnectAttach( 
						netmgr_strtond (name.getAddressRef().get_host_addr(),NULL),
						(int)name.getP2Ptr()[0],
						(int)name.getRawIdentifier(),
						0,
						0);
	if (coid != -1)
	{
		name.setRawIdentifier(coid);
		return name.getNameID();
	}

	/// failed.
	return YARPNameID();
}

int YARPNativeEndpointManager::ConnectEndpoints(YARPNameID& dest)
{
	return YARP_OK;
}

int YARPNativeEndpointManager::Close(void)
{
	ACE_DEBUG ((LM_DEBUG, "Close not implemented yet for QNX-QNET\n"));
	return YARP_OK;
}

///
///
///
int YARPNativeNameService::RegisterName(const char *name)
{
	return YARP_OK;
}

YARPNameID YARPNativeNameService::LocateName(const char *name)
{
	return YARPNameID();
}

int YARPNativeNameService::IsNonTrivial()
{
	return 1;
}

/*
int register_name(const char * name, int chid)
{
	char hostname[250];
	YARPRegisteredName frname;
	YARPNameManager  * namemanager;
		
	namemanager = new YARPNameManager(FILE_NAME);
	
	assert(namemanager != NULL);
	
	gethostname(hostname,256);
	YARPRegisteredName	rname(name,
						  hostname,
						  getpid(),
						  chid);
	
	namemanager->Add(rname);		
	//namemanager->List();	
	delete namemanager;		
	
}

int name_search(const char * name)
{
	YARPNameManager  * namemanager;
	YARPRegisteredName rname;
	int coid = 0;
	   
	namemanager = new YARPNameManager(FILE_NAME);
  	
  	if(namemanager->FindName((char *)name,rname))
  	{
  		coid = ConnectAttach( netmgr_strtond(rname.GetNode(),NULL),
  					   rname.GetProcId(),
  					   rname.GetChId(),
  					   0,
  					   0);
 		//cout << "I have found: "<< coid << " " << rname ;
  	}      
       
    delete namemanager;   
    return coid;
}

*/
