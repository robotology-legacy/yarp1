// behaviorRnd.cpp : Defines the entry point for the console application.
//

#include "rndBehavior.h"

const double __wrist[] = {0,0,0,7*degToRad,0,0};
const double __forearm[] = {0,0,7*degToRad,0,0,0};
const double __arm[] = {0,7*degToRad,0,0,0,0};

int main(int argc, char* argv[])
{
	RndSharedData _data;

	RndBehavior _rnd(&_data);

	RBInitMotion initMotion;
	RBInitShake initShakeWrist(YVector(6, __wrist), "wrist");
	RBInitShake initShakeForearm(YVector(6, __forearm), "forearm");
	RBInitShake initShakeArm(YVector(6, __arm), "arm");

	RBWaitIdle waitIdle;
	RBWaitMotion waitMotion("waiting on arm random motion");
	RBWaitMotion waitShakeWrist("waiting on wrist shake");
	RBWaitMotion waitShakeForearm("waiting on forearm shake");
	RBWaitMotion waitShakeArm("waiting on arm shake");

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
	_rnd.add(&motionDone, &waitMotion, &initShakeWrist);
	_rnd.add(&rest, &waitMotion, &waitRest);

	// shake sequences
	_rnd.add(NULL, &initShakeWrist, &waitShakeWrist);
	_rnd.add(&motionDone, &waitShakeWrist, &initShakeForearm);
	_rnd.add(NULL, &initShakeForearm, &waitShakeForearm);
	_rnd.add(&motionDone, &waitShakeForearm, &initShakeArm);
	_rnd.add(NULL, &initShakeArm, &waitShakeArm);
	_rnd.add(&motionDone, &waitShakeArm, &initMotion);
	
	// rest
	_rnd.add(&rest, &waitShakeWrist, &waitRest);
	_rnd.add(&rest, &waitShakeForearm, &waitRest);
	_rnd.add(&rest, &waitShakeArm, &waitRest);

	// wait rest 
	_rnd.add(&restDone, &waitRest, &initMotion);
	
	// stop states
	_rnd.add(&stop, &waitMotion, &waitIdle);
	_rnd.add(&stop, &waitShakeWrist, &waitIdle);
	_rnd.add(&stop, &waitShakeForearm, &waitIdle);
	_rnd.add(&stop, &waitShakeArm, &waitIdle);


	_rnd.Begin();
	_rnd.loop();
	
	return 0;
}
