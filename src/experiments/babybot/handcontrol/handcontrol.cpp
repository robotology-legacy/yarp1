// handcontrol.cpp : Defines the entry point for the console application.
//

#include "HandThread.h"
#include <iostream>

#include "YARPNameClient.h"

#include "YARPScheduler.h"
#include "YARPConfigFile.h"

#include "HandBehavior.h"

char menu();

using namespace std;

char __configFile[] = "hand.ini";

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	cout << "Starting hand control...\n";
	int _hand_thread_rate;
	
	YARPConfigFile file;
	file.set("Y:\\conf\\babybot\\", __configFile);
	file.get("[THREAD]", "Rate", &_hand_thread_rate, 1);
		
	HandThread hand_thread(_hand_thread_rate,
							"hand thread",
							__configFile);

	hand_thread.start();

	HandBehavior _hand(&hand_thread, YBLabelMotor, "/handcontrol/behavior/i");
	
	HBWaitIdle waitIdle;
	HBWaitMotion waitMotion;
	HBInputCommand inputCmd;
	HBInputReset inputRst;
	HBShakeCmdInput shkFingersIn(YBVHandShake);
	HBShakeCmdOutput shakeCmd;
	HBCheckMotionDone checkMotionDone(YBVHandDone);
	HBOutputCommand outputCmd;
	
	_hand.setInitialState(&waitIdle);
	
	// reset encoders
	_hand.add(&inputRst, &waitIdle, &waitIdle);
	// single command sequence
	_hand.add(&inputCmd, &waitIdle, &waitMotion, &outputCmd);
	_hand.add(&checkMotionDone, &waitMotion, &waitIdle);
	// multiple command (shake) sequence
	_hand.add(&shkFingersIn, &waitIdle, &waitMotion, &shakeCmd);
	_hand.add(&checkMotionDone, &waitMotion, &waitIdle);
	
	// start
	_hand.Begin();
	// blocking loop
	_hand.loop();
	// close
	// _hand.Join();

	hand_thread.terminate(false);	// no timeout here, important !

	cout << "exiting...\n";

	return 0;
}

char menu()
{
	char c;
	cout << "--------------------\n";
	cout << "What ?\n";
	cout << "s: shake\n";
	cout << "e: exit :( \n";
	cout << "--------------------\n";

	cin >> c;
	return c;
}