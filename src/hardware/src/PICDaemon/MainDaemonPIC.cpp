/*
 * main.c -- InterSense Daemon main loop
 *
 * created.  Eduardo 11/25/01
 * modified.  Lijin 4/02
 */

#include <sys/kernel.h>
#include <iostream.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/name.h>
#include <sys/sched.h>
#include <stdlib.h>
#include <string.h>

#include "DaemonPIC.h"
#include "DaemonPICMessages.h"
#include "stdio.h"
#include "q_controller.h"
#include "q_sio_util.h"

int GlobalVariable=1;
CDaemonPIC Daemon;

/****************************************************************************/
/*Function terminate() -This function is called when a terminate signal is */
/*                      sent to the driver.  This function will detach the */
/*                      device and the prefix.                              */
/*Parameters Passed : none                                                  */
/****************************************************************************/
void terminate()
{
     printf("Start terminate\n");
     Daemon.PICS.NmcChangeBaud(0XFF, 19200);
     printf("Changed baud rate\n");
     qnx_name_detach(0, Daemon.name_id);
     printf("qnx name detached \n");
     close(1);
     exit(0); 
 }


/* PIC Daemon's main function.
After initialization, Daemon goes into a continuous loop servicing requests 
from PIC Clients.
*/

void main(int argc, char *argv[])
{
  
  char reply=0;
  //CDaemonPIC 			Daemon;
  struct _type_readall AllArm;
  int j;
  pid_t pid;
  int t=0;
  time_t start_time,end_time;
  
  for (j=0;j<3;j++)
    {
      AllArm.ADval[j]=0;
    }
  
  for (j=0;j<8;j++)
    {
      AllArm.Digital[j]=0;
    }
  
  for (j=0;j<3;j++)
    {
      AllArm.encoder[j]=0;
    }

	
  /* parse the command line */
  //parse_options(argc, argv, &di);
  
  // Setup the terminate signal handler function.
  for (int si = _SIGMIN; si <= _SIGMAX; si++)
    if (si != SIGTERM) signal( si, SIG_IGN);
  signal(SIGTERM, (void (* )(int))&terminate);

  //assign procname
  Daemon.procname = argv[1];
  Daemon.serialport = argv[2];
  printf("Process name = %s Serial port = %s\n",Daemon.procname,Daemon.serialport);
  /* initialize the resource manager */
  printf("Executing manager init\n");
  Daemon.manager_init();
  printf("Manager init done\n");
  /* do any hardware setup required when initializing */
  Daemon.hardware_init();
  printf("Hardware init done\n");
  
  printf("Updated\n");
  start_time=time(NULL);

/**************** Message processing loop. ****************************/
  while(1)
  {
    //printf("Waiting for message...\n");
    pid = Receive(0, &(Daemon.io_msg), sizeof(Daemon.io_msg));
    
    
    if (pid!=-1)
      {
	//Read message and perform requests

	if (Daemon.io_msg.id != _D_READENCODER)
	  printf("Receiving Data. ID= %d\n",Daemon.io_msg.id);
	switch(Daemon.io_msg.id) 
	  {
	    
	  case _D_SERVOSETGAIN:
	    reply=Daemon.D_ServoSetGain();
	    break;
	  case _D_MOVEPOSNOWAIT:
	    reply=Daemon.D_MovePosNoWait();
	    break;
	  case _D_MOVEPOSWAIT:
	    reply=Daemon.D_MovePosWait();
	    break;
	  case _D_MOVEPWM:
	    reply=Daemon.D_MovePWM();
	    break;
	    
	  case _D_READENCODER:
	    reply=Daemon.D_ReadEncoder();
	    break;
	    
	  case _D_READAD:
	    reply=Daemon.D_ReadAD();
	    break;
	  case _D_READDIGITAL:
	    reply=Daemon.D_ReadDigital();
	    break;
	    
	  case _D_CALIBRATE_PWM:
	    reply=Daemon.D_CalibratePWM();
	    break;
	  case _D_CALIBRATE_POS:
	    reply=Daemon.D_CalibratePosition();
	    break;
	  case _D_READALL:
	    reply=Daemon.D_ReadAll(&AllArm,0);
	    break;
	  case _D_RESETENCODER:
	    reply=Daemon.D_ResetEncoder();
	    break;
	  case _D_GETNUMMOTORS:
	    reply=Daemon.D_GetNumMotors();
	    break;
	  case _D_ISMOVEDONE:
	    reply=Daemon.D_IsMoveDone();
	    
	  }

	//if reply is okay (non-zero), reply to clients
	if (reply)
	  Reply(pid, &(Daemon.io_msg), sizeof(Daemon.io_msg));
	
	
	}

	
  }
}








