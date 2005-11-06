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
/// $Id: BlockSender.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#ifndef BLOCKSENDER_H_INC
#define BLOCKSENDER_H_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
/// STILL under testing especially for efficiency.
#include <ace/Containers_T.h>

#include "yarp_private/RefCounted.h"
#include <yarp/YARPNameID.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPPortContent.h>
#include <yarp/YARPList.h>

extern int __debug_level;

/**
 * \file BlockSender.h these classes are part of the multi-part message 
 * implementation.
 */

using namespace std;

/**
 * It must match Block class in YARPMultipartMessage implementation. This is
 * an old class name/implementation that was found on QNX4 where all this started
 * out.
 */
class _mxfer_entry
{
public:
	void *buffer;
	int len;

	/**
	 * Sets a buffer into the class.
	 * @param n_buffer is the pointer to the buffer. The memory block
	 * is not copied.
	 * @param n_len is the size of the buffer.
	 */
	void _set(void *n_buffer, int n_len)
	{ 
		buffer = n_buffer;  
		len = n_len; 
	}
};

#define _setmx(p,b,l) (p)->_set((b),(l))

/**
 * Wraps over the block entry type.
 */
class BlockUnit : public _mxfer_entry
{
public:

	/**
	 * Contructor.
	 */
	BlockUnit() {}

	/**
	 * Contructor. Adds a buffer to the unit.
	 * @param buffer is the pointer to the buffer.
	 * @param len is the len of the buffer.
	 */
	BlockUnit(char *buffer, int len)
	{
		Set(buffer, len);
	}

	/**
	 * Sets a buffer into the unit.
	 * @param buffer is the pointer to the buffer.
	 * @param len is the len of the buffer.
	 */
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

/**
 * This is a sender class for a message block. This class 
 * knows how to send a buffer to a remote endpoint. The blocks
 * are stored into a vector of BlockUnit classes.
 */
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

	/**
	 * Constructor.
	 */
	BlockSender() : cursor(entries)
	{
		max_packet = MAX_PACKET;
		Begin (YARPNameID (YARP_NO_SERVICE_AVAILABLE, ACE_INVALID_HANDLE));
	}

	/**
	 * Destructor.
	 */
	virtual ~BlockSender() {}

	/**
	 * Sets the maximum size of a packet. Messages are forced to
	 * comply with this constraint.
	 * @param n_max_packet is the packet size in bytes.
	 */
	void SetMaxPacket(int n_max_packet)
	{
		ACE_UNUSED_ARG (n_max_packet);
		available += (n_max_packet-max_packet);
		max_packet = n_max_packet;
	}

	/**
	 * Starts using the sender for a specific destination.
	 * @param npid is the destination identifier.
	 */
	void Begin(const YARPNameID& npid)
	{
		pid = npid;
		cursor.go_head();
		available = max_packet;
		pieces = 0;
		failed = 0;
	}

	/**
	 * Adds a new chunk to the message.
	 * @param buffer is the pointer to the buffer.
	 * @param len is the length of the buffer.
	 * @return 1 if successful.
	 */
	int Add(char *buffer, int len);

	/**
	 * Calls Add().
	 * @param buffer is a pointer to the buffer.
	 * @param length is the length of the buffer.
	 * @return 1 if successful.
	 */
	virtual int Write(char *buffer, int length)
	{
		return Add (buffer, length);
	}

	/**
	 * Terminates the composition of this message and sends it
	 * to destination.
	 * @return 1 if successful.
	 */
	int End()
	{
		Fire();
		pid = YARPNameID (YARP_NO_SERVICE_AVAILABLE, ACE_INVALID_HANDLE);
		return !failed;
	}

	/**
	 * Sends the message to destination.
	 * @return 1 if successful.
	 */
	int Fire();
};

/**
 * A template class for multipart messages where the units are of
 * generic type.
 */
template <class T>
class HeaderedBlockSender : public BlockSender
{
public:
	int add_header;
	YARPVector<T> headers;
	YARPVectorIterator<T> header_cursor;

	/**
	 * Constructor.
	 */
	HeaderedBlockSender() : header_cursor(headers)
	{
		add_header = 0;
		header_cursor = 0;
	}

	/**
	 * Starts using the sender for a specific destination.
	 * @param pid is the destination identifier.
	 */
	void Begin(const YARPNameID& pid)
	{
		add_header = 0;
		header_cursor = 0;
		BlockSender::Begin(pid);
	}

	/**
	 * Adds a new chunk to the message.
	 * @param buffer is the pointer to the buffer.
	 * @param len is the length of the buffer.
	 * @return 1 if successful.
	 */
	int Add(char *buffer, int len)
	{
		Check();
		return BlockSender::Add(buffer,len);
	}
  
	/**
	 * Calls Add().
	 * @param buffer is a pointer to the buffer.
	 * @param length is the length of the buffer.
	 * @return 1 if successful.
	 */
	virtual int Write(char *buffer, int length)
	{
		return Add(buffer,length);
	}

	/**
	 * Terminates the composition of this message and sends it
	 * to destination.
	 * @return 1 if successful.
	 */
	int End()
	{
		Check();
		return BlockSender::End();
	}

	/**
	 * Add the header to the message if needed.
	 */
	void Check()
	{
		if (add_header)
		{
			BlockSender::Add((char*)(&headers[header_cursor]),sizeof(T));
			add_header = 0;
			++header_cursor;
		}
	}
  
	/**
	 * Adds a new header and returns a pointer to it.
	 * @return a pointer to the newly added header of type T.
	 */
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
