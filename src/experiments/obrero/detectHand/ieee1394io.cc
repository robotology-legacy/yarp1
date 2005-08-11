/*
* ieee1394io.cc -- asynchronously grabbing DV data
* Copyright (C) 2000 Arne Schirmacher <arne@schirmacher.de>
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

/**
    \page The IEEE 1394 Reader Class
 
    This text explains how the IEEE 1394 Reader class works.
 
    The IEEE1394Reader object maintains a connection to a DV
    camcorder. It reads DV frames from the camcorder and stores them
    in a queue. The frames can then be retrieved from the buffer and
    displayed, stored, or processed in other ways.
 
    The IEEE1394Reader class supports asynchronous operation: it
    starts a separate thread, which reads as fast as possible from the
    ieee1394 interface card to make sure that no frames are
    lost. Since the buffer can be configured to hold many frames, no
    frames will be lost even if the disk access is temporarily slow.
 
    There are two queues available in an IEEE1394Reader object. One
    queue holds empty frames, the other holds frames filled with DV
    content just read from the interface. During operation the reader
    thread takes unused frames from the inFrames queue, fills them and
    places them in the outFrame queue. The program can then take
    frames from the outFrames queue, process them and finally put
    them back in the inFrames queue.
 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <deque>
#include <iostream>

using std::cerr;
using std::endl;

#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>

#include <libraw1394/raw1394.h>
#include <libraw1394/csr.h>
#include <libavc1394/avc1394.h>
#include <libavc1394/avc1394_vcr.h>
#include <libavc1394/rom1394.h>

#include "ieee1394io.h"
#include "frame.h"
#include "error.h"

/* static member variables.
 
   I tried non-static member variables first but they did not work
   when used by two threads. I have no idea why though.
 
   CY: Because are threads are running on static methods :-/ - to
   remove it we need to pass the object pointer into the callback
   method (somehow).
*/

int IEEE1394Reader::droppedFrames;
pthread_mutex_t IEEE1394Reader::mutex;
pthread_mutex_t IEEE1394Reader::condition_mutex;
pthread_cond_t IEEE1394Reader::condition;
Frame *IEEE1394Reader::currentFrame;
deque < Frame* > IEEE1394Reader::inFrames;
deque < Frame* > IEEE1394Reader::outFrames;
raw1394handle_t raw1394Reader::handle;
unsigned char *dv1394Reader::m_dv1394_map;
int dv1394Reader::m_dv1394_fd;
raw1394handle_t AVC::avc_handle;
pthread_mutex_t AVC::avc_mutex;

/** Initializes the IEEE1394Reader object.
 
    The object is initialized with port and channel number. These
    parameters define the interface card and the iso channel on which
    the camcorder sends its data.
 
    The object contains a list of empty frames, which are allocated
    here. 50 frames (2 seconds) should be enough in most cases.
 
    \param p the number of the interface card to use
    \param c the iso channel number to use
    \param bufSize the number of frames to allocate for the frames buffer
 */


IEEE1394Reader::IEEE1394Reader( int c, int bufSize ) : channel( c )
{
	Frame * frame;

	/* Create empty frames and put them in our inFrames queue */
	for ( int i = 0; i < bufSize; ++i )
	{
		frame = new Frame();
		inFrames.push_back( frame );
	}

	/* Initialize mutexes */
	pthread_mutex_init( &mutex, NULL );

	/* Initialise mutex and condition for action triggerring */
	pthread_mutex_init( &condition_mutex, NULL );
	pthread_cond_init( &condition, NULL );

	/* Other */
	droppedFrames = 0;
}


/** Destroys the IEEE1394Reader object.
 
    In particular, it deletes all frames in the inFrames and outFrames
    queues, as well as the one currently in use.  Note that one or
    more frames may have been taken out of the queues by a user of the
    IEEE1394Reader class.
 
*/

IEEE1394Reader::~IEEE1394Reader()
{
	Frame * frame;

	for ( int i = inFrames.size(); i > 0; --i )
	{
		frame = inFrames[ 0 ];
		inFrames.pop_front();
		delete frame;
	}
	for ( int i = outFrames.size(); i > 0; --i )
	{
		frame = outFrames[ 0 ];
		outFrames.pop_front();
		delete frame;
	}
	if ( currentFrame != NULL )
	{
		delete currentFrame;
		currentFrame = NULL;
	}
	pthread_mutex_destroy( &condition_mutex );
	pthread_cond_destroy( &condition );
}


/** Fetches the next frame from the output queue
 
    The outFrames contains a list of frames to be processed (saved,
    displayed) by the user of this class.  Copy the first frame
    (actually only a pointer to it) and remove it from the queue.
 
    \note If this returns NULL, wait some time (1/25 sec.) before
    calling it again.
 
    \return a pointer to the current frame, or NULL if no frames are
    in the queue
 
 */

Frame* IEEE1394Reader::GetFrame()
{
	Frame * frame = NULL;

	pthread_mutex_lock( &mutex );

	if ( outFrames.size() > 0 )
	{
		frame = outFrames[ 0 ];
		outFrames.pop_front();
	}
	pthread_mutex_unlock( &mutex );
	if ( frame != NULL )
		frame->ExtractHeader();

	return frame;
}


/** Put back a frame to the queue of available frames
*/

void IEEE1394Reader::DoneWithFrame( Frame* frame )
{
	pthread_mutex_lock( &mutex );
	inFrames.push_back( frame );
	pthread_mutex_unlock( &mutex );
}


/** Return the number of dropped frames since last call
*/

int IEEE1394Reader::GetDroppedFrames( void )
{
	pthread_mutex_lock( &mutex );
	int n = droppedFrames;
	droppedFrames = 0;
	pthread_mutex_unlock( &mutex );
	return n;
}


/** Throw away all currently available frames.
 
    All frames in the outFrames queue are put back to the inFrames
    queue.  Also the currentFrame is put back too.  */

void IEEE1394Reader::Flush()
{
	Frame * frame = NULL;

	for ( int i = outFrames.size(); i > 0; --i )
	{
		frame = outFrames[ 0 ];
		outFrames.pop_front();
		inFrames.push_back( frame );
	}
	if ( currentFrame != NULL )
	{
		inFrames.push_back( currentFrame );
		currentFrame = NULL;
	}
}

bool IEEE1394Reader::WaitForAction( )
{
	pthread_mutex_lock( &mutex );
	int size = outFrames.size( );
	pthread_mutex_unlock( &mutex );

	if ( size == 0 )
	{
		pthread_mutex_lock( &condition_mutex );
		pthread_cond_wait( &condition, &condition_mutex );
		pthread_mutex_unlock( &condition_mutex );
		pthread_mutex_lock( &mutex );
		size = outFrames.size( );
		pthread_mutex_unlock( &mutex );
	}

	return size != 0;
}

void IEEE1394Reader::TriggerAction( )
{
	pthread_mutex_lock( &condition_mutex );
	pthread_cond_signal( &condition );
	pthread_mutex_unlock( &condition_mutex );
}


/** Initializes the raw1394Reader object.
 
    The object is initialized with port and channel number. These
    parameters define the interface card and the iso channel on which
    the camcorder sends its data.
 
    \param p the number of the interface card to use
    \param c the iso channel number to use
    \param bufSize the number of frames to allocate for the frames buffer
 */


raw1394Reader::raw1394Reader( int p, int c, int bufSize ) :
		IEEE1394Reader( c, bufSize ), port( p )
{
	handle = NULL;
	//cerr << ">>> Using raw1394 capture" << endl;
}


raw1394Reader::~raw1394Reader()
{}


/** Start receiving DV frames
 
    The ieee1394 subsystem is initialized with the parameters provided
    to the constructor (port and channel).  The received frames can be
    retrieved from the outFrames queue.
 
*/

bool raw1394Reader::StartThread()
{
	pthread_mutex_lock( &mutex );
	currentFrame = NULL;
	if ( Open() && StartReceive() )
	{
		pthread_create( &thread, NULL, Thread, this );
		pthread_mutex_unlock( &mutex );
		return true;
	}
	else
	{
		Close();
		pthread_mutex_unlock( &mutex );
		return false;
	}
}


/** Stop the receiver thread.
 
    The receiver thread is being canceled. It will finish the next
    time it calls the pthread_testcancel() function.  After it is
    canceled, we turn off iso receive and close the ieee1394
    subsystem.  We also remove all frames in the outFrames queue that
    have not been processed until now.
 
    \bugs It is not clear what happens if no data is being sent by the
    camcorder. Will we still hit the pthread_testcancel function in
    this case?
 
*/

void raw1394Reader::StopThread()
{
	pthread_mutex_lock( &mutex );
	pthread_cancel( thread );
	pthread_join( thread, NULL );
	StopReceive();
	Close();
	Flush();
	pthread_mutex_unlock( &mutex );
	TriggerAction( );
}


/** Open the raw1394 interface
 
    \return success/failure
*/

bool raw1394Reader::Open()
{
	bool success;
	int numcards;
	struct raw1394_portinfo pinf[ 16 ];
	iso_handler_t oldhandler;

	assert( handle == NULL );

	try
	{

#ifdef LIBRAW1394_OLD
		handle = raw1394_get_handle();
#else

		handle = raw1394_new_handle();
#endif

		if ( handle == 0 )
			return false;
		fail_neg( numcards = raw1394_get_port_info( handle, pinf, 16 ) );
		fail_neg( raw1394_set_port( handle, port ) );

		oldhandler = raw1394_set_iso_handler( handle, channel, Handler );

		success = true;

	}
	catch ( string exc )
	{
		Close();
		cerr << exc << endl;
		success = false;
	}
	return success;
}


/** Close the raw1394 interface
 
*/

void raw1394Reader::Close()
{
	if ( handle != NULL )
	{
		raw1394_destroy_handle( handle );
		handle = NULL;
	}
}

bool raw1394Reader::StartReceive()
{
	bool	success;

	/* Starting iso receive */
	try
	{
		fail_neg( raw1394_start_iso_rcv( handle, channel ) );
		success = true;
	}
	catch ( string exc )
	{
		cerr << exc << endl;
		success = false;
	}
	return success;
}


void raw1394Reader::StopReceive()
{
	if ( handle != NULL )
		raw1394_stop_iso_rcv( handle, channel );
}


/** assembles a DIF sequence to a DV frame
 
    The PAL/NTSC DV data has the following format:
 
    - packets of 496 bytes length
    - packets of  16 bytes length.
 
    The long packets contain the actual video and audio contents that
    goes into the AVI file.
       
    The actual DV data starts at quadlet 4 of the long packet, so we
    have 480 bytes of DV data per packet.  For PAL, each rame is made
    out of 300 packets and there are 25 frames per second.  That is
    144000 bytes per frame and 3600000 bytes per second.  For NTSC we
    have 250 packages per frame and 30 frames per second.  That is
    120000 bytes per frame and 3600000 bytes per second too.
 
    We also attempt to detect incomplete frames. The idea is: If we
    hit the begin of frame indicator and this is not the very first
    packet for this frame, then at least one packed has been dropped
    by the driver. This does not guarantee that no incomplete frames
    are saved, because if we miss the frame header of the next frame,
    we can´t tell whether the last one is incomplete.
 
    \param handle the handle to the ieee1394 subsystem
    \param channel the channel this data has been read from
    \param length the length of the data
    \param data a pointer to the data
    \return unused, we return always zero 
 
*/

int raw1394Reader::Handler( raw1394handle_t handle, int channel, size_t length,
                            quadlet_t *data )
{
	/* skip empty packets */

	if ( length > 16 )
	{
		unsigned char * p = ( unsigned char* ) & data[ 3 ];
		int section_type = p[ 0 ] >> 5;           /* section type is in bits 5 - 7 */
		int dif_sequence = p[ 1 ] >> 4;           /* dif sequence number is in bits 4 - 7 */
		int dif_block = p[ 2 ];

		/* if we are at the beginning of a frame, we put the previous
		   frame in our output_queue.  Then we try to get an unused
		   frame_buffer from the buffer_queue for the current frame.
		   We must lock the queues because they are shared between
		   this thread and the main thread. */
		if ( section_type == 0 && dif_sequence == 0 )
		{
			pthread_mutex_lock( &mutex );
			if ( currentFrame != NULL )
			{
				outFrames.push_back( currentFrame );
				currentFrame = NULL;
				//cerr << "reader > # inFrames: " << inFrames.size() << ", # outFrames: " << outFrames.size() << endl;
				TriggerAction( );
			}
			if ( inFrames.size() > 0 )
			{
				currentFrame = inFrames.front();
				currentFrame->bytesInFrame = 0;
				inFrames.pop_front();
				//if ( outFrames.size( ) >= 25 )
				//cerr << "reader < # inFrames: " << inFrames.size() << ", # outFrames: " << outFrames.size() << endl;
			}
			else
			{
				droppedFrames++;
				//cerr << "Warning: raw1394 dropped frames: " << droppedFrames << endl;
			}
			pthread_mutex_unlock( &mutex );
		}
		if ( currentFrame != NULL )
		{
			switch ( section_type )
			{
			case 0:    /* 1 Header block */
				/* p[3] |= 0x80; // hack to force PAL data */
				memcpy( currentFrame->data + dif_sequence * 150 * 80, p, 480 );
				break;

			case 1:    /* 2 Subcode blocks */
				memcpy( currentFrame->data + dif_sequence * 150 * 80 + ( 1 + dif_block ) * 80, p, 480 );
				break;

			case 2:    /* 3 VAUX blocks */
				memcpy( currentFrame->data + dif_sequence * 150 * 80 + ( 3 + dif_block ) * 80, p, 480 );
				break;

			case 3:    /* 9 Audio blocks interleaved with video */
				memcpy( currentFrame->data + dif_sequence * 150 * 80 + ( 6 + dif_block * 16 ) * 80, p, 480 );
				break;

			case 4:    /* 135 Video blocks interleaved with audio */
				memcpy( currentFrame->data + dif_sequence * 150 * 80 + ( 7 + ( dif_block / 15 ) + dif_block ) * 80, p, 480 );
				break;

			default:    /* we can´t handle any other data */
				break;
			}
			currentFrame->bytesInFrame += 480;
		}
	}
	return 0;
}


/** The thread responsible for polling the raw1394 interface.
 
    Though this is an infinite loop, it can be canceled by StopThread,
    but only in the pthread_testcancel() function.
 
*/
void* raw1394Reader::Thread( void* arg )
{
	raw1394Reader * theReader = ( raw1394Reader* ) arg;

	while ( true )
	{
		raw1394_loop_iterate( theReader->handle );
		pthread_testcancel();
	}
	return NULL;
}



/** Initializes the dv1394Reader object.
 
    \param c the iso channel number to use
    \param bufSize the number of frames to allocate for the frames buffer
 */


dv1394Reader::dv1394Reader( char *device, int c, int bufSize ) :
		IEEE1394Reader( c, bufSize )
{
	m_dv1394_map = NULL;
	m_dv1394_fd = -1;
	m_device_file = device;
	//cerr << ">>> Using dv1394 capture" << endl;
}


dv1394Reader::~dv1394Reader()
{}


/** Start receiving DV frames
 
    The ieee1394 subsystem is initialized with the parameters provided
    to the constructor (port and channel).  The received frames can be
    retrieved from the outFrames queue.
 
*/

bool dv1394Reader::StartThread()
{
	pthread_mutex_lock( &mutex );
	currentFrame = NULL;
	if ( Open() && StartReceive() )
	{
		pthread_create( &thread, NULL, Thread, this );
		pthread_mutex_unlock( &mutex );
		return true;
	}
	else
	{
		Close();
		pthread_mutex_unlock( &mutex );
		return false;
	}
}


/** Stop the receiver thread.
 
    The receiver thread is being canceled. It will finish the next
    time it calls the pthread_testcancel() function.  After it is
    canceled, we turn off iso receive and close the ieee1394
    subsystem.  We also remove all frames in the outFrames queue that
    have not been processed until now.
 
    \bugs It is not clear what happens if no data is being sent by the
    camcorder. Will we still hit the pthread_testcancel function in
    this case?
 
*/

void dv1394Reader::StopThread()
{
	pthread_mutex_lock( &mutex );
	pthread_cancel( thread );
	pthread_join( thread, NULL );
	StopReceive();
	Close();
	Flush();
	pthread_mutex_unlock( &mutex );
}


/** Open the dv1394 interface
 
    \return success/failure
*/

bool dv1394Reader::Open()
{
	int n_frames = DV1394_MAX_FRAMES / 4;
	struct dv1394_init init =
	    {
		    DV1394_API_VERSION, channel, n_frames, DV1394_PAL, 0, 0, 0
	    };

	m_dv1394_fd = open( m_device_file, O_RDWR );
	if ( m_dv1394_fd == -1 )
	{
		perror( "dv1394 open" );
		cerr << "dv1394 open: " << m_device_file << endl;
		return false;
	}

	if ( ioctl( m_dv1394_fd, DV1394_INIT, &init ) )
	{
		perror( "dv1394 INIT ioctl" );
		close( m_dv1394_fd );
		m_dv1394_fd = -1;
		return false;
	}

	m_dv1394_map = ( unsigned char * ) mmap( NULL, DV1394_PAL_FRAME_SIZE * n_frames,
	               PROT_READ | PROT_WRITE, MAP_SHARED, m_dv1394_fd, 0 );
	if ( m_dv1394_map == MAP_FAILED )
	{
		perror( "mmap frame buffers" );
		close( m_dv1394_fd );
		m_dv1394_fd = -1;
		m_dv1394_map = NULL;
		return false;
	}

	return true;
}


/** Close the dv1394 interface
 
*/
void dv1394Reader::Close()
{
	if ( m_dv1394_fd != -1 )
	{
		if ( m_dv1394_map != NULL )
			munmap( m_dv1394_map, DV1394_PAL_FRAME_SIZE * DV1394_MAX_FRAMES / 4 );
		close( m_dv1394_fd );
		m_dv1394_map = NULL;
		m_dv1394_fd = -1;
	}
}


bool dv1394Reader::StartReceive()
{
	/* Starting iso receive */
	if ( ioctl( m_dv1394_fd, DV1394_START_RECEIVE, NULL ) )
	{
		perror( "dv1394 START_RECEIVE ioctl" );
		return false;
	}
	return true;
}


void dv1394Reader::StopReceive()
{}

bool dv1394Reader::Handler( int handle )
{
	struct dv1394_status dvst;

	if ( ioctl( handle, DV1394_WAIT_FRAMES, 1 ) )
	{
		perror( "error: ioctl WAIT_FRAMES" );
		return false;
	}

	if ( ioctl( handle, DV1394_GET_STATUS, &dvst ) )
	{
		perror( "ioctl GET_STATUS" );
		return false;
	}

	if ( dvst.dropped_frames > 0 )
	{
		droppedFrames += dvst.dropped_frames;
	}

	for ( unsigned int i = 0; i < dvst.n_clear_frames; i++ )
	{
		pthread_mutex_lock( &mutex );
		if ( currentFrame != NULL )
		{
			outFrames.push_back( currentFrame );
			currentFrame = NULL;
			TriggerAction( );
			//printf("reader > out: buffer %d, output %d\n", inFrames.size(), outFrames.size());
			//fflush(stdout);
		}


		if ( inFrames.size() > 0 )
		{
			currentFrame = inFrames.front();
			currentFrame->bytesInFrame = 0;
			inFrames.pop_front();
			//printf("reader < buf: buffer %d, output %d\n", inFrames.size(), outFrames.size());
			//fflush(stdout);
		}
		else
		{
			droppedFrames++;
			//cerr << "reader < # dropped frames: " << droppedFrames << endl;
		}

		pthread_mutex_unlock( &mutex );

		if ( currentFrame != NULL )
		{
			memcpy( currentFrame->data,
			        ( m_dv1394_map + ( dvst.first_clear_frame * DV1394_PAL_FRAME_SIZE ) ),
			        DV1394_PAL_FRAME_SIZE );
			currentFrame->bytesInFrame = currentFrame->GetFrameSize( );
		}

		if ( ioctl( handle, DV1394_RECEIVE_FRAMES, 1 ) )
		{
			perror( "error: ioctl RECEIVE_FRAMES" );
			return false;
		}

		if ( ioctl( handle, DV1394_GET_STATUS, &dvst ) )
		{
			perror( "ioctl GET_STATUS" );
			return false;
		}
	}
	return true;
}


/** The thread responsible for polling the dv1394 interface.
 
    Though this is an infinite loop, it can be canceled by StopThread,
    but only in the pthread_testcancel() function.
 
*/
void* dv1394Reader::Thread( void* arg )
{
	dv1394Reader * theReader = ( dv1394Reader* ) arg;

	while ( true )
	{
		if ( ! Handler( theReader->m_dv1394_fd ) )
			break;
		pthread_testcancel();
	}
	return NULL;
}


/** Initializes the AVC object.
 
    \param p the number of the interface card to use (port)
 */


AVC::AVC( int p ) : port( p )
{
	pthread_mutex_init( &avc_mutex, NULL );
	avc_handle = NULL;
	int numcards;
	struct raw1394_portinfo pinf[ 16 ];

	try
	{

#ifdef LIBRAW1394_OLD
		avc_handle = raw1394_get_handle();
#else

		avc_handle = raw1394_new_handle();
#endif

		if ( avc_handle == 0 )
			return ;
		fail_neg( numcards = raw1394_get_port_info( avc_handle, pinf, 16 ) );
		fail_neg( raw1394_set_port( avc_handle, port ) );

		raw1394_set_bus_reset_handler( avc_handle, this->ResetHandler );

	}
	catch ( string exc )
	{
		if ( avc_handle != NULL )
			raw1394_destroy_handle( avc_handle );
		avc_handle = NULL;
		cerr << exc << endl;
	}
	return ;
}


/** Destroys the AVC object.
 
*/

AVC::~AVC()
{
	if ( avc_handle != NULL )
	{
		pthread_mutex_lock( &avc_mutex );
		raw1394_destroy_handle( avc_handle );
		avc_handle = NULL;
		pthread_mutex_unlock( &avc_mutex );
	}
}


int AVC::ResetHandler( raw1394handle_t handle, unsigned int generation )
{
	cerr << "Reset Handler received" << endl;
	raw1394_update_generation( handle, generation );
	//common->getPageCapture()->driver_locked = true;
	return 0;
}


/** See if a node_id is still valid and pointing to an AV/C Recorder.
 
	If the node_id is not valid, then look for the first AV/C device on
	the bus;
	
	\param phyID The node_id to check.
	\return The same node_id if valid, a new node_id if not valid and a
	        another AV/C recorder exists, or -1 if not valid and no
			AV/C recorders exist.
   
*/
int AVC::isPhyIDValid( int phyID )
{
	int value = -1;
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		int currentNode, nodeCount;
		rom1394_directory rom1394_dir;

		nodeCount = raw1394_get_nodecount( avc_handle );

		if ( phyID >= 0 && phyID < nodeCount )
		{
			rom1394_get_directory( avc_handle, phyID, &rom1394_dir );
			if ( rom1394_get_node_type( &rom1394_dir ) == ROM1394_NODE_TYPE_AVC )
			{
				if ( avc1394_check_subunit_type( avc_handle, phyID, AVC1394_SUBUNIT_TYPE_VCR ) )
					value = phyID;
			}
			rom1394_free_directory( &rom1394_dir );
		}

		// look for a new AVC recorder
		for ( currentNode = 0; value == -1 && currentNode < nodeCount; currentNode++ )
		{
			rom1394_get_directory( avc_handle, currentNode, &rom1394_dir );
			if ( rom1394_get_node_type( &rom1394_dir ) == ROM1394_NODE_TYPE_AVC )
			{
				if ( avc1394_check_subunit_type( avc_handle, currentNode, AVC1394_SUBUNIT_TYPE_VCR ) )
				{
					// set Preferences to the newly found AVC node and return
					octlet_t guid = rom1394_get_guid( avc_handle, currentNode );
					//snprintf( Preferences::getInstance().avcGUID, 64, "%08x%08x", (quadlet_t) (guid>>32),
					//(quadlet_t) (guid & 0xffffffff) );
					value = currentNode;
				}
			}
			rom1394_free_directory( &rom1394_dir );
		}
	}
	pthread_mutex_unlock( &avc_mutex );
	return value;
}


/** Do not do anything but let raw1394 make necessary
    callbacks (bus reset)
*/
void AVC::Noop( void )
{
	struct pollfd raw1394_poll;
	raw1394_poll.fd = raw1394_get_fd( avc_handle );
	raw1394_poll.events = POLLIN;
	raw1394_poll.revents = 0;
	if ( poll( &raw1394_poll, 1, 100 ) > 0 )
	{
		if ( ( raw1394_poll.revents & POLLIN )
		        || ( raw1394_poll.revents & POLLPRI ) )
			raw1394_loop_iterate( avc_handle );
	}
}


int AVC::Play( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
		{
			if ( !avc1394_vcr_is_recording( avc_handle, phyID ) &&
				avc1394_vcr_is_playing( avc_handle, phyID ) != AVC1394_VCR_OPERAND_PLAY_FORWARD )
					avc1394_vcr_play( avc_handle, phyID );
		}
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}


int AVC::Pause( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
		{
			if ( !avc1394_vcr_is_recording( avc_handle, phyID ) &&
				( avc1394_vcr_is_playing( avc_handle, phyID ) != AVC1394_VCR_OPERAND_PLAY_FORWARD_PAUSE ) )
					avc1394_vcr_pause( avc_handle, phyID );
		}
	}
	struct timespec t =
	    {
		    0, 250000000
	    };
	nanosleep( &t, NULL );
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}


int AVC::Stop( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_stop( avc_handle, phyID );
	}
	struct timespec t =
	    {
		    0, 250000000
	    };
	nanosleep( &t, NULL );
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}


int AVC::Rewind( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_rewind( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}


int AVC::FastForward( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_forward( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::Forward( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_next( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::Back( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_previous( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::NextScene( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_next_index( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::PreviousScene( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_previous_index( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::Record( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_record( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

int AVC::Shuttle( int phyID, int speed )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_trick_play( avc_handle, phyID, speed );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

unsigned int AVC::TransportStatus( int phyID )
{
	quadlet_t val = 0;
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			val = avc1394_vcr_status( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return val;
}

bool AVC::Timecode( int phyID, char* timecode )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
		{
			quadlet_t request[ 2 ];
			quadlet_t *response;

			request[ 0 ] = AVC1394_CTYPE_STATUS | AVC1394_SUBUNIT_TYPE_TAPE_RECORDER | AVC1394_SUBUNIT_ID_0 |
			               AVC1394_VCR_COMMAND_TIME_CODE | AVC1394_VCR_OPERAND_TIME_CODE_STATUS;
			request[ 1 ] = 0xFFFFFFFF;
			response = avc1394_transaction_block( avc_handle, phyID, request, 2, 1 );
			if ( response == NULL )
			{
				pthread_mutex_unlock( &avc_mutex );
				return false;
			}

			sprintf( timecode, "%2.2x:%2.2x:%2.2x:%2.2x",
			         response[ 1 ] & 0x000000ff,
			         ( response[ 1 ] >> 8 ) & 0x000000ff,
			         ( response[ 1 ] >> 16 ) & 0x000000ff,
			         ( response[ 1 ] >> 24 ) & 0x000000ff );
		}

	}
	pthread_mutex_unlock( &avc_mutex );
	return true;
}

int AVC::getNodeId( const char *guid )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		for ( int currentNode = 0; currentNode < raw1394_get_nodecount( avc_handle ); currentNode++ )
		{
			octlet_t currentGUID = rom1394_get_guid( avc_handle, currentNode );
			char currentGUIDStr[ 65 ];
			snprintf( currentGUIDStr, 64, "%08x%08x", ( quadlet_t ) ( currentGUID >> 32 ),
			          ( quadlet_t ) ( currentGUID & 0xffffffff ) );
			if ( strncmp( currentGUIDStr, guid, 64 ) == 0 )
			{
				pthread_mutex_unlock( &avc_mutex );
				return currentNode;
			}
		}
		pthread_mutex_unlock( &avc_mutex );
	}
	return -1;
}

int AVC::Reverse( int phyID )
{
	pthread_mutex_lock( &avc_mutex );
	if ( avc_handle != NULL )
	{
		if ( phyID >= 0 )
			avc1394_vcr_reverse( avc_handle, phyID );
	}
	pthread_mutex_unlock( &avc_mutex );
	return 0;
}

/** Start receiving DV frames
 
    The received frames can be retrieved from the outFrames queue.
 
*/
bool pipeReader::StartThread()
{
	pthread_mutex_lock( &mutex );
	currentFrame = NULL;
	pthread_create( &thread, NULL, Thread, this );
	pthread_mutex_unlock( &mutex );
	return true;
}


/** Stop the receiver thread.
 
    The receiver thread is being canceled. It will finish the next
    time it calls the pthread_testcancel() function. We also remove all frames 
    in the outFrames queue that have not been processed until now.
 
*/
void pipeReader::StopThread()
{
	pthread_mutex_lock( &mutex );
	pthread_cancel( thread );
	pthread_join( thread, NULL );
	Flush();
	pthread_mutex_unlock( &mutex );
}


bool pipeReader::Handler()
{
	bool ret = true;

	pthread_mutex_lock( &mutex );
	if ( currentFrame == NULL && inFrames.size() > 0 )
	{
		currentFrame = inFrames.front();
		currentFrame->bytesInFrame = 0;
		inFrames.pop_front();
		//printf("reader < buf: buffer %d, output %d\n", inFrames.size(), outFrames.size());
		//fflush(stdout);
	}
	pthread_mutex_unlock( &mutex );
	if ( currentFrame != NULL )
	{
		ret = ( fread( currentFrame->data, 120000, 1, stdin ) == 1 );
		if ( ret && currentFrame->data[ 3 ] & 0x80 )
		{
			ret = ( fread( currentFrame->data + 120000, 24000, 1, stdin ) == 1 );
			if ( ret )
				currentFrame->bytesInFrame = 144000;
		}
		else if ( ret )
		{
			currentFrame->bytesInFrame = 120000;
		}
		if ( ret )
		{
			pthread_mutex_lock( &mutex );
			outFrames.push_back( currentFrame );
			currentFrame = NULL;
			TriggerAction( );
			pthread_mutex_unlock( &mutex );
			//printf("reader > out: buffer %d, output %d\n", inFrames.size(), outFrames.size());
			//fflush(stdout);
		}
	}
	return ret;
}


/** The thread responsible for polling the rawdv interface.
 
    Though this is an infinite loop, it can be canceled by StopThread,
    but only in the pthread_testcancel() function.
 
*/
void* pipeReader::Thread( void* arg )
{
	pipeReader * theReader = ( pipeReader* ) arg;

	while ( true )
	{
		if ( ! Handler() )
			break;
		pthread_testcancel();
	}
	cerr << "End of pipe\n";
	pthread_mutex_lock( &mutex );
	currentFrame = NULL;
	outFrames.push_back( currentFrame );
	TriggerAction( );
	pthread_mutex_unlock( &mutex );
	return NULL;
}


