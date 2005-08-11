/*
* dvgrab.h -- DVGrab control class
* Copyright (C) 2003 Dan Dennedy <dan@dennedy.org>
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
*/


#ifndef _DVGRAB_H
#define _DVGRAB_H 1

#include <string>

#include <libraw1394/raw1394.h>
#include <pthread.h>

#include "ieee1394io.h"
#include "filehandler.h"
#include "frame.h"
#include "smiltime.h"

#include <stdint.h>

#define RAW_BUF_SIZE    (10240)

class DVgrab
{
private:
	/// the interface card to use (typically == 0)
	int	m_port;
	int m_node;

	bool m_reader_active;
	char *m_dst_file_name;
	int m_autosplit;
	int m_timestamp;
	int m_channel;
	int m_frame_count;
	int m_max_file_size;
	int m_file_format;
	bool m_open_dml;
	int m_frame_every;
	char *m_dv1394;
	int m_jpeg_quality;
	bool m_jpeg_deinterlace;
	int m_jpeg_width;
	int m_jpeg_height;
	bool m_jpeg_overwrite;
	int	m_dropped_frames;
	bool	m_interactive;
	int m_buffers;
	int	m_total_frames;
	std::string m_duration;
	SMIL::MediaClippingTime* m_timeDuration;
	bool	m_stdin;
	bool	m_noavc;
	uint64_t m_guid;

	static FileHandler *m_writer;
	bool m_captureActive;

	static pthread_mutex_t capture_mutex;
	static pthread_t capture_thread;
	AVC *m_avc;
	IEEE1394Reader *m_reader;
	static Frame *m_frame;

	unsigned int m_transportStatus;

	static void *captureThread( void* );

public:
	DVgrab( int argc, char *argv[] );
	~DVgrab();

	void getargs( int argc, char *argv[] );
	void startCapture();
	void stopCapture();
	void status();
	void captureThreadRun();
	bool execute( const char );
	void sendEvent( const char *format, ... );
	bool isPlaying();
	bool isInteractive()
	{
		return m_interactive;
	}
	bool done();
};

#endif
