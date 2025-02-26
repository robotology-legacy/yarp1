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

#ifndef _IEEE1394IO_H
#define _IEEE1394IO_H 1

#include <libraw1394/raw1394.h>
#include <libraw1394/csr.h>
#include <libdv/dv1394.h>

#include <string>
using std::string;
#include <deque>
using std::deque;

class Frame;

class IEEE1394Reader
{
private:
	/// the number of frames that had to be thrown away because
	/// our inFrames queue did not contain available frames
	static int droppedFrames;

	/// a pointer to the frame which is currently been transmitted
	static Frame	*currentFrame;

	/// a list of empty frames
	static deque < Frame* > inFrames;

	/// a list of already received frames
	static deque < Frame* > outFrames;

public:

	IEEE1394Reader( int chn = 63, int frames = 50 );
	virtual ~IEEE1394Reader();

	// Mutex protected public methods
	virtual bool StartThread( void ) = 0;
	virtual void StopThread( void ) = 0;
	Frame* GetFrame( void );
	void DoneWithFrame( Frame* );
	int GetDroppedFrames( void );
	int GetOutQueueSize( void )
	{
		return outFrames.size();
	}
	int GetInQueueSize( void )
	{
		return inFrames.size();
	}

	// These two public methods are not mutex protected
	virtual bool Open( void ) = 0;
	virtual void Close( void ) = 0;

	static bool WaitForAction( );
	static void TriggerAction( );

	virtual bool StartReceive( void ) = 0;
	virtual void StopReceive( void ) = 0;

protected:
	/// the iso channel we listen to (typically == 63)
	int	channel;

	/// contains information about our thread after calling StartThread
	pthread_t thread;

	/// this mutex protects capture related variables that could possibly
	/// accessed from two threads at the same time
	static pthread_mutex_t mutex;

	// This condition and mutex are used to indicate when new frames are
	// received
	static pthread_mutex_t condition_mutex;
	static pthread_cond_t condition;

	void Flush( void );
};


class raw1394Reader: public IEEE1394Reader
{
private:

	/// the interface card to use (typically == 0)
	int	port;

	/// the handle to the ieee1394 subsystem
	static raw1394handle_t handle;

public:

	raw1394Reader( int crd = 0, int chn = 63, int frames = 50 );
	~raw1394Reader();

	bool Open( void );
	void Close( void );
	bool StartReceive( void );
	void StopReceive( void );
	bool StartThread( void );
	void StopThread( void );

private:
	static int Handler( raw1394handle_t handle, int channel, size_t length, quadlet_t *data );
	static void* Thread( void *arg );
};


class dv1394Reader: public IEEE1394Reader
{
private:

	static unsigned char *m_dv1394_map;
	static int m_dv1394_fd;
	char *m_device_file;

public:

	dv1394Reader( char *device, int chn = 63, int frames = 50 );
	~dv1394Reader();

	bool Open( void );
	void Close( void );
	bool StartReceive( void );
	void StopReceive( void );
	bool StartThread( void );
	void StopThread( void );

private:
	static bool Handler( int handle );
	static void* Thread( void *arg );
};


class AVC
{
private:
	/// the interface card to use (typically == 0)
	int	port;

	/// this mutex protects avc related variables that could possibly
	/// accessed from two threads at the same time
	static pthread_mutex_t avc_mutex;

	/// the handle to the ieee1394 subsystem
	static raw1394handle_t avc_handle;

public:
	AVC( int crd = 0 );
	~AVC();

	int isPhyIDValid( int id );
	void Noop( void );
	int Play( int id );
	int Pause( int id );
	int Stop( int id );
	int FastForward( int id );
	int Rewind( int id );
	int Forward( int id );
	int Back( int id );
	int NextScene( int id );
	int PreviousScene( int id );
	int Record( int id );
	int Shuttle( int id, int speed );
	unsigned int TransportStatus( int id );
	bool Timecode( int id, char* timecode );
	int getNodeId( const char *guid );
	int Reverse( int id );

private:
	static int ResetHandler( raw1394handle_t handle, unsigned int generation );

};


class pipeReader: public IEEE1394Reader
{
public:

	pipeReader( int frames = 50 ) :
			IEEE1394Reader( 0, frames )
	{}
	;
	~pipeReader()
	{}
	;

	bool Open( void )
	{
		return true;
	};
	void Close( void )
	{}
	;
	bool StartReceive( void )
	{
		return true;
	};
	void StopReceive( void )
	{}
	;
	bool StartThread( void );
	void StopThread( void );

private:
	static bool Handler();
	static void* Thread( void *arg );
};


#endif
