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
/// $Id: BlockSender.cpp,v 1.5 2003-05-12 23:32:43 gmetta Exp $
///
///

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

#ifndef __QNX4__
using namespace std;
#endif

//#include <sys/kernel.h>
//#include <sys/sendmx.h>
#include "BlockSender.h"

#include "debug.h"
#include <errno.h>

#include "YARPSyncComm.h"

//#undef DEBUG_LEVEL
//#define DEBUG_LEVEL 100

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

#define THIS_DBG 50

static char _buf[100];

int BlockSender::Fire()
{
	int result = 0;
	if (pieces > 0)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Sending %d pieces\n",pieces));
		errno = 0;
		BlockUnit *bu = entries.begin();
		YARPMultipartMessage msg(bu, pieces);	// this are allocated whenever there's a Fire...
		YARPMultipartMessage reply_msg(1);		// LATER: optimize by allocating in advance.
		//char buf[100];
		reply_msg.Set(0,_buf,sizeof(_buf));
		result = YARPSyncComm::Send(pid, msg, reply_msg);
		pieces = 0;
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Sent %d pieces\n",pieces));
	}
	cursor = entries.begin();
	if (result == YARP_FAIL)
	{
		failed = 1;
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** BlockSender::Fire() failed, err# %P\n"));
	}
	return (result != YARP_FAIL);
}

int BlockSender::AddPiece(char *buffer, int len)
{
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Adding piece, length %d (avail %d)\n", len, available));
	if (cursor == entries.end ())
	{
		///YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** NEW stl %s : %s\n", __FILE__, __LINE__));
		cursor = entries.insert (cursor, BlockUnit(buffer,len));
	}
	else
	{
		cursor->Set (buffer, len);
	}

	pieces++;
	cursor++;
	return 1;
}

///
/// why is the buffer split in many pieces. 
///	NT? shouldn't be more efficient to let the OS do the job.
///	this might be the legacy of QNX 4.25?
///
///
int BlockSender::Add(char *buffer, int len)
{
	while (len > 0)
	{
		if (len > available)
		{
			AddPiece(buffer, available);
			len -= available;
			buffer += available;
			Fire();
			available = max_packet;
		}
		else
		{
			AddPiece(buffer,len);
			available -= len;
			if (available == 0)
			{
				Fire();
				available = max_packet;
			}
			len = 0;
		}
	}
	return 1;
}

#undef THIS_DBG