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
/// $Id: RefCounted.h,v 1.1 2004-07-01 15:29:30 eshuy Exp $
///
///

#ifndef REFCOUNTED_H_INC
#define REFCOUNTED_H_INC

///
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

// Certain strategic operations on refcounted objects
// are made atomic

#include "YARPSemaphore.h"


/// Lazy boy!
#define Sema YARPSemaphore


#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
extern Sema refcounted_sema;
#endif

extern Sema allocation_sema;

///
/// this is just an "atomic" counter (add/sub operations are atomized).
///
class RefCounted
{
public:
	int ref_count;
#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
	Sema& mutex;
#else
	Sema mutex;
#endif

	int GetReferenceCount() { return ref_count; }
	RefCounted ();
	virtual ~RefCounted();
	void AddRef();

	// set refcount to zero without destruction
	void ZeroRef();
	void RemoveRef();
	virtual int Destroy();
	virtual void *Clone(int *needed = NULL);
	void Wait();
	void Post();
};


//#include <sys/osinfo.h>

// this is a ref counted buffer with ownership and a few util functions.
class Buffer : public RefCounted
{
public:
	char *memory;
	int len;
	int owned;

	char *GetRawBuffer() { return memory; }
	int GetLength() { return len; }
	int GetReferenceCount() { return ref_count; }
	
	Buffer();
	Buffer(int nlen);
	Buffer(char *n_memory, int n_len, int n_owned = 0);
	void Set(char *n_memory, int n_len, int n_owned = 0);
	virtual ~Buffer();
	virtual void *Clone(int *needed = NULL);
};


// Intended to be applied to RefCounted objects only
template <class T>
class CountedPtr
{
public:
	T *ptr;

	CountedPtr() { ptr = NULL; }
	~CountedPtr() { Reset(); }

	CountedPtr(T *nptr) { ptr = NULL; Take(nptr); }

	T *Ptr() { return ptr; }

	void Set(T *nptr)
	{
		Reset();
		ptr = nptr;
//		ACE_ASSERT (ptr!=NULL);
		ptr->AddRef();
	}

	void Take(T *nptr)
	{
		Reset();
		ptr = nptr;
	}

	void Reset()
	{ 
		if (ptr!=NULL) ptr->RemoveRef(); 
		ptr = NULL; 
	}

	void Switch(CountedPtr<T>& peer)
	{
		T *tmp = ptr;
		ptr = peer.ptr;
		peer.ptr = tmp;
	}

	void MakeIndependent()
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
