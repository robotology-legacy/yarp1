//
// Shared definition of channels.
//

#ifndef __channelsh__
#define __channelsh__

#include "portnames.h"

#include <stdio.h>
#include <conf/lp_layout.h>
#include <conf/tx_data.h>

// new.
#include <Models.h>
#include <VisMatrix.h>

// old.
#include <MotorCtrl.h>

#include "YARPPort.h"

//
//
// LATER: this classes can be templatized very easily.

class CPosVisualChannel : public YARPInputPortOf<StereoPosData>
{
protected:
	YARPSemaphore	m_mutex;
	StereoPosData	m_pos_data;
	clock_t			m_timestamp;
	int				m_period;

public:
	CPosVisualChannel (int period) 
		: YARPInputPortOf<StereoPosData>(YARPInputPortOf::DOUBLE_BUFFERS), m_mutex(1)
	{
		memset (&m_pos_data, 0, sizeof(StereoPosData));
		m_timestamp = 0;
		m_period = period;
	}

	~CPosVisualChannel () {}

	virtual void OnRead (void)
	{
		Read ();
		m_mutex.Wait();
		m_pos_data = Content();
		m_mutex.Post();
	}

	int GetContent (StereoPosData& data)
	{
		m_mutex.Wait();
		data = m_pos_data;
		m_mutex.Post();

		return 0;
	}
};

//
// a vritual channel to read disparity stuff.
class CDisparityChannel : public YARPInputPortOf<DisparityData>
{
protected:
	YARPSemaphore	m_mutex;
	DisparityData	m_pos_data;
	clock_t m_timestamp;
	int m_period;

public:
	CDisparityChannel (int period)
		: YARPInputPortOf<DisparityData>(YARPInputPortOf::DOUBLE_BUFFERS), m_mutex(1)
	{
		memset (&m_pos_data, 0, sizeof(DisparityData));
		m_period = period;
		m_timestamp = 0;
	}

	virtual ~CDisparityChannel ()
	{
	}

	virtual void OnRead (void)
	{
		Read ();
		m_mutex.Wait();
		m_pos_data = Content();
		m_mutex.Post();
	}

	int GetContent (DisparityData& data)
	{
		m_mutex.Wait();
		data = m_pos_data;
		m_mutex.Post();

		return 0;
	}
};


// additional messages to the head controller.
class CExternalMessages : public YARPInputPortOf<HeadMessage>
{
protected:
	HeadMessage	m_message;
	YARPSemaphore m_mutex;
	double m_lastmessagetime;

public:
	CExternalMessages () : YARPInputPortOf<HeadMessage> (YARPInputPort::DOUBLE_BUFFERS), m_mutex(1)
	{
		//m_port.Register (MESSAGESCHANNELNAME);
		memset (&m_message, 0, sizeof(HeadMessage));
	}

	virtual ~CExternalMessages ()
	{
	}
	
	virtual void OnRead ()
	{
		Read ();
		m_mutex.Wait();
		m_message = Content();
		m_lastmessagetime = YARPTime::GetTimeAsSeconds();
		m_mutex.Post();
		switch (Content().type)
		{
		case HeadMsgNonValid:
			printf ("Null msg received\n");
			break;
		case HeadMsgOffset:
			printf ("Head offset received\n");
			break;
		case HeadMsgStopNeckMovement:
			printf ("Head stop neck received\n");
			break;
		case HeadMsgStopAll:
			printf ("Head stop all received\n");
			break;
		case HeadMsgStopAllVisual:
			printf ("Head stop visual received\n");
			break;
		case HeadMsgMoveToPosition:
			printf ("Head move to position received\n");
			break;
		case HeadMsgMoveToOrientation:
			printf ("Head move to orientation\n");
			break;
		case HeadMsgMoveToOrientationRight:
			printf ("Head move to orientation right eye\n");
			break;
		default:
			printf ("Unknown message received - must be a BUG!\n");
			break;
		}
	}

	inline int GetMessage (HeadMessage& msg) 
	{ 
		m_mutex.Wait();
		msg = m_message; 
		m_mutex.Post();
		return 0;
	}

	inline int SetMessage (HeadMessage& msg)
	{
		m_mutex.Wait();
		m_message = msg; 
		m_mutex.Post();
		return 0;
	}

	inline double GetLastMessageTime (void)
	{
		double ret;
		m_mutex.Wait();
		ret = m_lastmessagetime;
		m_mutex.Post();
		return ret;
	}
};


#endif
