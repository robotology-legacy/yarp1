//
// $Id: RefCounted.cpp,v 1.2 2003-04-10 15:01:29 gmetta Exp $
//
//

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

#include "RefCounted.h"

#include <stdio.h>
#include <string.h>


#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
Sema refcounted_sema(1);
#endif
Sema allocation_sema(1);


///
/// ref counted.
///
RefCounted::RefCounted () : 
#ifdef SINGLE_MUTEX_FOR_REFCOUNTED
    mutex(refcounted_sema)
#else
    mutex(1)
#endif
{ 
	//printf("RefCounted CONSTRUCTOR\n");
	ref_count = 1; 
}

RefCounted::~RefCounted ()
{
	//printf("RefCounted DESTRUCTOR\n");
}

void RefCounted::AddRef ()
{
	//printf("Waitaddref\n");
	mutex.Wait();
	ref_count++;
	//printf("Increase to %d\n", ref_count);
	mutex.Post();
	//printf("Postaddref\n");
}

void RefCounted::ZeroRef ()
{
	//printf("Waitzero\n");
	mutex.Wait();
	//printf("Waitedzero\n");
	ref_count = 0;
	//printf("Postingzero\n");
	mutex.Post();
	//printf("Postzero\n");
}

void RefCounted::RemoveRef ()
{
	int destroyed = 0;
	
	//printf("Waitremoveref\n");
	mutex.Wait();
	ACE_ASSERT (ref_count > 0);
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

int RefCounted::Destroy ()
{
	delete this;
	return 1;
}

void *RefCounted::Clone (int *needed)
{
	//printf("Waitclone\n");
	mutex.Wait();
	if (needed!=NULL)
	{
		*needed = (GetReferenceCount() > 1);
	}
	mutex.Post();
	//printf("Postclone\n");
	return NULL;
}

void RefCounted::Wait ()
{
	//printf("Waitgeneric\n");
	mutex.Wait();
}

void RefCounted::Post ()
{
	mutex.Post();
	//printf("Postgeneric\n");
}


Buffer::Buffer()
{
	memory = NULL; owned = 0;
}

Buffer::Buffer(int nlen) 
{ 
	//_osinfo d;
	//qnx_osinfo(1,&d);
	//cout << "***** ALLOCATING " << d.freememk << endl;
	memory = new char[nlen];
	if (memory==NULL) 
	{
		ACE_OS::printf("Failed to allocate %d bytes\n", nlen);
		//	cout << "Failed to allocate " << nlen << " bytes" << endl;
	}

	ACE_ASSERT (memory != NULL);
	owned = 1;
	len = nlen; 
}

Buffer::Buffer(char *n_memory, int n_len, int n_owned)
{ 
	memory = n_memory;  
	len = n_len;  
	owned = n_owned; 
}


void Buffer::Set(char *n_memory, int n_len, int n_owned)
{ 
	ACE_ASSERT (memory == NULL);  
	memory = n_memory;  
	len = n_len;  
	owned = n_owned; 
}

Buffer::~Buffer()
{
	//cout << "Buffer DESTRUCTOR 1" << endl;
	if (memory != NULL && owned)
	{
		delete[] memory;
	}
	memory = NULL;
	//cout << "Buffer DESTRUCTOR 2" << endl;
}  

void *Buffer::Clone(int *needed)
{
	// Need to be careful to do everything atomically
	mutex.Wait();

	Buffer *ptr = NULL;
	int copy = (GetReferenceCount() > 1);

	if (needed != NULL)
	{
		*needed = copy;
	}

	if (copy)
	{
		ACE_ASSERT (memory != NULL);
		ptr = new Buffer(GetLength());
		ACE_ASSERT (ptr != NULL);
		memcpy(ptr->memory, memory, GetLength());
		ref_count--;
	}

	mutex.Post();
	return ptr;
}
