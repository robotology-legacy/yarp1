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
/// $Id: YARPPortContent.h,v 1.2 2003-04-18 09:25:48 gmetta Exp $
///
///
/*
	paulfitz Sat May 26 22:27:07 EDT 2001
*/
#ifndef YARPPortContent_INC
#define YARPPortContent_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
///
///
class YARPPortReader
{
public:
	virtual int Read(char *buffer, int length) = 0;
};

class YARPPortWriter
{
public:
	virtual int Write(char *buffer, int length) = 0;
};

/*
  Instances of this class know how to read or write themselves, and are
  can be kept in a pool of objects that circulate from the user to the 
  communications code and back to the user.  Necessary for efficiency
  when transmitting to multiple targets that read data at different rates.
 */


template <class T>
struct HierarchyRoot
{
	// HierarchyId is a nested class
	struct HierarchyId {};
};

class YARPPortContent : public HierarchyRoot<YARPPortContent>
{
public:
	virtual int Read(YARPPortReader& reader) = 0;
	virtual int Write(YARPPortWriter& writer) = 0;

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() = 0;
};


template <class T>
class YARPPortContentOf : public YARPPortContent
{
public:
	T datum;

	T& Content() { return datum; }
	virtual int Read(YARPPortReader& reader) { return reader.Read((char*)(&datum),sizeof(datum)); }
	virtual int Write(YARPPortWriter& writer) { return writer.Write((char*)(&datum),sizeof(datum)); }
	virtual int Recycle() { return 0; }
};

#endif
