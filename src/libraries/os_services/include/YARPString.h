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
///	YARPString wrapper by pasa.
///

///
/// $Id: YARPString.h,v 1.4 2003-08-02 07:46:14 gmetta Exp $
///
///

#ifndef YARPString_INC
#define YARPString_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <iostream>

#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#ifndef __QNX4__
/// WINDOWS/LINUX/QNX6

#include <ace/OS.h>
#include <ace/String_Base.h>

class YARPString : public ACE_String_Base<char>
{
public:
	YARPString () : ACE_String_Base<char> () {}
	YARPString (const char *s) : ACE_String_Base<char> (s) {}
	YARPString (const char *s, size_t len) : ACE_String_Base<char> (s, len) {}
	YARPString (const ACE_String_Base<char>& s) : ACE_String_Base<char> (s) {}
	YARPString (const YARPString& s) : ACE_String_Base<char> (s) {}
	YARPString (char c) : ACE_String_Base<char> (c) {}
	YARPString (size_t len, char c = 0) : ACE_String_Base<char> (len, c) {}

	~YARPString () { clear(1); }

	YARPString& operator= (const YARPString& s) { ACE_String_Base<char>::operator= (s); return *this; }
	
	YARPString& append(const char *s) { ACE_String_Base<char>::operator+=(s); return *this; }
	YARPString& append(const YARPString& s) { ACE_String_Base<char>::operator+=(s); return *this; }
	bool empty (void) { return (length() == 0) ? true : false; }
};

///
inline std::ostream& operator<< (std::ostream& os, YARPString& s) { os << s.c_str(); return os; }
inline std::ostream& operator<< (std::ostream& os, const YARPString& s) { os << s.c_str(); return os; }
inline std::istream& operator>> (std::istream& is, YARPString& s) 
{
	char _buf[1024]; memset (_buf, 0, 1024);
	is.getline(_buf, 1024, '\n');
	s = _buf; 
	return is; 
}

#else

#include "strng.h"
#define string String
#define c_str AsChars

#endif

///typedef std::string YARPString;

#endif
