//
// $Id: RefCounted.h,v 1.2 2003-04-10 15:01:34 gmetta Exp $
//
//

#ifndef REFCOUNTED_H_INC
#define REFCOUNTED_H_INC

///
#include <conf/YARPConfig.h>

#include <stdio.h>
//#include <iostream>
//#include <assert.h>

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
///
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
