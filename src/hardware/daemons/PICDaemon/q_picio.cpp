

#include "q_sio_util.h"
#include "q_nmccom.h"
#include "q_picio.h"
#include <stdio.h>

//---------------------------------------------------------------------------
extern NMCMOD	mod[]; 		//Array of modules
extern "C" int nummod;
extern int ComPort;

//---------------------------------------------------------------------------
//Returns pointer to an initialized IOMOD structure
 IOMOD *IoNewMod()
{
IOMOD *p;

p = new IOMOD;
p->inbits = 0;
p->ad1 = 0;
p->ad2 = 0;
p->ad3 = 0;
p->timer = 0;
p->inbits_s = 0;
p->timer_s = 0;
p->pwm1 = 0;
p->pwm2 = 0;
p->bitdir = 0x0FFF;
p->outbits = 0;
p->timermode = 0;
return p;
}

//---------------------------------------------------------------------------
 char IoGetStat(byte addr)
{
int numbytes, numrcvd;
int i, bytecount;
byte cksum;
byte inbuf[20];
IOMOD *p;

p = (IOMOD *)(mod[addr].p);  //cast the data pointer to the right type

//Find number of bytes to read:
numbytes = 2;       //start with stat & cksum
if ( (mod[addr].statusitems) & SEND_INPUTS )	numbytes +=2;
if ( (mod[addr].statusitems) & SEND_AD1 ) 		numbytes +=1;
if ( (mod[addr].statusitems) & SEND_AD2 ) 		numbytes +=1;
if ( (mod[addr].statusitems) & SEND_AD3 ) 		numbytes +=1;
if ( (mod[addr].statusitems) & SEND_TIMER )		numbytes +=4;
if ( (mod[addr].statusitems) & SEND_ID ) 		numbytes +=2;
if ( (mod[addr].statusitems) & SEND_SYNC_IN )	numbytes +=2;
if ( (mod[addr].statusitems) & SEND_SYNC_TMR )	numbytes +=4;
numrcvd = SioGetChars(ComPort, (char *)inbuf, numbytes);

//Verify enough data was read
if (numrcvd != numbytes)
	{
    ErrorMsgBox("IoGetStat failed to read chars");
    return 0;
    }

//Verify checksum:
cksum = 0;
for (i=0; i<numbytes-1; i++) cksum = (byte)(cksum + inbuf[i]);
if (cksum != inbuf[numbytes-1])
	{
    ErrorMsgBox("IoGetStat: checksum error");
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
if ( (mod[addr].statusitems) & SEND_INPUTS )
	{
	p->inbits = *( (short int *)(inbuf + bytecount) );
    bytecount +=2;
    }
if ( (mod[addr].statusitems) & SEND_AD1 )
	{
    p->ad1 = inbuf[bytecount];
    bytecount +=1;
    }
if ( (mod[addr].statusitems) & SEND_AD2 )
	{
    p->ad2 = inbuf[bytecount];
    bytecount +=1;
    }
if ( (mod[addr].statusitems) & SEND_AD3 )
	{
    p->ad3 = inbuf[bytecount];
    bytecount +=1;
    }
if ( (mod[addr].statusitems) & SEND_TIMER )
	{
    p->timer = *( (unsigned long *)(inbuf + bytecount) );
    bytecount +=4;
    }
if ( (mod[addr].statusitems) & SEND_ID )
	{
    mod[addr].modtype = inbuf[bytecount];
    mod[addr].modver = inbuf[bytecount+1];
    bytecount +=2;
    }
if ( (mod[addr].statusitems) & SEND_SYNC_IN )
	{
    p->inbits_s = *( (short int *)(inbuf + bytecount) );
    bytecount +=2;
    }
if ( (mod[addr].statusitems) & SEND_SYNC_TMR )
	{
    p->timer_s = *( (unsigned long *)(inbuf + bytecount) );
    //bytecount +=4;
    }

return 1;
}

//---------------------------------------------------------------------------
 char IoInBitVal(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);
return ((p->inbits >> bitnum) & 1);
}
//---------------------------------------------------------------------------
 char IoInBitSVal(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);
return ((p->inbits_s >> bitnum) & 1);
}
//---------------------------------------------------------------------------
 char IoOutBitVal(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);
return ((p->outbits >> bitnum) & 1);
}
//---------------------------------------------------------------------------
 char IoSetOutBit(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->outbits = p->outbits | (short int)(1 << bitnum);

return NmcSendCmd(addr, SET_OUTPUT, (char *)(&(p->outbits)), 2, addr);
}
//---------------------------------------------------------------------------
 char IoClrOutBit(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->outbits = p->outbits & (short int)(~(1 << bitnum));

return NmcSendCmd(addr, SET_OUTPUT, (char *)(&(p->outbits)), 2, addr);
}
//---------------------------------------------------------------------------
 char IoGetBitDir(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);
return ((p->bitdir >> bitnum) & 1);
}
//---------------------------------------------------------------------------
 char IoBitDirOut(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->bitdir = p->bitdir & (short int)(~(1 << bitnum));

return NmcSendCmd(addr, SET_IO_DIR, (char *)(&(p->bitdir)), 2, addr);
}
//---------------------------------------------------------------------------
 char IoBitDirIn(byte addr, int bitnum)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->bitdir = p->bitdir | (short int)(1 << bitnum);

return NmcSendCmd(addr, SET_IO_DIR, (char *)(&(p->bitdir)), 2, addr);
}
//---------------------------------------------------------------------------
 byte IoGetADCVal(byte addr, int channel)
{
IOMOD * p;
p = (IOMOD *)(mod[addr].p);

switch (channel) {
	case 0: return p->ad1;
	case 1: return p->ad2;
	case 2: return p->ad3; 
    }

return 0;
}
//---------------------------------------------------------------------------
 char IoSetPWMVal(byte addr, byte pwm1, byte pwm2)
{
IOMOD * p;
char cmdstr[4];

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->pwm1 = pwm1;
p->pwm2 = pwm2;
cmdstr[0] = pwm1;
cmdstr[1] = pwm2;

return NmcSendCmd(addr, SET_PWM, cmdstr, 2, addr);
}
//---------------------------------------------------------------------------
 char IoSetSynchOutput(byte addr, short int outbits, byte pwm1, byte pwm2)
{
IOMOD * p;
char cmdstr[5];

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->outbits =  outbits;
p->pwm1 = pwm1;
p->pwm2 = pwm2;
cmdstr[0] = ((char *)(&outbits))[0];
cmdstr[1] = ((char *)(&outbits))[1];
cmdstr[2] = pwm1;
cmdstr[3] = pwm2;
return NmcSendCmd(addr, SET_SYNCH_OUT, cmdstr, 4, addr);
}
//---------------------------------------------------------------------------
 byte IoGetPWMVal(byte addr, int channel)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
if (channel == 0) return(p->pwm1);
else return(p->pwm2);
}
//---------------------------------------------------------------------------
 char IoSetTimerMode(byte addr, byte tmrmode)
{
IOMOD * p;
char cmdstr[2];

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
p->timermode = tmrmode;
cmdstr[0] = tmrmode;

return NmcSendCmd(addr, SET_TMR_MODE, cmdstr, 1, addr);
}
//---------------------------------------------------------------------------
 byte IoGetTimerMode(byte addr)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
return p->timermode;
}
//---------------------------------------------------------------------------
 unsigned long IoGetTimerVal(byte addr)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
return(p->timer);
}
//---------------------------------------------------------------------------
 unsigned long IoGetTimerSVal(byte addr)
{
IOMOD * p;

p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
return(p->timer_s);
}
//---------------------------------------------------------------------------

