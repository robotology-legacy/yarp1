// DaemonSerial.h: interface for the CDaemonSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DAEMOMPIC)
#define _DAEMOMPIC
#include "DaemonPICMessages.h"
#include "q_controller.h"

/* PIC Daemon class */ 
class CDaemonPIC
{
public:

  char *procname;    /* registered name of the driver process */
  char *serialport;  /* serial port name */
  int priority;      /* priority of driver process */
  int name_id;       /* name_id returned by name_attach */
  char name[255];
  int nmod;

  DaemonPICMessage io_msg;
  Cqnxmc PICS;
  
 public:
  char D_ResetEncoder();
  char D_ReadAll(struct _type_readall *p,int sw);
  char D_CalibratePosition();
  char D_CalibratePWM();
  char D_MovePWM();
  char D_ReadDigital();
  char D_ReadAD();
  char D_ReadEncoder();
  char D_MovePosWait();
  char D_MovePosNoWait();
  char D_ServoSetGain();
  char D_GetNumMotors();
  char D_HardReset();
  char D_IsMoveDone();
  
  CDaemonPIC();
  virtual ~CDaemonPIC();
  void manager_init();
  void hardware_init();
  
};



#endif 
