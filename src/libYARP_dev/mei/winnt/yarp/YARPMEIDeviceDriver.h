/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

//
//
// Wrapper for the MEI motor board
//
// April 2003 -- by nat
//
// win32: link Medvc60f.lib or later versions.
//
// $Id: YARPMEIDeviceDriver.h,v 1.2 2006-02-15 09:44:22 gmetta Exp $

#ifndef __YARP_MEI_DEVICE_DRIVER__
#define __YARP_MEI_DEVICE_DRIVER__

#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>

//
//
typedef short int16;

//
// constants - depend on the card type.
const int MAX_PORTS = 2;

struct MEIOpenParameters
{
	MEIOpenParameters()
	{
		hwnd = 0;
		nj = 8;
		meiPortAddr = 0x300;
		ioPorts = 0x00;
	}

	void *hwnd;
	int nj;
	int meiPortAddr;
	int ioPorts;		// bit 0 = 1 means port 0 -> output, and so on.
};

///
///
///
class YARPMEIDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPMEIDeviceDriver> 
{
public:
	YARPMEIDeviceDriver();
	~YARPMEIDeviceDriver();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

private:
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
	int safeVMove(void *spds);

	int definePositions(void *pos);
	int definePosition(void *cmd);

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
	
	int setCommands(void *cmd);
	int setCommand(void *cmd);

	int setHomeIndexConfig(void *cmd);
	int setHomeLevel(void *cmd);
	int setHome(void *cmd);
	int setHomeProcedure(void *cmd);

	// MOTION STATUS
	// non blocking call, check motion done and return
	int checkMotionDone(void *cmd);

	// blocking call, poll with sleep(time)
	// time is ms, 0: busy waiting
	int waitForMotionDone(void *cmd);
	int checkFramesLeft(void *cmd);
	int waitForFramesLeft(void *cmd);

	// analog input
	int readAnalog(void *cmd);
	int setAxisAnalog(void *cmd);
	
	int dummy(void *d);	// dummy function, for debug purpose

private:
	bool implemented;
	int _njoints;   
	int _dsp_rate;

	double *_ref_speeds;
	double *_ref_accs;
	double *_ref_positions;
	int16 **_filter_coeffs;
		
	int16 *_all_axes;
	int *_events;

	int16 *_winding;			/// counting how many times the encoder wraps up.
	double *_16bit_oldpos;		/// storing the old 16 bit position value (encoder).
	double *_position_zero;		/// the zero reading of the encoders.

	/// PASA:
	/// the dev driver doesn't have any control on the calibration of encoders!

	inline int _sgn(double x) { return (x>0) ? 1 : -1; }
	inline int round(double x) { return ((x)>=0?(int)((x)+0.5):(int)((x)-0.5)); }
};

#endif // .h