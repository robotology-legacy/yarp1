// DaemonSerial.cpp: implementation of the CDaemonSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "DaemonPIC.h"
#include <unistd.h>
#include <sys/name.h>
#include <sys/sched.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/psinfo.h>
#include <stdio.h>
#include <conio.h>

#include <termios.h>


#include <sys/dev.h>
#include <sys/kernel.h>
#include<sys/qioctl.h>
#include <i86.h>

#include <string.h>

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>

#define DEF_PRIORITY     10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDaemonPIC::CDaemonPIC()
{
  this->priority   = DEF_PRIORITY;
    printf("priority %d\n", this->priority);
}

CDaemonPIC::~CDaemonPIC()
{
	
}

/****************************************************************************/
/*Function manager_init()                                                   */
/*    -	This function is called from the main() during the 		 */
/*	initialization of the driver.  This function will 		 */
/*	register this application as a device driver, notify the */
/*	kernel that we are a server so that	we will receive 	 */
/*	system messages.						 */
/* Parameters Passed: char *devicename, int priority                        */
/****************************************************************************/
void CDaemonPIC::manager_init()
{
  int i;


  /* Boost our priority. */
  setprio(0, this->priority);
  printf("priority set\n");

  /* Allow only SIGTERM for graceful termination, ignore all others. */
  /* Ignore signals all signal except SIGTERM. */
 

  // Setup the terminate signal handler function. 
  /*
  for( i = _SIGMIN; i <= _SIGMAX; ++i) 
    if( i != SIGTERM) signal( i, SIG_IGN);
	signal(SIGTERM, (void (* )(int))&terminate);*/
  /* Get system messages. */
  if (qnx_pflags(~0, _PPF_SERVER, NULL, NULL)) {
    //perror("pflags");
    exit(-1);
  }
  printf("signal set\n");
  /* Attach name for our process */
  this->name_id = qnx_name_attach(0, this->procname);
  if (this->name_id == -1) {
    //perror("qnx_prefix_attach");
    exit(-1);
  }
  printf("name attached\n");
}


/****************************************************************************/
/*Function hardware_init()                                                   */
/*    -	This function is called from the main() during the 		 */
/*	initialization of the driver.  This function will 		 */
/*	initialize designated serial port and perform other hardware     */
/*      initialization.						 */
/* Parameters Passed: none                        */
/****************************************************************************/
void CDaemonPIC::hardware_init()
{
	
  int i;
  char datastr[80];
  nmod = PICS.NmcInit(serialport,19200);//Controllers on COM1, use 19200 baud
  printf("NMOD = %d\n",nmod);										   //Returns the number of modules found
  if (nmod!=0) 
    {
      for (i=0; i<nmod; i++)
	{
	  printf("i = %d ",i);
	  if (PICS.NmcGetModType((byte)(i+1)) == SERVOMODTYPE)  //Determine the type for module 1
	    {
	      printf("Module %d: PIC-SERVO Controller\n", i+1);
	    }
	  if (PICS.NmcGetModType((byte)(i+1)) == IOMODTYPE)
	    {
	      printf("Module %d: PIC-IO Controller\n", i+1);
	    }
	}
    }
  printf("Numero Modules= %d\n",nmod);
  
  if (!nmod)
    {	
      //PICS.NmcShutdown();
      exit(0); 
    }
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to set gain values to a particular motor. This function will */
/*      parse io_msg, retrieve motor's address & desired gain values, and set*/
/*      gain values as requested                  */
/* Parameters Passed: none                                                */
/****************************************************************************/
char CDaemonPIC::D_ServoSetGain()
{
  int naxis;
  naxis=io_msg.msg.fdServoSetGain.addr;
  PICS.ServoSetGain(naxis,				//axis = 1
		    io_msg.msg.fdServoSetGain.kp,	//Kp = 100
		    io_msg.msg.fdServoSetGain.kd,	//Kd = 1000
		    io_msg.msg.fdServoSetGain.ki,	//Ki = 0
		    io_msg.msg.fdServoSetGain.il,	//IL = 0
		    io_msg.msg.fdServoSetGain.ol,	//OL = 255
		    io_msg.msg.fdServoSetGain.cl,	//CL = 0
		    io_msg.msg.fdServoSetGain.el,	//EL = 4000
		    io_msg.msg.fdServoSetGain.sr,	//SR = 1
		    io_msg.msg.fdServoSetGain.dc	//DC = 0
		    );
  
  PICS.ServoStopMotor(naxis, AMP_ENABLE | MOTOR_OFF);  //enable amp
  PICS.ServoStopMotor(naxis, AMP_ENABLE | STOP_ABRUPT);  //stop at current pos.
  //PICS.ServoResetPos(naxis); 
  return(1);
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to move a particular motor. This function will */
/*      parse io_msg, retrieve motor's address & move parameters, and perform*/
/*      move as requested (and return w/out waiting for move to be completed */
/* Parameters Passed: none                                                */
/****************************************************************************/
char CDaemonPIC::D_MovePosNoWait()
{
  int naxis;
  
  
  naxis=io_msg.msg.fdServoLoadTraj.addr;
  
  if(io_msg.msg.fdServoLoadTraj.mode==0)
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
			 LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW, 
			 io_msg.msg.fdServoLoadTraj.pos,		//pos = 2000
			 io_msg.msg.fdServoLoadTraj.vel, 	//vel = 100,000
			 io_msg.msg.fdServoLoadTraj.acc, 		//acc = 100
			 io_msg.msg.fdServoLoadTraj.pwm			//pwm = 0
			 );
    }
  else
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
		LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | MOVE_REL | START_NOW, 
			 io_msg.msg.fdServoLoadTraj.pos,		//pos = 2000
			 io_msg.msg.fdServoLoadTraj.vel, 	//vel = 100,000
			 io_msg.msg.fdServoLoadTraj.acc, 		//acc = 100
			 io_msg.msg.fdServoLoadTraj.pwm			//pwm = 0
			 );
    }
  
  return(1);
}


/****************************************************************************/
/*    -	This function is the same as the one above, but it won't return until*//* move is completed */
/* Parameters Passed: none                                                */
/****************************************************************************/
char CDaemonPIC::D_MovePosWait()
{
  unsigned char statbyte;
  int naxis;
  
  naxis=io_msg.msg.fdServoLoadTraj.addr;
  
  PICS.ServoLoadTraj(naxis, 		//addr = 0
		     LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW, 
		     io_msg.msg.fdServoLoadTraj.pos,		//pos = 2000
		     io_msg.msg.fdServoLoadTraj.vel, 	//vel = 100,000
		     io_msg.msg.fdServoLoadTraj.acc, 		//acc = 100
		     io_msg.msg.fdServoLoadTraj.pwm			//pwm = 0
		     );
  
  
  do
    {
      PICS.NmcNoOp(naxis);	//poll controller to get current status data
      statbyte = PICS.NmcGetStat(naxis);
    }
  while ( !(statbyte & MOVE_DONE) ); 
  return(1);
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to check if previous move to a particular move has been  */
/*      completed. This function will parse io_msg, retrieve motor's address */
/*      and check its current status and put reply into io_msg               */
/* Parameters Passed: none                                                */
/****************************************************************************/
char CDaemonPIC::D_IsMoveDone()
{
  unsigned char statbyte;
  int naxis = io_msg.msg.fdIsMoveDone.addr;
  //printf("Check move status for axis %d\n",naxis);
  PICS.NmcNoOp(naxis); //poll controller to get current status data
  
  statbyte = PICS.NmcGetStat(naxis);
  // printf("statbyte = %d ",(int)statbyte); fflush(stdout);
  //printf("MOVEDONE = %d \n",(int)MOVE_DONE); fflush(stdout);
  if (statbyte & MOVE_DONE)
    io_msg.msg.fdIsMoveDone.done = 1;
  else
    io_msg.msg.fdIsMoveDone.done = 0;
  return(1);
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to read encoder value of a particular motor. This function */
/*      will parse io_msg, retrieve motor's address, retrieve encoder value, */
/*       and put reply into io_msg               */
/* Parameters Passed: none                                                */
/****************************************************************************/                   
char CDaemonPIC::D_ReadEncoder()
{
  long position;
  char msgstr[40];
  int naxis;
  
  naxis=io_msg.msg.fdReadEncoder.addr;
  
  
  if(io_msg.msg.fdReadEncoder.set)
    {
      
      //Cause PIC-SERVO controller to send back position data with each command
      //Position data will also be sent back with this command
      PICS.NmcDefineStatus(naxis,    		//addr = 1
			   SEND_POS    //Send back position data only
			   );
    }
  
  
  //Retrieve the position data from the local data structure
  position = PICS.ServoGetPos(naxis);
  
  io_msg.msg.fdReadEncoder.pos=position;
  
  return(1);
}

/* This function is not currently used in YAPOC */
char CDaemonPIC::D_ReadAD()
{
  int naxis;
  int j;
  
  naxis=io_msg.msg.fdReadAD.addr;
  
  if(io_msg.msg.fdReadAD.set)
    {
      PICS.NmcDefineStatus(naxis,    		//addr = 1
			   SEND_AD1|SEND_AD2|SEND_AD3   //Send back A/D data only
			   );
    }
  PICS.NmcNoOp(naxis);
  for (j=0;j<3;j++)
    {
      io_msg.msg.fdReadAD.ADval[j]=PICS.IoGetADCVal(naxis,j);	//addr = 1
    }	
  return(1);
}

/* This function is not currently used in YAPOC */
char CDaemonPIC::D_ReadDigital()
{
  int naxis;
  
  naxis=io_msg.msg.fdReadDigital.addr;
  if(io_msg.msg.fdReadDigital.set)
    {
      PICS.NmcDefineStatus(naxis,    		//addr = 1
			   SEND_INPUTS   //Send back Bit only
			   );	
    }
  
  PICS.NmcNoOp(naxis);
  io_msg.msg.fdReadDigital.value=PICS.IoInBitVal(naxis,io_msg.msg.fdReadDigital.nbit);
  
  
  return(1);
}

/* This function is not currently used in YAPOC */
char CDaemonPIC::D_MovePWM()
{
  int naxis;
  
  naxis=io_msg.msg.fdServoLoadTraj.addr;
  if(io_msg.msg.fdServoLoadTraj.dir>=0)
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
			 LOAD_PWM | START_NOW, 
			 0,		//pos = 2000
			 0, 	//vel = 100,000
			 0, 		//acc = 100
			 io_msg.msg.fdServoLoadTraj.pwm			//pwm = 0
			 );
    }
  else
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
			 LOAD_PWM | REVERSE | START_NOW, 
			 0,		//pos = 2000
			 0, 	//vel = 100,000
			 0, 		//acc = 100
			 io_msg.msg.fdServoLoadTraj.pwm			//pwm = 0
			 );
    }
  return(1);
}


/* This function is not currently used in YAPOC */
char CDaemonPIC::D_CalibratePWM()
{
  int naxis;
  long pos0,pos1,dpos;
  byte statbyte;
  int pwm;
  
  naxis=io_msg.msg.fdCalibrate.addr;
  pwm=io_msg.msg.fdCalibrate.pwm;
  
  PICS.ServoSetGain(naxis,     //axis = 1
		    1000,	//Kp = 100
		    100,	//Kd = 1000
		    0,		//Ki = 0
		    0,		//IL = 0
		    255,	//OL = 255
		    0,		//CL = 0
		    30000,	//EL = 4000
		    1,		//SR = 1
		    0		//DC = 0
		    );
  
  PICS.ServoStopMotor(naxis, AMP_ENABLE | MOTOR_OFF);  //enable amp
  PICS.ServoStopMotor(naxis, AMP_ENABLE | STOP_ABRUPT);  //stop at current pos.
  PICS.ServoResetPos(naxis); 
  
  PICS.ServoClearBits(naxis);
  PICS.NmcDefineStatus(naxis,SEND_POS);
  
  PICS.NmcNoOp(naxis);
  pos0 = PICS.ServoGetPos(naxis);
  
  if(pwm>=0)
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
			 LOAD_PWM |START_NOW, 
			 0,		//pos = 2000
			 0, 	//vel = 100,000
			 0, 		//acc = 100
			 pwm	//pwm = 0
			 );
	}
  else
    {
      PICS.ServoLoadTraj(naxis, 		//addr = 0
			LOAD_PWM |REVERSE |START_NOW, 
			 0,		//pos = 2000
			 0, 	//vel = 100,000
			 0, 		//acc = 100
			 -pwm	//pwm = 0
			 );
    }
  
  delay(1000);
  do
    {	delay(100);
    PICS.NmcNoOp(naxis);
    pos1 = PICS.ServoGetPos(naxis);
    dpos=labs(pos1-pos0);
    pos0=pos1;
    printf("%ld\n",dpos);
    
    }
  while(dpos>io_msg.msg.fdCalibrate.mindesp);
  
  PICS.ServoStopMotor(naxis, AMP_ENABLE | STOP_ABRUPT);  //stop at current pos.
  PICS.ServoClearBits(naxis);
  
  return(1);
}

/* This function is not currently used in YAPOC */
char CDaemonPIC::D_CalibratePosition()
{
  int naxis;
  long int despla;
  int maxerror;
  
  naxis=io_msg.msg.fdCalibrate.addr;
  
  despla=io_msg.msg.fdCalibrate.despla;
  maxerror=io_msg.msg.fdCalibrate.maxerror;
  
  PICS.ServoSetGain(naxis,     //axis = 1
		    1000,	//Kp = 100
		    100,	//Kd = 1000
		    0,		//Ki = 0
		    0,		//IL = 0
		    255,	//OL = 255
		    0,		//CL = 0
		    maxerror,	//EL = 4000
		    1,		//SR = 1
		    0		//DC = 0
		    );
  
  PICS.ServoStopMotor(naxis, AMP_ENABLE | MOTOR_OFF);  //enable amp
  PICS.ServoStopMotor(naxis, AMP_ENABLE | STOP_ABRUPT);  //stop at current pos.
  PICS.ServoResetPos(naxis); 
  
  PICS.ServoLoadTraj(naxis, 		//addr = 0
		LOAD_POS | LOAD_VEL | LOAD_ACC | ENABLE_SERVO|START_NOW, 
		     despla,		//pos = 2000
		     200000, 	//vel = 100,000
		     100, 		//acc = 100
		     0		//pwm = 0
		     );
  
  
  byte statbyte;
	PICS.ServoClearBits(naxis);
	do
	  {
	    PICS.NmcNoOp(naxis);	//poll controller to get current status data
		statbyte = PICS.NmcGetStat(naxis);
	  }
	while ( !(statbyte & POS_ERR) ); 
	PICS.ServoClearBits(naxis);
	PICS.ServoResetPos(naxis);
	
	return(1);
}

/* This function is not currently used in YAPOC */
char CDaemonPIC::D_ReadAll(_type_readall *p,int sw)
{
  
  int j;
  int naxis;
  
  if(sw==0)
    {
      
      for (j=0;j<3;j++)
	{
	  io_msg.msg.fdReadAll.ADval[j]=p->ADval[j];
	}
		for (j=0;j<8;j++)
		  {
		    io_msg.msg.fdReadAll.Digital[j]=p->Digital[j];
		}
		for (j=0;j<3;j++)
		  {
		    io_msg.msg.fdReadAll.encoder[j]=p->encoder[j];
		  }
	}
  else
    {
      naxis=1;
      PICS.NmcDefineStatus(naxis,SEND_AD1|SEND_AD2|SEND_AD3|SEND_INPUTS);
      PICS.NmcNoOp(naxis);
      for (j=0;j<3;j++)
	{
	  p->ADval[j]=PICS.IoGetADCVal(naxis,j);	
	}
      for (j=0;j<8;j++)
	{
			p->Digital[j]=PICS.IoInBitVal(naxis,j);
	}
      
      for(naxis=3;naxis<5;naxis++)
	{
	  //Cause PIC-SERVO controller to send back position data with each command
	  //Position data will also be sent back with this command
	  PICS.NmcDefineStatus(naxis, SEND_POS);    //Send back position data only
	  PICS.NmcNoOp(naxis);
	  //Retrieve the position data from the local data structure
	  p->encoder[naxis-2]= PICS.ServoGetPos(naxis);
	}	
      
    }
  
  return(1);
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to reset encoder value of a particular motor. This function */
/*      will parse io_msg, retrieve motor's address and reset encoder value */
/*       for that motor               */
/* Parameters Passed: none                                                */
/****************************************************************************/ 
char CDaemonPIC::D_ResetEncoder()
{
  int naxis;
  
  naxis=io_msg.msg.fdServoSetGain.addr;
  PICS.ServoResetPos(naxis); 
  return(1);
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request for the total number of motors in the cluster. This function */
/*      will put reply into io_msg             */
/* Parameters Passed: none                                                */
/****************************************************************************/ 
char CDaemonPIC::D_GetNumMotors()
{
  io_msg.msg.fdGetNumMotors.nmotors = nmod;
  return(1);
  
}

/****************************************************************************/
/*    -	This function is called from the main() upon receiving a client's */
/*      request to reset the JRKERR board.                                */
/* Parameters Passed: none                                                */
/****************************************************************************/ 
char CDaemonPIC::D_HardReset()
{
  PICS.NmcHardReset(0xFF);
  return(1);
}

