/*
* dvgrab.cc -- DVGrab control class
*
* Hooks added for sidetap by Paul Fitzpatrick <paulfitz@ai.mit.edu>
* on 4 June 2004 (marked with keyword VIDEOBOX)
*
* Copyright (C) 2003 Dan Dennedy <dan@dennedy.org>
* Major rewrite of code based upon older versions of dvgrab by Arne Schirmacher
*    and some Kino code also contributed by Charles Yates.
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
* Revision 1.2  2005/03/20 03:45:05  eshuy
* *** empty log message ***
*
* Revision 1.1  2005/03/15 20:23:40  eshuy
* initial checking
*
* Revision 1.21  2004/01/14 13:46:29  ddennedy
* bugfix --every
*
* Revision 1.20  2003/12/30 05:47:24  ddennedy
* bugfixes to autosplit, added timecode discontinuity to autosplit, fix setting of a good sample frame, revert OpenDML warning on dv1
*
* Revision 1.19  2003/12/27 20:07:22  schirmacher
* bugfixes for DVGRAB-17, DVGRAB-19 and partially DVGRAB-16
* (capture aborted for files larger than 1008 MByte)
*
* Revision 1.18  2003/11/25 06:21:22  ddennedy
* make work in camera mode
*
* Revision 1.17  2003/11/16 22:36:49  ddennedy
* run make pretty and remove deprecated strstream
*
* Revision 1.16  2003/11/16 22:06:20  ddennedy
* allow interruption during initialization of start capture
*
* Revision 1.15  2003/10/14 05:59:36  ddennedy
* bugfixes, --guid, and --noavc improvements
*
* Revision 1.14  2003/10/14 05:52:05  ddennedy
* bugfixes, --guid, and --noavc improvements
*
* Revision 1.13  2003/10/09 19:42:02  ddennedy
* ring terminal bell on frame drop
*
* Revision 1.12  2003/10/09 19:06:18  ddennedy
* fix --opendml default
*
* Revision 1.11  2003/10/09 19:02:48  ddennedy
* little fix on smil time doc, set default --frames to 0, documemt --frames 0 = ulimited
*
* Revision 1.10  2003/10/08 21:31:12  ddennedy
* added --noavc option
*
* Revision 1.9  2003/10/08 04:23:26  ddennedy
* make duration work with non-file capture
*
* Revision 1.8  2003/09/27 01:30:54  ddennedy
* require --stdin to read from pipe
*
* Revision 1.7  2003/09/26 18:33:32  ddennedy
* rework avc discovery and argv parsing order
*
* Revision 1.6  2003/09/26 14:11:38  ddennedy
* rawdv on stdin, smil-based duration
*
* Revision 1.5  2003/09/17 22:10:12  ddennedy
* major new features: namely interactive mode and AV/C control
*
* Revision 1.4  2003/04/21 17:14:00  ddennedy
* result return on WriteFrame methods, auto set OpenDML where needed
*
* Revision 1.3  2003/02/23 02:02:12  ddennedy
* added raw to stdout, updated docs
*
* Revision 1.2  2003/02/07 04:25:15  ddennedy
* bugfix processing jpeg options
*
* Revision 1.4  2002/03/25 21:34:25  arne
* Support for large (64 bit) files mostly completed
*
* Revision 1.3  2002/03/10 21:28:29  arne
* release 1.1b1, 64 bit support for type 1 avis
*
* Revision 1.2  2002/03/04 19:22:43  arne
* updated to latest Kino avi code
*
* Revision 1.1.1.1  2002/03/03 19:08:08  arne
* import of version 1.01
*
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
using std::cerr;
using std::endl;

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/select.h>
#include <libavc1394/avc1394.h>
#include <libavc1394/avc1394_vcr.h>
#include <libavc1394/rom1394.h>

#include "error.h"
#include "riff.h"
#include "avi.h"
#include "frame.h"
#include "dvgrab.h"
#include "raw1394util.h"

// VIDEOBOX PATCH BEGINS MOD002
#include "sidetap.h"
#include "assert.h"
// VIDEOBOX PATCH ENDS

extern bool g_done;
bool m_log = false;

pthread_mutex_t DVgrab::capture_mutex;
pthread_t DVgrab::capture_thread;
Frame *DVgrab::m_frame;
FileHandler *DVgrab::m_writer;

DVgrab::DVgrab( int argc, char *argv[] ) :
		m_port( -1 ), m_node( -1 ), m_reader_active( false ), m_autosplit( false ),
		m_timestamp( false ), m_channel( 63 ), m_frame_count( 0 ), m_max_file_size( 1000 ),
		m_file_format( AVI_DV2_FORMAT ), m_open_dml( false ), m_frame_every( 1 ),
		m_dv1394( NULL ), m_jpeg_quality( 75 ),
		m_jpeg_deinterlace( false ), m_jpeg_width( -1 ), m_jpeg_height( -1 ),
		m_jpeg_overwrite( false ), m_captureActive( false ), m_avc( NULL ), m_reader( NULL ),
		m_dropped_frames( 0 ), m_interactive( false ), m_buffers( 100 ), m_total_frames( 0 ),
		m_duration( "" ), m_timeDuration( NULL ), m_stdin( false ), m_noavc( false ),
		m_guid( 0 )
{
	m_frame = NULL;
	m_writer = NULL;
	m_dst_file_name = "dvgrab-";

	getargs( argc, argv );

	if ( m_stdin )
		// if reading stdin, make sure its not a tty!
		m_stdin = ( ! isatty( fileno( stdin ) ) && ! m_interactive );

	if ( ! m_stdin && ( ! m_noavc || m_port == -1 ) )
		m_node = discoverAVC( &m_port, m_guid );

	if ( ( m_interactive || ! m_stdin ) && ( ! m_noavc && m_node == -1 ) )
		throw string( "no camera exists" );

	pthread_mutex_init( &capture_mutex, NULL );
	if ( m_port != -1 )
	{
		if ( ! m_noavc )
		{
			m_avc = new AVC( m_port );
			if ( m_avc == NULL )
				throw( "failed to initialize AV/C" );
			m_avc->Pause( m_node );
		}

		if ( m_dv1394 != NULL )
		{
			if ( strcmp( m_dv1394, "" ) == 0 && m_port != -1 )
			{
				char devfs[ 32 ];
				snprintf( devfs, 32, "/dev/ieee1394/dv/host%d/PAL/in", m_port );
				free( m_dv1394 );
				m_dv1394 = strdup( devfs );
			}
			m_reader = new dv1394Reader( m_dv1394, m_channel, m_buffers );
		}
		else
		{
			m_reader = new raw1394Reader( m_port, m_channel, m_buffers );
		}
	}
	else if ( m_stdin )
	{
		m_reader = new pipeReader( m_buffers );
	}
	else
		throw( "invalid DV source specified" );

	if ( m_reader )
	{
		pthread_create( &capture_thread, NULL, captureThread, this );
		m_reader->StartThread();
	}
}

DVgrab::~DVgrab()
{
	stopCapture();
	if ( m_avc )
		m_avc->Stop( m_node );
	m_reader_active = false;
	if ( m_reader )
	{
		m_reader->TriggerAction( );
		pthread_join( capture_thread, NULL );
		m_reader->StopThread();
		delete m_reader;
	}
	if ( m_avc )
		delete m_avc;
	if ( m_timeDuration )
		delete m_timeDuration;
	if ( m_dv1394 )
		free( m_dv1394 );
}

static void usage()
{
	cerr << "Usage: dvgrab [options] [file]\n";
	cerr << "Try dvgrab --help for more information\n";
}

void DVgrab::getargs( int argc, char *argv[] )
{
	for ( int i = 1; i < argc; ++i )
	{
		if ( strcmp( "--autosplit", argv[ i ] ) == 0 )
		{
			m_autosplit = true;
		}
		else if ( strcmp( "-i", argv[ i ] ) == 0 || strcmp( "--interactive", argv[ i ] ) == 0 )
		{
			m_interactive = true;
		}
		else if ( strcmp( "--timestamp", argv[ i ] ) == 0 )
		{
			m_timestamp = true;
		}
		else if ( strcmp( "--log", argv[ i ] ) == 0 )
		{
			m_log = true;
		}
		else if ( ( strcmp( "--format", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( strcmp( "dv1", argv[ i ] ) == 0 )
			{
				m_file_format = AVI_DV1_FORMAT;
			}
			else if ( strcmp( "dv2", argv[ i ] ) == 0 )
			{
				m_file_format = AVI_DV2_FORMAT;
			}
			else if ( strcmp( "raw", argv[ i ] ) == 0 )
			{
				m_file_format = RAW_FORMAT;
			}
			else if ( strcmp( "qt", argv[ i ] ) == 0 )
			{
				m_file_format = QT_FORMAT;
			}
			else if ( strcmp( "dif", argv[ i ] ) == 0 )
			{
				m_file_format = DIF_FORMAT;
#if defined(HAVE_LIBJPEG) && defined(HAVE_LIBDV)

			}
			else if ( strcmp( "jpeg", argv[ i ] ) == 0 || strcmp( "jpg", argv[ i ] ) == 0 )
			{
				m_file_format = JPEG_FORMAT;
#endif

			}
			else
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--opendml", argv[ i ] ) == 0 ) )
		{
			m_open_dml = true;
		}
		else if ( ( strcmp( "--size", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_max_file_size ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--frames", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_frame_count ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--every", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_frame_every ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--duration", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			m_duration = argv[ i ];
			if ( argv[ i ][ 0 ] == '-' )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( strcmp( "--stdin", argv[ i ] ) == 0 )
		{
			m_stdin = true;
		}
		else if ( strcmp( "--noavc", argv[ i ] ) == 0 )
		{
			m_noavc = true;
		}
		else if ( ( strcmp( "--guid", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%llx", &m_guid ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( strcmp( "--dv1394", argv[ i ] ) == 0 )
		{
			if ( i + 1 < argc && argv[ i + 1 ][ 0 ] != '-' )
			{
				i++;
				m_dv1394 = strdup( argv[ i ] );
			}
			else
				m_dv1394 = strdup( "" ); //signal to build string from detected port
		}
		else if ( ( strcmp( "--card", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_port ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--channel", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_channel ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--buffers", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_buffers ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
#if defined(HAVE_LIBJPEG) && defined(HAVE_LIBDV)

		}
		else if ( strcmp( "--jpeg-overwrite", argv[ i ] ) == 0 )
		{
			m_jpeg_overwrite = true;
		}
		else if ( ( strcmp( "--jpeg-quality", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_jpeg_quality ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( strcmp( "--jpeg-deinterlace", argv[ i ] ) == 0 )
		{
			m_jpeg_deinterlace = true;
		}
		else if ( ( strcmp( "--jpeg-width", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_jpeg_width ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
		}
		else if ( ( strcmp( "--jpeg-height", argv[ i ] ) == 0 ) && ( i < argc ) )
		{
			i++;
			if ( sscanf( argv[ i ], "%d", &m_jpeg_height ) != 1 )
			{
				usage();
				exit( EXIT_FAILURE );
			}
#endif

		}
		else if ( strcmp( "-h", argv[ i ] ) == 0 || strcmp( "--help", argv[ i ] ) == 0 )
		{
			fprintf( stderr, "Usage: %s [options] [file]\n\n", argv[ 0 ] );
			fprintf( stderr, "If file is '-' or stdout is piped or redirected, then raw DV is sent to stdout.\n\n" );
			fprintf( stderr, "Options:\n\n" );
			fprintf( stderr, "  -i --interactive  go interactive with camera VTR and capture control!\n" );
			fprintf( stderr, "  --noavc           disable use of AV/C VTR control\n" );
			fprintf( stderr, "  --autosplit       start a new file when a new recording is detected\n" );
			fprintf( stderr, "  --timestamp       put the date and time of recording into the file name.\n" );
			fprintf( stderr, "  --format dv1      save as a 'Type 1' DV AVI file. %s\n", m_file_format == AVI_DV1_FORMAT ? "This is the default." : " " );
			fprintf( stderr, "  --format dv2      save as a 'Type 2' DV AVI file. %s\n", m_file_format == AVI_DV2_FORMAT ? "This is the default." : " " );
			fprintf( stderr, "  --opendml         use the OpenDML extensions to write large (>1GB)\n" );
			fprintf( stderr, "                      'Type 2' DV AVI files (default %s).\n", m_open_dml ? "on" : "off" );
			fprintf( stderr, "  --format raw      save as a raw DV file with a .dv extension.\n" );
			fprintf( stderr, "  --format dif      save as a raw DV file with a .dif extension.\n" );
#ifdef HAVE_LIBQUICKTIME

			fprintf( stderr, "  --format qt       save as a QuickTime movie.\n" );
#endif
#if defined(HAVE_LIBJPEG) && defined(HAVE_LIBDV)

			fprintf( stderr, "  --format jpeg     save as a sequence of JPEG files.\n" );
			fprintf( stderr, "  --jpeg-overwrite  overwrite the same file instead of createing a sequence.\n" );
			fprintf( stderr, "  --jpeg-quality n  set the JPEG compression level.\n" );
			fprintf( stderr, "  --jpeg-deinterlace deinterlace the output by line doubling the upper field.\n" );
			fprintf( stderr, "  --jpeg-width n    scale the output to the specified width (max=2048).\n" );
			fprintf( stderr, "  --jpeg-height n   scale the output to the specified height (max=2048).\n" );
#endif

			fprintf( stderr, "  --frames number   max number of frames per split, 0 = unlimited (default %d).\n", m_frame_count );
			fprintf( stderr, "  --size number     max file size per split, 0 = unlimited (default %d MB).\n", m_max_file_size );
			fprintf( stderr, "  --every number    write every n'th frame only (default all frames).\n" );
			fprintf( stderr, "  --duration time   total capture duration specified as a SMIL time value:\n" );
			fprintf( stderr, "                      XXX[.Y]h, XXX[.Y]min, XXX[.Y][s], XXXms,\n" );
			fprintf( stderr, "                      [[HH:]MM:]SS[.ms], or smpte=HH[:MM[:SS[:FF]]]\n" );
			fprintf( stderr, "                      (default unlimited).\n" );
			fprintf( stderr, "  --card number     card number (default is automatic).\n" );
			fprintf( stderr, "  --guid hex        select one of multiple DV devices by its GUID.\n" );
			fprintf( stderr, "                      GUID is in hexadecimal; see /proc/bus/ieee1394/devices.\n" );
			fprintf( stderr, "  --channel number  iso channel number for listening (default %d).\n", m_channel );
			fprintf( stderr, "  --buffers number  the number of internal frames to buffer (default %d).\n", m_buffers );
			fprintf( stderr, "  --stdin           read raw DV from stdin pipe (default uses raw1394.)\n" );
#ifdef HAVE_LIBDV_DV1394_H

			fprintf( stderr, "  --dv1394 [device] use the dv1394 device named device (default uses raw1394.)\n" );
			fprintf( stderr, "                      device specification is optional if you follow the dv1394\n" );
			fprintf( stderr, "                      devfs naming convention: /dev/ieee1394/dv/hostX/....\n" );
#endif

			fprintf( stderr, "  --help            display this help and exit.\n" );
			fprintf( stderr, "  --version         display version information and exit.\n" );
			fprintf( stderr, "\nCheck out the dvgrab website for the latest version, news and other software:\n" );
			fprintf( stderr, "http://kino.schirmacher.de/\n\n" );
			exit( EXIT_SUCCESS );
		}
		else if ( strcmp( "--version", argv[ i ] ) == 0 )
		{
			fprintf( stderr, PACKAGE " " VERSION "\n" );
			exit( EXIT_SUCCESS );
		}
		else
		{
			m_dst_file_name = argv[ i ];
		}
	}
	if ( m_dst_file_name == NULL && isatty( fileno( stdout ) ) )
	{
		usage();
		exit( EXIT_FAILURE );
	}
}

void DVgrab::startCapture()
{
	int tryCounter = 400;

	if ( strcmp( m_dst_file_name, "-" ) != 0 )
	{
		pthread_mutex_lock( &capture_mutex );
		switch ( m_file_format )
		{
		case RAW_FORMAT:
			m_writer = new RawHandler();
			break;

		case DIF_FORMAT:
			m_writer = new RawHandler( ".dif" );
			break;

		case AVI_DV1_FORMAT:
			{
				AVIHandler *aviWriter = new AVIHandler( AVI_DV1_FORMAT );
				m_writer = aviWriter;
				break;
			}

		case AVI_DV2_FORMAT:
			{
				AVIHandler *aviWriter = new AVIHandler( AVI_DV2_FORMAT );
				m_writer = aviWriter;
				if ( m_max_file_size == 0 || m_max_file_size > 1000 )
				{
					cerr << "Turning on OpenDML to support large file size.\n";
					m_open_dml = true;
				}
				aviWriter->SetOpenDML( m_open_dml );
				break;
			}

#ifdef HAVE_LIBQUICKTIME
		case QT_FORMAT:
			m_writer = new QtHandler();
			break;
#endif

#if defined(HAVE_LIBJPEG) && defined(HAVE_LIBDV)

		case JPEG_FORMAT:
			m_writer = new JPEGHandler( m_jpeg_quality, m_jpeg_deinterlace, m_jpeg_width, m_jpeg_height, m_jpeg_overwrite );
			break;
#endif

		}
		m_writer->SetTimeStamp( m_timestamp );
		m_writer->SetBaseName( m_dst_file_name );
		m_writer->SetMaxFrameCount( m_frame_count );
		m_writer->SetAutoSplit( m_autosplit );
		m_writer->SetEveryNthFrame( m_frame_every );
		m_writer->SetMaxFileSize( ( off_t ) m_max_file_size * ( off_t ) ( 1024 * 1024 ) );
	}
	if ( m_avc )
		m_avc->Play( m_node );

	// this is a little unclean, checking global g_done from main.cc to allow interruption
	while ( !g_done && m_frame == NULL && tryCounter-- > 0 )
	{
		timespec t = {0, 25000000};
		nanosleep( &t, NULL );
	}

	if ( m_frame )
	{
		sendEvent( "Capture Started" );
		m_captureActive = true;
		m_total_frames = 0;
		pthread_mutex_unlock( &capture_mutex );

		// parse the SMIL time value duration
		if ( m_timeDuration == NULL && ! m_duration.empty() )
			m_timeDuration = new SMIL::MediaClippingTime( m_duration, m_frame->GetFrameRate() );
	}
	else
	{
		pthread_mutex_unlock( &capture_mutex );
		throw string( "no DV" );
	}
}


void DVgrab::stopCapture()
{
	pthread_mutex_lock( &capture_mutex );
	if ( m_writer != NULL )
	{
		string filename = m_writer->GetFileName();
		int frames = m_writer->GetFramesWritten();
		float size = ( float ) m_writer->GetFileSize() / 1024 / 1024;

		m_writer->Close();
		delete m_writer;
		m_writer = NULL;
		if ( m_avc )
			m_avc->Pause( m_node );
		if ( m_frame != NULL )
		{
			TimeCode timeCode;
			struct tm recDate;
			m_frame->GetTimeCode( timeCode );
			m_frame->GetRecordingDate( recDate );
			sendEvent( "\"%s\": %8.2f MB %d frames timecode %2.2d:%2.2d:%2.2d.%2.2d date %4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
			           filename.c_str(), size, frames,
			           timeCode.hour, timeCode.min, timeCode.sec, timeCode.frame,
			           recDate.tm_year + 1900, recDate.tm_mon + 1, recDate.tm_mday,
			           recDate.tm_hour, recDate.tm_min, recDate.tm_sec
			         );
		}
		else
			sendEvent( "\"%s\" %8.2f MB %d frames", filename.c_str(), size, frames );
		sendEvent( "Capture Stopped" );

		if ( m_dropped_frames > 0 )
			sendEvent( "Warning: %d dropped frames.", m_dropped_frames );
		m_dropped_frames = 0;
		m_captureActive = false;
	}
	pthread_mutex_unlock( &capture_mutex );
}

void *DVgrab::captureThread( void *arg )
{
	DVgrab * me = ( DVgrab* ) arg;
	me->captureThreadRun();
	return NULL;
}

static char output_name_buf[1000] = "";

char *GetOutputName() {
  return output_name_buf;
}

void SetOutputName(const char *str) {
  strncpy(output_name_buf,str,sizeof(output_name_buf));
}

void DVgrab::captureThreadRun()
{
	static TimeCode prevTimeCode;
	
	prevTimeCode.sec = -1;
	m_reader_active = true;

	// Loop until we're informed otherwise
	while ( m_reader_active )
	{
		int dropped = m_reader->GetDroppedFrames();
		static TimeCode timeCode;
		static struct tm recDate;

		pthread_testcancel();
		// Wait for the reader to indicate that something has happened
		m_reader->WaitForAction( );

		// Get the next frame
		m_frame = m_reader->GetFrame();

		// Check if the out queue is falling behind
		bool critical_mass = m_reader->GetOutQueueSize( ) > m_reader->GetInQueueSize( );

		// Make sure we return the oldest frame first
		// Do with it what needs to be done
		if ( m_frame != NULL )
		{
		  const char *output_name = m_writer ? m_writer->GetFileName().c_str() : "";
		  SetOutputName(output_name);

			m_frame->GetTimeCode( timeCode );
			
			// VIDEOBOX PATCH BEGINS MOD000
			bool should_save = false;
			{
			  // right here is a good place to
			  // grab frame, sound

			  static SideTap *tap = CreateSideTap();
			  assert(tap!=NULL);
			  SideTapImage image;
			  SideTapSound sound;

			  bool supply_image = tap->ShouldSupplyImage();
			  bool supply_sound = tap->ShouldSupplySound();
			  bool recover_image = tap->ShouldRecoverImage();
			  bool recover_sound = tap->ShouldRecoverSound();
			  should_save = tap->ShouldSave();

			  if (recover_image||recover_sound) {
			    supply_image = supply_sound = true;
			    recover_image = recover_sound = true;
			  }

			  static int ctr = 0;
			  ctr++;
			  if (ctr%90==0)
			    printf("[%s] FRAME NUMBER %ld\n", output_name, ctr);

			  static JSAMPLE *image_buffer = 
			    new JSAMPLE[2048*2048*3];
			  if (tap->ShouldSupplyImage()) {
			    
			    assert(image_buffer!=NULL);
			    m_frame->ExtractRGB(image_buffer);
			    
			    // may need to deal with interlacing - 
			    // see filehandler.cc
			    JDIMENSION width = m_frame->GetWidth();
			    JDIMENSION height = m_frame->GetHeight();

			    image.Set((char*)image_buffer,width,height);
			  }

#define NSND 10000
			  static int16_t *snd_buffers[4] = {
			    new int16_t[NSND],
			    new int16_t[NSND],
			    new int16_t[NSND],
			    new int16_t[NSND]
			  };
			  if (tap->ShouldSupplySound()) {
			    AudioInfo info;
			    assert(m_frame->GetAudioInfo(info));
			    if (NSND<=info.samples) {
			      fprintf(stderr,"Not enough room to store %d sound samples\n", info.samples);
			      fprintf(stderr,"(freq %d, frames %d, channels %d, quantization %d\n", info.frequency, info.frames, info.channels, info.quantization);
			    }
			    assert(NSND>=info.samples);
			    m_frame->ExtractAudio(snd_buffers);
			    sound.Set(snd_buffers,
				      info.channels,
				      info.samples,
				      info.frequency,
				      info.quantization);
			  }

			  tap->Apply(image,sound);

			  if (recover_image||recover_sound) {
			    m_frame->ReturnFrame(image_buffer,snd_buffers);
			  }

			}
			// VIDEOBOX PATCH ENDS
			

			if ( dropped > 0 )
			{
				m_frame->GetRecordingDate( recDate );
				m_dropped_frames += dropped;
				sendEvent( "\a\"%s\": buffer underrun near: timecode %2.2d:%2.2d:%2.2d.%2.2d date %4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
				           m_writer ? m_writer->GetFileName().c_str() : "stdout",
				           timeCode.hour, timeCode.min, timeCode.sec, timeCode.frame,
				           recDate.tm_year + 1900, recDate.tm_mon + 1, recDate.tm_mday,
				           recDate.tm_hour, recDate.tm_min, recDate.tm_sec );
				sendEvent( "This error means that the frames could not be written fast enough." );
			}
			if ( m_frame->IsComplete() == false )
			{
				m_frame->GetRecordingDate( recDate );
				m_dropped_frames++;
				sendEvent( "\a\"%s\": frame dropped: timecode %2.2d:%2.2d:%2.2d.%2.2d date %4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
				           m_writer ? m_writer->GetFileName().c_str() : "stdout",
				           timeCode.hour, timeCode.min, timeCode.sec, timeCode.frame,
				           recDate.tm_year + 1900, recDate.tm_mon + 1, recDate.tm_mday,
				           recDate.tm_hour, recDate.tm_min, recDate.tm_sec );
				sendEvent( "This error means that the ieee1394 driver received an incomplete frame." );
			}
			else
			{
				if ( m_frame->IsNormalSpeed() )
				{
					// see if we have exceeded requested duration
					m_total_frames++;
					if ( m_timeDuration && m_timeDuration->isResolved() &&
					        ( m_total_frames / m_frame->GetFrameRate() * 1000 ) >
					        m_timeDuration->getResolvedOffset() )
					{
						pthread_mutex_unlock( &capture_mutex );
						stopCapture();
						m_reader_active = false;
					}
					else
					  // VIDEOBOX PATCH BEGINS MOD001
					  if (should_save)
					  // VIDEOBOX PATCH ENDS

					{
						// All access to the writer is protected
						pthread_mutex_lock( &capture_mutex );
						if ( m_writer != NULL )
						{
							float size = ( float ) m_writer->GetFileSize() / 1024 / 1024;

							// This check to split off a new file must occur outside the FileHandler
							// so the new frame is a part of the new file and not the old one
							// It checks for the new recording flag set in the DV stream as well as for
							// timecode discontinuity in seconds.
							int time_diff = timeCode.sec - prevTimeCode.sec;
							bool discontinuity = prevTimeCode.sec != -1 && ( time_diff > 1 || ( time_diff < 0 && time_diff > -59 ) );
							if ( m_writer->FileIsOpen() && m_autosplit && ( m_frame->IsNewRecording() || discontinuity ) )
							{
								m_writer->Close();
								m_frame->GetRecordingDate( recDate );
								sendEvent( "\"%s\": %8.2f MB %d frames timecode %2.2d:%2.2d:%2.2d.%2.2d date %4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
								           m_writer->GetFileName().c_str(), size, m_writer->GetFramesWritten(),
								           timeCode.hour, timeCode.min, timeCode.sec, timeCode.frame,
								           recDate.tm_year + 1900, recDate.tm_mon + 1, recDate.tm_mday,
								           recDate.tm_hour, recDate.tm_min, recDate.tm_sec );
								if ( m_dropped_frames > 0 )
									sendEvent( "Warning: %d dropped frames.", m_dropped_frames );
								m_dropped_frames = 0;
							}
							// Set the sample frame here to ensure we have a good (normal speed)
							// frame for setting audio stream format. Also, each recording could
							// have different params.
							if ( ! m_writer->FileIsOpen() )
							{
								m_writer->SetSampleFrame( *m_frame );
							}
							if ( ! m_writer->WriteFrame( *m_frame ) )
							{
								pthread_mutex_unlock( &capture_mutex );
								stopCapture();
								throw string( "writing failed" );
							}

							// The file will be closed if max size or frames are exceeded
							if ( !m_writer->FileIsOpen() )
							{
								m_frame->GetRecordingDate( recDate );
								sendEvent( "\"%s\": %8.2f MB %d frames timecode %2.2d:%2.2d:%2.2d.%2.2d date %4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
								           m_writer->GetFileName().c_str(), size, m_writer->GetFramesWritten(),
								           timeCode.hour, timeCode.min, timeCode.sec, timeCode.frame,
								           recDate.tm_year + 1900, recDate.tm_mon + 1, recDate.tm_mday,
								           recDate.tm_hour, recDate.tm_min, recDate.tm_sec );
								if ( m_dropped_frames > 0 )
									sendEvent( "Warning: %d dropped frames.", m_dropped_frames );
								m_dropped_frames = 0;
							}
						}
						pthread_mutex_unlock( &capture_mutex );
					}
				}

				// drop frame on stdout if getting low on buffers
				if ( !critical_mass && !isatty( fileno( stdout ) ) )
				{
					fd_set wfds;
					struct timeval tv =
					    {
						    0, 20000
					    }
					    ;
					FD_ZERO( &wfds );
					FD_SET( fileno( stdout ), &wfds );
					if ( select( fileno( stdout ) + 1, NULL, &wfds, NULL, &tv ) )
					{
						write( fileno( stdout ), m_frame->data, m_frame->GetFrameSize() );
					}
				}
			}
			m_reader->DoneWithFrame( m_frame );
			prevTimeCode.sec = timeCode.sec;
		}
		else
			// reader has erred or signaling a stop condition (end of pipe)
			break;
	}
	m_reader_active = false;
}


void DVgrab::status( )
{
	char s[ 32 ];
	unsigned int status;
	static unsigned int prevStatus = 0;
	static string transportStatus( "" );
	static string timecode( "" );
	static string filename( "" );
	static string duration( "" );

	if ( ! m_avc )
		return ;

	status = m_avc->TransportStatus( m_node );
	if ( ( int ) status >= 0 )
		transportStatus = avc1394_vcr_decode_status( status );
	if ( prevStatus == 0 )
		prevStatus = status;
	if ( status != prevStatus && AVC1394_MASK_RESPONSE_OPERAND( prevStatus, 2 ) == AVC1394_VCR_RESPONSE_TRANSPORT_STATE_WIND )
	{
		quadlet_t resp2 = AVC1394_MASK_RESPONSE_OPERAND( status, 2 );
		quadlet_t resp3 = AVC1394_MASK_RESPONSE_OPERAND( status, 3 );
		if ( resp2 == AVC1394_VCR_RESPONSE_TRANSPORT_STATE_WIND && resp3 == AVC1394_VCR_OPERAND_WIND_STOP )
			sendEvent( "Winding Stopped" );
	}
	m_transportStatus = prevStatus = status;

	if ( m_avc->Timecode( m_node, s ) )
		timecode = s;

	if ( m_writer != NULL )
		filename = m_writer->GetFileName();
	else
		filename = "";

	if ( m_frame != NULL && m_writer != NULL )
	{
		sprintf( s, "%8.2f", ( float ) m_writer->GetFramesWritten() / m_frame->GetFrameRate() );
		duration = s;
	}
	else
		duration = "";

	fprintf( stderr, "%-80.80s\r", " " );
	fprintf( stderr, "\"%s\" %s \"%s\" %8s sec\r", transportStatus.c_str(),
	         timecode.c_str(),
	         filename.c_str(),
	         duration.c_str() );
	fflush( stderr );
}

bool DVgrab::execute( const char cmd )
{
	bool result = true;
	switch ( cmd )
	{
	case 'p':
		if ( m_avc )
		{
			m_avc->Play( m_node );
		}
		break;
	case ' ':
		if ( m_avc )
		{
			if ( isPlaying() )
				m_avc->Pause( m_node );
			else
				m_avc->Play( m_node );
		}
		break;
	case 'h':
		if ( m_avc )
		{
			m_avc->Reverse( m_node );
		}
		break;
	case 'j':
		if ( m_avc )
		{
			m_avc->Pause( m_node );
			m_avc->Rewind( m_node );
		}
		break;
	case 'k':
		if ( m_avc )
		{
			m_avc->Pause( m_node );
		}
		break;
	case 'l':
		if ( m_avc )
		{
			m_avc->Pause( m_node );
			m_avc->FastForward( m_node );
		}
		break;
	case 'a':
		if ( m_avc )
		{
			m_avc->Stop( m_node );
			m_avc->Rewind( m_node );
		}
		break;
	case 'z':
		if ( m_avc )
		{
			m_avc->Stop( m_node );
			m_avc->FastForward( m_node );
		}
		break;
	case '1':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, -14 );
		}
		break;
	case '2':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, -11 );
		}
		break;
	case '3':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, -8 );
		}
		break;
	case '4':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, -4 );
		}
		break;
	case '5':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, -1 );
		}
		break;
	case '6':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, 1 );
		}
		break;
	case '7':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, 4 );
		}
		break;
	case '8':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, 8 );
		}
		break;
	case '9':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, 11 );
		}
		break;
	case '0':
		if ( m_avc )
		{
			m_avc->Shuttle( m_node, 14 );
		}
		break;
	case 's':
	case 0x1b:    // Esc
		if ( m_captureActive )
			stopCapture();
		else if ( m_avc )
			m_avc->Stop( m_node );
		break;
	case 'c':
		startCapture();
		break;
	case 'q':
		result = false;
		break;
	case '?':
		cerr << "q=quit, p=play, c=capture, Esc=stop, h=reverse, j=backward scan, k=pause\n";
		cerr << "l=forward scan, a=rewind, z=fast forward, 0-9=trickplay, <space>=play/pause\n";
		break;
	default:
		//fprintf( stderr, "\nunkown key 0x%2.2x", cmd );
		//result = false;
		break;
	}
	return result;
}

void DVgrab::sendEvent( const char *format, ... )
{
	va_list list;
	va_start( list, format );
	char line[ 1024 ];

	fprintf( stderr, "%-80.80s\r", " " );
	if ( snprintf( line, 1024, "%s\n", format ) != 0 )
		vfprintf( stderr, line, list );
	va_end( list );
}

bool DVgrab::isPlaying()
{
	if ( ! m_avc )
		return false;
	unsigned int status = m_avc->TransportStatus( m_node );
	quadlet_t resp2 = AVC1394_MASK_RESPONSE_OPERAND( status, 2 );
	quadlet_t resp3 = AVC1394_MASK_RESPONSE_OPERAND( status, 3 );
	return ( ( resp2 == AVC1394_VCR_RESPONSE_TRANSPORT_STATE_PLAY && resp3 != AVC1394_VCR_OPERAND_PLAY_FORWARD_PAUSE ) ||
		( resp2 == AVC1394_VCR_RESPONSE_TRANSPORT_STATE_RECORD && resp3 != AVC1394_VCR_OPERAND_RECORD_PAUSE ) );
}

bool DVgrab::done()
{
	timespec t = {0, 25000000};
	nanosleep( &t, NULL );
	return ! m_reader_active;
}
