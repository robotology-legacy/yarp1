// Hand thread
//
// Feb 2003 -- by nat

#ifndef __HAND_THREAD__
#define __HAND_THREAD__

#include <conf/YarpConfig.h>

#include <YARPRateThread.h>

#include "ace\high_res_timer.h"
#include "ace\time_value.h"

#include <fstream>

#include <YARPBabybotHand.h>

class HandThread : public YARPRateThread
{
public:
	HandThread(int rate, 
			   const char *name = "prova nome");
	virtual ~HandThread();

	void doInit()
	{
		_hand.initialize("Y:\\conf\\babybot\\hand.ini");
		_hand.idleMode();
		_hand.resetEncoders();

		_nit = 0;

	}

	void doLoop()
	{
		_nit++;
		timer.start_incr();
		_hand.input();

		// do nothing !

		_hand.output();
		timer.stop_incr();
	}

	void doRelease()
	{
		_hand.uninitialize();

		timer.elapsed_time_incr(period);

		ofstream tmp("q:\\Ini Files\\thread_stats.txt");
		tmp << period.usec()/(1000.0*_nit); 
		tmp.close();
	}

	void synchro()
	{	
		_hand.activatePID();
	}

	YARPBabybotHand _hand;

	unsigned int _nit;

	ACE_High_Res_Timer	timer;
	ACE_Time_Value		period;

};

#endif // .h