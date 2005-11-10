
// SerialProtocol.cpp: implementation of the SerialProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPPeakHelper.h"
#include "libpcan.h"
#include <yarp/YARPAll.h>
#include <ace/OS.h>
#include <fcntl.h> //O_RDWR

const int TIMEOUT=100000; //100ms

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PeakHelper::PeakHelper()
{
  _handle = NULL;
  _inBuf = new TPCANRdMsg;
  _outBuf = new TPCANMsg;
}

PeakHelper::~PeakHelper()
{
  close();
}

int PeakHelper::write(const unsigned char *data, int len)
{
  int err;
  TPCANMsg *msg = (TPCANMsg *) _outBuf;

  msg->LEN=len;
  memcpy(msg->DATA, data, msg->LEN);

  //printf("Writing to can %x %d\n", msg->ID, msg->LEN);
  
  err = CAN_Write(_handle, msg);
  
  if (err!=CAN_ERR_OK)
    return YARP_FAIL;
  
  return YARP_OK;
}

int PeakHelper::read(unsigned char *data)
{
  int n;
  int err;
  TPCANRdMsg *msg = (TPCANRdMsg *) _inBuf;
 
  err = LINUX_CAN_Read_Timeout(_handle, msg, TIMEOUT);
  if (err!=CAN_ERR_OK)
      fprintf(stderr, "Read(char *) returned %d\n", err);

  //  fprintf(stderr, "Read from can %x %d\n", msg->Msg.ID, msg->Msg.LEN);
  //fprintf(stderr, "%.2x%.2x", msg->DATA[0], msg->DATA[1]); 
    
  if (err!=CAN_ERR_OK)
    return YARP_FAIL;

  memcpy(data, msg->Msg.DATA, msg->Msg.LEN);

  return YARP_OK;
}

// dummy read, nothing is returned
int PeakHelper::read()
{
  int n;
  int err;
  TPCANRdMsg *msg = (TPCANRdMsg *) _inBuf;
 
  //  err = CAN_Read(_handle, msg);
  err = LINUX_CAN_Read_Timeout(_handle, msg, TIMEOUT);
  if (err!=CAN_ERR_OK)
    fprintf(stderr, "Read(): returned %d, maybe a timeout?\n",err);

  //  printf("Read from can %x %d\n", msg->ID, msg->LEN);
  
  if (err!=CAN_ERR_OK)
    return YARP_FAIL;

  return YARP_OK;
}

int PeakHelper::close()
{
  CAN_Close(_handle);

  return YARP_OK;
}

int PeakHelper::open(PeakOpenParameters *p)
{
  int err;
  TPCANMsg *msg = (TPCANMsg *) _outBuf;

  msg->ID=p->_id;
  //  printf("\n%x\n", msg->ID);
  
  msg->LEN=8;
    
  _handle=LINUX_CAN_Open(p->_portname, O_RDWR);
  if(_handle==NULL)
    return YARP_FAIL;

  err = CAN_Status(_handle);
  fprintf(stderr, "Status: %d\n", err);
  err = CAN_Init(_handle, p->_wBTR0BTR1, p->_frameType);

  if (err!=CAN_ERR_OK)
    return YARP_FAIL;

  return YARP_OK;
}
