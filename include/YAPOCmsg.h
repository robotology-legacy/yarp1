/* this class is used as a structure for encoding commands and their input
parameters. it's currently passed from clients to YAPOC. it provides conversion
function which client can use for converting position input (from radians to
ticks, since YAPOC assumes all position commands are given in ticks). */


#ifndef _YAPOC_MSG_H_
#define _YAPOC_MSG_H_

#include "conf/K4_data.h"

//legal values for the status bits
#define OFF 0
#define ON 1

//convert parameter filename
#define CONVERT_FILENAME  "/ai/ai/mb/cdp/src/YARP/conf/K4convert.ini"

//not currently used
#define CALIBRATE_EACH 1

//CALIBRATE_ALL used to calibrate the dofs
//     input: none
//     result: 
//        for MEI the origins of the dofs will be set to the current
//        positions except for the eye tilt
//        for JCAR who knows
//        returns before the calibration completes
//        **commands sent prior to completion are dropped**
#define CALIBRATE_ALL 2

//POSITIONCOMMAND used to set the position of all dof
//   input : params and status bits
//   result: moves all requested dofs w/ status bit set to ON (1)
//           ignore dofs w/ status bit set to OFF (0)
//           returns prior to completion of the move
//          **commands sent prior to completion are dropped**
#define POSITIONCOMMAND 3

//for resetting MEI board
#define MEID_RESET 4

//for resetting encoder value
#define ZERO_ENCODER 5

//for closing MEI Daemon
#define MEID_CLOSE 6

//for resetting JRKERR board
#define PIC_RESET 7

//for adjusting JRKERR PID gains
#define SET_PICGAIN 8

//SET_VELOCITY used to set the velocity of all dof
//   input : params and status bits
//   result: set velocity for all requested dofs w/ status bit set to ON (1)
//           ignore dofs w/ status bit set to OFF (0)
//           returns prior to completion of the move
#define SET_VELOCITY 9

//total number of commands
#define NCOMMANDTYPES 9

//motor's virtual numbers
#define NECK_TILT 0
#define LEFTEYE_PAN 1
#define RIGHTEYE_PAN 2
#define EYE_TILT 3
#define NECK_PAN 4
#define HEAD_ROLL 5
#define HEAD_TILT 6
#define NECK_CRANE 7
#define LEFT_EAR_PAN 8
#define LEFT_EAR_SHORT 9
#define LEFT_EAR_LONG 10
#define LEFT_EAR_TILT 11
#define RIGHT_EAR_PAN 12
#define RIGHT_EAR_LONG 13
#define RIGHT_EAR_SHORT 14
#define RIGHT_EAR_TILT 15
#define RIGHT_BROW_RAISE 16
#define RIGHT_BROW_ANGLE 17
#define LEFT_BROW_RAISE 18
#define LEFT_BROW_ANGLE 19
#define RIGHT_LOWER_LID 20
#define LEFT_UPPER_LID 21
#define RIGHT_UPPER_LID 22
#define LEFT_LOWER_LID 23
#define RIGHT_UPPER_LIP 24
#define LEFT_UPPER_LIP 25
#define RIGHT_LOWER_LIP 26
#define LEFT_LOWER_LIP 27
#define BASE_PAN 28
#define JAW 29




/* message structures */

class YAPOCMessage
{
public:
  short type;  //message type
  short axisvnumber;  //axis number
  short gains[N_PICGAINPARAM];  //PIC gain values
  float params[MAXVIRTUALAXES];  //used for both positioncommand and setvelocity
  short statusbits[MAXVIRTUALAXES]; //used for both positioncommand and setvelocity
  double m_etoangles[MAXVIRTUALAXES]; //used for converting from ticks to radian
  short uselimit_flag; //flag to indicate whether limits should be used in a move command
                      //only used for calibrating eye tilt and lids 
                      //1 = use limit, 0 = don't use limit
  //constructor
  YAPOCMessage();
  ~YAPOCMessage();
  
  //utility functions
  void SetParamsArg(float *pos, short* statusbits);
  void SetGainsArg(short *gains);
  void ReadConvertParams(const char *filename);
  //functions for creating each message type
  
  void CreateCalibrateAllMsg();
  void CreateCalibrateEachMsg(short axis);
  void CreatePositionCommandMsg(float *params, short *statusbits, short ulim);
  void CreateMEIDResetMsg();
  void CreateZeroEncoderMsg(short axis);
  void CreateMEIDCloseMsg();
  void CreatePICResetMsg();
  void CreateSetPICGainMsg(short *gains);
  void CreateSetVelocityMsg(float *params, short *statusbits);
  
  //conversion function
  float * ConvertFromRadian(float *vals, float *newvals);
  
};

#endif 









