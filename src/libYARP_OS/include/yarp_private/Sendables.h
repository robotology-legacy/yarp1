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
/// $Id: Sendables.h,v 1.4 2004-08-21 17:53:46 gmetta Exp $
///
///
#ifndef SENDABLES_H_INC
#define SENDABLES_H_INC

/**
 * \file Sendables.h This file contains classes that help in managing lists of
 * Sendable objects.
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPList.h>
#include "yarp_private/Sendable.h"
#include <yarp/YARPSemaphore.h>

extern YARPSemaphore refcounted_sema;

/**
 * PSendable is a simple class that manages a pointer to a Sendable object.
 */
class PSendable
{
public:
	/** A pointer to a Sendable object. */
	Sendable *sendable;

	/** 
	 * Constructor. 
	 * @param nsendable is a pointer to a sendable to associate to this object.
	 */
	PSendable(Sendable *nsendable=NULL) { sendable = nsendable; }

	/**
	 * Gets the content of this object: i.e. the pointer to the sendable.
	 * @return a pointer to the Sendable object managed by this object.
	 */
	Sendable *Content() { return sendable; }

	/**
	 * Destructor.
	 */
	~PSendable()
	{
		if (sendable!=NULL)
		{
			//	  printf("!!!!!!!!!!!! PSendable DESTRUCTOR! %ld\n", (long int) sendable);
			delete sendable;
		}
		sendable = NULL;
	}

	int operator == (const PSendable& s) const { ACE_UNUSED_ARG(s); return 1; }
	int operator != (const PSendable& s) const { ACE_UNUSED_ARG(s); return 0; }
	int operator < (const PSendable& s) const { ACE_UNUSED_ARG(s); return 0; }
};


/**
 * Sendables is a list of PSendable objects.
 */
class Sendables
{
public:
	/** This is a YARPList of PSendable objects. */
	YARPList<PSendable> sendables;

	/**
	 * Adds a sendable to the list.
	 * @param s is the Sendable to be added to the list.
	 */
	void PutSendable(Sendable *s)
	{
		refcounted_sema.Wait();
		YARPList<PSendable>::iterator cursor(sendables);
		sendables.push_back(PSendable());

		ACE_ASSERT (s!=NULL);
		///ACE_ASSERT (sendables.begin() != sendables.end());

		cursor.go_tail();
		///cursor = sendables.end();
		///--cursor;
		(*cursor).sendable = s;
		s->owner = this;

		refcounted_sema.Post();
	}

	/**
	 * Adds a sendable to the list (calls PutSendable()).
	 * @param s is the Sendable to be added to the list.
	 */
	void TakeBack(Sendable *s)
	{
		PutSendable(s);
	}
  
	/**
	 * Gets the last sendable on the list.
	 * @return a pointer to a Sendable object. The object has been removed from
	 * the list.
	 */
	Sendable *GetSendable()
	{
		Sendable *s = NULL;
		refcounted_sema.Wait();
		YARPList<PSendable>::iterator cursor(sendables); //= sendables.end();
		cursor.go_tail();

		if (!sendables.is_empty()) //sendables.begin() != sendables.end())
		{
			//--cursor;
			s = (*cursor).sendable;
			(*cursor).sendable = NULL;
			sendables.pop_back();	
			ACE_ASSERT (s!=NULL);
			s->ref_count = 0;
		}

		refcounted_sema.Post();
		return s;
	}
};


/**
 * SendablesOf is a template class that manages a list of generic sendables.
 */
template <class T>
class SendablesOf : public Sendables
{
public:
	/**
	 * Adds a sendable to the list.
	 * @param s is the sendable to add to the list. It has the same type
	 * of the template argument T.
	 */
	void Put(T *s)
	{
		PutSendable(s);
	}

	/**
	 * Gets a sendable from the list.
	 * @return the last sendable from the list. It has the same type
	 * of the template argument T.
	 */
	T *Get()
	{
		T *t = (T*)GetSendable();
		if (t==NULL)
		{
			//T *buf = (T *)new char[sizeof(T)];
			//assert(buf!=NULL);
			//t = new (buf) T;
			t = new T;	
			ACE_ASSERT (t!=NULL);
			t->ZeroRef();
		}
		ACE_ASSERT (t!=NULL);
		t->owner = this;
		return t;
	}
};


#endif
