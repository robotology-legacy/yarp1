// behaviorRnd.cpp : Defines the entry point for the console application.
//

#include "rndBehavior.h"

int main(int argc, char* argv[])
{
	RndSharedData _data;

	RndBehavior _rnd(&_data);

	RBInitMotion initMotion;
	RBInitShake initShake;
	RBWaitIdle waitIdle;
	RBWaitMotion waitMotion;
	RBWaitMotion waitShake;
	RBCheckMotionDone motionDone;
	RBStart start;
	RBStop stop;

	_rnd.setInitialState(&waitIdle);
	_rnd.add(&start, &waitIdle, &initMotion);
	_rnd.add(NULL, &initMotion, &waitMotion);
	_rnd.add(&motionDone, &waitMotion, &initShake);
	_rnd.add(NULL, &initShake, &waitShake);
	_rnd.add(&motionDone, &waitShake, &initMotion);
	
	// stop states
	_rnd.add(&stop, &waitMotion, &waitIdle);
	_rnd.add(&stop, &waitShake, &waitIdle);

	_rnd.Begin();
	_rnd.loop();
	// _rnd.Join();

	return 0;
}
