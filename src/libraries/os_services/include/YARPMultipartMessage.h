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
/// $Id: YARPMultipartMessage.h,v 1.3 2003-05-12 23:32:43 gmetta Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPMultipartMessage_INC
#define YARPMultipartMessage_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

//#include <stdlib.h>

///
/// This class represents a packetized message. I imagine this WAS needed for QNX4.
///	it makes little sense on NT sockets...
///
class YARPMultipartMessage
{
protected:
	void *system_resource;
	int top_index, length, owned;

	/// topindex is the last part that has been set (allocated).
	///	length is the number of parts.
	/// owned, whether the memory is owned by the class and needs to be freed.
public:
	YARPMultipartMessage() 
	{ 
		system_resource = NULL; 
		top_index = -1; 
		length = -1; 
		owned = 1; 
	}

	YARPMultipartMessage(int n_length)
	{ 
		system_resource = NULL; 
		top_index = -1; 
		length = -1; 
		Resize(n_length);  
		owned = 1; 
	}

	YARPMultipartMessage(void *buffer, int entries)
	{ 
		owned = 0;  
		system_resource = buffer;  
		top_index = entries-1;
		length = entries; 
	}

	virtual ~YARPMultipartMessage();

	void Resize(int n_length);

	void Set(int index, char *buffer, int buffer_length);
	char *GetBuffer(int index);
	int GetBufferLength(int index);

	void Reset();

	int GetParts()       { return top_index+1; }
	void *GetRawBuffer() { return system_resource; }
};


#endif
