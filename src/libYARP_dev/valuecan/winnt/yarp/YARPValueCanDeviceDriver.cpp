/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
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
/// $Id: YARPValueCanDeviceDriver.cpp,v 1.22 2005-04-17 15:41:55 babybot Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPScheduler.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

/// specific to this device driver.
#include "YARPValueCanDeviceDriver.h"
#include "../dd_orig/include/icsnVC40.h"

/// get the message types from the DSP code.
#define __ONLY_DEF
#include "../56f807/cotroller_dc/Code/controller.h"

const int MAX_CHANNELS	= 255;
const int MAX_NID		= 16;
const int BUF_SIZE		= 20000;
const int VALUE_CAN_SKIP_ADDR = 0x80;


typedef int (*PV) (char *fmt, ...);


class ValueCanResources
{
public:
	ValueCanResources ();
	~ValueCanResources ();

	int initialize (const ValueCanOpenParameters& parms);
	int uninitialize ();
	int read ();

	int startPacket ();
	int addMessage (int msg_id, int joint);
	int writePacket ();

	int printMessage (const icsSpyMessage& m);
	int dumpBuffers (void);
	inline int getJoints (void) const { return _njoints; }
	inline int getErrorStatus (void) const { return _error_status; }
	
public:
	int _handle;
	char _printBuffer[16384];

	int _readMessages;
	int _numErrors;
	icsSpyMessage _readBuffer[BUF_SIZE];

	int _writeMessages;
	icsSpyMessage _writeBuffer[BUF_SIZE];

	icsSpyMessage _replyBuffer[BUF_SIZE];

	unsigned char _destinations[VALUE_MAX_CARDS];
	int	_my_address;
	int _polling_interval;
	int	_timeout;
	int	_njoints;
	
	int _error_status;

	PV _p;
};

ValueCanResources::ValueCanResources ()
{
	_handle = 0;
	memset (_readBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);

	memset (_destinations, 0, sizeof(unsigned char) * VALUE_MAX_CARDS);

	_my_address = 0;
	_polling_interval = 20;
	_timeout = 10;
	_njoints = 0;
	_p = NULL;

	_readMessages = 0;
	_writeMessages = 0;
	_numErrors = 0;
	_error_status = YARP_OK;
}

ValueCanResources::~ValueCanResources ()
{
	if (_handle)
		uninitialize ();
}

int ValueCanResources::initialize (const ValueCanOpenParameters& parms)
{
	if (_handle)
		return YARP_FAIL;

	memset (_readBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(icsSpyMessage)*BUF_SIZE);

	memcpy (_destinations, parms._destinations, sizeof(unsigned char)*VALUE_MAX_CARDS);
	_my_address = parms._my_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_njoints = parms._njoints;	
	_p = parms._p;

	_readMessages = 0;
	_writeMessages = 0;
	_numErrors = 0;
	_error_status = YARP_OK;

	int iDeviceTypes[255];
	int iComPort[255];
	int iSerialNum[255];
	int iNumDevices;

	unsigned char bNetworkID[16];		// array of network ids
	unsigned long lCount;				// counter variable
	unsigned long lResult;			
	
	// initialize the networkid array
	for (lCount = 0; lCount < 16; lCount++)
		bNetworkID[lCount] = lCount;

	if (icsneoFindAllCOMDevices (INTREPIDCS_DRIVER_STANDARD, 
								 0,			/** don't get ser numbers */
								 1,			/** stop at first */
								 1,			/** checks only USB com ports */
								 iDeviceTypes,
								 iComPort,
								 iSerialNum,
								 &iNumDevices))
	{
		YARPTime::DelayInSeconds (0.2);	/// buggy dev driver?

		if (iNumDevices > 0)
		{    
			lResult = icsneoOpenPortEx (iComPort[0] ,
										NEOVI_COMMTYPE_RS232, 
										INTREPIDCS_DRIVER_STANDARD,
										0,
										57600,
										1,
										bNetworkID, 
										&_handle);
			if (lResult == 0)
				return YARP_FAIL;
		}
	}
	else
	{
		return YARP_FAIL;
	}

	return YARP_OK;
}


int ValueCanResources::uninitialize ()
{
	int iResult;

	if (!_handle)
		return YARP_FAIL;

	// Close Communication
	iResult = icsneoClosePort(_handle, &_numErrors);
	_handle = 0;

	// Test the Result
	if (iResult == 0)
		return YARP_FAIL;

	return YARP_OK;
}


int ValueCanResources::read ()
{
	int iResult;

	// read out the messages
	iResult = icsneoGetMessages (_handle,
								 _readBuffer,
								 &_readMessages,
								 &_numErrors);
	if (iResult == 0)
		return YARP_FAIL;

	return YARP_OK;
}

int ValueCanResources::startPacket ()
{
	_writeMessages = 0;
	return YARP_OK;
}

int ValueCanResources::addMessage (int msg_id, int joint)
{
	icsSpyMessage x;
	memset (&x, 0, sizeof(icsSpyMessage));

	x.ArbIDOrHeader = _my_address << 4;
	x.ArbIDOrHeader = _destinations[joint/2] & 0x0f;

	x.NumberBytesData = 1;
	x.Data[0] = msg_id;
	if ((joint % 2) == 1)
		x.Data[0] |= 0x80;

	_writeBuffer[_writeMessages] = x;
	_writeMessages ++;

	return YARP_OK;
}

int ValueCanResources::writePacket ()
{
	int i;
	memcpy (_replyBuffer, _writeBuffer, sizeof(icsSpyMessage) * _writeMessages);

	for (i = 0; i < _writeMessages; i++)
	{
		int iResult = icsneoTxMessages (_handle, &_replyBuffer[i], NETID_HSCAN, 1);
		if (iResult == 0)
			return YARP_FAIL;
	}

	return YARP_OK;
}

int ValueCanResources::printMessage (const icsSpyMessage& m)
{
	if (!_p)
		return YARP_FAIL;

	int ret = ACE_OS::sprintf (_printBuffer, "class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
		  (m.ArbIDOrHeader & 0x700) >> 8,
		  (m.ArbIDOrHeader & 0xf0) >> 4, 
		  (m.ArbIDOrHeader & 0x0f), 
		  ((m.Data[0] & 0x80)==0)?0:1,
		  (m.Data[0] & 0x7f),
		  (m.Data[0] & 0x7f));

	if (m.NumberBytesData > 1)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "x: "); 
	}

	int j;
	for (j = 1; j < m.NumberBytesData; j++)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "%x ", m.Data[j]);
	}

	ret += ACE_OS::sprintf(_printBuffer+ret, "st: %x\n", m.StatusBitField);


	(*_p) 
		("%s", _printBuffer);

	return YARP_OK;
}


int ValueCanResources::dumpBuffers (void)
{
	if (!_p) return YARP_FAIL;

	int j;

	/// dump the error.
	(*_p) ("CAN: write buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_writeBuffer[j]);

	(*_p) ("CAN: reply buffer\n");
	for (j = 0; j < _writeMessages; j++)
		printMessage (_replyBuffer[j]);

	(*_p) ("CAN: read buffer\n");
	for (j = 0; j < _readMessages; j++)
		printMessage (_readBuffer[j]);
	(*_p) ("CAN: -------------\n");

	return YARP_OK;
}

///
///
///
inline ValueCanResources& RES(void *res) { return *(ValueCanResources *)res; }


YARPValueCanDeviceDriver::YARPValueCanDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPValueCanDeviceDriver>(CBNCmds), _mutex(1), _done(0)
{
	system_resources = (void *) new ValueCanResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[CMDGetPosition] = &YARPValueCanDeviceDriver::getPosition;
	m_cmds[CMDGetPositions] = &YARPValueCanDeviceDriver::getPositions;
	m_cmds[CMDGetRefPosition] = &YARPValueCanDeviceDriver::getRefPosition;
	m_cmds[CMDGetRefPositions] = &YARPValueCanDeviceDriver::getRefPositions;
	m_cmds[CMDSetPosition] = &YARPValueCanDeviceDriver::setPosition;
	m_cmds[CMDSetPositions] = &YARPValueCanDeviceDriver::setPositions;
	m_cmds[CMDGetPIDError] = &YARPValueCanDeviceDriver::getError;

	m_cmds[CMDSetSpeed] = &YARPValueCanDeviceDriver::setSpeed;
	m_cmds[CMDSetSpeeds] = &YARPValueCanDeviceDriver::setSpeeds;
	m_cmds[CMDGetSpeeds] = &YARPValueCanDeviceDriver::getSpeeds;
	m_cmds[CMDGetRefSpeeds] = &YARPValueCanDeviceDriver::getRefSpeeds;
	m_cmds[CMDSetAcceleration] = &YARPValueCanDeviceDriver::setAcceleration;
	m_cmds[CMDSetAccelerations] = &YARPValueCanDeviceDriver::setAccelerations;
	m_cmds[CMDGetRefAccelerations] = &YARPValueCanDeviceDriver::getRefAccelerations;

	m_cmds[CMDSetOffset] = &YARPValueCanDeviceDriver::setOffset;
	m_cmds[CMDSetOffsets] = &YARPValueCanDeviceDriver::setOffsets;

	m_cmds[CMDSetPID] = &YARPValueCanDeviceDriver::setPid;
	m_cmds[CMDGetPID] = &YARPValueCanDeviceDriver::getPid;

	m_cmds[CMDSetIntegratorLimit] = &YARPValueCanDeviceDriver::setIntegratorLimit;
	m_cmds[CMDSetIntegratorLimits] = &YARPValueCanDeviceDriver::setIntegratorLimits;

	m_cmds[CMDDefinePosition] = &YARPValueCanDeviceDriver::definePosition;
	m_cmds[CMDDefinePositions] = &YARPValueCanDeviceDriver::definePositions;
	m_cmds[CMDDisableAmp] = &YARPValueCanDeviceDriver::disableAmp;
	m_cmds[CMDEnableAmp] = &YARPValueCanDeviceDriver::enableAmp;
	m_cmds[CMDControllerIdle] = &YARPValueCanDeviceDriver::controllerIdle;
	m_cmds[CMDControllerRun] = &YARPValueCanDeviceDriver::controllerRun;

	m_cmds[CMDVMove] = &YARPValueCanDeviceDriver::velocityMove;
	m_cmds[CMDSafeVMove] = &YARPValueCanDeviceDriver::velocityMove;

	m_cmds[CMDSetCommand] = &YARPValueCanDeviceDriver::setCommand;
	m_cmds[CMDSetCommands] = &YARPValueCanDeviceDriver::setCommands;
	m_cmds[CMDCheckMotionDone] = &YARPValueCanDeviceDriver::checkMotionDone;

	m_cmds[CMDGetTorque] = &YARPValueCanDeviceDriver::getTorque;
	m_cmds[CMDGetTorques] = &YARPValueCanDeviceDriver::getTorques;
	m_cmds[CMDLoadBootMemory] = &YARPValueCanDeviceDriver::readBootMemory;
	m_cmds[CMDSaveBootMemory] = &YARPValueCanDeviceDriver::writeBootMemory;

	m_cmds[CMDSetSWPositiveLimit] = &YARPValueCanDeviceDriver::setSwPositiveLimit;
	m_cmds[CMDSetSWNegativeLimit] = &YARPValueCanDeviceDriver::setSwNegativeLimit;
	m_cmds[CMDGetSWPositiveLimit] = &YARPValueCanDeviceDriver::getSwPositiveLimit;
	m_cmds[CMDGetSWNegativeLimit] = &YARPValueCanDeviceDriver::getSwNegativeLimit;
	
	m_cmds[CMDGetTorqueLimit] = &YARPValueCanDeviceDriver::getTorqueLimit;
	m_cmds[CMDGetTorqueLimits] = &YARPValueCanDeviceDriver::getTorqueLimits;
	m_cmds[CMDSetTorqueLimit] = &YARPValueCanDeviceDriver::setTorqueLimit;
	m_cmds[CMDSetTorqueLimits] = &YARPValueCanDeviceDriver::setTorqueLimits;

	m_cmds[CMDSetDebugMessageFilter] = &YARPValueCanDeviceDriver::setDebugMessageFilter;
	m_cmds[CMDSetDebugPrintFunction] = &YARPValueCanDeviceDriver::setDebugPrintFunction;

	m_cmds[CMDGetErrorStatus] = &YARPValueCanDeviceDriver::getErrorStatus;
}


YARPValueCanDeviceDriver::~YARPValueCanDeviceDriver ()
{
	if (system_resources != NULL)
		delete (ValueCanResources *)system_resources;
	system_resources = NULL;
}


int YARPValueCanDeviceDriver::open (void *p)
{
	if (p == NULL)
		return YARP_FAIL;

	_mutex.Wait();

	ValueCanOpenParameters& parms = *(ValueCanOpenParameters *)p;
	ValueCanResources& r = RES (system_resources);
	
	if (r.initialize (parms) != YARP_OK)
		return YARP_FAIL;
	
	Begin ();
	_done.Wait ();

	/// used for printing debug messages.
	_p = parms._p;
	_filter = -1;
	_writerequested = false;
	_noreply = false;

	/// temporary variables used by the ddriver.
	_ref_positions = new double [r.getJoints()];		
	_ref_speeds = new double [r.getJoints()];
	_ref_accs = new double [r.getJoints()];
	ACE_ASSERT (_ref_positions != NULL && _ref_speeds != NULL && _ref_accs != NULL);

	_mutex.Post ();

	return YARP_OK;
}

int YARPValueCanDeviceDriver::close (void)
{
	ValueCanResources& d = RES(system_resources);

	End ();	/// stops the thread first (joins too).

	if (_ref_positions != NULL) delete[] _ref_positions;
	if (_ref_speeds != NULL) delete[] _ref_speeds;
	if (_ref_accs != NULL) delete[] _ref_accs;

	int ret = d.uninitialize ();

	return ret;
}


///
///
///
void YARPValueCanDeviceDriver::Body (void)
{
	ValueCanResources& r = RES (system_resources);

	/// init part.
	bool messagePending = false;
	int i = 0;
	int remainingMsgs = 0;
	bool noreply = false;
	double now = 0;
	double before = 0;
	int counter = 0;

	_writerequested = false;
	_noreply = false;
	
	r._error_status = YARP_OK;

	/// ok, init completed. 
	_done.Post ();

	while (!IsTerminated() || messagePending)
	{
		before = YARPTime::GetTimeAsSeconds();

		_mutex.Wait ();
		if (r.read () != YARP_OK)
			if (r._p) 
				(*r._p) ("CAN: read failed\n");

		if (messagePending)
		{
			for (i = 0; i < r._readMessages; i++)
			{
				icsSpyMessage& m = r._readBuffer[i];
				if (m.StatusBitField & SPY_STATUS_GLOBAL_ERR)
					if (r._p) 
					{
						(*r._p) ("CAN: error in message %x len: %d type: %x: %x\n", 
							m.ArbIDOrHeader, m.NumberBytesData, m.Data[0], m.StatusBitField);
						
						continue;
						//r.dumpBuffers ();
					}

				if (((m.ArbIDOrHeader &0x700) == 0) && 
					((m.Data[0] & 0x7f) != _filter))
					r.printMessage (m);

				/// the pending msg doesn't require a reply.
				if (noreply)
				{
					if (m.StatusBitField & SPY_STATUS_TX_MSG) 
					{
						remainingMsgs --;
						if (remainingMsgs < 1)
						{
							messagePending = false;
							r._error_status = YARP_OK;
							goto AckMessageLoop;
						}
					}
				}
				else /// this requires a reply.
				{
					if (((m.ArbIDOrHeader & 0x700) == 0) &&				/// class 0 msg.
						((m.ArbIDOrHeader & 0x0f) == r._my_address))
					{
						/// legitimate message directed here, checks whether replies to any message.
						int j;
						for (j = 0; j < r._writeMessages; j++)
						{
							if (((r._writeBuffer[j].ArbIDOrHeader & 0x0f) == ((m.ArbIDOrHeader & 0xf0) >> 4)) &&
								(m.Data[0] == r._writeBuffer[j].Data[0]))
							{
								if (r._replyBuffer[j].ArbIDOrHeader != 0)
								{
									if (r._p)
									{
										(*r._p) ("CAN: message %x was already replied\n", m.ArbIDOrHeader);
										r.printMessage (m);
										//r.dumpBuffers ();
									}
								}
								else
								{
									memcpy (&r._replyBuffer[j], &m, sizeof(icsSpyMessage));
									remainingMsgs --;
									if (remainingMsgs < 1)
									{
										messagePending = false;
										r._error_status = YARP_OK;
										goto AckMessageLoop;
									}
								}
							}
						}
					}
				}
			}

			/// timeout
			counter ++;
			if (counter > r._timeout)
			{	
				/// complains.
				if (r._p)
				{
					(*r._p) ("CAN: timeout - still %d messages unacknowledged\n", remainingMsgs);
					r.dumpBuffers ();

#if 0
					int j;
					for (j = 0; j < r._writeMessages; j++)
					{
						if (r._replyBuffer[j].ArbIDOrHeader == 0)
						{
							r.printMessage (r._writeBuffer[j]);
						}
					}
#endif
				}

				messagePending = false;
				r._error_status = YARP_FAIL;
				goto AckMessageLoop;
			}

AckMessageLoop:
			if (!messagePending)
			{
				/// tell the caller it can continue.
				_done.Post();
			}
		}
		else
		{
			/// any write?
			if (_writerequested)
			{
				if (r._writeMessages > 0)
				{
					if (r.writePacket () != YARP_OK)
					{
						if (r._p)
						{
							(*r._p) ("CAN: write message of %d elments failed\n", r._writeMessages);
						}
					}
					else
					{
						messagePending = true;
						_writerequested = false;
						remainingMsgs = r._writeMessages;
						noreply = _noreply;
						r._error_status = YARP_OK;
						counter = 0;
						memset (r._replyBuffer, 0, sizeof(icsSpyMessage) * r._writeMessages);
					}
				}
			}
		}

		_mutex.Post ();

		/// wait.
		now = YARPTime::GetTimeAsSeconds();
		if ((now - before)*1000 < r._polling_interval)
		{
			YARPTime::DelayInSeconds(double(r._polling_interval)/1000.0-(now-before));
		}
		else 
		{
			if (r._p) (*r._p)("CAN: thread can't poll fast enough (time: %f)\n", now-before);
		}
		before = now;
	}

	///
}




///
///
/// specific messages that change the driver behavior.
int YARPValueCanDeviceDriver::setDebugMessageFilter (void *cmd)
{
	_mutex.Wait();
	ValueCanResources& r = RES(system_resources);
	_filter = *(int *)cmd;
	_mutex.Post();

	return YARP_OK;
}

int YARPValueCanDeviceDriver::setDebugPrintFunction (void *cmd)
{
	_mutex.Wait();
	ValueCanResources& r = RES(system_resources);
	_p = (int (*) (char *fmt, ...))cmd;
	_mutex.Post();

	return YARP_OK;
}


///
/// 
/// control card commands.
///

/// cmd is an array of double
int YARPValueCanDeviceDriver::getPositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i, value = 0;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (CAN_GET_ENCODER_POSITION, i);
		}
		else
			tmp[i] = 0;
	}

	if (r._writeMessages < 1)
		return YARP_FAIL;

	_writerequested = true;
	_noreply = false;
	_mutex.Post();

	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		memset (tmp, 0, sizeof(double) * r.getJoints());
		return YARP_FAIL;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			icsSpyMessage& m = r._replyBuffer[j];
			if (m.ArbIDOrHeader == 0xffff)
				tmp[i] = 0;
			else
				tmp[i] = *((int *)(m.Data+1));
			j++;
		}
	}

	return YARP_OK;
}

/// cmd is a SingleAxisParameters pointer with double arg
int YARPValueCanDeviceDriver::getPosition (void *cmd)
{
	/// prepare can message.
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	int value;
	if (_readDWord (CAN_GET_ENCODER_POSITION, axis, value) == YARP_OK)
	{
		*((double *)tmp->parameters) = double(value);
		return YARP_OK;
	}
	
	*((double *)tmp->parameters) = 0;
	return YARP_FAIL;
}

/// cmd is an array of double (njoints long)
int YARPValueCanDeviceDriver::setPositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints (); i++)
	{
		if (ENABLED(i))
		{
			SingleAxisParameters x;
			x.axis = i;
			x.parameters = tmp+i;	

			r.addMessage (CAN_POSITION_MOVE, i);
			_ref_positions[i] = tmp[i];
			*((int*)(r._writeBuffer[i].Data+1)) = S_32(_ref_positions[i]);		/// pos
			*((short*)(r._writeBuffer[i].Data+5)) = S_16(_ref_speeds[i]);		/// speed
			r._writeBuffer[i].NumberBytesData = 7;
		}
		else
		{
			_ref_positions[i] = tmp[i];
		}
	}

	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

inline bool YARPValueCanDeviceDriver::ENABLED (int axis)
{
	ValueCanResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & VALUE_CAN_SKIP_ADDR) == 0) ? true : false;
}

///
///
/// cmd is a SingleAxisParameters pointer to a double argument
int YARPValueCanDeviceDriver::setPosition (void *cmd)
{
	/// prepare can message.
	ValueCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED (axis))
	{
		// still fills the _ref_position structure.
		_ref_positions[axis] = *((double *)tmp->parameters);
		return YARP_OK;
	}

	_mutex.Wait();

	r.startPacket();
	r.addMessage (CAN_POSITION_MOVE, axis);

	_ref_positions[axis] = *((double *)tmp->parameters);
	*((int*)(r._writeBuffer[0].Data+1)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r._writeBuffer[0].Data+5)) = S_16(_ref_speeds[axis]);			/// speed
	r._writeBuffer[0].NumberBytesData = 7;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
int YARPValueCanDeviceDriver::velocityMove (void *cmd)
{
	/// prepare can message.
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED (i))
		{
			r.addMessage (CAN_VELOCITY_MOVE, i);
			_ref_speeds[i] = tmp[i];
			*((short*)(r._writeBuffer[i].Data+1)) = S_16(_ref_speeds[i]);		/// speed
			*((short*)(r._writeBuffer[i].Data+3)) = S_16(_ref_accs[i]);		/// accel
			r._writeBuffer[i].NumberBytesData = 5;
		}
		else
		{
			_ref_speeds[i] = tmp[i];
		}
	}

	_writerequested = true;
	_noreply = true;

	_mutex.Post();

	_done.Wait();

	return YARP_OK;
}

/// cmd is a SingleAxis poitner with 1 double arg
int YARPValueCanDeviceDriver::definePosition (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::definePositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeDWord (CAN_SET_ENCODER_POSITION, i, S_32(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis poitner with 1 double arg
int YARPValueCanDeviceDriver::setCommand (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_COMMAND_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setCommands (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeDWord (CAN_SET_COMMAND_POSITION, i, S_32(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getRefPositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i, value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readDWord (CAN_GET_DESIRED_POSITION, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::getRefPosition (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	int value = 0;
	if (_readDWord (CAN_GET_DESIRED_POSITION, axis, value) == YARP_OK)
		*((double *)(tmp->parameters)) = double (value);
	else
		return YARP_FAIL;

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::setSpeed (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	_ref_speeds[axis] = *((double *)(tmp->parameters));
	const short s = S_16(_ref_speeds[axis]);
	return _writeWord16 (CAN_SET_DESIRED_VELOCITY, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setSpeeds (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_speeds[i] = tmp[i];
		if (_writeWord16 (CAN_SET_DESIRED_VELOCITY, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getSpeeds (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_ENCODER_VELOCITY, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getRefSpeeds (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_DESIRED_VELOCITY, i, value) == YARP_OK)
			_ref_speeds[i] = out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with a double arg
int YARPValueCanDeviceDriver::setAcceleration (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	_ref_accs[axis] = *((double *)tmp->parameters);
	const short s = S_16(_ref_accs[axis]);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setAccelerations (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_accs[i] = tmp[i];
		if (_writeWord16 (CAN_SET_DESIRED_ACCELER, i, S_16(_ref_accs[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getRefAccelerations (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_DESIRED_ACCELER, i, value) == YARP_OK)
			_ref_accs[i] = out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SigleAxis pointer with 1 double arg
int YARPValueCanDeviceDriver::setOffset (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_OFFSET, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setOffsets (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_OFFSET, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with 1 double arg
int YARPValueCanDeviceDriver::setIntegratorLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_ILIM_GAIN, axis, s);
}

/// cmd is an array of double 
int YARPValueCanDeviceDriver::setIntegratorLimits (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_ILIM_GAIN, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with a LowLevelPID argument pointer
int YARPValueCanDeviceDriver::setPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	_writeWord16 (CAN_SET_P_GAIN, axis, S_16(pid->KP));
	_writeWord16 (CAN_SET_D_GAIN, axis, S_16(pid->KD));
	_writeWord16 (CAN_SET_I_GAIN, axis, S_16(pid->KI));
	_writeWord16 (CAN_SET_ILIM_GAIN, axis, S_16(pid->I_LIMIT));
	_writeWord16 (CAN_SET_OFFSET, axis, S_16(pid->OFFSET));
	_writeWord16 (CAN_SET_SCALE, axis, S_16(pid->SHIFT));
	_writeWord16 (CAN_SET_TLIM, axis, S_16(pid->T_LIMIT));

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with a LowLevelPID argument pointer
int YARPValueCanDeviceDriver::getPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
	short s;

	_readWord16 (CAN_GET_P_GAIN, axis, s); pid->KP = double(s);
	_readWord16 (CAN_GET_D_GAIN, axis, s); pid->KD = double(s);
	_readWord16 (CAN_GET_I_GAIN, axis, s); pid->KI = double(s);
	_readWord16 (CAN_GET_ILIM_GAIN, axis, s); pid->I_LIMIT = double(s);
	_readWord16 (CAN_GET_OFFSET, axis, s); pid->OFFSET = double(s);
	_readWord16 (CAN_GET_SCALE, axis, s); pid->SHIFT = double(s);
	_readWord16 (CAN_GET_TLIM, axis, s); pid->T_LIMIT = double(s);

	return YARP_OK;
}

/// cmd is an int *
int YARPValueCanDeviceDriver::enableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::disableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::controllerRun (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::controllerIdle (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getTorques (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_PID_OUTPUT, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxisParameters struct with a double argument.
int YARPValueCanDeviceDriver::getTorque (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	short value;

	if (_readWord16 (CAN_GET_PID_OUTPUT, axis, value) != YARP_OK)
	{
		*((double *)tmp->parameters) = 0;
		return YARP_FAIL;
	}

	*((double *)tmp->parameters) = double(value);
	return YARP_OK;
}


/// cmd is a SingleAxisParameters struct with a double argument.
int YARPValueCanDeviceDriver::getError (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	short value;

	if (_readWord16 (CAN_GET_PID_ERROR, axis, value) != YARP_OK)
	{
		*((double *)tmp->parameters) = 0;
		return YARP_FAIL;
	}

	*((double *)tmp->parameters) = double(value);
	return YARP_OK;
}


/// cmd is a pointer to SingleAxisParameters struct with no argument.
int YARPValueCanDeviceDriver::readBootMemory (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	return _writeNone (CAN_READ_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPValueCanDeviceDriver::writeBootMemory (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	return _writeNone (CAN_WRITE_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPValueCanDeviceDriver::setSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a pointer to SingleAxisParameters struct with a single double arg.
int YARPValueCanDeviceDriver::setSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::getSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MIN_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::getSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MAX_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}


/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::setTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_TLIM, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setTorqueLimits (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeWord16 (CAN_SET_TLIM, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::getTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	short value;

	_readWord16 (CAN_GET_TLIM, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getTorqueLimits (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (_readWord16 (CAN_GET_TLIM, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a pointer to an integer
int YARPValueCanDeviceDriver::getErrorStatus (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	int *out = (int *) cmd;
	short value = 0;

	/// the axis is irrelevant.
	/// MSB of the word is 0!
	if (_readWord16 (CAN_GET_ERROR_STATUS, 0, value) == YARP_OK)
		*out = int(value);
	else
		return YARP_FAIL;

	return YARP_OK;
}

/// cmd is a pointer to a bool
int YARPValueCanDeviceDriver::checkMotionDone (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
	int i;
	short value;
	bool *out = (bool *) cmd;

	for(i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			if (_readWord16 (CAN_MOTION_DONE, i, value) == YARP_OK)
			{
				if (!value)
				{
					*out = false;
					return YARP_OK;
				}
			}
		}
	}

	*out = true;
	return YARP_OK;
}

///
/// helper functions.
///
///
///

/// sends a message without parameters
int YARPValueCanDeviceDriver::_writeNone (int msg, int axis)
{
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		return YARP_OK;
	}

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

int YARPValueCanDeviceDriver::_readWord16 (int msg, int axis, short& value)
{
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		value = short(0);
		return YARP_OK;
	}

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_writerequested = true;
	_noreply = false;

	_mutex.Post();

	/// reads back position info.
	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((short *)(r._replyBuffer[0].Data+1));
	return YARP_OK;
}

/// to send a Word16.
int YARPValueCanDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	
	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].Data+1)) = s;
	r._writeBuffer[0].NumberBytesData = 3;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	/// hopefully ok...
	return YARP_OK;
}

/// write a DWord
int YARPValueCanDeviceDriver::_writeDWord (int msg, int axis, int value)
{
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((int*)(r._writeBuffer[0].Data+1)) = value;
	r._writeBuffer[0].NumberBytesData = 5;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

/// two shorts in a single Can message (both must belong to the same control card).
int YARPValueCanDeviceDriver::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	ValueCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);
	ACE_ASSERT ((axis % 2) == 0);	/// axis is even.

	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].Data+1)) = s1;
	*((short *)(r._writeBuffer[0].Data+3)) = s2;
	r._writeBuffer[0].NumberBytesData = 5;

	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	/// hopefully ok...
	return YARP_OK;
}

///
/// sends a message and gets a dword back.
/// 
int YARPValueCanDeviceDriver::_readDWord (int msg, int axis, int& value)
{
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		value = 0;
		return YARP_OK;
	}

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_writerequested = true;
	_noreply = false;
	
	_mutex.Post();

	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((int *)(r._replyBuffer[0].Data+1));
	return YARP_OK;
}
