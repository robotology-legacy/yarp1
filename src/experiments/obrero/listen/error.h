/*
* error.h Error handling
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

#ifndef _ERROR_H
#define _ERROR_H 1

#ifdef __cplusplus
extern "C"
{
#endif

#define fail_neg(eval)  real_fail_neg  (eval, #eval, __ASSERT_FUNCTION, __FILE__, __LINE__)
#define fail_null(eval) real_fail_null (eval, #eval, __ASSERT_FUNCTION, __FILE__, __LINE__)
#define fail_if(eval)   real_fail_if   (eval, #eval, __ASSERT_FUNCTION, __FILE__, __LINE__)

	void real_fail_neg ( int eval, const char * eval_str, const char * func, const char * file, int line );
	void real_fail_null ( const void * eval, const char * eval_str, const char * func, const char * file, int line );
	void real_fail_if ( bool eval, const char * eval_str, const char * func, const char * file, int line );

#ifdef __cplusplus
}
#endif

#endif
