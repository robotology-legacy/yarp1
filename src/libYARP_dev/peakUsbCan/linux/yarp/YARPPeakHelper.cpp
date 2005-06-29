
// SerialProtocol.cpp: implementation of the SerialProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPPeakHelper.h"
#include "libpcan.h"
#include <yarp/YARPAll.h>
#include <ace/OS.h>
#include <fcntl.h> //O_RDWR

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PeakHelper::PeakHelper()
{
  _handle = NULL;
  _inBuf = new TPCANMsg;
  _outBuf = new TPCANMsg;
}

PeakHelper::~PeakHelper()
{
  close();
}

int PeakHelper::write(const char *data)
{
  int err;
  TPCANMsg *msg = (TPCANMsg *) _outBuf;

  msg->LEN=8;
  memcpy(msg->DATA, data, 8);

  //  printf("Writing to can %x %d\n", msg->ID, msg->LEN);
  
  err = CAN_Write(_handle, msg);
  
  if (err=CAN_ERR_OK)
    return YARP_FAIL;
  
  return YARP_OK;
}

int PeakHelper::read(char *data)
{
  int n;
  int err;
  TPCANMsg *msg = (TPCANMsg *) _inBuf;
 
  // DEBUG
  err = CAN_Read(_handle, msg);
  //  printf("Read from can %x %d\n", msg->ID, msg->LEN);
    
  if (err!=CAN_ERR_OK)
    return YARP_FAIL;

  memcpy(data, msg->DATA, 8);
  
  return YARP_OK;
}

// dummy read, nothing is returned
int PeakHelper::read()
{
  int n;
  int err;
  TPCANMsg *msg = (TPCANMsg *) _inBuf;
 
  // DEBUG
  err = CAN_Read(_handle, msg);
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
  err = CAN_Init(_handle, p->_wBTR0BTR1, p->_frameType);

  if (err!=CAN_ERR_OK)
    return YARP_FAIL;

  return YARP_OK;
}

