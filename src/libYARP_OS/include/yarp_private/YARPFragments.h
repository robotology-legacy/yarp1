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
/// $Id: YARPFragments.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#ifndef FRAGMENTS_INC
#define FRAGMENTS_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPFragments.h definitions for a node of a list of counted buffers.
 * @see Buffer.
 */

/**
 * A class that contains a reference to another copy of itself (sort of).
 * A node for a list of counted buffers.
 */
class Fragments
{
protected:
	Fragments *next;

public:
	char *buffer;
	int len;
	int clen;
	int owned;

	/**
	 * Default constructor.
	 */
	Fragments()	{ buffer = NULL;  clen = len = 0;  owned = 0;  next = NULL;}

	/**
	 * Builds an object starting from an existing buffer.
	 * @param nbuffer is a pointer to a chunk of memory.
	 * @param nlen the size of the buffer.
	 * @param nowned tells whether the memory object is owned by this instance.
	 */
	Fragments(char *nbuffer, int nlen, int nowned) { buffer = nbuffer;  clen = len = nlen;  owned = nowned;  next = NULL; }

	/**
	 * Destructor.
	 */
	virtual ~Fragments() { ClearChildren(); Clear(); }

	/**
	 * Clears the variables associated with this object
	 * and if owned also the buffer.
	 */
	void Clear()
	{ 
		if (owned&&buffer!=NULL) delete[] buffer;
		buffer = NULL;  clen = len = 0;  owned = 0; 
	}

	/**
	 * Reallocates the buffer to be at least of length @a nlen.
	 * @param nlen is the new length of the buffer.
	 */
	void Require(int nlen)
	{
		if (nlen<1) nlen = 1;
		if (buffer!=NULL && nlen<=len)
		{
			clen = nlen;
		}
		else
		{
			Clear();
			buffer = new char[nlen];
			ACE_ASSERT (buffer!=NULL);
			len = nlen;
			clen = nlen;
			owned = 1;
		}
	}

	/**
	 * Takes an existing object.
	 * Accident with search-and-replace
	 * @param fragments is the object to take the buffer from.
	 */
	void Take(Fragments& fragments)
	{
		buffer = fragments.buffer;
		len = fragments.len;
		clen = fragments.clen;
		owned = fragments.owned;
		fragments.buffer = NULL;
		fragments.Clear();
	}

	/**
	 * Switches the current object with the one passed as
	 * argument by replacing it with a Take().
	 * @param fragments is a reference to the object to take.
	 */
	void SwitchOne(Fragments& fragments)
	{
		int minlen = fragments.len;
		Fragments tmp;
		tmp.Take(fragments);
		fragments.Take(*this);
		fragments.Require(minlen);
		Take(tmp);
	}

	/**
	 * Switches the current object with the one in argument.
	 * @param fragments is the reference to the object to
	 * exchange with.
	 */
	void Switch(Fragments& fragments)
	{
		SwitchOne(fragments);
		Fragments *tmp = next;
		next = fragments.next;
		fragments.next = tmp;
	}

	/**
	 * Gets the internal buffer.
	 * @return the pointer to the internal buffer.
	 */
	char *GetBuffer() { return buffer; }

	/**
	 * Gets the size of the buffer.
	 * @return the size of the buffer in bytes.
	 */
	int GetLength() { return clen; }

	/**
	 * Gets the size of the allocated memory.
	 * @return the length of the allocated memory buffer.
	 */
	int GetAllocatedLength() { return len; }

	/**
	 * Checks whether the buffer is owned.
	 * @return 1 if the buffer is owned by this object.
	 */
	int IsOwner() { return owned; }

	/**
	 * Gets the next object in the list.
	 * @return the pointer to the child object next in the list.
	 */
	Fragments *GetNext() { return next; }

	/**
	 * Adds an object next to this one.
	 * @return the pointer to the newly allocated object.
	 */
	Fragments *AddNext()
	{ 
		if (next==NULL) next = new Fragments;  
		ACE_ASSERT (next!=NULL); 
		return next;
	}

	/**
	 * Removes the children of this object, and clears
	 * the memory.
	 */
	void ClearChildren()
	{
		if (next!=NULL)
		{
			delete next;
			next = NULL;
		}
	}
};


/**
 * Copies a buffer of a generic type into a Fragments object.
 * @param Fragments is a reference to a Fragments object.
 * @param t is the buffer to copy from.
 */
template <class T>
void CopyToFragments(Fragments& Fragments, const T& t)
{
	Fragments.Require(sizeof(t));
	ACE_OS::memcpy(Fragments.GetBuffer(),(char *)(&t),sizeof(t));
}

/**
 * Copies the internal Fragments object into a buffer of a generic type.
 * @param Fragments is a reference to the Fragments object.
 * @param t is the buffer to copy to.
 */
template <class T>
void CopyFromFragments(Fragments& Fragments, const T& t)
{
	ACE_ASSERT (Fragments.GetLength()==sizeof(t));
	ACE_OS::memcpy((char *)(&t),Fragments.GetBuffer(),sizeof(t));
}


#endif
