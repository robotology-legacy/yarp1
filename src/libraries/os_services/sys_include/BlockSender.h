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
/// $Id: BlockSender.h,v 1.10 2003-08-10 07:08:40 gmetta Exp $
///
///

#ifndef BLOCKSENDER_H_INC
#define BLOCKSENDER_H_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/// LATER: Need remove this -- use ACE equivalent.
///#include <vector>

///
/// STILL under testing especially for efficiency.
#include <ace/Containers_T.h>

#include "RefCounted.h"
#include "YARPNameID.h"
#include "YARPNameID_defs.h"
#include "YARPPortContent.h"
#include "YARPList.h"

extern int __debug_level;

// Should find out exactly what this is

#ifdef __QNX4__
#include <sys/sendmx.h>
#else
using namespace std;

/* must match Block class in YARPMultipartMessage implementation */
class _mxfer_entry
{
public:
	void *buffer;
	int len;

	void _set(void *n_buffer, int n_len)
	{ 
		buffer = n_buffer;  
		len = n_len; 
	}
};

#define _setmx(p,b,l) (p)->_set((b),(l))
#endif

class BlockUnit : public _mxfer_entry
{
public:
	BlockUnit() {}

	BlockUnit(char *buffer, int len)
	{
		Set(buffer, len);
	}

	void Set(char *buffer, int len)
	{
		_setmx(this, buffer, len);
	}
};

///
/// handling of large messages is happy here.
/// it might require a better handling of the multipart stuff.
/// I simplified Add by neglecting the need to send a full packet...
///

///
///
///
class BlockSender : public YARPPortWriter
{
protected:
	int max_packet;
	int AddPiece(char *buffer, int len);

public:
	YARPNameID pid;
	int available;
	int pieces;
	int failed;
	YARPVector<BlockUnit> entries;
	YARPVectorIterator<BlockUnit> cursor;

	BlockSender() : cursor(entries)
	{
		max_packet = MAX_PACKET;
		Begin (YARPNameID (YARP_NO_SERVICE_AVAILABLE, ACE_INVALID_HANDLE));
	}

	virtual ~BlockSender() {}

	void SetMaxPacket(int n_max_packet)
	{
		ACE_UNUSED_ARG (n_max_packet);
		available += (n_max_packet-max_packet);
		max_packet = n_max_packet;
	}

	void Begin(const YARPNameID& npid)
	{
		pid = npid;
		cursor.go_head();
		available = max_packet;
		pieces = 0;
		failed = 0;
	}

	int Add(char *buffer, int len);

	virtual int Write(char *buffer, int length)
	{
		return Add (buffer, length);
	}

	int End()
	{
		Fire();
		pid = YARPNameID (YARP_NO_SERVICE_AVAILABLE, ACE_INVALID_HANDLE);
		return !failed;
	}

	int Fire();
};


template <class T>
class HeaderedBlockSender : public BlockSender
{
public:
	int add_header;
	YARPVector<T> headers;
	YARPVectorIterator<T> header_cursor;

	HeaderedBlockSender() : header_cursor(headers)
	{
		add_header = 0;
		header_cursor = 0;
	}

	void Begin(const YARPNameID& pid)
	{
		add_header = 0;
		header_cursor = 0;
		BlockSender::Begin(pid);
	}

	int Add(char *buffer, int len)
	{
		Check();
		return BlockSender::Add(buffer,len);
	}
  
	virtual int Write(char *buffer, int length)
	{
		return Add(buffer,length);
	}

	int End()
	{
		Check();
		return BlockSender::End();
	}

	void Check()
	{
		if (add_header)
		{
			BlockSender::Add((char*)(&headers[header_cursor]),sizeof(T));
			add_header = 0;
			++header_cursor;
		}
	}
  
	T *AddHeader()
	{
		Check();

		int sz = headers.size();
		if (header_cursor == sz)
		{
			sz++;
			headers.size(sz);
			headers[header_cursor] = T();
		}

		add_header = 1;
		return &headers[header_cursor];
	}
};


#endif
