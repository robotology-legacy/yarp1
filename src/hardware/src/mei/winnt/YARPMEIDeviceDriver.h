//
//
// Wrapper for the MEI motor board
//
// April 2003 -- by nat
//
// win32: link Medvc50f.lib 
// $Id: YARPMEIDeviceDriver.h,v 1.8 2003-05-21 13:27:22 natta Exp $
#ifndef __YARP_MEI_DEVICE_DRIVER__
#define __YARP_MEI_DEVICE_DRIVER__

#include <conf/YARPConfig.h>
#include <YARPDeviceDriver.h>
#include <YARPControlBoardUtils.h>
#include <YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>
// abstract, SYNC is the semaphore type.
//

typedef short int16;

struct MEIOpenParameters
{
	MEIOpenParameters()
	{
		hwnd = 0;
		nj = 8;
		meiPortAddr = 0x300;
	}

	void *hwnd;
	int nj;
	int meiPortAddr;
};

class YARPMEIDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPMEIDeviceDriver> 
{
public:
	YARPMEIDeviceDriver();
	~YARPMEIDeviceDriver();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

	//  functions
	int setSpeed(void *sp);
	int setPosition(void *pos);
	int setAcceleration(void *acc);
	int setPid(void *cmd);
	int getPosition(void *j);

	int setOutputPort(void *cmd);
	int getOutputPort(void *cmd);
	int setOutputBit(void *bit);
	int clearOutputBit(void *bit);

	int setSpeeds(void *spds);
	int setPositions(void *pos);
	int setAccelerations(void *acc);
	int getPositions(void *j);
	int setOffsets(void *offs);
	int setOffset(void *cmd);
	int vMove(void *spds);

	int definePositions(void *pos);
	int definePosition(void *cmd);

	int beginMotion(void *cmd);
	int beginMotions(void *d);

	int stopAxes(void *p);
	int readSwitches(void *p);

	int getSpeeds(void *sps);
	
	int getRefSpeeds(void *sps);
	int getRefAccelerations(void *accs);
	int getRefPositions(void *pos);
	int getPid(void *par);

	int getTorques(void *trqs);

	int getTorqueLimit(void *cmd);
	int getTorqueLimits(void *trqs);

	int setTorqueLimit(void *cmd);
	int setTorqueLimits(void *trqs);
	int getErrors(void *errs);
	int setIntLimits(void *lmts);
	int setIntLimit(void *cmd);

	int readInput(void *input);
	
	int initPortAsInput(void *p);
	int initPortAsOutput(void *p);
	int setAmpEnableLevel(void *cmd);
	int setAmpEnable(void *cmd);

	int disableAmp(void *axis);
	int enableAmp(void *axis);

	int controllerRun(void *axis);
	int controllerIdle(void *axis);
	int clearStop(void *axis);
	int setStopRate(void *axis);

	int setPositiveLimit(void *cmd);
	int setNegativeLimit(void *cmd);
	int setPositiveLevel(void *cmd);
	int setNegativeLevel(void *cmd);
	int checkMotionDone(void *cmd);
	int waitForMotionDone(void *cmd);
	int setCommands(void *cmd);
	int setCommand(void *cmd);

	int setHomeIndexConfig(void *cmd);
	int setHomeLevel(void *cmd);
	int setHome(void *cmd);
	
	int dummy(void *d);	// dummy function, for debug purpose

protected:
	int _njoints;   
	int _dsp_rate;

	double *_ref_speeds;
	double *_ref_accs;
	double *_ref_positions;
	int16 **_filter_coeffs;
		
	int16 *_all_axes;
	int *_events;

};

#endif // .h