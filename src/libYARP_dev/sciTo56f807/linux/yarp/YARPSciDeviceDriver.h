/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #nat#                         ///
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

///
/// $Id: YARPSciDeviceDriver.h,v 1.1 2005-02-19 20:51:00 natta Exp $
///
///

#ifndef __YARPSciDeviceDriverh__
#define __YARPSciDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>

/**
 * \file YARPSciDeviceDriver.h 
 * class for interfacing with the value can device driver.
 */

struct SciOpenParameters
{
	/**
	 * Constructor.
	 */
	SciOpenParameters (void)
	{
		// leave it empty for now
	}
};

/**
 * The sci device driver.
*/
class YARPSciDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPSciDeviceDriver >, public YARPThread
{
private:
	YARPSciDeviceDriver (const YARPSciDeviceDriver &);
	void operator=(const YARPSciDeviceDriver &);

public:
	/**
	 * Constructor.
	 */
	YARPSciDeviceDriver ();

	/**
	 * Destructor.
	 */
	virtual ~YARPSciDeviceDriver ();

	/**
	 * Opens the device driver.
	 * @param d is the pointer to the parameter structure which is expected to be
	 * of type SciCanOpenParameters.
	 * @return YARP_OK on success.
	 */ 
	virtual int open(void *d);

	/**
	 * Closes the device driver.
	 * @return YARP_OK on success.
	 */
	virtual int close(void);

protected:
	/*
	int getPosition(void *cmd);
	int getPositions(void *cmd);
	int getRefPosition (void *cmd);
	int getRefPositions(void *cmd);
	int setPosition(void *cmd);
	int setPositions(void *cmd);
	int getError(void *cmd);
	int setSpeed(void *cmd);
	int setSpeeds(void *cmd);
	int getSpeeds(void *cmd);
	int getRefSpeeds(void *cmd);
	int setAcceleration(void *cmd);
	int setAccelerations(void *cmd);
	int getRefAccelerations(void *cmd);
	int setOffset(void *cmd);
	int setOffsets(void *cmd);
	int setPid(void *cmd);
	int getPid(void *cmd);
	int setIntegratorLimit(void *cmd);
	int setIntegratorLimits(void *cmd);
	int definePosition(void *cmd);
	int definePositions(void *cmd);
	int enableAmp(void *cmd);
	int disableAmp(void *cmd);
	int controllerIdle(void *cmd);
	int controllerRun(void *cmd);
	int velocityMove(void *cmd);
	int setCommand(void *cmd);
	int setCommands(void *cmd);
	int getTorque(void *cmd);
	int getTorques(void *cmd);
	int readBootMemory(void *cmd);
	int writeBootMemory(void *cmd);
	int setSwPositiveLimit(void *cmd);
	int setSwNegativeLimit(void *cmd);
	int getSwPositiveLimit(void *cmd);
	int getSwNegativeLimit(void *cmd);
	int setTorqueLimit (void *cmd);
	int setTorqueLimits (void *cmd);
	int getTorqueLimit (void *cmd);
	int getTorqueLimits (void *cmd);
	int getErrorStatus (void *cmd);
	int checkMotionDone (void *cmd);

	int setDebugMessageFilter (void *cmd);
	int setDebugPrintFunction (void *cmd);
*/
protected:
	void *system_resources;
	YARPSemaphore _mutex;
};



#endif