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
///	$Id: YARPThread.h,v 1.9 2003-06-11 16:40:07 gmetta Exp $
///
///
/*
	paulfitz Mon May 21 13:42:24 EDT 2001
*/

#ifndef YARPThread_INC
#define YARPThread_INC

/*
Ideally, would use POSIX semaphores, threads etc.
 */
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>
#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
///
///
///
class YARPThread
{
private:
	void *system_resource;
	int identifier;
	int size;

public:
	///
	///
	///
	YARPThread();
	YARPThread(const YARPThread& yt);
	virtual ~YARPThread();

	// Assertion fails if insufficient resources at initialization.
	// stack_size of zero means use default stack size

	/// Begin and End are now virtual, overridable.
	virtual void Begin(int stack_size=0);
	virtual void End(bool dontkill = false);
	virtual void Body() = 0; // this is the body of the thread

	int GetIdentifier() { return identifier; }

	int IsTerminated();
	// If you are in MS-Windows, you should call this
	// every now and then, and leave Body() if the result
	// is non-zero.  If you don't, you may be terminated
	// forceably with loss of memory and resources you are
	// holding.

	// why... it might be fixed by using ACE under WIN32. 

	// Forcibly halt all threads (late addition, just in QNX implementation)
	static void TerminateAll();

	static void PrepareForDeath();
	static int IsDying();
#ifdef __WIN32__
	friend static unsigned __stdcall ExecuteThread (void *args);
#else
	friend unsigned ExecuteThread (void *args);
#endif

	int GetPriority (void);
	int SetPriority (int prio);
};


///
/// ACE_TSS_Type_Adapter<int>
template <class T>
class YARPThreadSpecific : public ACE_TSS<ACE_TSS_Type_Adapter <T> >
{
public:
	YARPThreadSpecific() : ACE_TSS<ACE_TSS_Type_Adapter <T> >() {}
///	T& Content() { return (this->ts_object ()->operator T& ()); } 
	
	T& Content() { return ((*((ACE_TSS<ACE_TSS_Type_Adapter <T> > *)this))->operator T& ()); } 
	///**(ACE_TSS<ACE_TSS_Type_Adapter <T> > *)(this); } ///return (T&)(*this); }
};

#endif
