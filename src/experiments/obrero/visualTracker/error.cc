/*
* error.cc Error handling
* Copyright (C) 2000 - 2002 Arne Schirmacher <arne@schirmacher.de>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
* Tag: $Name: not supported by cvs2svn $
*
* Change log:
* 
* $Log: not supported by cvs2svn $
* Revision 1.4  2003/11/17 00:15:19  ddennedy
* bump version, remove a couple more strstream, fixup man page
*
* Revision 1.3  2003/11/16 22:36:49  ddennedy
* run make pretty and remove deprecated strstream
*
* Revision 1.2  2003/09/17 22:10:12  ddennedy
* major new features: namely interactive mode and AV/C control
*
* Revision 1.1.1.1  2003/02/07 04:08:33  ddennedy
* initial import of dvgrab
*
* Revision 1.1  2002/03/04 19:24:57  arne
* updated to latest Kino avi code
*
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// C includes

#include <errno.h>
#include <string.h>

// C++ includes

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using std::ostringstream;
using std::string;
using std::endl;
using std::ends;

// local includes

#include "error.h"

void real_fail_neg( int eval, const char *eval_str, const char *func, const char *file, int line )
{
	if ( eval < 0 )
	{
		string exc;
		ostringstream sb;

		sb << file << ":" << line << ": In function \"" << func << "\": \"" << eval_str << "\" evaluated to " << eval;
		if ( errno != 0 )
			sb << endl << file << ":" << line << ": errno: " << errno << " (" << strerror( errno ) << ")";
		sb << ends;
		exc = sb.str();
		throw exc;
	}
}


/** error handler for NULL result codes
 
    Whenever this is called with a NULL argument, it will throw an
    exception. Typically used with functions like malloc() and new().
 
*/

void real_fail_null( const void *eval, const char *eval_str, const char *func, const char *file, int line )
{
	if ( eval == NULL )
	{

		string exc;
		ostringstream sb;

		sb << file << ":" << line << ": In function \"" << func << "\": " << eval_str << " is NULL" << ends;
		exc = sb.str();
		throw exc;
	}
}


void real_fail_if( bool eval, const char *eval_str, const char *func, const char *file, int line )
{
	if ( eval == true )
	{

		string exc;
		ostringstream sb;

		sb << file << ":" << line << ": In function \"" << func << "\": condition \"" << eval_str << "\" is true";
		if ( errno != 0 )
			sb << endl << file << ":" << line << ": errno: " << errno << " (" << strerror( errno ) << ")";
		sb << ends;
		exc = sb.str();
		throw exc;
	}
}
