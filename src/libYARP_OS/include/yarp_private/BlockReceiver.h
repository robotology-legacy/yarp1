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
/// $Id: BlockReceiver.h,v 1.4 2004-08-11 17:34:58 babybot Exp $
///
///

#ifndef BLOCKRECEIVER_H_INC
#define BLOCKRECEIVER_H_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file BlockReceiver.h Classes that define a port reader: i.e. a class that knows how
 * to receive a buffer.
 */

#include <yarp/YARPNameService.h>
#include <yarp/YARPPortContent.h>

/**
 * This class knows how to read from a socket and store incoming data in a multi-part
 * message.
 */
class BlockReceiver : public YARPPortReader
{
protected:
	YARPNameID pid;
	int has_msg;
	int offset;
	int reply_pending;
	int failed;

public:
	/**
	 * Constructor.
	 */
	BlockReceiver() { Begin(); }

	/**
	 * Destructor.
	 * Added only to fight annoying gcc warning!
	 */
	virtual ~BlockReceiver() {}

	/**
	 * Starts using this instance for receiving. This method just initializes to
	 * null values.
	 */
	void Begin() { failed = 0; pid.invalidate(); has_msg = 0; offset = 0; reply_pending = 0; }

	/**
	 * Starts using this instance for receiving from a destination.
	 * @param id is the ID of the remote endpoint.
	 */
	void Begin(const YARPNameID& id) { failed = 0; pid = id; has_msg = 0; offset = 0; reply_pending = 0; }

	/**
	 * Wait for a new message to be available.
	 * @return 1 if something can be read.
	 */
	int Get();

	/**
	 * Wait for a new message to be available and copies it into the buffer.
	 * @param buffer is the pointer to the buffer.
	 * @len is the length of the buffer in bytes.
	 * @return 1 if something has been read.
	 */
	int Get(char *buffer, int len);

	/**
	 * Finishes using this class, and sends a reply to the remote peer if
	 * needed.
	 * @return 1 if successful.
	 */
	int End();

	/**
	 * Equivalent to Get() but virtual. Reads a buffer from the channel.
	 * @param buffer is the pointer to the buffer.
	 * @length is the length of the buffer in bytes.
	 * @return 1 if something has been read.
	 */
	virtual int Read(char *buffer, int length) { return Get(buffer,length);	}
};



#endif
