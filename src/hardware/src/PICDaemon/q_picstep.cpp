#include "q_sio_util.h"
#include "q_picstep.h"
#include "q_nmccom.h"

#include <stdio.h>

//---------------------------------------------------------------------------
extern NMCMOD	mod[]; 		//Array of modules
extern "C" int nummod;
extern int ComPort;

//---------------------------------------------------------------------------
//Returns pointer to an initialized IOMOD structure
STEPMOD * StepNewMod()
{
STEPMOD *p;

p = new STEPMOD;
p->pos = 0;
p->ad = 0;
p->st = 0;
p->inbyte = 0;
p->home = 0;

p->cmdpos = 0;
p->cmdspeed = 1;
p->cmdacc = 1;
p->cmdst = 0;
p->min_speed = 1;
p->outbyte = 0;
p->homectrl = 0;
p->ctrlmode = SPEED_1X | ESTOP_OFF;
p->stopctrl = 0;
p->run_pwm = 0;
p->hold_pwm = 0;
p->therm_limit = 0;
return p;
}

//---------------------------------------------------------------------------
 char StepGetStat(byte addr)
{
int numbytes, numrcvd;
int i, bytecount;
byte cksum;
byte inbuf[20];
STEPMOD *p;
char msgstr[80];

p = (STEPMOD *)(mod[addr].p);  //cast the data pointer to the right type

//Find number of bytes to read:
numbytes = 2;       //start with stat & cksum
if ( (mod[addr].statusitems) & SEND_POS )	numbytes +=4;
if ( (mod[addr].statusitems) & SEND_AD ) 	numbytes +=1;
if ( (mod[addr].statusitems) & SEND_ST ) 	numbytes +=2;
if ( (mod[addr].statusitems) & SEND_INBYTE ) numbytes +=1;
if ( (mod[addr].statusitems) & SEND_HOME )	numbytes +=4;
if ( (mod[addr].statusitems) & SEND_ID ) 	numbytes +=2;
numrcvd = SioGetChars(ComPort, (char *)inbuf, numbytes);

//Verify enough data was read
if (numrcvd != numbytes)
	{
    sprintf(msgstr,"StepGetStat (%d) failed to read chars",addr);
    ErrorMsgBox(msgstr);
    return 0;
    }

//Verify checksum:
cksum = 0;
for (i=0; i<numbytes-1; i++) cksum = (byte)(cksum + inbuf[i]);
if (cksum != inbuf[numbytes-1])
	{
    sprintf(msgstr,"StepGetStat(%d): checksum error",addr);
    ErrorMsgBox(msgstr);
    return 0;
    }

//Verify command was received intact before updating status data
mod[addr].stat = inbuf[0];
if (mod[addr].stat & CKSUM_ERROR)
	{
    ErrorMsgBox("Command checksum error!");
    return 0;
    }

//Finally, fill in status data
bytecount = 1;
if ( (mod[addr].statusitems) & SEND_POS )
	{
	p->pos = *( (long *)(inbuf + bytecount) );
    bytecount +=4;
    }
if ( (mod[addr].statusitems) & SEND_AD )
	{
    p->ad = inbuf[bytecount];
    bytecount +=1;
    }
if ( (mod[addr].statusitems) & SEND_ST )
	{
	p->st = *( (unsigned short int *)(inbuf + bytecount) );
    bytecount +=2;
    }
if ( (mod[addr].statusitems) & SEND_INBYTE )
	{
    p->inbyte = inbuf[bytecount];
    bytecount +=1;
    }
if ( (mod[addr].statusitems) & SEND_HOME )
	{
    p->home = *( (unsigned long *)(inbuf + bytecount) );
    bytecount +=4;
    }
if ( (mod[addr].statusitems) & SEND_ID )
	{
    mod[addr].modtype = inbuf[bytecount];
    mod[addr].modver = inbuf[bytecount+1];
    //bytecount +=2;
    }

return 1;
}
//---------------------------------------------------------------------------
 long StepGetPos(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->pos;
}
//---------------------------------------------------------------------------
 byte StepGetAD(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->ad;
}
//---------------------------------------------------------------------------
 unsigned short int StepGetStepTime(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->st;
}
//---------------------------------------------------------------------------
 byte StepGetInbyte(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->inbyte;
}
//---------------------------------------------------------------------------
 long StepGetHome(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->home;
}
//---------------------------------------------------------------------------
 long StepGetCmdPos(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->cmdpos;
}
//---------------------------------------------------------------------------
 byte StepGetCmdSpeed(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->cmdspeed;
}
//---------------------------------------------------------------------------
 byte StepGetCmdAcc(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->cmdacc;
}
//---------------------------------------------------------------------------
 unsigned short int StepGetCmdST(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->cmdst;
}
//---------------------------------------------------------------------------
 byte StepGetMinSpeed(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->min_speed;
}
//---------------------------------------------------------------------------
 byte StepGetOutputs(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->outbyte;
}
//---------------------------------------------------------------------------
 byte StepGetCtrlMode(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->ctrlmode;
}
//---------------------------------------------------------------------------
 byte StepGetRunCurrent(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->run_pwm;
}
//---------------------------------------------------------------------------
 byte StepGetHoldCurrent(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->hold_pwm;
}
//---------------------------------------------------------------------------
 byte StepGetThermLimit(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->therm_limit;
}
//---------------------------------------------------------------------------
 byte StepGetHomeCtrl(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->homectrl;
}
//---------------------------------------------------------------------------
 byte StepGetStopCtrl(byte addr)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
return p->stopctrl;
}
//---------------------------------------------------------------------------
 char StepSetParam(byte addr, byte mode,
					byte minspeed, byte runcur, byte holdcur, byte thermlim)
{
STEPMOD * p;
char cmdstr[16];

p = (STEPMOD *)(mod[addr].p);
p->ctrlmode = mode;
p->min_speed = minspeed;
p->run_pwm = runcur;
p->hold_pwm = holdcur;
p->therm_limit = thermlim;

cmdstr[0] = mode;
cmdstr[1] = minspeed;
cmdstr[2] = runcur;
cmdstr[3] = holdcur;
cmdstr[4] = thermlim;

return NmcSendCmd(addr, SET_PARAM, cmdstr, 5, addr);
}
//---------------------------------------------------------------------------
 char StepLoadTraj(byte addr, byte mode,
			long pos, byte speed, byte acc, float raw_speed)
{
STEPMOD * p;
char cmdstr[16];
unsigned short int steptime;
byte nearspeed;
int count;

if (raw_speed < 0.4) raw_speed = 0.4;
if (raw_speed > 250.0) raw_speed = 250.0;
if (speed < 1 ) speed = 1;
if (acc < 1 ) acc = 1;

p = (STEPMOD *)(mod[addr].p);

steptime = (unsigned short int)(0x10000 - (unsigned)(25000.0/raw_speed));

//Adjust steptime for the fixed off time of the hardware timer
if ( (p->ctrlmode & 0x03)==SPEED_8X ) steptime += (byte)16;
else if ( (p->ctrlmode & 0x03)==SPEED_4X ) steptime += (byte)8;
else if ( (p->ctrlmode & 0x03)==SPEED_2X ) steptime += (byte)4;
else if ( (p->ctrlmode & 0x03)==SPEED_1X ) steptime += (byte)2;

nearspeed = (unsigned short int)(raw_speed + 0.5);

count = 0;
*( (byte *)(cmdstr + count) ) = mode;  count += 1;
if (mode & LOAD_POS)
  {
  p->cmdpos = pos;
  *( (long *)(cmdstr + count) ) = pos;
  count += 4;
  }
if (mode & LOAD_SPEED)
  {
  p->cmdspeed = speed;
  *( (byte *)(cmdstr + count) ) = speed;
  count += 1;
  }
if (mode & LOAD_ACC)
  {
  p->cmdacc = acc;
  *( (byte *)(cmdstr + count) ) = acc;
  count += 1;
  }
if (mode & LOAD_ST)
  {
  p->cmdst = steptime;
  *( (short int *)(cmdstr + count) ) = steptime;
  count += 2;
  *( (byte *)(cmdstr + count) ) = nearspeed;
  count += 1;
  }

return NmcSendCmd(addr, LOAD_TRAJ, cmdstr, (byte)count, addr);
}
//---------------------------------------------------------------------------
 char StepResetPos(byte addr)
{
return NmcSendCmd(addr, RESET_POS, NULL, 0, addr);
}
//---------------------------------------------------------------------------
 char StepStopMotor(byte addr, byte mode)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);

p->stopctrl = mode;

return NmcSendCmd(addr, STOP_MOTOR, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------
 char StepSetOutputs(byte addr, byte outbyte)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);

p->outbyte = outbyte;

return NmcSendCmd(addr, SET_OUTPUTS, (char *)(&outbyte), 1, addr);
}
//---------------------------------------------------------------------------
 char StepSetHoming(byte addr, byte mode)
{
STEPMOD * p;

p = (STEPMOD *)(mod[addr].p);
p->homectrl = mode;

return NmcSendCmd(addr, SET_HOMING, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------

