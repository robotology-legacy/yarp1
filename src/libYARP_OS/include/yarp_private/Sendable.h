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
/// $Id: Sendable.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#ifndef SENDABLE_H_INC
#define SENDABLE_H_INC

/**
 * \file Sendable.h It contains definition of classes, methods, and various
 * other things helpful in defining objects that can be sent across a network
 * using ports. These are the basic types used by port code when passing
 * messages around between the user and library code.
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "yarp_private/BlockSender.h"
#include "yarp_private/BlockReceiver.h"
#include "yarp_private/RefCounted.h"

class Sendables;

/**
 * A Sendable is in short a RefCounted object that knows how
 * to read or write itself on a network. It contains a Read() and Write()
 * method that internally calls sender/receiver functions which
 * in turn call YARPSyncComm code that in turn talks to the socket code.
 * @see BlockSender, BlockReceiver.
 */
class Sendable : public RefCounted
{
public:
	/** The list of sendables owning this object */
	Sendables *owner;

	/** Default constructor. */
	Sendable() { owner = NULL; }

	/** Destructor */
	virtual ~Sendable() {}

	/** 
	 * Writes the content of the sendable to the network. This is an empty
	 * implementation.
	 * @param sender is a reference to a sender object which knows how to
	 * talk to the network layer.
	 * @return always returns 1.
	 */
	virtual int Write(BlockSender& sender) { ACE_UNUSED_ARG(sender); return 1; }

	/** 
	 * Reads the content of the sendable from the network. This is an empty
	 * implementation.
	 * @param receiver is a reference to a receiver object which knows how to
	 * talk to the network layer.
	 * @return always returns 1.
	 */
	virtual int Read(BlockReceiver& receiver) { ACE_UNUSED_ARG(receiver); return 1; }

	/**
	 * Destroys the object and informs the owner.
	 * @return 1 if the object has been destroyed (and this deleted).
	 */
	virtual int Destroy();
};

#define Receivable Sendable
#define Gossip Sendable

/**
 * SimpleSendable is a template that implements the sendable for types
 * that allows a simple serialization.
 */
template <class T>
class SimpleSendable : public Sendable
{
public:
	/** The content of the Sendable */
	T datum;

	/** 
	 * Gets a reference to the internal data.
	 * @return a reference to the internal data of the generic type T.
	 */
	T& Content() { return datum; }

	/** 
	 * Writes the content of the sendable to the network. Uses sizeof() to
	 * get the size of the object.
	 * @param sender is a reference to a BlockSender object which knows how to
	 * talk to the network layer.
	 * @return always returns 1.
	 */
	virtual int Write(BlockSender& sender)
	{
		sender.Add((char*)(&datum),sizeof(datum));
		return 1;
	}

	/** 
	 * Reads the content of the sendable from the network. Uses sizeof() to 
	 * get the size of the object.
	 * @param receiver is a reference to a BlockReceiver object which knows how to
	 * talk to the network layer.
	 * @return always returns 1.
	 */
	virtual int Read(BlockReceiver& receiver)
	{
		receiver.Get((char*)(&datum),sizeof(datum));
		return 1;
	}
};


/** Maximum length of the simple text messages */
#define MAX_SIMPLE_TEXT 1000

typedef char SimpleTextMsg[MAX_SIMPLE_TEXT];

/**
 * TextSendable is a simple sendable for text messages of a certain maximum length.
 * This is a specialization of the SimpleSendable template.
 */
class TextSendable : public SimpleSendable<SimpleTextMsg>
{
public:
	/** 
	 * Writes the content of the sendable to the network.
	 * @param sender is a reference to a BlockSender object which knows how to
	 * talk to the network layer.
	 * @return always returns 1.
	 */
	virtual int Write(BlockSender& sender)
	{
		sender.Add((char*)(Content()),1+ACE_OS::strlen(Content()));
		return 1;
	}
};

#endif
