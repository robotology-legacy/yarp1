// touch.cpp : Defines the entry point for the console application.
//

// by nat June 2003

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPRateThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>
#include <YARPPort.h>
#include <YARPMath.h>
#include <YARPVectorPortContent.h>

#include <YARPTouchBoard.h>
#include <YARPParseParameters.h>
#include <YARPString.h>

using namespace std;

const int __defaultRate = 40;
const YARPString __defaultName = "/touch/o";
const int __nSensors = 17;

class MyThread : public YARPRateThread
{
public:
	MyThread (const char *name, int rate) : YARPRateThread(name, rate),
	_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
	{
		_name = YARPString(name);
		_readings.Resize(__nSensors);
		_readings = 0.0;
	}
	~MyThread()
	{
	}

	void doInit()
	{
		_touchBoard.initialize("Y:\\conf\\babybot\\touch.ini");

		_outPort.Register(_name.c_str());

	}

	void doLoop()
	{
	
		_touchBoard.read(_readings.data());

		_outPort.Content() = _readings;
				
		_outPort.Write();
	}

	void doRelease()
	{
		_touchBoard.uninitialize();
	}

	
	YARPTouchBoard _touchBoard;
	YARPOutputPortOf<YVector> _outPort;
	YVector _readings;
	YARPString _name;
};


int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling ();

	// parse command line
	int rate;
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "p", &rate))
		rate = __defaultRate;

	if (!YARPParseParameters::parse(argc, argv, "name", name))
		name = __defaultName;

	MyThread _thread(name.c_str(), rate);

	_thread.start();
	
	char c;
	cout << "Type 'e' to exit\n";
	while(cin >> c)
	{
		cout << "Type 'e' to exit\n";
		if (c == 'e')
			break;
	}
	
	_thread.terminate();
	return YARP_OK;
	
	return 0;
}

