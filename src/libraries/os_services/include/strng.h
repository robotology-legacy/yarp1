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
/// $Id: strng.h,v 1.4 2003-04-22 09:06:36 gmetta Exp $
///
///

#ifndef STRNG_INC
#define STRNG_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>

#include <stdlib.h>
#include <string.h>
///#include <assert.h>

//#include "String.h"


class String
{
private:
	char *str;

public:
	String() { str = NULL; }
	String ( const char *val ) 
	{ 
		if (val!=NULL) 
			str = strdup ( val ); 
	    else 
			str = NULL; 
	}

	String ( const String& s ) 
	{
		str = NULL;
		operator=(s);
	}

	~String()
	{ 
		SetNull(); 
	}
	  
	void SetNull()
	{ 
		if ( str != NULL ) 
		{ 
			free ( str ); 
			str = NULL; 
		} 
	}

	operator const char *() const { return str; }
	char operator[] (int index) { return str[index]; }

	const String& operator = ( const char *val )
	{ 
		SetNull(); 
		if ( val != NULL ) 
		{
			str = strdup ( val ); ACE_ASSERT (str!=NULL);
		} 
		return *this; 
	}

	const String& operator = ( const String& s ) { return operator=(s.AsChars()); }
	int isEqual ( const String& val ) const	{ return !strcmp ( str, val ); }
	int operator == ( const String& val ) const { return !strcmp ( str, val ); }
	const char *AsChars() const { return str; }
	int length() const { return (str!=NULL) ? strlen(str) : 0; }

	void operator += ( const char *val )
	{ 
		String old;
		old.str = str;
		str = (char*)malloc(length()+strlen(val)+1);
		ACE_ASSERT (str!=NULL);
		strcpy(str,old.str);
		strcat(str,val);
	}
};

#endif
