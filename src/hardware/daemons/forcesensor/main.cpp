// by nat June 2003


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPRateThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>
#include <YARPPort.h>

#include <YARPForceSensor.h>

int ParseParams (int argc, char *argv[]) 
{


	return YARP_OK; 
}

class Thread : public YARPRateThread
{
public:
	Thread():YARPRateThread("force", 40),
	_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
	}
	~Thread()
	{
	}

	void doInit()
	{
		fs.initialize("Y:\\conf\\babybot\\forcesensor.ini");

		_outPort.Register("/force/o:1");

	}

	void doLoop()
	{
	
		fs.read(_forces, _torques);

		// for(int i = 0; i < 3; i++)
		//	printf("%.3lf\t",_torques[i]);

		//	cout << "\n";
		
		memcpy(_outPort.Content(), _torques, sizeof(double)*3);
		
		_outPort.Write();
	}

	void doRelease()
	{
		fs.uninitialize();
	}

	
	YARPBabybotForceSensor fs;
	YARPOutputPortOf<double [3]> _outPort;
	double _forces[3];
	double _torques[3];
};


int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling ();

	Thread _thread;

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



