
#include "q_sio_util.h"

#include <unistd.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <fcntl.h>
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

//---------------------------------------------------------------------------
 int SimpleMsgBox(char *msgstr)
{
	printf("Message:\n%s\n",msgstr);
	//printf("Press any key to continue...\n");
	//getchar();
	 return (1);
	 
}

//---------------------------------------------------------------------------
 int ErrorMsgBox(char *msgstr)
{
	

		printf("Error:\n %s\n",msgstr);
		printf("Press any key to continue...\n");
		getchar();
		return (1);

}
//---------------------------------------------------------------------------
//Opens "COM1:" thru "COM4:", returns a handle to be used by other
//SIO operations.  Sets up read and write timeouts.
//*** Add parameter for baud rate ***
int SioOpen(char *name, unsigned int baudrate)
{
	int ComHandle;
	char msgstr[50];


	struct termios p_termios;
	speed_t speed;


//Open COM port as a file

	ComHandle = open( name, O_RDWR );
	

	if (ComHandle == -1)
	{
			sprintf(msgstr,"%s failed to open",name);
			ErrorMsgBox(msgstr);
			
			
    }
	else
	{
		printf("Serial Port Open. ID= %d\n",ComHandle);
	}

	if( tcgetattr(ComHandle,&p_termios) )
	{
		sprintf(msgstr,"%s failed to get attr.",name);
		ErrorMsgBox(msgstr);
		
	}


	//RAW mode
	//revisar
	p_termios.c_cc[VMIN]=0;
	p_termios.c_cc[VTIME]=0;
	p_termios.c_lflag &= ~(ECHO|ICANON|ISIG|ECHOE|ECHOK|ECHONL);
	p_termios.c_oflag &= ~(OPOST);



  speed=9600;
	cfsetispeed(&p_termios,speed);

  cfsetospeed(&p_termios,speed);

  printf("CHECK\n");
    tcsetattr(ComHandle,TCSADRAIN,&p_termios);
/*  if( tcsetattr(ComHandle,TCSANOW,&p_termios)==-1)
    {
      printf("failed\n");
      fflush(stdout);
      return(0);
    }
  printf("DONE\n");
  fflush(stdout);
*/
 return(ComHandle);
  
    /*
    cc.dcb.BaudRate = winrate;
    cc.dcb.fBinary = 1;
    cc.dcb.fParity = 0;
    cc.dcb.fOutxCtsFlow = 0;
    cc.dcb.fOutxDsrFlow = 0;
    cc.dcb.fDtrControl = DTR_CONTROL_DISABLE;
    cc.dcb.fDsrSensitivity = 0;
    cc.dcb.fTXContinueOnXoff = 0;
    cc.dcb.fOutX = 0;
    cc.dcb.fInX = 0;
    cc.dcb.fErrorChar = 0;
    cc.dcb.fNull = 0;
    cc.dcb.fRtsControl = RTS_CONTROL_DISABLE;
    cc.dcb.fAbortOnError = 0;
    cc.dcb.XonLim = 100;
    cc.dcb.XoffLim = 100;
    cc.dcb.ByteSize = 8;
    cc.dcb.Parity = NOPARITY;
    cc.dcb.StopBits = ONESTOPBIT;
    cc.dcb.XonChar = 'x';
    cc.dcb.XoffChar = 'y';
    cc.dcb.ErrorChar = 0;
    cc.dcb.EofChar = 0;
    cc.dcb.EvtChar = 0;
	*/

    /*
    //Set read/write timeout values for the file
    ct.ReadIntervalTimeout = MAXDWORD;  		//ignore interval timing
    ct.ReadTotalTimeoutMultiplier = 2; 	//ingore cumulative interval timeout
    ct.ReadTotalTimeoutConstant = 50;  		//use a total timeout of 50 msec
    ct.WriteTotalTimeoutMultiplier = 2;	//Set max time per char written
    ct.WriteTotalTimeoutConstant = 50;	//plus additional time
	*/
	

}
//---------------------------------------------------------------------------
//Change the baud rate to the specified values.  Valid rates are:
//9600, 19200, 38400, 57600, 115200.  Returns 1 on success.
 char SioChangeBaud(int ComPort, unsigned int baudrate)
{
	struct termios p_termios;
	speed_t speed;

	if( tcgetattr(ComPort,&p_termios) )
	{
		ErrorMsgBox("Failed to change speed\n");
		exit(-1);
	}

	
	speed=(speed_t)baudrate;
	cfsetispeed(&p_termios,speed);
	cfsetospeed(&p_termios,speed);
	if( tcsetattr(ComPort,TCSADRAIN,&p_termios)==-1)
	{
		return(0);
	}
	else
	{
		return(1);
	}

}

//---------------------------------------------------------------------------
//Write out N chars to the comport, returns only after chas have been sent
//return 0 on failure, non-zero on success
char SioPutChars(int ComPort, char *stuff, int n)
{
	ssize_t RetStat;
	
	//int l=strlen(stuff);
	RetStat=write(ComPort,stuff,n);
	if(RetStat==-1)
	{
		ErrorMsgBox("SioPutChars failed");
		return(0);
	}
	
	return(1);

}
//---------------------------------------------------------------------------
//Read n chars into the array stuff (not null terminated)
//Function returns the number of chars actually read.
int SioGetChars(int ComPort, char *stuff, int n)
{
	int numread;

	/*
    //Set read/write timeout values for the file
    ct.ReadIntervalTimeout = MAXDWORD;  		//ignore interval timing
    ct.ReadTotalTimeoutMultiplier = 2; 	//ingore cumulative interval timeout
    ct.ReadTotalTimeoutConstant = 50;  		//use a total timeout of 50 msec
    
	*/
	

	numread = dev_read(ComPort, stuff, n, n, 1, 1, 0, 0 );
	
	if(numread==-1)
	{
		ErrorMsgBox("SioReadChars failed\n");
		numread=0;
	}

	return numread;
}
//---------------------------------------------------------------------------
//Returns the number of chars in a port's input buffer
/*
int  SioTest(int ComPort)
{
COMSTAT cs;
DWORD Errors;
char RetStat;

RetStat = ClearCommError(ComPort, &Errors, &cs);
if (RetStat == 0) ErrorMsgBox("SioTest failed");
return cs.cbInQue;
}
*/
//---------------------------------------------------------------------------
//Purge all chars from the input buffer
char SioClrInbuf(int ComPort)
{

	
	int RetStat;
	RetStat=tcflush(ComPort,TCIFLUSH);
	if (RetStat == -1)
	{
		ErrorMsgBox("SioClrInbuf failed");
		
		return(0);
	}
	return (1);
}
//---------------------------------------------------------------------------
//Close a previously opened COM port
char SioClose(int ComPort)
{
	int d;
	d=close(ComPort);
	if(!d)
	{
		return(1);
	}
	else
	{
		return(0);
	}
	
}
//---------------------------------------------------------------------------

