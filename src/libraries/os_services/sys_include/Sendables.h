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
/// $Id: Sendables.h,v 1.3 2003-04-18 09:25:49 gmetta Exp $
///
///
#ifndef SENDABLES_H_INC
#define SENDABLES_H_INC

#include <conf/YARPConfig.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <list>
#ifndef __QNX__
using namespace std;
#endif

#include <assert.h>

#include "Sendable.h"
#include "YARPSemaphore.h"

extern YARPSemaphore refcounted_sema;


class PSendable
{
public:
	Sendable *sendable;
	PSendable(Sendable *nsendable=NULL) { sendable = nsendable; }

	Sendable *Content() { return sendable; }

	~PSendable()
	{
		if (sendable!=NULL)
		{
			//	  printf("!!!!!!!!!!!! PSendable DESTRUCTOR! %ld\n", (long int) sendable);
			delete sendable;
		}
		sendable = NULL;
	}

	int operator == (const PSendable& s) const { return 1; }
	int operator != (const PSendable& s) const { return 0; }
	int operator < (const PSendable& s) const { return 0; }
};

class Sendables
{
public:
	list<PSendable> sendables;

	void PutSendable(Sendable *s)
    {
		refcounted_sema.Wait();
		list<PSendable>::iterator cursor;
		//cout << "*** NEW stl " << __FILE__ << ":" << __LINE__ << endl;
		//sendables.insert(sendables.begin(),PSendable());
		//printf("PutSendable() > 1\n");
		sendables.push_back(PSendable());
		assert(s!=NULL);
		//printf("PutSendable() > 2\n");
		//      sendables.insert(sendables.begin(),*s);
		assert(sendables.begin() != sendables.end());
		//printf("PutSendable() > 3\n");
		cursor = sendables.end();
		--cursor;
		//printf("PutSendable() > 4\n");
		(*cursor).sendable = s;
		s->owner = this;
		//printf("PutSendable() > 1\n");
		refcounted_sema.Post();
    }

	void TakeBack(Sendable *s)
    {
		//printf("##### Take back called for %ld (%d)\n", (long int) s, s->ref_count);
		PutSendable(s);
		//printf("##### POST Take back called for %ld (%d)\n", (long int) s, s->ref_count);
    }
  
	Sendable *GetSendable()
    {
		Sendable *s = NULL;
		refcounted_sema.Wait();
		list<PSendable>::iterator cursor = sendables.end();
		if (sendables.begin() != sendables.end())
		{
			--cursor;
			s = (*cursor).sendable;
			//printf("### %d for %d\n", s->ref_count, (long int)s);
			(*cursor).sendable = NULL;
			sendables.pop_back();	
			assert(s!=NULL);
			//printf("### %d for %d\n", s->ref_count, (long int)s);
			s->ref_count = 0;
		}
		refcounted_sema.Post();
		return s;
    }
};

template <class T>
class SendablesOf : public Sendables
{
public:
	void Put(T *s)
	{
		PutSendable(s);
	}

	T *Get()
	{
		T *t = (T*)GetSendable();
		if (t==NULL)
		{
			//T *buf = (T *)new char[sizeof(T)];
			//assert(buf!=NULL);
			//t = new (buf) T;
			t = new T;	
			assert(t!=NULL);
			t->ZeroRef();
		}
		assert(t!=NULL);
		t->owner = this;
		return t;
	}
};


#endif
