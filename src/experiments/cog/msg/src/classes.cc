//
// classes.cc
//
//
//

#include "classes.h"


//
//
//
//
//
//
//
//

void ReachingThread::Body ()
{
	// Initialize.
	m_mode = 1;
	
	// for debugging.
	m_active3 = true;
	m_mode3debug = false;

	m_headpos.Register (HEADPOSITIONCHANNEL);
	m_armpos.Register (ARMPOSITIONCHANNEL);
	m_targetpos.Register (IMAGETARGETCHANNEL);

	m_ohead.Register (HEADPORTNAME);
	m_oarm.Register (ARMPORTNAME);

	m_msg_port.Register (MSGPORTNAME);
	m_ans_port.Register (MSGPORTREPLYNAME);

//	m_bh_channeli.Register (BEHAVIORINPUT);
//	m_bh_channelo.Register (BEHAVIOROUTPUT);

	// Main loop.
	while (1)
	{
		Lock ();
		
		switch (m_mode)
		{
		default:
		case 0:
			printf ("ReachingThread: something very wrong happened\n");
			break;

		case 1:
			m_mode_idle.Update (&m_mode_idle, this);
			break;

		case 2:
			m_mode_stdreach.Update (&m_mode_stdreach, this);
			break;

		case 3:
			if (m_active3)
			{
				m_mode_planar.Update (&m_mode_planar, this);
				if (m_mode3debug)
					m_active3 = false;
			}
			break;
		}

		Unlock ();

		// cycle delay (WARNING: not CONSTANT).
		YARPTime::DelayInSeconds (0.04);	// 40 ms.
	}
}


// change mode.
void ReachingThread::StartReaching ()
{
	Lock ();
	m_mode = 2;
	m_mode_stdreach.ResetState ();
	Unlock ();
}

void ReachingThread::StopReaching ()
{
	Lock ();
	m_mode = 1;
	m_mode_idle.ResetState ();
	Unlock ();
}

void ReachingThread::StartPlanar ()
{
	Lock ();
	m_mode = 3;
	m_mode_planar.ResetState ();
	Unlock ();
}

// DEBUG.
void ReachingThread::Mode3() 
{
	Lock ();
	m_active3 = true;
	Unlock ();
}

void ReachingThread::ToggleDebugMode3()
{
	Lock ();
	m_mode3debug = !m_mode3debug;
	if (m_mode3debug == false)
		m_active3 = true;
	else
		m_active3 = false;
	Unlock ();
}


//
//
//
//
//
void HighLevelPort::SendMsg (NetInt32 v)
{
	YARPBottleIterator it (Content());
	it.Reset();
	it.WriteVocab(v);
	Write(1);
}

void HighLevelPort::SendMsg (NetInt32 v, float x1, float x2)
{
	YARPBottleIterator it (Content());
	it.Reset();
	it.WriteVocab(v);
	it.WriteFloat(x1);
	it.WriteFloat(x2);
	Write(1);
}

void HighLevelPort::SendMsg (NetInt32 v, int x1)
{
	YARPBottleIterator it (Content());
	it.Reset();
	it.WriteVocab(v);
	it.WriteInt(x1);
	Write(1);
}



//
//
//
//
//
void HighLevelPortReply::OnRead(void)
{
	Read ();
	//printf ("RECEIVED a message in HighLevelPortReply\n");
	m_mutex.Wait();
	m_bottle = Content();
	m_mutex.Post();
}

void HighLevelPortReply::GetMsg (YARPBottle& b)
{
	m_mutex.Wait();
	b = m_bottle;
	m_mutex.Post();
}

int HighLevelPortReply::GetMsgBlock (NetInt32 v, float& x1, int& x2, double timeout)
{
	double initial = YARPTime::GetTimeAsSeconds();
	bool timedout = false;
	YARPBottleIterator it(m_bottle);

	double realtimeout = (timeout != -1.0) ? timeout : m_timeout;

	m_mutex.Wait();
	do
	{
		it.Attach (m_bottle);
//		it.Rewind();
		m_mutex.Post();
		timedout = (YARPTime::GetTimeAsSeconds() - initial > realtimeout) ? true : false;
		YARPTime::DelayInSeconds(0.02);
		m_mutex.Wait();
	}
	while (it.ReadVocab () != v && !timedout);

	if (!timedout)
	{
		x1 = it.ReadFloat();
		x2 = it.ReadInt();
		it.Reset ();
		it.WriteVocab(YBC_SHLC_NONE);
		m_mutex.Post();
		return 0;
	}

	x1 = 0;
	x2 = -1;
	it.Reset ();
	it.WriteVocab(YBC_SHLC_NONE);
	m_mutex.Post();
	return -1;
}

int HighLevelPortReply::GetMsgBlock (NetInt32 v, int& x1, double timeout)
{
	double initial = YARPTime::GetTimeAsSeconds();
	bool timedout = false;
	YARPBottleIterator it(m_bottle);

	double realtimeout = (timeout != -1.0) ? timeout : m_timeout;

	m_mutex.Wait();
	do
	{
		it.Attach (m_bottle);
//		it.Rewind();
		m_mutex.Post();
		timedout = (YARPTime::GetTimeAsSeconds() - initial > realtimeout) ? true : false;
		YARPTime::DelayInSeconds(0.02);
		m_mutex.Wait();
	}
	while (it.ReadVocab () != v && !timedout);

	if (!timedout)
	{
		x1 = it.ReadInt();
		it.Reset ();
		it.WriteVocab(YBC_SHLC_NONE);
		m_mutex.Post();
		return 0;
	}

	x1 = -1;
	it.Reset ();
	it.WriteVocab(YBC_SHLC_NONE);
	m_mutex.Post();
	return -1;
}

int HighLevelPortReply::GetMsgBlock (NetInt32 v, double timeout)
{
	double initial = YARPTime::GetTimeAsSeconds();
	bool timedout = false;
	YARPBottleIterator it(m_bottle);

	double realtimeout = (timeout != -1.0) ? timeout : m_timeout;

	m_mutex.Wait();
	do
	{
		it.Attach (m_bottle);
//		it.Rewind();
		m_mutex.Post();
		timedout = (YARPTime::GetTimeAsSeconds() - initial > realtimeout) ? true : false;
		YARPTime::DelayInSeconds(0.02);
		m_mutex.Wait();
	}
	while (it.ReadVocab () != v && !timedout);

	if (!timedout)
	{
		it.Reset ();
		it.WriteVocab(YBC_SHLC_NONE);
		m_mutex.Post();
		return 0;
	}

	it.Reset ();
	it.WriteVocab(YBC_SHLC_NONE);
	m_mutex.Post();
	return -1;
}



