// behaviorRnd.cpp : Defines the entry point for the console application.
//

#include "rndBehavior.h"

int main(int argc, char* argv[])
{
	RndSharedData _data;

	RndBehavior _rnd(&_data);

	RBInitMotion initMotion;
	RBWaitIdle waitIdle;
	RBWaitMotion waitMotion;
	RBCheckMotionDone motionDone;
	RBStart start;
	RBStop stop;

	_rnd.setInitialState(&waitIdle);
	_rnd.Begin();
	_rnd.add(&start, &waitIdle, &initMotion);
	_rnd.add(&stop, &waitMotion, &waitIdle);
	_rnd.add(&motionDone, &waitMotion, &initMotion);
	_rnd.add(NULL, &initMotion, &waitMotion);

	_rnd.loop();

	_rnd.End();
	return 0;
}
