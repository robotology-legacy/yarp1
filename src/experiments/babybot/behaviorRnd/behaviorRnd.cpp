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
	RBWaitMotion waitMotion("waiting on arm random motion");
	RBWaitMotion waitShake("waiting on arm shake");
	RBWaitMotion waitRest("wating on rest");
	
	// input states
	RBSimpleInput motionDone(YBVArmDone);
	RBSimpleInput start(YBVArmRndStart);
	RBSimpleInput stop(YBVArmRndStop);
	RBSimpleInput rest(YBVArmRest);
	RBSimpleInput restDone(YBVArmRestDone);
	///////////////////////////////////////

	// arm random movement
	_rnd.setInitialState(&waitIdle);
	_rnd.add(&start, &waitIdle, &initMotion);
	_rnd.add(NULL, &initMotion, &waitMotion);
	_rnd.add(&motionDone, &waitMotion, &initShake);
	_rnd.add(&rest, &waitMotion, &waitRest);

	// wrist shake
	_rnd.add(NULL, &initShake, &waitShake);
	_rnd.add(&motionDone, &waitShake, &initMotion);
	_rnd.add(&rest, &waitShake, &waitRest);

	// wait rest 
	_rnd.add(&restDone, &waitRest, &initMotion);
	
	// stop states
	_rnd.add(&stop, &waitMotion, &waitIdle);
	_rnd.add(&stop, &waitShake, &waitIdle);

	_rnd.Begin();
	_rnd.loop();
	// _rnd.Join();

	return 0;
}
