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
/// $Id: RefCounted.cpp,v 1.1 2004-07-01 15:29:30 eshuy Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

#include "RefCounted.h"
#include "YARPString.h"

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

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
