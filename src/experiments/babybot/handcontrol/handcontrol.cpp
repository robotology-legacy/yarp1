// handcontrol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "HandThread.h"
#include <iostream>

#include "YARPNameClient.h"

#include "YARPScheduler.h"
#include "YARPConfigFile.h"

#include "HandBehaviors.h"

char menu();

using namespace std;

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	cout << "Starting hand control...\n";
	int _hand_thread_rate;
	
	YARPConfigFile file;
	file.set("Y:\\conf\\babybot\\", "hand.ini");
	file.get("[THREAD]", "Rate", &_hand_thread_rate, 1);
		
	HandThread hand_thread(_hand_thread_rate, "hand thread", "Y:\\conf\\babybot\\hand.ini");

	hand_thread.start();

	WaitState wait;
	MoveState shake;
//	MoveState move;
	
	HandBehavior _behavior(&hand_thread);

	_behavior.setInitialState(&wait);
	_behavior.add(NULL, &wait, &shake);
	_behavior.add(NULL, &shake, &wait);

	HandMsgHandler _messageH(&_behavior, 0, "/behaviors/i:hand");
	_messageH.add(1, sigFunction, &wait);
	_messageH.add(0, sigFunction, &shake);

	/*
	_messageH.add(3, sigFunction, &move);
	_behavior.add(NULL, &move, &wait);
	*/

	
	char c;
	while(true)
	{
		_messageH.handle();
	}
	 	/*		
	for(;;)
	{
		bool loop = true;
		char c = menu();
		switch (c)
		{
			case 's':
			{
				hand_thread._startShake.set();
				break;
			}
			case 'e':
				loop = false;
				break;
		}

		if (!loop)
			break;
	}*/

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