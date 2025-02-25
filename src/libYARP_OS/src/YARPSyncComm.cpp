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
/// $Id: YARPSyncComm.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/OS.h>

//#include <stdio.h>

#include <yarp/YARPSyncComm.h>
#include <yarp/YARPNativeSyncComm.h>
#include <yarp/YARPSocketSyncComm.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPNativeNameService.h>

///
/// Native, at least for QNX is the QNet stuff.
///	pls, do not call this. it's not longer used/needed.
///
static void Complete(YARPNameID& dest)
{
	ACE_UNUSED_ARG (dest);
}

///
int YARPSyncComm::Send (const YARPNameID& dest, char *buffer, int buffer_length, char *return_buffer, int return_buffer_length)
{
	switch (dest.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::Send (dest, buffer, buffer_length, return_buffer, return_buffer_length);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::Send (dest, buffer, buffer_length, return_buffer, return_buffer_length);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::Send : name mode undefined\n"));
	}

	return YARP_FAIL;
}


YARPNameID YARPSyncComm::BlockingReceive (const YARPNameID& src, char *buffer, int buffer_length)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::BlockingReceive (src, buffer, buffer_length);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::BlockingReceive (src, buffer, buffer_length);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::BlockingReceive : name mode undefined\n"));
	}
	
	return YARPNameID();
}


YARPNameID YARPSyncComm::PollingReceive (const YARPNameID& src, char *buffer, int buffer_length)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::PollingReceive(src,buffer,buffer_length);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::PollingReceive (src, buffer, buffer_length);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::PollingReceive : name mode undefined\n"));
	}
	
	return YARPNameID();
}


int YARPSyncComm::ContinuedReceive (const YARPNameID& src, char *buffer, int buffer_length)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::ContinuedReceive (src, buffer, buffer_length);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::ContinuedReceive (src, buffer, buffer_length);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::ContinuedReceive : name mode undefined\n"));
	}
	
	return YARP_FAIL;
}


int YARPSyncComm::Reply (const YARPNameID& src, char *buffer, int buffer_length)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::Reply (src, buffer, buffer_length);
	
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::Reply (src, buffer, buffer_length);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::Reply : name mode undefined\n"));
	}
	
	return YARP_FAIL;
}


int YARPSyncComm::Send (const YARPNameID& dest, YARPMultipartMessage& msg, YARPMultipartMessage& return_msg)
{
	switch (dest.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::Send (dest, msg, return_msg);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::Send (dest, msg, return_msg);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::Send : name mode undefined\n"));
	}
	
	return YARP_FAIL;
}


YARPNameID YARPSyncComm::BlockingReceive (const YARPNameID& src, YARPMultipartMessage& msg)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::BlockingReceive (src, msg);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::BlockingReceive (src, msg);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::BlockingReceive : name mode undefined\n"));
	}
	
	return YARPNameID();
}


YARPNameID YARPSyncComm::PollingReceive (const YARPNameID& src, YARPMultipartMessage& msg)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::PollingReceive (src, msg);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::PollingReceive (src, msg);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::PollingReceive : name mode undefined\n"));
	}
	
	return YARPNameID();
}


int YARPSyncComm::Reply (const YARPNameID& src, YARPMultipartMessage& msg)
{
	switch (src.getServiceType ())
	{
	case YARP_QNET:
		return YARPNativeSyncComm::Reply (src, msg);

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketSyncComm::Reply (src, msg);

	default:
		ACE_DEBUG ((LM_DEBUG, "YARPSyncComm::Reply : name mode undefined\n"));
	}
	
	return YARP_FAIL;
}


