// armtrigger.cpp : Defines the entry point for the console application.
//

#include "triggerBehavior.h"

int main(int argc, char* argv[])
{
	TBSharedData _data;
	TriggerBehavior _behavior(&_data);
	
	TBCheckFixated			_isFixated;
	TBCheckAlmostFixated	_isAlmostFixated;
	TBWaitIdle				_wait("wait", 0.4);
	TBWaitIdle				_waitRest("wait rest");
	TBWaitIdle				_waitSaccade("wait for a new saccade");
	TBWaitIdle				_waitFixated("fixation", 0.4);
	//TBWaitIdle				_waitAFixated("almost fixation", 0.4);
	TBWaitIdle				_waitReaching("reaching", 6);
	TBWaitIdle				_waitAck1("ack1");
	TBWaitIdle				_waitAck2("ack2");
	TBWaitIdle				_waitPrep("prep");
	TBSimpleOutput			_prepareReaching(YBVReachingPrepare);
	TBSimpleOutput			_reaching(YBVReachingReach);
	TBArmInput				_armAck(YBVArmIssuedCmd);
	TBArmInput				_armDone(YBVArmDone);
	TBArmInput				_armRest(YBVArmRest);			
	TBArmInput				_armRestDone(YBVArmRestDone);			
	TBArmInput				_armIsBusy(YBVArmIsBusy);		
	TBArmInput				_newSaccade(YBVSaccadeNewTarget);		

	_behavior.setInitialState(&_waitSaccade);
	// _behavior.add(&_armAck, &_waitAck, &_waitAck);

	// _behavior.add(&_isAlmostFixated, &_wait, &_waitAFixated, &_prepareReaching);

	_behavior.add(&_newSaccade, &_waitSaccade, &_waitFixated);
	_behavior.add(&_isFixated, &_waitFixated, &_waitAck1, &_reaching);
	_behavior.add(&_armIsBusy, &_waitAck1, &_waitRest);
	_behavior.add(&_armAck, &_waitAck1, &_waitReaching);
	_behavior.add(NULL, &_waitReaching, &_waitSaccade);
	// _behavior.add(NULL, &_wait, &_wait);
	// _behavior.add(NULL, &_wait, &_waitAck1, &_reaching);
	_behavior.add(&_armRest, &_wait, &_waitRest);
	
	_behavior.add(&_armRestDone, &_waitRest, &_waitSaccade);

	//	_behavior.add(&_armDone, &_waitAck, &_wait);
	//	_behavior.add(&_armRestDone, &_waitAck, &_wait);
	//	_behavior.add(&_armAck, &_waitAck, &_waitReaching);
	
	_behavior.Begin();
	_behavior.loop();
	
	return 0;
}


