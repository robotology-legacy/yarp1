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

#include <YARPForceSensor.h>
#include <string>

using namespace std;

const int __defaultRate = 40;
const string __defaultName = "/force/o:1";

bool parseParams (int argc, char *argv[], const string &key, string &out)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == string(argv[i]+1)) 
			{
				// found key
				i++;
				if (i==argc)
					return false;
				else if (argv[i][0] == '-')
					return false;
				{
					out = string (argv[i]);
					return true;
				}
			}
		}
	}
	return false; 
}

bool parseParams (int argc, char *argv[], const string &key)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == string(argv[i]+1))
				return true;
		}
	}
	return false; 
}

bool parseParams (int argc, char *argv[], const string &key, int *out) 
{
	string dummy;
	if (parseParams(argc, argv, key, dummy))
	{
		*out = atoi(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool parseParams (int argc, char *argv[], const string &key, char *out) 
{
	string dummy;
	if (parseParams(argc, argv, key, dummy))
	{
		strcpy(out, dummy.c_str());
		return true;
	}
	else
		return false;

}

class Thread : public YARPRateThread
{
public:
	Thread (const char *name, int rate):YARPRateThread(name, rate),
	_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		_name = string(name);
		_forces.Resize(6);
	}
	~Thread()
	{
	}

	void doInit()
	{
		fs.initialize("Y:\\conf\\babybot\\forcesensor.ini");

		_outPort.Register(_name.c_str());

	}

	void doLoop()
	{
	
		fs.read(_forces);

		// for(int i = 0; i < 3; i++)
		//	printf("%.3lf\t",_torques[i]);

		//	cout << "\n";
		
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
	string name;
	if (!parseParams(argc, argv, "p", &rate))
		rate = __defaultRate;

	if (!parseParams(argc, argv, "name", name))
		name = __defaultName;

	if (parseParams(argc, argv, "a"))
		cout << "-a foun ";

	Thread _thread(name.c_str(), rate);

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



