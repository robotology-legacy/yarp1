
#ifndef REFCOUNTED_H_INC
#define REFCOUNTED_H_INC

#include <stdio.h>
//#include <iostream>
#include <assert.h>

// Certain strategic operations on refcounted objects
// are made atomic

#include "YARPSemaphore.h"

#define Sema YARPSemaphore

#define SINGLE_MUTEX_FOR_REFCOUNTED


#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
extern Sema refcounted_sema;
#endif

extern Sema allocation_sema;

class RefCounted
{
public:
  int ref_count;
#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
 Sema& mutex;
#else
 Sema mutex;
#endif

  int GetReferenceCount()
    { return ref_count; }

  RefCounted() : 
#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
    mutex(refcounted_sema)
#else
    mutex(1)
#endif
    { 
      //printf("RefCounted CONSTRUCTOR\n");
      ref_count = 1; 
    }

  virtual ~RefCounted()
    {
      //printf("RefCounted DESTRUCTOR\n");
    }

  void AddRef()
    {
      //printf("Waitaddref\n");
      mutex.Wait();
      ref_count++;
      //printf("Increase to %d\n", ref_count);
      mutex.Post();
      //printf("Postaddref\n");
    }

  // set refcount to zero without destruction
  void ZeroRef()
    {
      //printf("Waitzero\n");
      mutex.Wait();
      //printf("Waitedzero\n");
      ref_count = 0;
      //printf("Postingzero\n");
      mutex.Post();
      //printf("Postzero\n");
    }

  void RemoveRef()
    {
      int destroyed = 0;
      //printf("Waitremoveref\n");
      mutex.Wait();
      assert(ref_count>0);
      ref_count--;
      //printf("Decrease to %d\n", ref_count);
      if (ref_count==0)
	{
	  mutex.Post();
	  //printf("Postremoveref\n");
	  destroyed = Destroy();
	}
      else
	{
	  mutex.Post();
	  //printf("Postremoveref\n");
	}
      /*
	#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
	mutex.Post();
	#else
	if (!destroyed)
	{
	mutex.Post();
	}
	#endif
      */
    }

  virtual int Destroy()
    {
      delete this;
      return 1;
    }

  virtual void *Clone(int *needed = NULL)
    {
      //printf("Waitclone\n");
      mutex.Wait();
      if (needed!=NULL)
	{
	  *needed = (GetReferenceCount()>1);
	}
      mutex.Post();
      //printf("Postclone\n");
      return NULL;
    }

  void Wait()
    {
      //printf("Waitgeneric\n");
      mutex.Wait();
    }

  void Post()
    {
      mutex.Post();
      //printf("Postgeneric\n");
    }
};


//#include <sys/osinfo.h>

class Buffer : public RefCounted
{
public:
  char *memory;
  int len;
  int owned;

  char *GetRawBuffer()
    { return memory; }

  int GetLength()
    { return len; }

  int GetReferenceCount()
    { return ref_count; }

  Buffer()
    {
      memory = NULL; owned = 0;
    }

  Buffer(int nlen) 
    { 
      //_osinfo d;
      //qnx_osinfo(1,&d);
      //cout << "***** ALLOCATING " << d.freememk << endl;
      memory = new char[nlen];
      if (memory==NULL) {
	printf("Failed to allocate %d bytes\n", nlen);
	//	cout << "Failed to allocate " << nlen << " bytes" << endl;
      }
      assert(memory!=NULL);
      owned = 1;
      len = nlen; }

  Buffer(char *n_memory, int n_len, int n_owned = 0)
    { memory = n_memory;  len = n_len;  owned = n_owned; }


  void Set(char *n_memory, int n_len, int n_owned = 0)
    { 
      assert(memory==NULL);  memory = n_memory;  len = n_len;  
      owned = n_owned; 
    }

  virtual ~Buffer()
    {
      //cout << "Buffer DESTRUCTOR 1" << endl;
      if (memory!=NULL && owned)
	{
	  delete[] memory;
	}
      memory = NULL;
      //cout << "Buffer DESTRUCTOR 2" << endl;
    }  

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

  CountedPtr(T *nptr) { ptr = NULL;  Take(nptr); }

  T *Ptr() { return ptr; }

  void Set(T *nptr)
    {
      Reset();
      ptr = nptr;
      assert(ptr!=NULL);
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
	      assert(nptr!=NULL);
	      Take(nptr);
	    }
	}
    }
};

#endif
