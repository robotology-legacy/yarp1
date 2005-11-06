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
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPMultipartMessage.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

///
///
#include <yarp/YARPMultipartMessage.h>

/**
 * \file YARPMultipartMessage.cpp contains classes and implementation of
 * a multipart message. This is useful (in theory) to send messages of
 * arbitrary size.
 */

/**
 * A block of the multipart message (buffer).
 * Note that the QNX implementation is quite specific to be compatible 
 * to the OS message structure.
 */
class Block
{
public:
#ifndef __QNX6__
	char *buffer;
	int len;
#else
	iov_t iov;
#endif

	/**
	 * Constructor.
	 */
	Block () 
	{ 
#ifndef __QNX6__
		buffer = NULL; 
		len = 0; 
#endif
	}

	/**
	 * Constructor. A buffer is added to the message block.
	 * @param n_buffer is the pointer to the part of the message.
	 * @param n_len is the length of the buffer. 
	 */
	Block (char *n_buffer, int n_len)
	{
		Set(n_buffer, n_len);
	}

	/**
	 * Adds a buffer to the block.
	 * @param n_buffer is the pointer to the part of the message.
	 * @param n_len is the length of the buffer. 
	 */
	void Set (char *n_buffer, int n_len)
	{
#ifndef __QNX6__
		buffer = n_buffer;
		len = n_len;
#else
		/* MIG4NTO           _setmx Replaced by SETIOV() for use with the QNX Neutrino */
		/* MIG4NTO                  Msg*() functions. */
		//_setmx(this,buffer,len);
		SETIOV(&iov, n_buffer, n_len);
#endif
	}
};


YARPMultipartMessage::~YARPMultipartMessage ()
{
	if (owned)
	{
		if (system_resource != NULL)
		{
			delete[] ((Block*)system_resource);
			system_resource = NULL;
		}
	}
}

void YARPMultipartMessage::Resize (int n_length)
{
	if (n_length>length)
	{
		if (owned)
		{
			if (system_resource != NULL)
			{
				delete[] ((Block*)system_resource);
				system_resource = NULL;
			}
		}

		system_resource = new Block[n_length];
		ACE_ASSERT (system_resource != NULL);
		length = n_length;
	}
	top_index = -1;
	owned = 1;
}

void YARPMultipartMessage::Set (int index, char *buffer, int buffer_length)
{
	ACE_ASSERT (index < length);
	((Block*)system_resource)[index].Set(buffer,buffer_length);

	if (index > top_index)
	{
		top_index = index;
	}
}

void YARPMultipartMessage::Reset ()
{
	top_index = -1;
}



char *YARPMultipartMessage::GetBuffer(int index)
{
	ACE_ASSERT(index <= top_index);
#ifndef __QNX6__
	return ((Block*)system_resource)[index].buffer;
#else
	Block *blk = ((Block*)system_resource) + index;
	/* MIG4NTO            MK_FP NO LONGER SUPPORTED */
	//return (char *) MK_FP(blk->mxfer_seg,blk->mxfer_off);
	return ((char *) blk->iov.iov_base);
#endif
}


int YARPMultipartMessage::GetBufferLength(int index)
{
	ACE_ASSERT(index <= top_index);
#ifndef __QNX6__
	return ((Block*)system_resource)[index].len;
#else
	Block *blk = ((Block*)system_resource) + index;
	return blk->iov.iov_len;
#endif
}

