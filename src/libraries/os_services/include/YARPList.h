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
/// YARPList wrapped by pasa.
///

///
/// $Id: YARPList.h,v 1.1 2003-08-02 07:46:14 gmetta Exp $
///
///

#ifndef __YARPListh__
#define __YARPListh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Containers_T.h>


#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

template <class T>
class YARPListIterator : public ACE_DLList_Iterator<T>
{
public:
	YARPListIterator(ACE_DLList<T>& i) : ACE_DLList_Iterator<T>(i) {}
	YARPListIterator(const YARPListIterator& i) : ACE_DLList_Iterator<T>(i.dllist_) {}
	~YARPListIterator() {}

	int go_head(void) { return ACE_DLList_Iterator<T>::go_head(); }
	int go_tail(void) { return ACE_DLList_Iterator<T>::go_tail(); }

	T& operator *() const { return *(T*)(ACE_DLList_Iterator<T>::operator*().item_); }
};

template <class T>
class YARPList : public ACE_DLList<T>
{
public:
	friend class YARPListIterator<T>;

	YARPList(void) : ACE_DLList<T> () {}
	YARPList(const YARPList<T>& l) : ACE_DLList<T> (l) {}
	
	void operator= (const ACE_DLList<T> &l) { ACE_DLList<T>::operator=(l); } 
	void operator= (const YARPList<T> &l) { ACE_DLList<T>::operator=(l); } 

	~YARPList() { reset(); }

	void push_back (const T& new_item) { T* el = new T; *el = new_item; insert_tail(el); }
	void push_front (const T& new_item) { T* el = new T; *el = new_item; insert_head(el); }
	void pop_back (void) { T* el = delete_tail(); delete el; }
	void pop_front (void) { T* el = delete_head(); delete el; }

	typedef YARPListIterator<T> iterator;
};

#endif