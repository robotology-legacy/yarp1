// by nat June 2003


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPRateThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>

#include <YARPForceSensor.h>

int ParseParams (int argc, char *argv[]) 
{


	return YARP_OK; 
}

class Thread : public YARPRateThread
{
public:
	Thread():YARPRateThread("force", 100)
	{
		_forces = new double[3];
		_torques = new double [3];
	}
	~Thread()
	{
		delete [] _forces;
		delete [] _torques;
	}

	void doInit()
	{
		fs.initialize();
	}

	void doLoop()
	{
	
		fs.read(_forces, _torques);
		for(int i = 0; i < 3; i++)
			cout << _forces[i] << '\t';

		cout << '\n';
	}

	void doRelease()
	{
		fs.uninitialize();
	}

	
	YARPBabybotForceSensor fs;
	double *_forces;
	double *_torques;
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



