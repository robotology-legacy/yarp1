// by nat June 2003


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPRobotHardware.h>

#include <yarp/YARPRateThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPMath.h>

//#include <yarp/YARPGenericForceSensor.h>
//#include <yarp/YARPJR3Adapter.h>
#include <yarp/YARPParseParameters.h>
#include <string>

//typedef YARPForceSensor<YARPJR3Adapter, YARPJR3Params> YARPBabybotForceSensor;

using namespace std;

const int __defaultRate = 40;
const YARPString __defaultName = "/wristforce/o";

class MyThread : public YARPRateThread
{
public:
	MyThread (const char *name, int rate):YARPRateThread(name, rate),
	_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		_name = string(name);
		_forces.Resize(6);
	}
	~MyThread()
	{
	}

	void doInit()
	{
		fs.initialize("Y:\\conf\\babybot\\forcesensor.ini");

		_outPort.Register(_name.c_str());

	}

	void doLoop()
	{
	
		fs.read(_forces.data());

		_outPort.Content() = _forces;
				
		_outPort.Write();
	}

	void doRelease()
	{
		fs.uninitialize();
	}

	
	YARPBabybotForceSensor fs;
	YARPOutputPortOf<YVector> _outPort;
	YVector _forces;
	string _name;
};


int main (int argc, char *argv[])
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
}