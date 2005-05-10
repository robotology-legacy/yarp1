// ParallelPort.cpp: implementation of the ParallelPort class.
//
//////////////////////////////////////////////////////////////////////

#include "ParallelPort.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include<sys/time.h>
#include<time.h>


#include <linux/ppdev.h>
#include<linux/parport.h>

#define DATSTB_MASK PARPORT_CONTROL_AUTOFD
#define ADDSTB_MASK PARPORT_CONTROL_SELECT
#define WRITE_MASK  PARPORT_CONTROL_STROBE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParallelPort::ParallelPort()
{

}

ParallelPort::~ParallelPort()
{

}

int ParallelPort::openport(char *portname)
{

  int mode,r;
  unsigned char controlreg=0;
  timeval timeout;

  int i;

  fd = open(portname,O_RDWR);
  printf("%d\n",fd);
  if(fd== -1)
    { 
      return(-1);
    }
  if( r=ioctl(fd,PPCLAIM) )
    {
      printf("Error=%d\n",r);
      return(-1);
    }
		
  ioctl(fd, PPGETTIME, &timeout);
  printf("Timeout= %ld secs : %ld usecs\n",timeout.tv_sec, timeout.tv_usec);
  timeout.tv_sec=0;
  timeout.tv_usec=100000L;
	
  ioctl(fd, PPSETTIME, &timeout); 
  ioctl(fd, PPGETTIME, &timeout);
  printf("Timeout= %ld secs : %ld usecs\n",timeout.tv_sec, timeout.tv_usec);
	
	
  ioctl(fd,PPRCONTROL,&controlreg);
  controlreg=(controlreg & 0xF0)| 0x04;
  ioctl(fd,PPWCONTROL, &controlreg);
	
  //mode=IEEE1284_MODE_EPP | IEEE1284_ADDR;
  mode=IEEE1284_MODE_EPP | IEEE1284_DATA;
	
  if( r=ioctl(fd,PPSETMODE, &mode) )
    {
      printf("Error=%d\n",r);
      return(-1);
    }
	
  i=0;
	
  do
    {
      portn[i]=portname[i];
      i++;

    }	
  while(portname[i]!=0);

  return(0);

}

int ParallelPort::writeaddress(unsigned char value)
{

  int mode=IEEE1284_MODE_EPP | IEEE1284_ADDR;
  int r;
  unsigned char data=value;
  long int i;

  ioctl(fd,PPSETMODE, &mode);
  r=write(fd,&data,1);

  //usleep(DELAY_EPP);
  //nanosleep(&delay_epp,NULL);

  //check this out, the compiler could decide to take it out
  for(i=0;i<DELAY_EPP;i++);
  //printf("ADDR:%d %2X\n",r,data);
  return(1);

}

int ParallelPort::writedata(unsigned char value)
{

  int mode=IEEE1284_MODE_EPP | IEEE1284_DATA;
  int r;
  unsigned char data=value;
  long int i;

  ioctl(fd,PPSETMODE, &mode);
  r=write(fd,&data,1);
	
  for(i=0;i<DELAY_EPP;i++);

  //nanosleep(&delay_epp,NULL);
  //usleep(DELAY_EPP);
  //printf("DATA:%d %d",r,data);
  
  return(1);

}

int ParallelPort::readdata(unsigned char * value)
{
  int mode=IEEE1284_MODE_EPP | IEEE1284_DATA;
  int r;

  if( r=ioctl(fd,PPSETMODE, &mode) )
    {
      printf("Error=%d\n",r);
      return(-1);
    }
  r=read(fd,value,1);
  //printf("<%d %2X>",r,*value);

  
  return(1);

}

int ParallelPort::closeport()
{
  ioctl(fd,PPRELEASE);
  close(fd);
}

int ParallelPort::contolline(unsigned char mask, char value)
{
  unsigned char controlreg;
	
  ioctl(fd,PPRCONTROL,&controlreg);
  //printf("CONTROl=%2X\n",controlreg);
	
  switch(mask)
    {
    case DATSTB_MASK:
      if(value==0)
	{
	  controlreg=controlreg|DATSTB_MASK;
	}
      else
	{
	  controlreg=controlreg & (!DATSTB_MASK);
	}
      break;
    case ADDSTB_MASK:
      if(value==0)
	{
	  controlreg=controlreg|ADDSTB_MASK;
	}
      else
	{
	  controlreg=controlreg & (!ADDSTB_MASK);
	}
      break;
    case WRITE_MASK:
      if(value==0)
	{
	  controlreg=controlreg|WRITE_MASK;
	}
      else
	{
	  controlreg=controlreg & (!WRITE_MASK);
	}
      break;
    }
  ioctl(fd,PPWCONTROL, &controlreg);
  printf("CONTROl=%2X\n",controlreg);
	
  return(1);


}
