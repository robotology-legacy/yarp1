// ClientSerial.h: interface for the CClientSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CLIENTPIC)
#define _CLIENTPIC

#include <stdio.h>
#include <stdlib.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <assert.h>
#include "DaemonPICMessages.h"

#define MAX_MOTORS 8

// this class provides helper functions for sending messages to the PIC Daemons
class CClientPIC  
{
public:
	
  
  DaemonPICMessage out_msg,in_msg;
  CClientPIC();
  virtual ~CClientPIC();
  
  pid_t pid;
  int nmotors;
  
  //default velocity and acceleration
  long PICdefaultvel[MAX_MOTORS];
  long PICdefaultacc[MAX_MOTORS];
  
  //software limits for each motor
  int limits_max[MAX_MOTORS];
  int limits_min[MAX_MOTORS];
  
  //flag to indicate whether each motor has been calibrated
  int calibrated[MAX_MOTORS];
  
  void initialize(char *isd_name, char *ini_filename);
  void initialize(char *isd_name);
  
  //the following functions send message to PIC Daemon
  char D_ReadAll(struct _type_readall *p);
  char D_MovePosWait(byte addr, byte mode, long pos, long vel, long acc, byte pwm, short ulim);
  char D_MovePosNoWait(byte addr, byte mode, long pos, long vel, long acc, byte pwm, short uselimit_flag);
  char D_MovePosNoWait(byte addr, byte mode, long pos, byte pwm, short uselimit_flag);
  char D_ServoSetGain(byte addr, short int kp, short int kd, short int ki,short int il,
		      byte ol, byte cl, short int el,byte sr, byte dc);
  char D_ReadEncoder(byte addr, char set,int * encoder);
  char D_ReadAD(byte addr, char set,char* p);
  char D_ReadDigital(byte addr, char set,char bitnumber, char* bitvalue);
  char D_CalibratePosition(byte addr,long despla, int maxerror);
  char D_CalibratePWM(byte addr,int pwm, int mindesp);
  char D_MovePWM(byte addr, int pwm);
  char D_ResetEncoder(byte addr);
  void D_GetNumMotors();
  char D_HardReset();
  char D_IsMoveDone(byte addr);
  void EatLine(FILE *fp);
  void ReadConfig(char * fname);
  void ResetCalibrated();
  int CalibrateAxis(int);
  void SetVelocity(byte addr, long newvel);

};

#endif 



