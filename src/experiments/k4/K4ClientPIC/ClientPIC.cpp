// ClientSerial.cpp: implementation of the CClientPIC class.
//
//////////////////////////////////////////////////////////////////////

#include "ClientPIC.h"
#include <iostream.h>

#define DEBUG 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientPIC::CClientPIC()
{

}

CClientPIC::~CClientPIC()
{

}

/* initialization */

//to be used if controls want to move motors
/* this function takes 2 inputs: PIC Daemon's qnx process name and 
   configuration filename */
void CClientPIC::initialize(char *isd_name, char *ini_filename)
{
  //connect to daemon
  this->pid = qnx_name_locate(0, isd_name, 0, NULL); 
  //ask daemon for the number of motors
  D_GetNumMotors();
  //read configuration file: gains and limits_max/min
  ReadConfig(ini_filename);
  //reset calibrated flags
  ResetCalibrated();
  
}

//to be used if controls only want to read encoder values
/* this function takes 2 input: PIC Daemon's qnx process name */
void CClientPIC::initialize(char *isd_name)
{
  //connect to daemon
  this->pid = qnx_name_locate(0, isd_name, 0, NULL);
  //ask daemon for the number of motors
  D_GetNumMotors();
  //reset calibrated flags, this shouldn't be necessary but just in case
  ResetCalibrated();
}

/* NOTE:
the following functions composes an outgoing message (out_msg) to PIC Daemon 
using Send(..).
Upon receiving message, PIC Daemon puts corresponding reply into in_msg and 
Send(..) will return either 0 or -1 depending on whether any errors appeared.
*/


/* this function sends a request to PIC Daemon to set gain values for one motor
   (addr). 
it takes as input: motor's address and 9 desired gain & limit values. 
it returns 0 if no error and -1 otherwise */
char CClientPIC::D_ServoSetGain(byte addr, short int kp, short int kd, short int ki, short int il, byte ol, byte cl, short int el, byte sr, byte dc)
{
  int status;
  
  //set parameters for out_msg
  this->out_msg.id=_D_SERVOSETGAIN;
  this->out_msg.msg.fdServoSetGain.addr=addr;
  this->out_msg.msg.fdServoSetGain.kp=kp;
  this->out_msg.msg.fdServoSetGain.ki=ki;
  this->out_msg.msg.fdServoSetGain.kd=kd;
  this->out_msg.msg.fdServoSetGain.cl=cl;
  this->out_msg.msg.fdServoSetGain.dc=dc;
  this->out_msg.msg.fdServoSetGain.el=el;
  this->out_msg.msg.fdServoSetGain.il=il;
  this->out_msg.msg.fdServoSetGain.ol=ol;
  this->out_msg.msg.fdServoSetGain.sr=sr;
  
  //send out_msg to PIC Daemon who will place reply into in_msg and return
  //0 if no error and -1 otherwise
  status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
  return(status);
}


/* this function sends a request to PIC Daemon to move one motor (addr) and 
returns without waiting for the move to finish.
it takes as input: mode, position, velocity, acceleration, pwm, and uselimit 
flag. uselimit flag is used to distinguish calibration moves from post
calibration moves. 
it returns 0 if no error and -1 otherwise */
//mode = 0 for absolute move, else for relative move
char CClientPIC::D_MovePosNoWait(byte addr, byte mode, long pos, long vel, long acc, byte pwm, short uselimit_flag)
{
  //check first if motor has been calibrated (unless uselimit_flag is 0 which
  //means that this is a move for calibration
  if (calibrated[addr-1] == 1 || uselimit_flag == 0){

    if (uselimit_flag == 1){
      printf("Checking limits\n");
      //now check if pos is within limits_min & max
      if (pos < limits_min[addr-1]){
	if (DEBUG) printf("Pos = %d < limits_min = %d",pos,limits_min[addr-1]);
	//change pos to limits_min + 1
	pos = limits_min[addr-1] + 1;
	if (DEBUG) printf(", change pos to %d\n",pos);
      }
      if (pos > limits_max[addr-1]){
	if (DEBUG) printf("Pos = %d > limits_max = %d",pos,limits_max[addr-1]);
	//change pos to limits_max - 1
	pos = limits_max[addr-1] - 1;
	if (DEBUG) printf(", change pos to %d\n",pos);
      }
    }	

    int status;
    //set parameters for out_msg
    this->out_msg.id=_D_MOVEPOSNOWAIT;    
    this->out_msg.msg.fdServoLoadTraj.addr=addr;
    this->out_msg.msg.fdServoLoadTraj.acc=acc;
    this->out_msg.msg.fdServoLoadTraj.mode=mode;
    this->out_msg.msg.fdServoLoadTraj.pos=pos;
    this->out_msg.msg.fdServoLoadTraj.pwm=pwm;
    this->out_msg.msg.fdServoLoadTraj.vel=vel;
    
    //send out_msg to PIC Daemon who will place reply into in_msg and return
    //0 if no error and -1 otherwise
    status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
    return(status);
  }
  else {
    //if not calibrated
    printf("Motor addr %d has not been calibrated. Ignore move.\n",addr);
    return 0;
  }
}

/* this function is similar to the one right above, except that PICdefaultvel
   and PICdefaultacc are used instead of passed in as input.
   this is not currently used by YAPOC */
 char CClientPIC::D_MovePosNoWait(byte addr, byte mode, long pos, byte pwm,short uselimit_flag)
  {
     //check first if motor has been calibrated
     if (calibrated[addr-1] == 1 || uselimit_flag == 0){
       
       if (uselimit_flag == 1){

	 printf("Checking limits\n");
           //now check if pos is within limits_min & max
           if (pos < limits_min[addr-1]){
	           if (DEBUG) printf("Pos = %d < limits_min = %d",pos,limits_min[addr-1]);
	           //change pos to limits_min + 1
	           pos = limits_min[addr-1] + 1;
	           if (DEBUG) printf(", change pos to %d\n",pos);
	   }
           if (pos > limits_max[addr-1]){
	           if (DEBUG) printf("Pos = %d > limits_max = %d",pos,limits_max[addr-1]);
	           //change pos to limits_max - 1
	           pos = limits_max[addr-1] - 1;
	            if (DEBUG) printf(", change pos to %d\n",pos);
	   }
       }
           int status;
       
           this->out_msg.id=_D_MOVEPOSNOWAIT;
       
           this->out_msg.msg.fdServoLoadTraj.addr=addr;
           this->out_msg.msg.fdServoLoadTraj.acc=PICdefaultacc[addr-1];
           this->out_msg.msg.fdServoLoadTraj.mode=mode;
           this->out_msg.msg.fdServoLoadTraj.pos=pos;
           this->out_msg.msg.fdServoLoadTraj.pwm=pwm;
           this->out_msg.msg.fdServoLoadTraj.vel=PICdefaultvel[addr-1];
       
       printf("Move axis %d to %d w/ vel %d acc %d\n",(int)addr,pos,PICdefaultvel[addr-1],PICdefaultacc[addr-1]);
       
       
           status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
           return(status);
     }
     else {
           //if not calibrated
           printf("Motor addr %d has not been calibrated. Ignore move.\n",addr);
           return 0;
     }
  }

/* this function is also similar to the above two functions except that it
   does not return until the requested move has been finished 
   this is also not currently used by YAPOC */
  
char CClientPIC::D_MovePosWait(byte addr, byte mode, long pos, long vel, long acc, byte pwm,  short uselimit_flag)
{

  //check first if motor has been calibrated
    if (calibrated[addr-1] == 1 || uselimit_flag == 0){
      
      if (uselimit_flag == 1){
	//now check if pos is within limits_min & max
	if (pos < limits_min[addr-1]){
	  if (DEBUG) printf("Pos = %d < limits_min = %d",pos,limits_min[addr-1]);
	  //change pos to limits_min + 1
	  pos = limits_min[addr-1] + 1;
	  if (DEBUG) printf(", change pos to %d\n",pos);
	}
	if (pos > limits_max[addr-1]){
	  if (DEBUG) printf("Pos = %d > limits_max = %d",pos,limits_max[addr-1]);
	  //change pos to limits_max - 1
	  pos = limits_max[addr-1] - 1;
	  if (DEBUG) printf(", change pos to %d\n",pos);
	}
      }
          int status;
      
          this->out_msg.id=_D_MOVEPOSWAIT;
      
          this->out_msg.msg.fdServoLoadTraj.addr=addr;
          this->out_msg.msg.fdServoLoadTraj.acc=acc;
          this->out_msg.msg.fdServoLoadTraj.mode=mode;
          this->out_msg.msg.fdServoLoadTraj.pos=pos;
          this->out_msg.msg.fdServoLoadTraj.pwm=pwm;
          this->out_msg.msg.fdServoLoadTraj.vel=vel;
      
      
      
          status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
          return(status);
    }
    else {
          //if not calibrated
          printf("Motor addr %d has not been calibrated. Ignore move.\n",addr);
      return 0;
    }
}

/* this functions requests the PIC Daemon to read encoder value of motor #addr.
   resulting encoder value is then placed in (int *encoder) */
char CClientPIC::D_ReadEncoder(byte addr, char set,int * encoder)
{
	int status;

	
	this->out_msg.id=_D_READENCODER;

	this->out_msg.msg.fdReadEncoder.addr=addr;
	this->out_msg.msg.fdReadEncoder.pos=0;
	this->out_msg.msg.fdReadEncoder.set=set;

     //printf("Sending ReadEncoder to pid = %d for axis %d\n",this->pid,addr);
	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));

	if (status)
	{	
		return(-1);
	}
	
	*encoder = in_msg.msg.fdReadEncoder.pos;
	
	return(status);
}

/* this function is not currently used in YAPOC */
char CClientPIC::D_ReadAD(byte addr, char set,char * p)
{
	int status,i;

	
	this->out_msg.id=_D_READAD;

	this->out_msg.msg.fdReadAD.addr=addr;
	this->out_msg.msg.fdReadAD.ADval[0]=0;
	this->out_msg.msg.fdReadAD.ADval[1]=0;
	this->out_msg.msg.fdReadAD.ADval[2]=0;
	this->out_msg.msg.fdReadAD.set=set;

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));

	if (status)
	{	
		return(-1);
	}
	
	for(i=0;i<3;i++)
	{
		p[i] = in_msg.msg.fdReadAD.ADval[i];
	}
	
	return(status);
}
 
/* this function is not currently used in YAPOC */
char CClientPIC::D_ReadDigital(byte addr, char set,char bitnumber, char* bitvalue)
{
	int status;

	
	this->out_msg.id=_D_READDIGITAL;

	this->out_msg.msg.fdReadDigital.addr=addr;
	this->out_msg.msg.fdReadDigital.nbit=bitnumber;
	this->out_msg.msg.fdReadDigital.set=set;
	this->out_msg.msg.fdReadDigital.value=1;

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));

	if (status)
	{	
		return(-1);
	}
	
	*bitvalue = in_msg.msg.fdReadDigital.value;
	
	return(status);
}				
 			

/* this function is not currently used in YAPOC */		
char CClientPIC::D_MovePWM(byte addr, int pwm)
{
	int status;
	byte p;
	int s=(pwm>=0)?1:-1;

	p=(byte)(s*pwm);
	

	
	this->out_msg.id=_D_MOVEPWM;

	this->out_msg.msg.fdServoLoadTraj.addr=addr;
	this->out_msg.msg.fdServoLoadTraj.pwm=p;
	this->out_msg.msg.fdServoLoadTraj.dir=s;
	

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
	return(status);
}

/* this function is not currently used in YAPOC */
char CClientPIC::D_CalibratePWM(byte addr,int pwm, int mindesp)
{
	int status;
	
	this->out_msg.id=_D_CALIBRATE_PWM;

	this->out_msg.msg.fdCalibrate.addr=addr;
	this->out_msg.msg.fdCalibrate.pwm=pwm;
	this->out_msg.msg.fdCalibrate.mindesp=mindesp;
		

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
	return(status);
}

/* this function is not currently used in YAPOC */
char CClientPIC::D_CalibratePosition(byte addr,long despla, int maxerror)
{
	int status;

	this->out_msg.id=_D_CALIBRATE_POS;

	this->out_msg.msg.fdCalibrate.addr=addr;
	this->out_msg.msg.fdCalibrate.despla=despla;
	this->out_msg.msg.fdCalibrate.maxerror=maxerror;
	

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
	return(status);
}

/* this function is not currently used in YAPOC */
char CClientPIC::D_ReadAll(struct _type_readall *p)
{
	int status;
	int i;

	this->out_msg.id=_D_READALL;

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));

	for(i=0;i<3;i++)
	{
		p->ADval[i] = in_msg.msg.fdReadAll.ADval[i];
	}
	for(i=0;i<3;i++)
	{
		p->encoder[i] = in_msg.msg.fdReadAll.encoder[i];
	}
	for(i=0;i<8;i++)
	{
		p->Digital[i] = in_msg.msg.fdReadAll.Digital[i];
	}

	return(status);
}

/* this function requests the PIC Daemon to reset encoder value for motor #addr
 */
char CClientPIC::D_ResetEncoder(byte addr)
{
	int status;

	this->out_msg.id=_D_RESETENCODER;
	this->out_msg.msg.fdServoSetGain.addr=addr;

	status=Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
	return(status);
}

/* this function requests the PIC Daemon to figure out the total number of
   motors residing in its cluster */
void CClientPIC::D_GetNumMotors()
{
  int status;
  this->out_msg.id = _D_GETNUMMOTORS;
  //this->out_msg.num = num;
  status = Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
  nmotors = in_msg.msg.fdGetNumMotors.nmotors;
  printf("Receive nmotors = %d \n",nmotors);
}

/* this function requests the PIC Daemon to reset the JRKERR board */
char CClientPIC::D_HardReset()
{
  int status;
  this->out_msg.id = _D_HARDRESET;
  status = Send(this->pid,&out_msg,&in_msg,sizeof(out_msg),sizeof(in_msg));
  return(status);
}

/* this function requests the PIC Daemon to check if the previous move to motor
   #addr has been completed */
 char CClientPIC::D_IsMoveDone(byte addr)
 {
   int status;
   this->out_msg.id = _D_ISMOVEDONE;
   this->out_msg.msg.fdIsMoveDone.addr = addr;
   status= Send(this->pid,&out_msg, &in_msg, sizeof(out_msg),sizeof(in_msg));
   
   return(in_msg.msg.fdIsMoveDone.done);
 }
 
/* this function performs a simple calibration procedure for motor #addr.
essentially, all motors are initially hand-calibrated to their predetermined
zero position before startup. thus, this calibration process simply reset 
encoder value for motor #addr */
int CClientPIC::CalibrateAxis(int addr)
 {
   //do this twice b/c sometime encoder doesn't always get zeroed after 1 reset
   
   D_ResetEncoder((byte)addr);
   D_ResetEncoder((byte)addr);
   //for now just set calibrated flag to 1
     calibrated[addr-1] = 1;  
   
   return 0;
 }

/* this is a utility function used in ReadConfig to eat all char until a \n 
   is encountered */
void CClientPIC::EatLine (FILE *fp)
{
  // check if line begins with '//'
  for (;;)
    {
      char c1, c2;
      fread (&c1, 1, 1, fp);
      fread (&c2, 1, 1, fp);
      
      if (c1 == '/' && c2 == '/')
	{
	  do
	    {
	      fread (&c1, 1, 1, fp);
	    }
	  while (c1 != '\n' && !feof(fp));
	}
      else
	{
	  fseek (fp, -2, SEEK_CUR);
	  return;         // exit from here.
	}
    }
}

/* this functions read configuration file containing: gain values, software 
   limits, default velocity values for all motors */
void CClientPIC::ReadConfig (char *filename)
{
    int Kp, Kd, Ki, IL; 
    FILE *fp = fopen (filename, "r");
  assert (fp != NULL);
  
  //ignore comments
    EatLine (fp);
    
  //read gains
  for (int i = 1; i <= nmotors; i++){
    fscanf (fp, "%d %d %d %d\n",
	    &Kp, &Kd, &Ki, &IL);
    
    D_ServoSetGain(i, Kp, Kd, Ki, IL, 255,0,400,1,0);
    if (DEBUG) printf("Read gains: %d %d %d %d\n",Kp,Kd,Ki,IL);
    EatLine (fp);  
  }
  //now read limits_max
  //reading backward b/c daemon number is opposite of virtual number
  if (DEBUG) printf("limits max: ");
  for ( i = 0; i < nmotors; i++){
    fscanf (fp, "%d ", &limits_max[i]);
    if (DEBUG) printf(" %d ",limits_max[i]);
  }
  if (DEBUG) printf(" min: ");
  EatLine(fp);
  //now read limits_min
  for ( i = 0; i < nmotors; i++){
      fscanf (fp, "%d ", &limits_min[i]);
    if (DEBUG) printf(" %d ",limits_min[i]);
  }
  if (DEBUG) printf("\n");
  
  //now read velocity
  //acceleration is velocity * 20
  EatLine(fp);
  for (i = 0; i < nmotors; i++){
    fscanf(fp,"%d ", &PICdefaultvel[i]);
    PICdefaultacc[i] = PICdefaultvel[i]*20;
     if (DEBUG) printf(" vel %d acc %d ;",PICdefaultvel[i],PICdefaultacc[i]);
  }
  if (DEBUG) printf("\n");
}

/* this function resets calibrated flag for all motors to 0 */
 void CClientPIC::ResetCalibrated(){
   for (int i = 0; i < nmotors; i++)
     calibrated[i] = 0;
   
 }

/* this function changes PICdefaultvel and PICdefaultacc value for motor #addr */
 void CClientPIC::SetVelocity(byte addr, long newvel){
   PICdefaultvel[addr-1] = newvel;
   PICdefaultacc[addr-1] = newvel * 20;
   printf("PICdefaultvel for axis %d is now %d\n",(int)addr, newvel);
 }




