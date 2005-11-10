/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #nat#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPPeakSerialDeviceDriver.cpp,v 1.9 2005-11-10 20:11:13 natta Exp $
///
///
/// June 05 -- by nat

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>

/// specific to this device driver.
#include "YARPPeakSerialDeviceDriver.h"
#include "can_messages.h"
const int MAX_ADC = 15;
const int __nj = 6;
#include <yarp/YARPTime.h>

/// get the message types from the DSP code.
// #include "../56f807/cotroller_dc/Code/controller.h"

YARPPeakSerialDeviceDriver::YARPPeakSerialDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPPeakSerialDeviceDriver>(CBNCmds), _mutex(1)
{
  m_cmds[CMDGetPosition] = &YARPPeakSerialDeviceDriver::getPosition;
  m_cmds[CMDGetPositions] = &YARPPeakSerialDeviceDriver::getPositions;
  m_cmds[CMDSetPositionControlMode] = &YARPPeakSerialDeviceDriver::setPositionMode;
  m_cmds[CMDSetPosition] = &YARPPeakSerialDeviceDriver::setPosition;
  m_cmds[CMDSetPositions] = &YARPPeakSerialDeviceDriver::setPositions;
  m_cmds[CMDGetSpeeds] = &YARPPeakSerialDeviceDriver::getSpeeds;
  m_cmds[CMDGetAccelerations] = &YARPPeakSerialDeviceDriver::getAccelerations;
  m_cmds[CMDGetSpeed] = &YARPPeakSerialDeviceDriver::getSpeed;
  m_cmds[CMDGetTorque] = &YARPPeakSerialDeviceDriver::getTorque;
  m_cmds[CMDGetTorques] = &YARPPeakSerialDeviceDriver::getTorques;
  m_cmds[CMDGetPWMs] = &YARPPeakSerialDeviceDriver::getPWMs;
  m_cmds[CMDGetPWM] = &YARPPeakSerialDeviceDriver::getPWM;
  m_cmds[CMDReadAnalog] = &YARPPeakSerialDeviceDriver::readAnalog;
  m_cmds[CMDServoHere] = &YARPPeakSerialDeviceDriver::servoHere;

  m_cmds[CMDGetPIDError] = &YARPPeakSerialDeviceDriver::getPIDError;
  m_cmds[CMDRelativeMotion] = &YARPPeakSerialDeviceDriver::relativeMotion;
  m_cmds[CMDRelativeMotionMultiple] = &YARPPeakSerialDeviceDriver::relativeMotionMultiple;

  m_cmds[CMDSetForceControlMode]=&YARPPeakSerialDeviceDriver::setForceMode;
  m_cmds[CMDSetOffset] = &YARPPeakSerialDeviceDriver::setOffset;
  m_cmds[CMDSetOffsets] = &YARPPeakSerialDeviceDriver::setOffsets;

  m_cmds[CMDSetPID] = &YARPPeakSerialDeviceDriver::setPid;
  m_cmds[CMDGetPID] = &YARPPeakSerialDeviceDriver::getPid;

  _nj = 0;

  _tmpDouble = NULL;
}

YARPPeakSerialDeviceDriver::~YARPPeakSerialDeviceDriver ()
{
  if (_tmpDouble != NULL)
    delete [] _tmpDouble;
}

int YARPPeakSerialDeviceDriver::open (void *res)
{
  _mutex.Wait();

  PeakOpenParameters *par = (PeakOpenParameters *)(res);
  _nj = par->_njoints;
  int ret = _canPort.open(par);

  ACE_ASSERT (_nj==__nj);	// LATER: remove this and __nj
	
  if (_tmpDouble != NULL)
    delete [] _tmpDouble;
  _tmpDouble = new double[_nj];
	
  _mutex.Post();

  return ret;
}

int YARPPeakSerialDeviceDriver::close (void)
{
  _mutex.Wait(); 
  
  int ret = _canPort.close();
	
  if (_tmpDouble != NULL)
    delete [] _tmpDouble;

  _tmpDouble = NULL;

  _mutex.Post();
  return ret;
}

int YARPPeakSerialDeviceDriver::getPosition(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  unsigned int value;

  ACE_ASSERT (axis >= 0 && axis <= (MAX_ADC));

  ret = _readUWord(CAN_READ_ANALOG, axis, value, CAN_REPLY_NO_CHECK);
  // if ret != YARP_OK value == 0, so no need to check ret

  *((double *)tmp->parameters) = (double) (value);
	
  return ret;
}

int YARPPeakSerialDeviceDriver::getPIDError(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  int value;

  ACE_ASSERT (axis >= 0 && axis <= (MAX_ADC));

  ret = _readSWord(CAN_READ_POS_ERROR, axis, value, CAN_REPLY_NO_CHECK);

  // if ret != YARP_OK value == 0, so no need to check ret
  *((double *)tmp->parameters) = (double) (value);
	
  return ret;
}

int YARPPeakSerialDeviceDriver::getPid(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
  int value;

  ret = _readSWord(CAN_GET_PID, axis, value, CAN_REPLY_NO_CHECK);

  pid->KP = (double) value;
  pid->KI = 0;
  pid->KD = 0;
  pid->AC_FF = 0;
  pid->VEL_FF = 0;
  pid->I_LIMIT = 0;
  pid->OFFSET = 0;
  pid->T_LIMIT = 0;
  pid->SHIFT = 0;
  pid->FRICT_FF = 0;

  return ret;
}

int YARPPeakSerialDeviceDriver::setPid(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
  int value = ( ((int) pid->KP & 0x00007FFF)); //prevent overflow, make always > 0

  ret = _writeWord(CAN_SET_PID, axis, value);

  return ret;
}

int YARPPeakSerialDeviceDriver::getPositions(void *cmd)
{
  int ret;
  ACE_ASSERT (cmd!=NULL);
  double *tmp = (double *) cmd;
  ///=  fprintf(stderr, "A");
  ret = _readU16Vector(CAN_READ_POSITIONS_0TO3, tmp, 3, CAN_REPLY_POSITIONS1);
  //  fprintf(stderr, "B");
  //fprintf(stderr, "%.2lf %.2lf %.2lf %.2lf\n", tmp[0], tmp[1], tmp[2], tmp[3]);
  if (ret == YARP_FAIL)
    return YARP_FAIL;
  //  fprintf(stderr, "C");
  ret = _readU16Vector(CAN_READ_POSITIONS_4TO5, tmp+3, 3, CAN_REPLY_POSITIONS2);
  //  fprintf(stderr, " %.2lf %.2lf\n", tmp[4], tmp[5]);
  return ret;
}

int YARPPeakSerialDeviceDriver::setPositionMode(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  int value;
	
  ret = _writeWord(CAN_POSITION_CONTROL_MODE, axis, CAN_REPLY_NO_CHECK);
  return ret;
}

int YARPPeakSerialDeviceDriver::setForceMode(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  int value;
	
  ret = _writeWord(CAN_FORCE_CONTROL_MODE, axis, CAN_REPLY_NO_CHECK);
  return ret;
}

int YARPPeakSerialDeviceDriver::setPosition(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  double value = *((double *) tmp->parameters);

  ret = _writeWord(CAN_SET_POSITION, axis, (int)(value), CAN_REPLY_NO_CHECK);
  return ret;
}

int YARPPeakSerialDeviceDriver::setOffset(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  double value = *((double *) tmp->parameters);

  ret = _writeWord(CAN_SET_OFFSET, axis, (int)(value), CAN_REPLY_NO_CHECK);
  return ret;
}

int YARPPeakSerialDeviceDriver::relativeMotion(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  double value = *((double *) tmp->parameters);

  ret = _writeWord(CAN_RELATIVE_MOTION, axis, (int)(value), CAN_REPLY_NO_CHECK);
  return ret;
}

int YARPPeakSerialDeviceDriver::relativeMotionMultiple(void *cmd)
{
  int ret;
  
  double *tmp = (double *) cmd;

  ret =  _writeS16Vector(CAN_RELATIVE_MOTION_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
 
  if (ret!=YARP_OK)
    return ret;
  
  ret = _writeS16Vector(CAN_RELATIVE_MOTION_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);

  return ret;
}

int YARPPeakSerialDeviceDriver::setPositions(void *cmd)
{
  int ret;
  
  double *tmp = (double *) cmd;

  ret =  _writeU16Vector(CAN_SET_POSITIONS_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
 
  if (ret!=YARP_OK)
    return ret;
  
  ret = _writeU16Vector(CAN_SET_POSITIONS_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);

  return ret;
}

int YARPPeakSerialDeviceDriver::setOffsets(void *cmd)
{
  int ret;
  
  double *tmp = (double *) cmd;

  ret =  _writeS16Vector(CAN_SET_OFFSETS_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
 
  if (ret!=YARP_OK)
    return ret;
  
  ret = _writeS16Vector(CAN_SET_OFFSETS_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);

  return ret;
}

int YARPPeakSerialDeviceDriver::getPWM(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;

  ACE_ASSERT (axis >= 0 && axis <= __nj);

  ret = _readPWMGroup(CAN_READ_PWMS_0TO2, _tmpDouble, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readPWMGroup(CAN_READ_PWMS_3TO5, _tmpDouble+3, 3, CAN_REPLY_NO_CHECK);
  // if ret != YARP_OK _tmpDouble[..] = 0, so no need to check ret
  *((double *)tmp->parameters) = _tmpDouble[axis];
  return ret;
}

int YARPPeakSerialDeviceDriver::getPWMs(void *cmd)
{	
  int ret;
  double * tmp = (double *) cmd;

  ret = _readPWMGroup(CAN_READ_PWMS_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readPWMGroup(CAN_READ_PWMS_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);

  // if ret != YARP_OK tmp[..] = 0, so no need to check ret
  return ret;
}


int YARPPeakSerialDeviceDriver::getTorque(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;

  ACE_ASSERT (axis >= 0 && axis <= __nj);

  ret = _readS16Vector(CAN_READ_TORQUES_0TO2, _tmpDouble, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readS16Vector(CAN_READ_TORQUES_3TO5, _tmpDouble+3, 3, CAN_REPLY_NO_CHECK);
	
  *((double *)tmp->parameters) = _tmpDouble[axis];
	
  return ret;
}


int YARPPeakSerialDeviceDriver::getTorques(void *cmd)
{
  int ret;
  ACE_ASSERT (cmd!=NULL);
  double *tmp = (double *) cmd;

  ret = _readS16Vector(CAN_READ_TORQUES_0TO2, tmp, 3, CAN_REPLY_TORQUES1);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readS16Vector(CAN_READ_TORQUES_3TO5, tmp+3, 3, CAN_REPLY_TORQUES2);

  return ret;
}

int YARPPeakSerialDeviceDriver::readAnalog(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;
  unsigned int value;

  ACE_ASSERT (axis >= 0 && axis <= (MAX_ADC));

  ret = _readUWord(CAN_READ_ANALOG, axis, value, CAN_REPLY_NO_CHECK);
  // if ret != YARP_OK value == 0, so no need to check ret
  *((double *)tmp->parameters) = (double) (value);
  return ret;
}

int YARPPeakSerialDeviceDriver::getSpeed(void *cmd)
{
  int ret;
  SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
  const int axis = tmp->axis;

  ret = _readS16Vector(CAN_READ_SPEEDS_0TO2, _tmpDouble, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readS16Vector(CAN_READ_SPEEDS_3TO5, _tmpDouble+3, 3, CAN_REPLY_NO_CHECK);

  *((double *)tmp->parameters) = (double) _tmpDouble[axis];
  return ret;
}

int YARPPeakSerialDeviceDriver::getSpeeds(void *cmd)
{
  int ret;
  ACE_ASSERT (cmd!=NULL);
  double *tmp = (double *) cmd;
  ret = _readS16Vector(CAN_READ_SPEEDS_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readS16Vector(CAN_READ_SPEEDS_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);
  return YARP_OK;
}

int YARPPeakSerialDeviceDriver::getAccelerations(void *cmd)
{
  int ret;
  ACE_ASSERT (cmd!=NULL);
  double *tmp = (double *) cmd;

  ret = _readS16Vector(CAN_READ_ACCS_0TO2, tmp, 3, CAN_REPLY_NO_CHECK);
  if (ret == YARP_FAIL)
    return YARP_FAIL;

  ret = _readS16Vector(CAN_READ_ACCS_3TO5, tmp+3, 3, CAN_REPLY_NO_CHECK);

  return YARP_OK;
}

int YARPPeakSerialDeviceDriver::servoHere(void *cmd)
{
  int ret;

  ret = _writeWord(CAN_STOP, CAN_REPLY_NO_CHECK);

  return ret;
}

int YARPPeakSerialDeviceDriver::_readPWMGroup(char msg, double *v, int n, char reply)
{
  int ret;
  bool ack=true;
	
  _message[0] = msg;
  ret = _canPort.write(_message,1);

  if (ret != YARP_OK)
    {
      for (int i = 0; i < n; i++)
	v[i] = 0.0;
      return ret;
    }

  ret = _canPort.read(_message);

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret != YARP_OK) || (!ack) )
    {
      for (int i = 0; i <n; i++)
	v[i] = 0.0;
      return YARP_FAIL;
    }
  else
    {
      // ok read pwms
      int jj;	// we skip the first byte
      char signs = _message[1];
      jj = 2;
      char mask = 0x04;
      for (int i = 0; i < n; i++)
	{	
	  if (signs&mask) 
	    v[i] = (unsigned int) _message[jj]+ ( (unsigned int) _message[jj+1]<<8);
	  else
	    v[i] = -( (unsigned int) _message[jj]+ ( (unsigned int) _message[jj+1]<<8));
			
	  jj+=2;
	}
		
      return YARP_OK;
    }
}

int YARPPeakSerialDeviceDriver::_readS16Vector(char msg, double *v, int n, char reply)
{
  int ret;
  bool ack=true;
	
  _message[0] = msg;

  ret = _canPort.write(_message,1);

  if (ret != YARP_OK)
    {
      for (int i = 0; i <n; i++)
	v[i] = -1.0;
      return ret;
    }

  ret = _canPort.read(_message);

  if (ret!=YARP_OK)
    {
      for (int i = 0; i <n; i++)
	v[i] = -1.0;
      return YARP_FAIL;
    }

  if (reply!=-1)
    {
      int count=0;
      // try again, forever
      while(_message[0]!=reply)
	{
	  count++;
	  fprintf(stderr, "Trying again (re-synch) %d\n", count);
	  ret=_canPort.read(_message);
	}
	ack=true;
    }
  else
    ack=true;

  int jj = 1;
  for (int i = 0; i <n; i++)
    {
      v[i] = (short)(_message[jj]+_message[jj+1]*256);
      jj+=2;
    }
  return YARP_OK;
}

int YARPPeakSerialDeviceDriver::_readU16Vector(char msg, double *v, int n, char reply)
{
  int ret;
  bool ack=true;
  ACE_ASSERT(n<=4);

  _message[0] = msg;

  ret = _canPort.write(_message,1);

  if (ret != YARP_OK)
    {
      for (int i = 0; i <n; i++)
	v[i] = -1.0;
      return ret;
    }

  ret = _canPort.read(_message);

  //  fprintf(stderr, "ACK_MESSAGE= %d\n", _message[0]);
  if (ret!=YARP_OK)
    {
      for (int i = 0; i <n; i++)
	v[i] = -1.0;
      return YARP_FAIL;
    }

  if (reply!=-1)
    {
      int count=0;
      // try again, forever
      while(_message[0]!=reply)
	{
	  count++;
	  fprintf(stderr, "Trying again (re-synch) %d\n", count);
	  ret=_canPort.read(_message);
	}
	ack=true;
    }
  else
    ack=true;

  int jj = 1;//we skip the first byte
  for (int i = 0; i <n; i++)
    {
      v[i] = ((unsigned int) _message[jj]+( (unsigned int) _message[jj+1]<<8));
      jj+=2;
    }
  return YARP_OK;
}

int YARPPeakSerialDeviceDriver::_readSWord(char msg, char joint, int &value, char reply)
{
  int ret;
  bool ack=true;

  _message[0] = msg;
  _message[1] = joint;

  ret = _canPort.write(_message,2);

  if (ret != YARP_OK)
    {
      value = -1;
      return ret;
    }

  ret = _canPort.read(_message);

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ((ret != YARP_OK)||(ack!=true))
    {
      value = 0;
      return YARP_FAIL;
    }
  else
    {
      value = (short)(_message[1]+_message[2]*256);
      return YARP_OK;
    }
}

int YARPPeakSerialDeviceDriver::_readUWord(char msg, char joint, unsigned int &value, char reply)
{
  int ret;
  bool ack=true;

  _message[0] = msg;
  _message[1] = joint;

  ret = _canPort.write(_message,2);

  if (ret != YARP_OK)
    {
      value = 0;
      return ret;
    }

  ret = _canPort.read(_message);

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret != YARP_OK)||(ack!=true))
    {
      value = 0;
      return YARP_FAIL;
    }
  else
    {
      value = ((unsigned int) _message[1]+ ( (unsigned int) _message[2]<<8) );
      return YARP_OK;
    }
}

int YARPPeakSerialDeviceDriver::_writeWord(char msg, char joint, char reply)
{
  int ret;
  bool ack=true;

  _message[0] = msg;
  _message[1] = joint;
  ret = _canPort.write(_message,2);

  // given the protocol for each write we have a read back
  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;
  
  return ret;
}

int YARPPeakSerialDeviceDriver::_writeWord(char msg, char joint, int value, char reply)
{
  int ret;
  bool ack=true;

  _message[0] = msg;
  _message[1] = joint;
  _message[2] = value;
  _message[3] = (value>>8);

  ret = _canPort.write(_message,4);
  // given the protocol for each write we have a read back
  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;
  
  return ret;
}

int YARPPeakSerialDeviceDriver::_writeVector(char msg, const int *values, int n, char reply)
{
  int ret;
  int k;
  unsigned char *tmp;
  bool ack=true;

  ACE_ASSERT(n<=3);

  tmp = (unsigned char *) (values);

  _message[0] = msg;
  
  for(k=0; k<(2*n); k++)
    _message[k+1]=tmp[k];

  ret = _canPort.write(_message,(1+2*n));
  // given the protocol for each write we have a read back
  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;
  
  return ret;
}

int YARPPeakSerialDeviceDriver::_writeU16Vector(char msg, const double *values, int n, char reply)
{
  int ret;
  unsigned int tmp;
  bool ack=true;

  ACE_ASSERT(n<=3);

  _message[0] = msg;
  
  int k=1;
  for(int i=0; i<n; i++)
    {
      tmp = (unsigned int) values[i];
      _message[k++] = tmp;
      _message[k++]= tmp>>8;
    }

  ret = _canPort.write(_message, (1+2*n));
  // given the protocol for each write we have a read back
  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;
  
  return ret;
}

int YARPPeakSerialDeviceDriver::_writeS16Vector(char msg, const double *values, int n, char reply)
{
  int ret;
  int k;
  int tmp;
  bool ack=true;

  ACE_ASSERT(n<=3);

  _message[0] = msg;
  
  k=1;
  for(int i=0; i<n; i++)
    {
      tmp = (int) values[i];
      _message[k++] = tmp;
      _message[k++]= tmp>>8;
    }

  ret = _canPort.write(_message,(1+2*n));
  // given the protocol for each write we have a read back

  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;

  return ret;
}

int YARPPeakSerialDeviceDriver::_writeWord(char msg, char reply)
{
  int ret;
  bool ack=true;

  _message[0] = msg;
  
  ret = _canPort.write(_message,1);
  // given the protocol for each write we have a read back
  ret = _canPort.read();

  if (reply!=-1)
    {
      if (_message[0]==reply)
	ack=true;
      else
	ack=false;
    }
  else
    ack=true;

  if ( (ret!=YARP_OK) || (ack!=true))
    ret=YARP_FAIL;
  else
    ret=YARP_OK;
  
  return ret;
}
