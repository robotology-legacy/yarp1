// -----------------------------------
// ControlThread abstract class
// -----------------------------------

#include <yarp/YARPThread.h>

// a control thread controls a piece of hardware via teleoperation.

class ControlThread : public YARPThread {
public:
	ControlThread(const double frequency, YARPSemaphore& sema)
		: _streamingFrequency(frequency), _sema(sema) {}
	virtual void Body (void) = 0;
	virtual void calibrate(void) = 0;
	virtual void initialise(void) = 0;
	virtual void shutdown(void) = 0;
protected:
	const double _streamingFrequency;
	YARPSemaphore& _sema;
};
