// headsmooth.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPBottleContent.h>
#include <YARPString.h>
#include <YARPTime.h>
#include <YARPParseParameters.h>

#include "smoothcontrol.h"

const int __inSize = 2;
const int __outSize = 5;
const char *__baseName = "/headsmooth/";
const char *__configFile = "headcontrol.ini";

int main(int argc, char* argv[])
{
	bool inhibitNeck = false;
	YARPString basename;
	YARPBottle b;
	
	YARPScheduler::setHighResScheduling();

	YVector _in(__inSize);
	YVector _out(__outSize);
	YARPInputPortOf<YVector> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	SmoothControl _control(__configFile, __inSize, __outSize);

	// PARSE
	if (!YARPParseParameters::parse(argc, argv, "name", basename))
		basename = YARPString(__baseName);

	if (YARPParseParameters::parse(argc, argv, "inhibitneck"))
	{
		inhibitNeck = true;
		_control.inhibitNeck();
	}
	//////////

	YARPString base1(basename);
	YARPString base2(basename);
	_inPort.Register(base1.append("i").c_str());
	_outPort.Register(base2.append("o").c_str());

	double time1;
	double time2;
	double period = 0.0;
	time1 = YARPTime::GetTimeAsSeconds();
	const int N = 200;
	int counter = 0;
	while(true)
	{
		counter++;
		_inPort.Read();
		
		_control.apply(_inPort.Content(), b);
		_outPort.Content() = b;
		// b.display();
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
