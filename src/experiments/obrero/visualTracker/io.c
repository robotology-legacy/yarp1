/*
* io.c -- dv1394d client demo input/output
* Copyright (C) 2002-2003 Ushodaya Enterprises Limited
* Author: Charles Yates <charles.yates@pandora.be>
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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* System header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

/* Application header files */
#include "io.h"

char *chomp( char *input )
{
	if ( input != NULL )
	{
		int length = strlen( input );
		if ( length && input[ length - 1 ] == '\n' )
			input[ length - 1 ] = '\0';
		if ( length > 1 && input[ length - 2 ] == '\r' )
			input[ length - 2 ] = '\0';
	}
	return input;
}

char *trim( char *input )
{
	if ( input != NULL )
	{
		int length = strlen( input );
		int first = 0;
		while ( first < length && isspace( input[ first ] ) )
			first ++;
		memmove( input, input + first, length - first + 1 );
		length = length - first;
		while ( length > 0 && isspace( input[ length - 1 ] ) )
			input[ -- length ] = '\0';
	}
	return input;
}

char *strip_quotes( char *input )
{
	if ( input != NULL )
	{
		char * ptr = strrchr( input, '\"' );
		if ( ptr != NULL )
			* ptr = '\0';
		if ( input[ 0 ] == '\"' )
			strcpy( input, input + 1 );
	}
	return input;
}

char *get_string( char *output, int maxlength, char *use )
{
	char * value = NULL;
	strcpy( output, use );
	if ( trim( chomp( fgets( output, maxlength, stdin ) ) ) != NULL )
	{
		if ( !strcmp( output, "" ) )
			strcpy( output, use );
		value = output;
	}
	return value;
}

int *get_int( int *output, int use )
{
	int * value = NULL;
	char temp[ 132 ];
	*output = use;
	if ( trim( chomp( fgets( temp, 132, stdin ) ) ) != NULL )
	{
		if ( strcmp( temp, "" ) )
			* output = atoi( temp );
		value = output;
	}
	return value;
}

/** This stores the previous settings
*/

static struct termios oldtty;
static int mode = 0;

/** This is called automatically on application exit to restore the
	previous tty settings.
*/

void term_exit( void )
{
	if ( mode == 1 )
	{
		tcsetattr( 0, TCSANOW, &oldtty );
		mode = 0;
	}
}

/** Init terminal so that we can grab keys without blocking.
*/

void term_init( )
{
	struct termios tty;

	if ( isatty( fileno( stdin ) ) )
	{
		tcgetattr( 0, &tty );
		oldtty = tty;

		tty.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON );
		tty.c_oflag |= OPOST;
		tty.c_lflag &= ~( ECHO | ECHONL | ICANON | IEXTEN );
		tty.c_cflag &= ~( CSIZE | PARENB );
		tty.c_cflag |= CS8;
		tty.c_cc[ VMIN ] = 1;
		tty.c_cc[ VTIME ] = 0;

		tcsetattr( 0, TCSANOW, &tty );

		mode = 1;

		atexit( term_exit );
	}
	else
	{
		fcntl( fileno( stdin ), F_SETFL, O_NONBLOCK );
	}
}

/** Check for a keypress without blocking infinitely.
	Returns: ASCII value of keypress or -1 if no keypress detected.
*/

int term_read( )
{
	int n = 1;
	unsigned char ch;
	struct timeval tv;
	fd_set rfds;

	FD_ZERO( &rfds );
	FD_SET( 0, &rfds );
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	n = select( 1, &rfds, NULL, NULL, &tv );
	if ( n > 0 )
	{
		n = read( 0, &ch, 1 );
		if ( isatty( fileno( stdin ) ) )
			tcflush( 0, TCIFLUSH );
		if ( n == 1 )
			return ch;
		return n;
	}
	return -1;
}

char get_keypress( )
{
	char value = '\0';
	int pressed = 0;

	fflush( stderr );

	term_init( );
	while ( ( pressed = term_read( ) ) == -1 )
		;
	term_exit( );

	value = ( char ) pressed;

	return value;
}

void wait_for_any_key( char *message )
{
	if ( message == NULL )
		printf( "Press any key to continue: " );
	else
		printf( "%s", message );

	get_keypress( );

	printf( "\n\n" );
}

void beep( )
{
	printf( "%c", 7 );
	fflush( stderr );
}
