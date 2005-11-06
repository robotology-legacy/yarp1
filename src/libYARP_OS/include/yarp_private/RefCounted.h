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
/// $Id: RefCounted.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#ifndef REFCOUNTED_H_INC
#define REFCOUNTED_H_INC

/**
 * \file RefCounted.h This file contains certain operations on 
 * ref counted buffers.
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

// Certain strategic operations on refcounted objects
// are made atomic

#include <yarp/YARPSemaphore.h>


/// Lazy boy!
#define Sema YARPSemaphore


#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
extern Sema refcounted_sema;
#endif

extern Sema allocation_sema;

/**
 * RefCounted is just an "atomic" counter (add/sub operations are atomized).
 */
class RefCounted
{
public:
	int ref_count;
#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
	Sema& mutex;
#else
	Sema mutex;
#endif

	/**
	 * Gets the number of references to this object.
	 * @return the number of references.
	 */
	int GetReferenceCount() { return ref_count; }

	/**
	 * Defaul constructor.
	 */
	RefCounted ();

	/**
	 * Destructor.
	 */
	virtual ~RefCounted();

	/**
	 * Add a reference to the counted object.
	 */
	void AddRef();

	/**
	 * Sets refcount to zero without destruction.
	 */
	void ZeroRef();

	/**
	 * Removes a reference to the object.
	 */
	void RemoveRef();

	/**
	 * Destroys this object.
	 * @return always 1.
	 */
	virtual int Destroy();

	/**
	 * Clones the object. Tells whether the object can be cloned. 
	 * Is it still used?
	 * @param needed if it's not NULL then asks whether the object is to be cloned
	 * and returns true in the variable. If NULL does nothing.
	 * @return always NULL.
	 */
	virtual void *Clone(int *needed = NULL);

	/**
	 * Waits on a semaphore.
	 */
	void Wait();

	/** 
	 * Posts a semaphore.
	 */
	void Post();
};


/**
 * Buffer is a ref counted buffer with ownership and a few utility functions.
 */
class Buffer : public RefCounted
{
public:
	char *memory;
	int len;
	int owned;

	/**
	 * Gets the buffer pointer.
	 * @return a char pointer to the managed buffer.
	 */
	char *GetRawBuffer() { return memory; }

	/**
	 * Gets the size of the buffer.
	 * @return the size of the buffer in bytes.
	 */
	int GetLength() { return len; }

	/**
	 * Gets the number of references to the buffer.
	 * @return the number of references to the managed buffer.
	 */
	int GetReferenceCount() { return ref_count; }
	
	/**
	 * Default constructor.
	 */
	Buffer();

	/**
	 * Creates a buffer of size @a nlen.
	 * @param nlen the size of the buffer in bytes.
	 */
	Buffer(int nlen);

	/**
	 * Creates a counted buffer from an existing one. Memory is not copied.
	 * @param n_memory is the buffer (must be allocated).
	 * @param n_len the size of the buffer.
	 * @param n_owned ownership flag for destruction (use new/delete).
	 */
	Buffer(char *n_memory, int n_len, int n_owned = 0);

	/**
	 * Sets the buffer.
	 * @param n_memory is the buffer (must be allocated).
	 * @param n_len the size of the buffer.
	 * @param n_owned ownership flag for destruction (use new/delete).
	 */
	void Set(char *n_memory, int n_len, int n_owned = 0);

	/**
	 * Destructor.
	 */
	virtual ~Buffer();

	/**
	 * Clone method as for the RefCounted class.
	 * Copies the buffer into a new buffer and decrements the counter by one.
	 */
	virtual void *Clone(int *needed = NULL);
};

/**
 * CountedPtr is a template for counted objects intended to be applied 
 * to RefCounted objects only.
 */
template <class T>
class CountedPtr
{
public:
	T *ptr;

	/**
	 * Default constructor.
	 */
	CountedPtr() { ptr = NULL; }

	/**
	 * Destructor.
	 */
	~CountedPtr() { Reset(); }

	/**
	 * Builds a new counted object starting from a pointer
	 * to the internal buffer.
	 * @param nptr is the pointer to create the buffer from.
	 */
	CountedPtr(T *nptr) { ptr = NULL; Take(nptr); }

	/**
	 * Returns the internal pointer.
	 * @return the internal pointer to the buffer.
	 */
	T *Ptr() { return ptr; }

	/**
	 * Sets the internal pointer to the buffer provided.
	 * @param nptr the buffer to attach to. Adds a reference
	 * to the pointed object.
	 */
	void Set(T *nptr)
	{
		Reset();
		ptr = nptr;
//		ACE_ASSERT (ptr!=NULL);
		ptr->AddRef();
	}

	/**
	 * Attaches to the esisting object without incrementing
	 * the ref count.
	 * @param nptr is the object to attach to.
	 */
	void Take(T *nptr)
	{
		Reset();
		ptr = nptr;
	}

	/**
	 * Resets the object. Detaches from the pointed object and
	 * removes its reference to it.
	 */
	void Reset(void)
	{ 
		if (ptr!=NULL) ptr->RemoveRef(); 
		ptr = NULL; 
	}

	/**
	 * Switches pointers with a peer.
	 * @param peer is the peer object to switch ownership with.
	 */
	void Switch(CountedPtr<T>& peer)
	{
		T *tmp = ptr;
		ptr = peer.ptr;
		peer.ptr = tmp;
	}

	/**
	 * Detaches from the attached object by cloning the buffer.
	 */
	void MakeIndependent(void)
	{
		int needed;
		T *nptr;
		if (ptr!=NULL)
		{
			nptr = (T*)ptr->Clone(&needed);
			if (needed)
			{
//				ACE_ASSERT (nptr!=NULL);
				Take(nptr);
			}
		}
	}
};

#endif
