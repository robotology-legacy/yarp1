// headsink.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

#include "sink.h"

const int __rate = 20;
const char *__name = "headsink";
const char *__inifile = "headcontrol.ini";

int main(int argc, char* argv[])
{
	Sink _headSink(__rate, __name, __inifile);
	_headSink.start();

	while(true)
	{
		char c;
		cin >> c;
	}

	_headSink.terminate(false);

	return 0;
}
