// touch.cpp : Defines the entry point for the console application.
//

// by nat June 2003

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPRateThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPMath.h>

#include <yarp/YARPTouchBoard.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPString.h>

using namespace std;

const int __defaultRate = 40;
const YARPString __defaultName = "/touch/o";
const int __nSensors = 17;

int counter = 0;

// we keep a history of 3 elements and we apply a median filter
class MyThread : public YARPRateThread
{
public:
	MyThread (const char *name, int rate) : YARPRateThread(name, rate),
	_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
	{
		_name = YARPString(name);
		_readings.Resize(__nSensors);
		_readings = 0.0;

		int i;
		for(i = 0; i < 3; i++)
		{
			_history[i].Resize(__nSensors);
			_history[i] = 0.0;
		}
		_index = 0;
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
	
		_touchBoard.read(_history[_index].data());
		_filter();

		_outPort.Content() = _readings;
				
		_outPort.Write();

		_index++;
		if (_index == 3)
			_index = 0;
	}

	void doRelease()
	{
		_touchBoard.uninitialize();
	}

	void _filter()
	{
		double tmp[3];
//		ACE_OS::printf("%4d ", counter++);
		for(int j = 1; j<=__nSensors; j++)
		{
			tmp[0] = _history[0](j);
			tmp[1] = _history[1](j);
			tmp[2] = _history[2](j);

			sort(tmp);
			_readings(j) = tmp[1];
//			ACE_OS::printf("%3.lf ",tmp[1]);
		}
//		ACE_OS::printf("\r");
	}

	// bubble sort with 3 elements
	void sort(double *a)
	{
		int j; int i;
		double tmp;
		for (i=0; i<3; i++)
		{
			for (j=0; j<2; j++)
				if (a[j+1] < a[j])
				{  /* compare the two neighbors */
					tmp = a[j];         /* swap a[j] and a[j+1]      */
					a[j] = a[j+1];
					a[j+1] = tmp;
				}
		}
	}
	
	YARPTouchBoard _touchBoard;
	YARPOutputPortOf<YVector> _outPort;
	YVector _history[3];
	YVector _readings;
	YARPString _name;
	int _index;
};


int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling ();

	// parse command line
	int rate;
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "-p", &rate))
		rate = __defaultRate;

	printf("Using rate: %d\n", rate);

	if (!YARPParseParameters::parse(argc, argv, "-name", name))
		name = __defaultName;

	MyThread _thread(name.c_str(), rate);

	_thread.start();
	
	YARPString str;
	cout << "Type 'q!' to exit\n";
	while(cin >> str)
	{
		cout << "Type 'q!' to exit\n";
		if (str == "q!")
			break;
	}
	
	_thread.terminate();
	return YARP_OK;
	
	return 0;
}

