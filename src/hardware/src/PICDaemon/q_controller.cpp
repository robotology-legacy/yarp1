//---------------------------------------------------------------------------
#include "q_controller.h"
#include "q_sio_util.h"


#include <stdio.h>
#include <i86.h>

Cqnxmc::Cqnxmc()
{
	nummod = 0;				//start off with no modules
	SioError = 0;
}

Cqnxmc::~Cqnxmc()
{
	if(nummod)
	{
		nummod=0;
		NmcShutdown();
	}
}

//---------------------------------------------------------------------------
ADCMOD * Cqnxmc::AdcNewMod()
{
	return NULL;
}

//---------------------------------------------------------------------------
//Reset all controllers with group address 'addr' (should include all modules)
char Cqnxmc::NmcHardReset(byte addr)
{
	int i;
	//char cstr[5];
	
	//Send string of 0's to flush input buffers
	cstr[0] = 0;
	for (i=0; i<20; i++) SioPutChars(ComPort,cstr, 1);
	
	//Send out reset command string
	cstr[0] = 0xAA;			//Header
	cstr[1] = addr;
	cstr[2] = HARD_RESET;
	cstr[3] = (byte)(cstr[1] + cstr[2]);  //checksum
	SioPutChars(ComPort, cstr, 4);//send reset
	
	delay(100);		//wait for reset to execute
	
	for (i=0; i<nummod; i++) delete mod[i].p;  //delete and prev. declared modules
	nummod = 0;
	SioError = 0;	//reset the number of errors to 0
	
	SioChangeBaud(ComPort, 19200);	//Reset the baud rate to the default
	
	SioClrInbuf(ComPort);   //clear out any random crap left in buffer
	
	return (1);
}

//---------------------------------------------------------------------------
//Change the baud rate of all controllers with group address 'groupaddr'
//(should include all modules) and also changes host's baud rate.
//There should be no group leader for 'addr'
char Cqnxmc::NmcChangeBaud(byte groupaddr, unsigned int baudrate)
{
	//char cstr[6];
	
	cstr[0] = 0xAA;			//Header
	cstr[1] = groupaddr;
	cstr[2] = 0x10 | SET_BAUD;
	switch (baudrate) {
	case 19200:		cstr[3] = PB19200; break;
	case 57600:		cstr[3] = PB57600; break;
	case 115200:	cstr[3] = PB115200; break;
    default:	ErrorMsgBox("Baud rate not supported - using default of 19200");
		cstr[3] = PB19200;
		baudrate = 19200;
    }
	cstr[4] = (byte)(cstr[1] + cstr[2] + cstr[3]);  //checksum
	SioPutChars(ComPort, cstr, 5);//send command
	delay(100);		//Wait for command to execute
	
	SioChangeBaud(ComPort, baudrate);	//Reset the baud rate to the default
	
	SioClrInbuf(ComPort);   //clear out any random crap left in buffer
	delay(100);
	
	BaudRate = baudrate;
	return (1);
}

//---------------------------------------------------------------------------
//Initialize misc network variables
void Cqnxmc::InitVars(void)
{
	int i;
	
	mod[0].modtype = 0;   //Default to a known module type for module 0
	mod[0].modver = 0;
	mod[0].stat = 0;
	mod[0].statusitems = 0;
	mod[0].groupaddr = 0xFF;
	mod[0].groupleader = 0;
	
	for (i=1; i<MAXNUMMOD; i++)
	{
		mod[i].modtype = 0xFF;
		mod[i].modver = 0;
		mod[i].stat = 0;
		mod[i].statusitems = 0;
		mod[i].groupaddr = 0xFF;
		mod[i].groupleader = 0;
    }
}
//---------------------------------------------------------------------------
//Initialize the network of controllers with sequential addresses starting
//at 0.  Retunrs the number of controllers found.
int Cqnxmc::NmcInit(char *portname, unsigned int baudrate)
{
	int numread;
	byte addr;
	//char cstr[20];
	//char mess[40];
	
	BaudRate = baudrate;
	
	InitVars();
	
	ComPort = SioOpen(portname, 19200);    //Open with default rate of 19200
        printf("ComPort = %d ",ComPort);
  fflush(stdout);
  if (ComPort == INVALID_HANDLE_VALUE) return 0;
	
	printf("ComPort is valid\n");
	
	NmcHardReset(0xFF);
  printf("CHECK\n"); fflush(stdout);
	NmcHardReset(0xFF);
	printf("CHECK2\n");
	addr = 1;
  printf("About to loop\n");fflush(stdout);
  fflush(stdout);
	while (1)
	{
		//First set the address to a unique value:
		cstr[0] = 0xAA;		//Header
		cstr[1] = 0;  		//Send to default address of 0
		cstr[2] = 0x20 | SET_ADDR;
		cstr[3] = addr;  	//Set new address sequentially
		cstr[4] = 0xFF;		//Set group address to 0xFF
		cstr[5] = (byte)(cstr[1] + cstr[2] + cstr[3] + cstr[4]);  //checksum
		SioPutChars(ComPort, cstr, 6);  //Send command
		
		numread = SioGetChars(ComPort, cstr, 2);  //get back status, cksum
		if (numread!=2) break;	//if no response, punt out of loop
		if (cstr[0] != cstr[1])
		{
			ErrorMsgBox("Status checksum error - please reset the Network");
			SioClose(ComPort);
			return(0);
		}
		
		//Read the device type
		cstr[0] = 0xAA;		//Header
		cstr[1] = addr;  		//Send to addr
		cstr[2] = 0x10 | READ_STAT;
		cstr[3] = SEND_ID;  //send ID only
		cstr[4] = (byte)(cstr[1] + cstr[2] + cstr[3]);  //checksum
		SioPutChars(ComPort, cstr, 5);  //Send command
		
		numread = SioGetChars(ComPort, cstr, 4);  //get back stat, type, ver, cksum
		
		printf("Estoy aqui y numread=%d\n",numread);
		
		/*
		if (numread!=4)
		{
		ErrorMsgBox("Could not read device type");
		SioClose(ComPort);
		return(0);
		}
		*/
		
		if(numread==4)
		{
			mod[addr].stat = cstr[0];
			mod[addr].modtype = cstr[1];
			mod[addr].modver = cstr[2];
			mod[addr].statusitems = 0;
			mod[addr].groupaddr = 0xFF;
			mod[addr].groupleader = 0;
			switch (mod[addr].modtype)
			{
			case SERVOMODTYPE: 	mod[addr].p = ServoNewMod(); break;
			case ADCMODTYPE: 	mod[addr].p = AdcNewMod(); break;
			case IOMODTYPE: 	mod[addr].p = IoNewMod(); break;
			case STEPMODTYPE: 	mod[addr].p = StepNewMod(); break;
			}
			
			
			addr++;		//increment the address
		}
	}
	
	nummod = addr-1;
	
	if (nummod > 0)
	{
		NmcChangeBaud(0xFF, baudrate);
		//if (nummod>1) sprintf(mess,"%d Modules found", nummod);
		//else sprintf(mess,"%d Module found", nummod);
		//ErrorMsgBox(mess);
	}
	else
	{
		//ErrorMsgBox("No modules found on the network.\nPlease check power and connections.");
		SioClose(ComPort);
	}
	
	return(nummod);	//Return number of modules
}
//---------------------------------------------------------------------------
char Cqnxmc::AdcGetStat(byte addr)
{
	return 0;
}
//---------------------------------------------------------------------------
//Sends a command to addr and shoves the returned status data in the
//structure for module address stataddr.  If stataddr = 0, the command
//is taken as a group command with no group leader (no status returned).
//n is the number of auxilliary data bytes.
char Cqnxmc::NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr)
{
	byte cksum;
	char mess[40];
	byte outstr[20];
	int i;
	char iostat;
	int IOBusy = 0;
	
	//If too many SIO errors, punt
	if (SioError > MAXSIOERROR) return(0);
	
	IOBusy = 1;
	
	//Check if data is to be returned from to a known module type
	if ( !( ( mod[stataddr].modtype==SERVOMODTYPE) ||
        ( mod[stataddr].modtype==ADCMODTYPE) ||
        ( mod[stataddr].modtype==IOMODTYPE)  ||
        ( mod[stataddr].modtype==STEPMODTYPE) ) )
	{
		sprintf(mess,"Module type %d not supported",mod[stataddr].modtype);
		ErrorMsgBox(mess);
		return (0);
    }
	
	//Calculate the adjust command byte, calculate checksum and send the command
	cksum = 0;
	outstr[0] = 0xAA;	//start with header byte
	outstr[1] = addr;	//add on address byte
	cksum += outstr[1];
	outstr[2] = (byte)(((n<<4) & 0xF0) | cmd);
	cksum += outstr[2];
	for (i=0; i<n; i++)
	{
		outstr[i+3] = datastr[i];
		cksum += outstr[i+3];
    }
	outstr[n+3] = cksum;
	
	SioClrInbuf(ComPort);  //Get rid of any old input chars
	SioPutChars(ComPort, (char *)outstr, n+4); //Send the command string
	
	if (stataddr == 0)		//If a group command w/ no leader, add delay then exit
	{
		delay(60);
		IOBusy = 0;
		return (1);
    }
	
	switch (mod[stataddr].modtype) {
	case SERVOMODTYPE:	iostat = ServoGetStat(stataddr); break;
	case ADCMODTYPE:	iostat = AdcGetStat(stataddr); break;
	case IOMODTYPE:		iostat = IoGetStat(stataddr); break;
	case STEPMODTYPE:	iostat = StepGetStat(stataddr); break;
    }
	
	if (iostat == 0)
	{
		SioError++;
		FixSioError(stataddr);
    }
	else SioError = 0;
	
	IOBusy = 0;
	return iostat;
}

//---------------------------------------------------------------------------
//Attempt to re-synch communications
void Cqnxmc::FixSioError(byte addr)
{
	int i;
	char teststr[2];
	
	//If too many errors - prompt to reset
	if (SioError >= MAXSIOERROR)
	{
		ErrorMsgBox("Multiple communication errors - please the Network");
		return;
    }
	
	//Otherwise, send out a null string to re-synch
	teststr[0] = 0;
	for (i=0; i<30; i++) SioPutChars(ComPort, teststr, 1);
	delay(100);
	SioClrInbuf(ComPort);
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcSetGroupAddr(byte addr, byte groupaddr, char leader)
{
	char cmdstr[2];
	
	if (!(groupaddr & 0x80)) return 0;  //punt if not valid group address
	
	cmdstr[0] = addr;
	cmdstr[1] = groupaddr;
	if (leader) cmdstr[1] &= 0x7F;  //clear upper bit if a group leader
	mod[addr].groupaddr = groupaddr;
	mod[addr].groupleader = leader;
	return NmcSendCmd(addr, SET_ADDR, cmdstr, 2, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcSynchOutput(byte groupaddr, byte leaderaddr)
//Synchronous output command issued to groupaddr.  Status data goes to
//the leaderaddr.  If no leader, use leaderaddr = 0.
{
	return NmcSendCmd(groupaddr, SYNCH_OUT, NULL, 0, leaderaddr);
}
//---------------------------------------------------------------------------
//Synchronous input command issued to groupaddr.  Status data goes to
//the leaderaddr.  If no leader, use leaderaddr = 0.
char Cqnxmc::NmcSynchInput(byte groupaddr, byte leaderaddr)
{
	return NmcSendCmd(groupaddr, SYNCH_INPUT, NULL, 0, leaderaddr);
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcNoOp(byte addr)
{
	return NmcSendCmd(addr, NOP, NULL, 0, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcReadStatus(byte addr, byte statusitems)
{
	char cmdstr[2];
	byte oldstat;
	char retval;
	
	cmdstr[0] = statusitems;
	oldstat = mod[addr].statusitems;
	mod[addr].statusitems = statusitems;
	retval = NmcSendCmd(addr, READ_STAT, cmdstr, 1, addr);
	mod[addr].statusitems = oldstat;
	
	return retval;
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcDefineStatus(byte addr, byte statusitems)
{
	char cmdstr[2];
	
	cmdstr[0] = statusitems;
	mod[addr].statusitems = statusitems;
	return NmcSendCmd(addr, DEF_STAT, cmdstr, 1, addr);
}
//---------------------------------------------------------------------------
byte Cqnxmc::NmcGetStat(byte addr)
{
	return mod[addr].stat;
}
//---------------------------------------------------------------------------
byte Cqnxmc::NmcGetStatItems(byte addr)
{
	return mod[addr].statusitems;
}
//---------------------------------------------------------------------------
byte Cqnxmc::NmcGetModType(byte addr)
{
	return mod[addr].modtype;
}
//---------------------------------------------------------------------------
byte Cqnxmc::NmcGetModVer(byte addr)
{
	return mod[addr].modver;
}
//---------------------------------------------------------------------------
byte Cqnxmc::NmcGetGroupAddr(byte addr)
{
	return mod[addr].groupaddr;
}
//---------------------------------------------------------------------------
char Cqnxmc::NmcGroupLeader(byte addr)
{
	return mod[addr].groupleader;
}
//---------------------------------------------------------------------------
void Cqnxmc::NmcShutdown(void)
{
	//int retval;
	
	//retval = MessageBox(NULL, "Do you wish to reset the NMC network?",
	//                                 "",MB_YESNO | MB_DEFBUTTON1 | MB_TASKMODAL);
	//
	//if (retval == IDYES)
	//	{
	//	if (ComPort!=INVALID_HANDLE_VALUE && ComPort!=NULL) NmcHardReset(0xFF);
	//    }
	//else
	//	{
	//    if (BaudRate != 19200)
	//      	{
	//      	ErrorMsgBox("Returning NMC Baud rate to 19200 (default)");
	//      	NmcChangeBaud(0xFF, 19200);
	//      	}
	//    }
	if (ComPort!=INVALID_HANDLE_VALUE && ComPort!=NULL) NmcHardReset(0xFF);
	
	nummod = 0;    
	SioClose(ComPort);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SERVOMOD * Cqnxmc::ServoNewMod()
{
	SERVOMOD *p;
	
	p = new SERVOMOD;
	p->pos = 0;
	p->ad = 0;
	p->vel = 0;
	p->aux = 0;
	p->home = 0;
	p->perror = 0;
	p->cmdpos = 0;
	p->cmdvel = 0;
	p->cmdacc = 0;
	p->cmdpwm = 0;
	
	(p->gain).kp = 0;
	(p->gain).kd = 0;
	(p->gain).ki = 0;
	(p->gain).il = 0;
	(p->gain).ol = 0;
	(p->gain).cl = 0;
	(p->gain).el = 0;
	(p->gain).sr = 1;
	(p->gain).dc = 0;
	
	p->stoppos = 0;
	p->ioctrl = IO1_IN | IO2_IN;
	p->homectrl = 0;
	p->movectrl = 0;
	p->stopctrl = 0;
	p->ph_adv = 0;
	p->ph_off = 0;
	return p;
}

//---------------------------------------------------------------------------
char Cqnxmc::ServoGetStat(byte addr)
{
	int numbytes, numrcvd;
	int i, bytecount;
	byte cksum;
	byte inbuf[20];
	SERVOMOD *p;
	char msgstr[80];
	
	p = (SERVOMOD *)(mod[addr].p);  //cast the data pointer to the right type
	
	//Find number of bytes to read:
	numbytes = 2;       //start with stat & cksum
	if ( (mod[addr].statusitems) & SEND_POS )	numbytes +=4;
	if ( (mod[addr].statusitems) & SEND_AD ) 	numbytes +=1;
	if ( (mod[addr].statusitems) & SEND_VEL ) 	numbytes +=2;
	if ( (mod[addr].statusitems) & SEND_AUX ) 	numbytes +=1;
	if ( (mod[addr].statusitems) & SEND_HOME )	numbytes +=4;
	if ( (mod[addr].statusitems) & SEND_ID ) 	numbytes +=2;
	if ( (mod[addr].statusitems) & SEND_PERROR ) numbytes +=2;
	if ( (mod[addr].statusitems) & SEND_NPOINTS ) numbytes +=1;
	numrcvd = SioGetChars(ComPort, (char *)inbuf, numbytes);
	
	//Verify enough data was read
	if (numrcvd != numbytes)
	{
		sprintf(msgstr,"ServoGetStat (%d) failed to read chars",addr);
		ErrorMsgBox(msgstr);
		return 0;
    }
	
	//Verify checksum:
	cksum = 0;
	for (i=0; i<numbytes-1; i++) cksum = (byte)(cksum + inbuf[i]);
	if (cksum != inbuf[numbytes-1])
	{
		sprintf(msgstr,"ServoGetStat(%d): checksum error",addr);
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
	if ( (mod[addr].statusitems) & SEND_VEL )
	{
		p->vel = *( (short int *)(inbuf + bytecount) );
		bytecount +=2;
    }
	if ( (mod[addr].statusitems) & SEND_AUX )
	{
		p->aux = inbuf[bytecount];
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
		bytecount +=2;
    }
	if ( (mod[addr].statusitems) & SEND_PERROR )
	{
		p->perror = *( (short int *)(inbuf + bytecount) );
		bytecount +=2;
    }
	if ( (mod[addr].statusitems) & SEND_NPOINTS )
	{
		p->npoints = inbuf[bytecount];
		//bytecount +=1;
    }
	
	return 1;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetPos(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->pos;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetAD(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->ad;
}
//---------------------------------------------------------------------------
short int Cqnxmc::ServoGetVel(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->vel;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetAux(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->aux;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetHome(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->home;
}
//---------------------------------------------------------------------------
short int Cqnxmc::ServoGetPError(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->perror;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetNPoints(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->npoints;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetCmdPos(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->cmdpos;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetCmdVel(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->cmdvel;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetCmdAcc(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->cmdacc;
}
//---------------------------------------------------------------------------
long Cqnxmc::ServoGetStopPos(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->stoppos;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetCmdPwm(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->cmdpwm;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetIoCtrl(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->ioctrl;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetHomeCtrl(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->homectrl;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetStopCtrl(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->stopctrl;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetMoveCtrl(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->movectrl;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetPhAdv(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->ph_adv;
}
//---------------------------------------------------------------------------
byte Cqnxmc::ServoGetPhOff(byte addr)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	return p->ph_off;
}
//---------------------------------------------------------------------------
void Cqnxmc::ServoGetGain(byte addr, short int * kp, short int * kd, short int * ki,
				  short int * il, byte * ol, byte * cl, short int * el,
                  byte * sr, byte * dc)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	*kp = (p->gain).kp;
	*kd = (p->gain).kd;
	*ki = (p->gain).ki;
	*il = (p->gain).il;
	*ol = (p->gain).ol;
	*cl = (p->gain).cl;
	*el = (p->gain).el;
	*sr = (p->gain).sr;
	*dc = (p->gain).dc;
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoSetGain(byte addr, short int kp, short int kd, short int ki,
				  short int il, byte ol, byte cl, short int el,
                  byte sr, byte dc)
{
	SERVOMOD * p;
	char cmdstr[16];
	
	p = (SERVOMOD *)(mod[addr].p);
	(p->gain).kp = kp;
	(p->gain).kd = kd;
	(p->gain).ki = ki;
	(p->gain).il = il;
	(p->gain).ol = ol;
	(p->gain).cl = cl;
	(p->gain).el = el;
	(p->gain).sr = sr;
	(p->gain).dc = dc;
	
	*( (short int *)(cmdstr) ) = kp;
	*( (short int *)(cmdstr+2) ) = kd;
	*( (short int *)(cmdstr+4) ) = ki;
	*( (short int *)(cmdstr+6) ) = il;
	*( (byte *)(cmdstr+8) ) = ol;
	*( (byte *)(cmdstr+9) ) = cl;
	*( (short int *)(cmdstr+10) ) = el;
	*( (byte *)(cmdstr+12) ) = sr;
	*( (byte *)(cmdstr+13) ) = dc;
	
	return NmcSendCmd(addr, SET_GAIN, cmdstr, 14, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoLoadTraj(byte addr, byte mode, long pos, long vel, long acc, byte pwm)
{
	SERVOMOD * p;
	char cmdstr[16];
	int count;
	
	p = (SERVOMOD *)(mod[addr].p);
	p->movectrl = mode;
	p->cmdpos = pos;
	p->cmdvel = vel;
	p->cmdacc = acc;
	p->cmdpwm = pwm;
	
	count = 0;
	*( (byte *)(cmdstr + count) ) = mode;  count += 1;
	if (mode & LOAD_POS) { *( (long *)(cmdstr + count) ) = pos; count += 4; }
	if (mode & LOAD_VEL) { *( (long *)(cmdstr + count) ) = vel; count += 4; }
	if (mode & LOAD_ACC) { *( (long *)(cmdstr + count) ) = acc; count += 4; }
	if (mode & LOAD_PWM) { *( (byte *)(cmdstr + count) ) = pwm; count += 1; }
	
	return NmcSendCmd(addr, LOAD_TRAJ, cmdstr, (byte)count, addr);
}
//---------------------------------------------------------------------------
void Cqnxmc::ServoInitPath(byte addr)
{
	SERVOMOD * p;
	
	NmcReadStatus(addr, SEND_POS | SEND_PERROR);
	
	p = (SERVOMOD *)(mod[addr].p);
	p->last_ppoint = p->pos + p->perror;
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoAddPathpoints(byte addr, int npoints, long *path, int freq)
{
	SERVOMOD * p;
	char cmdstr[16];
	long diff;
	int rev;
	int i;
	
	p = (SERVOMOD *)(mod[addr].p);
	
	for (i=0; i<npoints; i++)
	{
		diff = path[i] - p->last_ppoint;
		if (diff<0)
		{
			rev = 0x01;
			diff = -diff;
		}
		else rev = 0x00;
		
		//Scale the difference appropriately for path freq. used
		if (p->ioctrl & FAST_PATH)  //scale for 60/120 Hz fast path
		{
			if (freq == P_60HZ)
			{
				diff *= (256/32);
				diff |= 0x02;     //60 Hz -> set bit 1 = 1
			}
			else if (freq == P_120HZ) diff *= (256/16);
			else return(0);
		}
		else  //scale for 30/60 Hz slow path
		{
			if (freq == P_30HZ)
			{
				diff *= (256/64);
				diff |= 0x02;     //30 Hz -> set bit 1 = 1
			}
			else if (freq == P_60HZ) diff *= (256/32);
			else return(0);
		}
		
		diff |= rev;  //bit 0 = reverse bit
		
		*( (short int *)(cmdstr + 2*i) ) = (short int)diff;
		
		p->last_ppoint = path[i];
	}
	
	
	return NmcSendCmd(addr, ADD_PATHPOINT, cmdstr, (byte)(npoints*2), addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoStartPathMode(byte groupaddr, byte groupleader)
{
	return NmcSendCmd(groupaddr, ADD_PATHPOINT, NULL, 0, groupleader);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoResetPos(byte addr)
{
	return NmcSendCmd(addr, RESET_POS, NULL, 0, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoResetRelHome(byte addr)
{
	byte mode;
	
	mode = REL_HOME;
	return NmcSendCmd(addr, RESET_POS, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoClearBits(byte addr)
{
	return NmcSendCmd(addr, CLEAR_BITS, NULL, 0, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoStopMotor(byte addr, byte mode)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	
	mode &= (byte)(~STOP_HERE);
	p->stopctrl = mode;       //make sure STOP_HERE bit is cleared
	
	return NmcSendCmd(addr, STOP_MOTOR, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoSetIoCtrl(byte addr, byte mode)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	
	p->ioctrl = mode;
	
	return NmcSendCmd(addr, IO_CTRL, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::ServoSetHoming(byte addr, byte mode)
{
	SERVOMOD * p;
	
	p = (SERVOMOD *)(mod[addr].p);
	p->homectrl = mode;
	
	return NmcSendCmd(addr, SET_HOMING, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------

char Cqnxmc::ServoSetPhase(byte addr, int padvance, int poffset, int maxpwm)
{
	int i;
	char mode;
	SERVOMOD * p;
	
	if (maxpwm>255 || maxpwm<128) return(0);
	
	p = (SERVOMOD *)(mod[addr].p);
	p->ph_adv = (byte)padvance;
	p->ph_off = (byte)poffset;
	
	
	//First set PWM to 0:
	ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);  //set PWM to 0 now
	
	//Set phase advance:
	for (i=0; i<padvance; i++)
	{
		mode = 0xC0; 	//set dir bit hi
		NmcSendCmd(addr, 0x04, &mode, 1, addr);
		mode = 0x80; 	//set dir bit lo
		NmcSendCmd(addr, 0x04, &mode, 1, addr);
	}
	//Toggle PWM high then low:
	ServoLoadTraj(addr, 0x88, 0, 0, 0, 255);  //set PWM to 255 now
	ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);    //set PWM to 0 now
	
	//Set index phasing count (icount):
	for (i=0; i<poffset; i++)
	{
		mode = 0xC0; 	//set dir bit hi
		NmcSendCmd(addr, 0x04, &mode, 1, addr);
		mode = 0x80; 	//set dir bit lo
		NmcSendCmd(addr, 0x04, &mode, 1, addr);
	}
	
	ServoClearBits(addr);  //Clear any position errors
	
	for (i=127; i<=maxpwm; i++)    
	{
		ServoLoadTraj(addr, 0x88, 0, 0, 0, (byte)i);  //ramp up PWM to 255
		if ( (NmcGetStat(addr) & POS_ERR) )     //check if power still there
		{
			ServoLoadTraj(addr, 0x88, 0, 0, 0, 0);   // set PWM to zero on error
			break;
		}
	}
	
	delay(500);
	return( ServoLoadTraj(addr, 0x88, 0, 0, 0, 0) );    //set PWM to 0 now
	
}

//Returns pointer to an initialized IOMOD structure
IOMOD * Cqnxmc::IoNewMod()
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
char Cqnxmc::IoGetStat(byte addr)
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
char Cqnxmc::IoInBitVal(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);
	return ((p->inbits >> bitnum) & 1);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoInBitSVal(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);
	return ((p->inbits_s >> bitnum) & 1);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoOutBitVal(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);
	return ((p->outbits >> bitnum) & 1);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoSetOutBit(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	p->outbits = p->outbits | (short int)(1 << bitnum);
	
	return NmcSendCmd(addr, SET_OUTPUT, (char *)(&(p->outbits)), 2, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoClrOutBit(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	p->outbits = p->outbits & (short int)(~(1 << bitnum));
	
	return NmcSendCmd(addr, SET_OUTPUT, (char *)(&(p->outbits)), 2, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoGetBitDir(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);
	return ((p->bitdir >> bitnum) & 1);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoBitDirOut(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	p->bitdir = p->bitdir & (short int)(~(1 << bitnum));
	
	return NmcSendCmd(addr, SET_IO_DIR, (char *)(&(p->bitdir)), 2, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoBitDirIn(byte addr, int bitnum)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	p->bitdir = p->bitdir | (short int)(1 << bitnum);
	
	return NmcSendCmd(addr, SET_IO_DIR, (char *)(&(p->bitdir)), 2, addr);
}
//---------------------------------------------------------------------------
byte Cqnxmc::IoGetADCVal(byte addr, int channel)
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
char Cqnxmc::IoSetPWMVal(byte addr, byte pwm1, byte pwm2)
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
char Cqnxmc::IoSetSynchOutput(byte addr, short int outbits, byte pwm1, byte pwm2)
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
byte Cqnxmc::IoGetPWMVal(byte addr, int channel)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	if (channel == 0) return(p->pwm1);
	else return(p->pwm2);
}
//---------------------------------------------------------------------------
char Cqnxmc::IoSetTimerMode(byte addr, byte tmrmode)
{
	IOMOD * p;
	char cmdstr[2];
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	p->timermode = tmrmode;
	cmdstr[0] = tmrmode;
	
	return NmcSendCmd(addr, SET_TMR_MODE, cmdstr, 1, addr);
}
//---------------------------------------------------------------------------
byte Cqnxmc::IoGetTimerMode(byte addr)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	return p->timermode;
}
//---------------------------------------------------------------------------
unsigned long Cqnxmc::IoGetTimerVal(byte addr)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	return(p->timer);
}
//---------------------------------------------------------------------------
unsigned long Cqnxmc::IoGetTimerSVal(byte addr)
{
	IOMOD * p;
	
	p = (IOMOD *)(mod[addr].p);  			//Point to the IO data structure
	return(p->timer_s);
}
//---------------------------------------------------------------------------

STEPMOD * Cqnxmc::StepNewMod()
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
char Cqnxmc::StepGetStat(byte addr)
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
long Cqnxmc::StepGetPos(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->pos;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetAD(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->ad;
}
//---------------------------------------------------------------------------
unsigned short int Cqnxmc::StepGetStepTime(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->st;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetInbyte(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->inbyte;
}
//---------------------------------------------------------------------------
long Cqnxmc::StepGetHome(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->home;
}
//---------------------------------------------------------------------------
long Cqnxmc::StepGetCmdPos(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->cmdpos;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetCmdSpeed(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->cmdspeed;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetCmdAcc(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->cmdacc;
}
//---------------------------------------------------------------------------
unsigned short int Cqnxmc::StepGetCmdST(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->cmdst;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetMinSpeed(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->min_speed;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetOutputs(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->outbyte;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetCtrlMode(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->ctrlmode;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetRunCurrent(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->run_pwm;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetHoldCurrent(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->hold_pwm;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetThermLimit(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->therm_limit;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetHomeCtrl(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->homectrl;
}
//---------------------------------------------------------------------------
byte Cqnxmc::StepGetStopCtrl(byte addr)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	return p->stopctrl;
}
//---------------------------------------------------------------------------
char Cqnxmc::StepSetParam(byte addr, byte mode,
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
char Cqnxmc::StepLoadTraj(byte addr, byte mode,
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
char Cqnxmc::StepResetPos(byte addr)
{
	return NmcSendCmd(addr, RESET_POS, NULL, 0, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::StepStopMotor(byte addr, byte mode)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	
	p->stopctrl = mode;
	
	return NmcSendCmd(addr, STOP_MOTOR, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::StepSetOutputs(byte addr, byte outbyte)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	
	p->outbyte = outbyte;
	
	return NmcSendCmd(addr, SET_OUTPUTS, (char *)(&outbyte), 1, addr);
}
//---------------------------------------------------------------------------
char Cqnxmc::StepSetHoming(byte addr, byte mode)
{
	STEPMOD * p;
	
	p = (STEPMOD *)(mod[addr].p);
	p->homectrl = mode;
	
	return NmcSendCmd(addr, SET_HOMING, (char *)(&mode), 1, addr);
}
//---------------------------------------------------------------------------

