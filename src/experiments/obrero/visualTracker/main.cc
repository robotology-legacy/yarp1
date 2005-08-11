/*
* main.cc -- A DV/1394 capture utility
* Orginally Copyright (C) 2000 - 2002 Arne Schirmacher <arne@schirmacher.de>
* This portion rewritten by and Copyright (C) 2003 Dan Dennedy <dan@dennedy.org>
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
* Revision 1.5  2003/11/16 22:36:49  ddennedy
* run make pretty and remove deprecated strstream
*
* Revision 1.4  2003/10/14 05:52:05  ddennedy
* bugfixes, --guid, and --noavc improvements
*
* Revision 1.3  2003/09/26 18:33:32  ddennedy
* rework avc discovery and argv parsing order
*
* Revision 1.2  2003/09/26 14:11:38  ddennedy
* rawdv on stdin, smil-based duration
*
* Revision 1.1  2003/09/17 22:10:12  ddennedy
* major new features: namely interactive mode and AV/C control
*
*
*/

/** the dvgrab main program
 
    contains the main logic
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// C++ includes

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// C includes

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

// local includes

#include "io.h"
#include "dvgrab.h"

bool g_done = false;

void signal_handler( int sig )
{
	g_done = true;
}

int main( int argc, char *argv[] )
{
	fcntl( fileno( stderr ), F_SETFL, O_NONBLOCK );
	try
	{
		char c;
		DVgrab dvgrab( argc, argv );

		signal( SIGINT, signal_handler );
		signal( SIGTERM, signal_handler );
		signal( SIGHUP, signal_handler );

		if ( dvgrab.isInteractive() )
		{
			term_init();
			fprintf( stderr, "Going interactive. Press '?' for help.\n" );
			while ( !g_done )
			{
				dvgrab.status( );
				if ( ( c = term_read() ) != -1 )
					g_done = !dvgrab.execute( c );
			}
			term_exit();
		}
		else
		{
			dvgrab.startCapture();
			while ( !g_done )
				if ( dvgrab.done() )
					break;
			dvgrab.stopCapture();
		}
	}
	catch ( string s )
	{
		fprintf( stderr, "Error: %s\n", s.c_str() );
		fflush( stderr );
	}
	catch ( ... )
	{
		fprintf( stderr, "Error: unknown\n" );
		fflush( stderr );
	}

	fprintf( stderr, "\n" );
	return 0;
}
