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
///
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
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: BlockReceiver.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/ACE.h>

#include "yarp_private/BlockReceiver.h"
#include <yarp/debug.h>
#include <yarp/YARPSyncComm.h>

/// class/unit specific debug level.
#define THIS_DBG 50

int BlockReceiver::End()
{
	if (reply_pending) // && !failed)
	{
		if (pid.isValid()) 
		{
			char buf[] = "ok";
			int r = YARPSyncComm::Reply(pid, buf, sizeof(buf));
			if (r == YARP_FAIL)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "BlockReceiver failed 1\n"));
				failed = 1;
			}
		}
		reply_pending = 0;
	}
	return !failed;
}

int BlockReceiver::Get()
{
	if (!has_msg && !failed)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Receiving...\n"));

		char buf[100];
		pid = YARPSyncComm::BlockingReceive(pid, buf, 0);
		if (pid.isValid())
		{
			has_msg = 1;
			offset = 0;
			reply_pending = 1;
		}
		else
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "BlockReceiver failed 2\n"));
			failed = 1;
			pid.invalidate();
		}
	}
	return has_msg;
}

int BlockReceiver::Get(char *buffer, int len)
{
	int bytes;
	int target = len;
	int terminated = 0;

	while (len > 0 && pid.isValid() && !terminated)
	{
		Get();
		if (has_msg)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "Reading %d, %d remaining...\n",target,len));
			//bytes = Readmsg(pid,offset,buffer,len);
			bytes = YARPSyncComm::ContinuedReceive(pid,buffer,len);
			if (bytes==0)
			{
				End();
				has_msg = 0;
				Get();
				if (has_msg)
				{
					bytes = YARPSyncComm::ContinuedReceive(pid,buffer,len);
				}
			}
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "Got %d of %d...\n",bytes,target));

			if (bytes<0)
			{
				failed = 1;
				pid.invalidate();
			}
			else
			{
				if (bytes>len)
				{
					failed = 1;
					terminated = 1;
				}
				if (bytes>0)
				{
					buffer += bytes;
					len -= bytes;
					offset += bytes;
				}
			}
		}
		else
		{
			terminated = 1;
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Stopping %d failed=%d terminated=%d...\n",target, failed,terminated));
	
	return !failed;
}


#undef THIS_DBG

