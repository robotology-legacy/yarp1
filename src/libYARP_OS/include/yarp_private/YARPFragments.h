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
/// $Id: YARPFragments.h,v 1.3 2004-07-09 13:46:03 eshuy Exp $
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

class Fragments
{
protected:
	Fragments *next;

public:
	char *buffer;
	int len;
	int clen;
	int owned;

	Fragments()	{ buffer = NULL;  clen = len = 0;  owned = 0;  next = NULL;}

	Fragments(char *nbuffer, int nlen, int nowned) { buffer = nbuffer;  clen = len = nlen;  owned = nowned;  next = NULL; }

	virtual ~Fragments() { ClearChildren(); Clear(); }

	void Clear()
	{ 
		if (owned&&buffer!=NULL) delete[] buffer;
		buffer = NULL;  clen = len = 0;  owned = 0; 
	}

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

	//accident with search-and-replace
	void Take(Fragments& fragments)
	{
		buffer = fragments.buffer;
		len = fragments.len;
		clen = fragments.clen;
		owned = fragments.owned;
		fragments.buffer = NULL;
		fragments.Clear();
	}

	void SwitchOne(Fragments& fragments)
	{
		int minlen = fragments.len;
		Fragments tmp;
		tmp.Take(fragments);
		fragments.Take(*this);
		fragments.Require(minlen);
		Take(tmp);
	}

	void Switch(Fragments& fragments)
	{
		SwitchOne(fragments);
		Fragments *tmp = next;
		next = fragments.next;
		fragments.next = tmp;
	}

	char *GetBuffer() { return buffer; }
	int GetLength() { return clen; }
	int GetAllocatedLength() { return len; }
	int IsOwner() { return owned; }

	Fragments *GetNext() { return next; }

	Fragments *AddNext()
	{ 
		if (next==NULL) next = new Fragments;  
		ACE_ASSERT (next!=NULL); 
		return next;
	}

	void ClearChildren()
	{
		if (next!=NULL)
		{
			delete next;
			next = NULL;
		}
	}
};



template <class T>
void CopyToFragments(Fragments& Fragments, const T& t)
{
	Fragments.Require(sizeof(t));
	memcpy(Fragments.GetBuffer(),(char *)(&t),sizeof(t));
}

template <class T>
void CopyFromFragments(Fragments& Fragments, const T& t)
{
	ACE_ASSERT (Fragments.GetLength()==sizeof(t));
	memcpy((char *)(&t),Fragments.GetBuffer(),sizeof(t));
}


#endif
