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
/// $Id: YARPPeakSerialDeviceDriver.h,v 1.7 2006-02-09 22:01:02 natta Exp $
///
///
/// Implements device driver for the Peak usb to can board. The driver
/// supposes a serial connection between the board and the dsp; the dsp
/// is used as slave. 
/// June 05 -- nat
/// Nov 05 -- added synchronization

#ifndef __YARPPeakSerialDeviceDriverh__
#define __YARPPeakSerialDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPPeakHelper.h>


/**
 * The Peak USB/CAN device driver.
*/
class YARPPeakSerialDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPPeakSerialDeviceDriver >
{
 private:
  YARPPeakSerialDeviceDriver (const YARPPeakSerialDeviceDriver &);
  void operator=(const YARPPeakSerialDeviceDriver &);

 public:
  /**
   * Constructor.
   */
  YARPPeakSerialDeviceDriver ();

  /**
   * Destructor.
   */
  virtual ~YARPPeakSerialDeviceDriver ();

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

 public: //later private:
	
  int getPosition(void *cmd);
  int setPosition(void *cmd);
  int setPositions(void *cmd);
  int setPositionMode(void *cmd);
  int setForceMode(void *cmd);
	
  int getPositions(void *cmd);
  int getSpeeds(void *cmd);
  int getAccelerations(void *cmd);
  int getSpeed(void *cmd);
  int getPWM(void *cmd);
  int getPWMs(void *cmd);
  int getTorques(void *cmd);
  int getTorque(void *cmd);
  int readAnalog(void *cmd);
  int servoHere(void *cmd);

  int getPIDError(void *cmd);
  int relativeMotion(void *cmd);
  int relativeMotionMultiple(void *cmd);

  int setOffset(void *cmd);
  int setOffsets(void *cmd);

  int setPid(void *cmd);
  int getPid(void *cmd);

  int setDebugPrintFunction (void *cmd);

  /**
   * Helpers to write/read from/to the serial port
   */
	
  // LATER: inline
  int _readUWord(char msg, char joint, unsigned int &value, char checkReply);
  int _readSWord(char msg, char joint, int &value, char checkReply);

  int _readU16Vector(char msg, double *v, int n, char checkReply);
  int _readS16Vector(char msg, double *v, int n, char checkReply);

  int _writeWord(char msg, char checkReply);
  int _writeWord(char msg, char joint, char checkReply);
  int _writeWord(char msg, char joint, int value, char checkReply);

  int _readPWMGroup(char msg, double *v, int n, char checkReply);

  int _writeVector(char msg, const int *values, int n, char checkReply);
  int _writeU16Vector(char msg, const double *values, int n, char checkReply);
  int _writeS16Vector(char msg, const double *values, int n, char checkReply);
 
  void readDebugger();
	
 protected:
  YARPSemaphore _mutex;
  PeakHelper _canPort;
  unsigned char _message[8];
  double *_tmpDouble;
  int _nj;
};

#endif
