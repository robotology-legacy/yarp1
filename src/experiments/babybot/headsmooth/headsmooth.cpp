// headsmooth.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPString.h>
#include <YARPTime.h>

#include "smoothcontrol.h"

const int __inSize = 2;
const int __outSize = 5;
const char *__baseName = "/headsmooth/";
const char *__configFile = "headcontrol.ini";

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	YVector _in(__inSize);
	YVector _out(__outSize);
	YARPInputPortOf<YVector> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YVector> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	SmoothControl _control(__configFile, __inSize, __outSize);

	YARPString base1(__baseName);
	YARPString base2(__baseName);
	_inPort.Register(base1.append("i").c_str());
	_outPort.Register(base2.append("o").c_str());

	YVector v(__outSize);
	v = 0.0;

	double time1;
	double time2;
	double period = 0.0;
	time1 = YARPTime::GetTimeAsSeconds();
	const int N = 1000;
	int counter = 0;
	while(true)
	{
		counter++;
		_inPort.Read();
		_control.apply(_inPort.Content(), v);
		_outPort.Content() = v;
		_outPort.Write();
		time2 = time1;
		time1 = YARPTime::GetTimeAsSeconds();

		period += (time1-time2);
		if (counter == N)
		{
			printf("average= %lf \n", period/N);
			period = 0.0;
			counter = 0;
		}
	}
	return 0;
}
