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
/// $Id: YARPEsdCanDeviceDriver.cpp,v 1.5 2006-06-13 12:31:46 gmetta Exp $
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
#include "YARPEsdCanDeviceDriver.h"
#include "../dd_orig/include/ntcan.h"

/// get the message types from the DSP code.
#define __ONLY_DEF
#include "../56f807/cotroller_dc/Code/controller.h"

#define BUF_SIZE 2047

typedef int (*PV) (const char *fmt, ...);

///
///
///
class BCastBufferElement
{
public:
	// msg 1
	int _position;
	double _update_p;

	// msg 2
	short _pid_value;
	double _update_v;

	// msg 3
	short _fault;
	double _update_e;

	// msg 4
	short _current;
	short _position_error;
	double _update_c;

	BCastBufferElement () { zero (); }

	void zero (void)
	{
		_position = 0;
		_pid_value = 0;
		_current = 0;
		_fault = 0;
		_position_error = 0;

		_update_p = .0;
		_update_v = .0;
		_update_e = .0;
		_update_c = .0;
	}
};


///
///
///
class EsdCanResources
{
public:
	EsdCanResources ();
	~EsdCanResources ();

	int initialize (const EsdCanOpenParameters& parms);
	int uninitialize ();
	int read ();
	bool error (const CMSG& m);

	int startPacket ();
	int addMessage (int msg_id, int joint);
	int writePacket ();

	int printMessage (const CMSG& m);
	int dumpBuffers (void);
	inline int getJoints (void) const { return _njoints; }
	inline int getErrorStatus (void) const { return _error_status; }
	
public:
	enum { ESD_TIMEOUT = 20, ESD_POLLING_INTERVAL = 10 };

	HANDLE _handle;								/// the actual ddriver handle.
	int _timeout;								/// this is my thread timeout.
	
	int _polling_interval;						/// thread polling interval.
	int _speed;									/// speed of the bus.
	int _networkN;								/// network number.
	
	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _readMessages;							/// size of the last read buffer.
	int _msg_lost;								/// # of lost messages.
	CMSG _readBuffer[BUF_SIZE];					/// read buffer.
	int _writeMessages;							/// size of the write packet.
	CMSG _writeBuffer[BUF_SIZE];				/// write buffer.
	CMSG _replyBuffer[BUF_SIZE];				/// reply buffer.

	BCastBufferElement *_bcastRecvBuffer;		/// local storage for bcast messages.

	unsigned char _my_address;					/// 
	unsigned char _destinations[ESD_MAX_CARDS];	/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).

	int _error_status;							/// error status of the last packet.

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	int _filter;								/// don't print filtered messages.
	
	char _printBuffer[16384];
};

EsdCanResources::EsdCanResources ()
{
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;						/// default 1Mbit/s
	_networkN = 0;

	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	memset (_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);

	_my_address = 0;
	_njoints = 0;
	_p = NULL;

	_readMessages = 0;
	_msg_lost = 0;
	_writeMessages = 0;
	_bcastRecvBuffer = NULL;

	_error_status = YARP_OK;
}

EsdCanResources::~EsdCanResources () 
{ 
	uninitialize(); 
}

int EsdCanResources::initialize (const EsdCanOpenParameters& parms)
{
	if (_handle != ACE_INVALID_HANDLE)
		return YARP_FAIL;

	/// general variable init.
	_handle = ACE_INVALID_HANDLE;
	_speed = 0;						/// default 1Mbit/s
	_networkN = parms._networkN;

	_readMessages = 0;
	_writeMessages = 0;
	_msg_lost = 0;
	_error_status = YARP_OK;

	memcpy (_destinations, parms._destinations, sizeof(unsigned char)*ESD_MAX_CARDS);
	_my_address = parms._my_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_njoints = parms._njoints;	
	_p = parms._p;
	_filter = -1;

	_txQueueSize = parms._txQueueSize;
	_rxQueueSize = parms._rxQueueSize;
	_txTimeout = parms._txTimeout;
	_rxTimeout = parms._rxTimeout;

	_bcastRecvBuffer = new BCastBufferElement[_njoints];
	ACE_ASSERT (_bcastRecvBuffer != NULL);
    /// suspect an issue with allocation of array of classes.
    /// it requires further investigation.

	/// clean up buffers.
	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	/// open the device.
	int res = canOpen (_networkN, 0, _txQueueSize, _rxQueueSize, _txTimeout, _rxTimeout, &_handle);
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	res = canSetBaudrate (_handle, _speed);
	if (res != NTCAN_SUCCESS)
	{
		canClose (_handle);
		return YARP_FAIL;
	}

	/// sets all message ID's for class 0 and 1.
	int i;
	for (i = 0; i < 0xff; i++)
		canIdAdd (_handle, i);
	
	for (i = 0x100; i < 0x1ff; i++)
		canIdAdd (_handle, i);

	return YARP_OK;
}


int EsdCanResources::uninitialize ()
{
	if (_bcastRecvBuffer != NULL) delete[] _bcastRecvBuffer;

	if (_handle != ACE_INVALID_HANDLE)
	{
		int res = canClose (_handle);
		if (res != NTCAN_SUCCESS)
			return YARP_FAIL;
		_handle = ACE_INVALID_HANDLE;
	}
	
	return YARP_OK;
}


int EsdCanResources::read ()
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _readBuffer, &messages); 
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	_readMessages = messages;
	return YARP_OK;
}

int EsdCanResources::startPacket ()
{
	_writeMessages = 0;
	return YARP_OK;
}

int EsdCanResources::addMessage (int msg_id, int joint)
{
	CMSG x;
	memset (&x, 0, sizeof(CMSG));

	x.id = _my_address << 4;
	x.id = _destinations[joint/2] & 0x0f;

	x.len = 1;
	x.data[0] = msg_id;
	if ((joint % 2) == 1)
		x.data[0] |= 0x80;

	_writeBuffer[_writeMessages] = x;
	_writeMessages ++;

	return YARP_OK;
}

int EsdCanResources::writePacket ()
{
	if (_writeMessages < 1)
		return YARP_FAIL;

	long len = _writeMessages;

	int res = canSend (_handle, _writeBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _writeMessages)
		return YARP_FAIL;

	return YARP_OK;
}

int EsdCanResources::printMessage (const CMSG& m)
{
	if (!_p)
		return YARP_FAIL;

	int ret = ACE_OS::sprintf (_printBuffer, "class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
		  (m.id & 0x700) >> 8,
		  (m.id & 0xf0) >> 4, 
		  (m.id & 0x0f), 
		  ((m.data[0] & 0x80)==0)?0:1,
		  (m.data[0] & 0x7f),
		  (m.data[0] & 0x7f));

	if (m.len > 1)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "x: "); 
	}

	int j;
	for (j = 1; j < m.len; j++)
	{
		ret += ACE_OS::sprintf (_printBuffer+ret, "%x ", m.data[j]);
	}

	ret += ACE_OS::sprintf(_printBuffer+ret, "st: %x\n", m.msg_lost);


	(*_p) 
		("%s", _printBuffer);

	return YARP_OK;
}


int EsdCanResources::dumpBuffers (void)
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

bool EsdCanResources::error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

///
///
///
inline EsdCanResources& RES(void *res) { return *(EsdCanResources *)res; }


YARPEsdCanDeviceDriver::YARPEsdCanDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPEsdCanDeviceDriver>(CBNCmds), _mutex(1), _done(0)
{
	system_resources = (void *) new EsdCanResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[CMDGetPosition] = &YARPEsdCanDeviceDriver::getBCastPosition;
	m_cmds[CMDGetPositions] = &YARPEsdCanDeviceDriver::getBCastPositions;
	m_cmds[CMDGetRefPosition] = &YARPEsdCanDeviceDriver::getRefPosition;
	m_cmds[CMDGetRefPositions] = &YARPEsdCanDeviceDriver::getRefPositions;
	m_cmds[CMDSetPosition] = &YARPEsdCanDeviceDriver::setPosition;
	m_cmds[CMDSetPositions] = &YARPEsdCanDeviceDriver::setPositions;
	m_cmds[CMDGetPIDError] = &YARPEsdCanDeviceDriver::getBCastPositionError;
	m_cmds[CMDGetPIDErrors] = &YARPEsdCanDeviceDriver::getBCastPositionErrors;
	m_cmds[CMDGetPWM] = &YARPEsdCanDeviceDriver::getBCastPIDOutput;
	m_cmds[CMDGetPWMs] = &YARPEsdCanDeviceDriver::getBCastPIDOutputs;
	m_cmds[CMDSetSpeed] = &YARPEsdCanDeviceDriver::setSpeed;
	m_cmds[CMDSetSpeeds] = &YARPEsdCanDeviceDriver::setSpeeds;
	m_cmds[CMDGetSpeeds] = &YARPEsdCanDeviceDriver::getSpeeds;
	m_cmds[CMDGetRefSpeeds] = &YARPEsdCanDeviceDriver::getRefSpeeds;
	//m_cmds[CMDGetAccelerations] = &YARPEsdCanDeviceDriver::getBCastAccelerations;
	m_cmds[CMDSetAcceleration] = &YARPEsdCanDeviceDriver::setAcceleration;
	m_cmds[CMDSetAccelerations] = &YARPEsdCanDeviceDriver::setAccelerations;
	m_cmds[CMDGetRefAccelerations] = &YARPEsdCanDeviceDriver::getRefAccelerations;
	m_cmds[CMDSetOffset] = &YARPEsdCanDeviceDriver::setOffset;
	m_cmds[CMDSetOffsets] = &YARPEsdCanDeviceDriver::setOffsets;
	m_cmds[CMDSetPID] = &YARPEsdCanDeviceDriver::setPid;
	m_cmds[CMDGetPID] = &YARPEsdCanDeviceDriver::getPid;
	m_cmds[CMDSetIntegratorLimit] = &YARPEsdCanDeviceDriver::setIntegratorLimit;
	m_cmds[CMDSetIntegratorLimits] = &YARPEsdCanDeviceDriver::setIntegratorLimits;
	m_cmds[CMDDefinePosition] = &YARPEsdCanDeviceDriver::definePosition;
	m_cmds[CMDDefinePositions] = &YARPEsdCanDeviceDriver::definePositions;
	m_cmds[CMDDisableAmp] = &YARPEsdCanDeviceDriver::disableAmp;
	m_cmds[CMDEnableAmp] = &YARPEsdCanDeviceDriver::enableAmp;
	m_cmds[CMDControllerIdle] = &YARPEsdCanDeviceDriver::controllerIdle;
	m_cmds[CMDControllerRun] = &YARPEsdCanDeviceDriver::controllerRun;
	m_cmds[CMDVMove] = &YARPEsdCanDeviceDriver::velocityMove;
	m_cmds[CMDSetCommand] = &YARPEsdCanDeviceDriver::setCommand;
	m_cmds[CMDSetCommands] = &YARPEsdCanDeviceDriver::setCommands;
	m_cmds[CMDCheckMotionDone] = &YARPEsdCanDeviceDriver::checkMotionDone;
    m_cmds[CMDCheckMotionDoneSingle] = &YARPEsdCanDeviceDriver::checkMotionDoneSingle;
	m_cmds[CMDGetTorque] = &YARPEsdCanDeviceDriver::getBCastCurrent;
	m_cmds[CMDGetTorques] = &YARPEsdCanDeviceDriver::getBCastCurrents;
	m_cmds[CMDLoadBootMemory] = &YARPEsdCanDeviceDriver::readBootMemory;
	m_cmds[CMDSaveBootMemory] = &YARPEsdCanDeviceDriver::writeBootMemory;
	m_cmds[CMDSetSWPositiveLimit] = &YARPEsdCanDeviceDriver::setSwPositiveLimit;
	m_cmds[CMDSetSWNegativeLimit] = &YARPEsdCanDeviceDriver::setSwNegativeLimit;
	m_cmds[CMDGetSWPositiveLimit] = &YARPEsdCanDeviceDriver::getSwPositiveLimit;
	m_cmds[CMDGetSWNegativeLimit] = &YARPEsdCanDeviceDriver::getSwNegativeLimit;
	m_cmds[CMDGetTorqueLimit] = &YARPEsdCanDeviceDriver::getTorqueLimit;
	m_cmds[CMDGetTorqueLimits] = &YARPEsdCanDeviceDriver::getTorqueLimits;
	m_cmds[CMDSetTorqueLimit] = &YARPEsdCanDeviceDriver::setTorqueLimit;
	m_cmds[CMDSetTorqueLimits] = &YARPEsdCanDeviceDriver::setTorqueLimits;
	m_cmds[CMDSetCurrentLimit] = &YARPEsdCanDeviceDriver::setCurrentLimit;
	m_cmds[CMDSetCurrentLimits] = &YARPEsdCanDeviceDriver::setCurrentLimits;
	m_cmds[CMDGetAnalogChannel] = &YARPEsdCanDeviceDriver::getBCastCurrent;
	m_cmds[CMDGetAnalogChannels] = &YARPEsdCanDeviceDriver::getBCastCurrents;
	//m_cmds[CMDGetFault] = &YARPEsdCanDeviceDriver::getBCastFault;
	m_cmds[CMDGetFaults] = &YARPEsdCanDeviceDriver::getBCastFaults;
    m_cmds[CMDStartCalibration] = &YARPEsdCanDeviceDriver::startCalibration;
    m_cmds[CMDGetControlMode] = &YARPEsdCanDeviceDriver::getControlMode;
	m_cmds[CMDSetDebugMessageFilter] = &YARPEsdCanDeviceDriver::setDebugMessageFilter;
	m_cmds[CMDSetDebugPrintFunction] = &YARPEsdCanDeviceDriver::setDebugPrintFunction;
	m_cmds[CMDGetErrorStatus] = &YARPEsdCanDeviceDriver::getErrorStatus;
}


YARPEsdCanDeviceDriver::~YARPEsdCanDeviceDriver ()
{
	if (system_resources != NULL)
		delete (EsdCanResources *)system_resources;
	system_resources = NULL;
}


int YARPEsdCanDeviceDriver::open (void *p)
{
	if (p == NULL)
		return YARP_FAIL;

	_mutex.Wait();

	EsdCanOpenParameters& parms = *(EsdCanOpenParameters *)p;
	EsdCanResources& r = RES (system_resources);
	
	if (r.initialize (parms) != YARP_OK)
	{
		_mutex.Post();
		return YARP_FAIL;
	}

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

	/// default initialization for this device driver.
	int i;
	for(i = 0; i < r.getJoints(); i++)
	{
		SingleAxisParameters cmd;
		cmd.axis = i;
		double tmp = double(0x1E);	/// 0x1A activates position and current consumption broadcast + fault events.
		cmd.parameters = &tmp;
		setBCastMessages(&cmd);	
	}

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::close (void)
{
	EsdCanResources& d = RES(system_resources);

    if (YARPThread::running)
    {
	    /// default initialization for this device driver.
	    int i;
	    for(i = 0; i < d.getJoints(); i++)
	    {
		    SingleAxisParameters cmd;
		    cmd.axis = i;
		    double tmp = double(0x00);
		    cmd.parameters = &tmp;
		    setBCastMessages(&cmd);	
	    }
    }

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
void YARPEsdCanDeviceDriver::Body (void)
{
	EsdCanResources& r = RES (system_resources);

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
		before = YARPTime::GetTimeAsSecondsHr();

		_mutex.Wait ();
		if (r.read () != YARP_OK)
			if (r._p) 
				(*r._p) ("CAN: read failed\n");

		// handle broadcast messages.
		// (class 1, 8 bits of the ID used to define the message type and source address).
		//
		for (i = 0; i < r._readMessages; i++)
		{
			CMSG& m = r._readBuffer[i];
			if (m.len & NTCAN_NO_DATA)
				if (r._p)
				{
					(*r._p) ("CAN: error in message %x len: %d type: %x: %x\n",
							m.id, m.len, m.data[0], m.msg_lost);
						continue;
				}

			if ((m.id & 0x700) == 0x100) // class = 1.
			{
				// 4 next bits = source address, next 4 bits = msg type
				// this allows sending two 32-bit numbers is a single CAN message.
				//
				// need an array here for storing the messages on a per-joint basis.

				const int addr = ((m.id & 0x0f0) >> 4);
				int j;
				for (j = 0; j < ESD_MAX_CARDS; j++)
				{
					if (r._destinations[j] == addr)
						break;
				}

				j *= 2;

				/* less sign nibble specifies msg type */
				switch (m.id & 0x00f)
				{
				case CAN_BCAST_POSITION:
					r._bcastRecvBuffer[j]._position = *((int *)(m.data));
					r._bcastRecvBuffer[j]._update_p = before;
					j++;
					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._position = *((int *)(m.data+4));
						r._bcastRecvBuffer[j]._update_p = before;
					}
					break;

				case CAN_BCAST_PID_VAL:
					r._bcastRecvBuffer[j]._pid_value = *((short *)(m.data));
					r._bcastRecvBuffer[j]._update_v = before;

					j++;
					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._pid_value = *((short *)(m.data+2));
						r._bcastRecvBuffer[j]._update_v = before;
					}
					break;

				case CAN_BCAST_FAULT:
					// fault signals.
					r._bcastRecvBuffer[j]._fault = *((short *)(m.data));
					r._bcastRecvBuffer[j]._update_e = before;
					j++;

					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._fault = *((short *)(m.data+2));
						r._bcastRecvBuffer[j]._update_e = before;
					}
					break;

				case CAN_BCAST_CURRENT:
					// also receives the control values.
					r._bcastRecvBuffer[j]._current = *((short *)(m.data));

					r._bcastRecvBuffer[j]._position_error = *((short *)(m.data+4));
					r._bcastRecvBuffer[j]._update_c = before;
					j++;
					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._current = *((short *)(m.data+2));

						r._bcastRecvBuffer[j]._position_error = *((short *)(m.data+6));
						r._bcastRecvBuffer[j]._update_c = before;
					}
					break;

				default:
					break;
				}
			}
		}

		//
		// handle class 0 messages - polling messages.
		// (class 0, 8 bits of the ID used to represent the source and destination).
		// the first byte of the message is the message type and motor number (0 or 1).
		//
		if (messagePending)
		{
			for (i = 0; i < r._readMessages; i++)
			{
				CMSG& m = r._readBuffer[i];
				if (m.len & NTCAN_NO_DATA)
					if (r._p) 
					{
						(*r._p) ("CAN: error in message %x len: %d type: %x: %x\n", 
							m.id, m.len, m.data[0], m.msg_lost);
						
						continue;
					}

				if (((m.id &0x700) == 0) && 
					((m.data[0] & 0x7f) != _filter) &&
					 (m.data[0] & 0x7f) < NUM_OF_MESSAGES)
					r.printMessage (m);

				if (!noreply) /// this requires a reply.
				{
					if (((m.id & 0x700) == 0) &&				/// class 0 msg.
						((m.id & 0x0f) == r._my_address))
					{
						/// legitimate message directed here, checks whether replies to any message.
						int j;
						for (j = 0; j < r._writeMessages; j++)
						{
							if (((r._writeBuffer[j].id & 0x0f) == ((m.id & 0xf0) >> 4)) &&
								(m.data[0] == r._writeBuffer[j].data[0]))
							{
								if (r._replyBuffer[j].id != 0)
								{
									if (r._p)
									{
										(*r._p) ("CAN: message %x was already replied\n", m.id);
										r.printMessage (m);
									}
								}
								else
								{
									memcpy (&r._replyBuffer[j], &m, sizeof(CMSG));
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

			/// the pending msg doesn't require a reply.
			if (noreply)
			{
				remainingMsgs = 0;
				messagePending = false;
				r._error_status = YARP_OK;
				goto AckMessageLoop;
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
						memset (r._replyBuffer, 0, sizeof(CMSG) * r._writeMessages);

						if (r._p)
						{
							int j;
							for (j = 0; j < r._writeMessages; j++)
							{
								if ((r._writeBuffer[j].data[0] & 0x7f) != _filter)
									r.printMessage (r._writeBuffer[j]);
							}
						}
					}
				}
			}
		}

		_mutex.Post ();

		/// wait.
		now = YARPTime::GetTimeAsSecondsHr();
		if ((now - before)*1000 < r._polling_interval)
		{
            double k = double(r._polling_interval)/1000.0-(now-before);
			YARPTime::DelayInSeconds(k);
            //before = now + k;
		}
		else 
		{
			if (r._p) (*r._p)("CAN: thread can't poll fast enough (time: %f)\n", now-before);
    		//before = now;
		}
	}

	///
}


///
///
/// specific messages that change the driver behavior.
int YARPEsdCanDeviceDriver::setDebugMessageFilter (void *cmd)
{
	_mutex.Wait();
	EsdCanResources& r = RES(system_resources);
	_filter = *(int *)cmd;
	_mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::setDebugPrintFunction (void *cmd)
{
	_mutex.Wait();
	EsdCanResources& r = RES(system_resources);
	_p = (int (*) (const char *fmt, ...))cmd;
	_mutex.Post();

	return YARP_OK;
}


///
/// 
/// control card commands.
///

/// cmd is an array of double
int YARPEsdCanDeviceDriver::getPositions (void *cmd)
{
	return _readDWordArray (CAN_GET_ENCODER_POSITION, (double *)cmd);
}

/// cmd is a SingleAxisParameters pointer with double arg
int YARPEsdCanDeviceDriver::getPosition (void *cmd)
{
	/// prepare can message.
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

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
int YARPEsdCanDeviceDriver::setPositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints (); i++)
	{
		if (ENABLED(i))
		{
			//SingleAxisParameters x;
			//x.axis = i;
			//x.parameters = tmp+i;	

			r.addMessage (CAN_POSITION_MOVE, i);
			const int j = r._writeMessages - 1;
			_ref_positions[i] = tmp[i];
			*((int*)(r._writeBuffer[j].data+1)) = S_32(_ref_positions[i]);		/// pos
			*((short*)(r._writeBuffer[j].data+5)) = S_16(_ref_speeds[i]);		/// speed
			r._writeBuffer[j].len = 7;
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

inline bool YARPEsdCanDeviceDriver::ENABLED (int axis)
{
	EsdCanResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & ESD_CAN_SKIP_ADDR) == 0) ? true : false;
}

///
///
/// cmd is a SingleAxisParameters pointer to a double argument
int YARPEsdCanDeviceDriver::setPosition (void *cmd)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

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
	*((int*)(r._writeBuffer[0].data+1)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r._writeBuffer[0].data+5)) = S_16(_ref_speeds[axis]);			/// speed
	r._writeBuffer[0].len = 7;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

///
/// starts the homing procedure for a given joint.
int YARPEsdCanDeviceDriver::startCalibration (void *cmd)
{
	/// prepare can message.
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeWord16 (CAN_CALIBRATE_ENCODER, axis, S_16(*((double *)tmp->parameters)));   
}

///
/// gets the control mode according to some controller specific coding (if any).
int YARPEsdCanDeviceDriver::getControlMode (void *cmd)
{
    short value = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

   	if (_readWord16 (CAN_GET_CONTROL_MODE, axis, value) != YARP_OK)
	{
		*((double *)tmp->parameters) = 0;
		return YARP_FAIL;
	}

	*((double *)tmp->parameters) = double(value); 
    return YARP_OK;
}

/// cmd is an array of double of length njoints specifying speed 
/// for each axis
int YARPEsdCanDeviceDriver::velocityMove (void *cmd)
{
	/// prepare can message.
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED (i))
		{
			r.addMessage (CAN_VELOCITY_MOVE, i);
			const int j = r._writeMessages - 1;
			_ref_speeds[i] = tmp[i];
			*((short*)(r._writeBuffer[j].data+1)) = S_16(_ref_speeds[i]);	/// speed
			*((short*)(r._writeBuffer[j].data+3)) = S_16(_ref_accs[i]);		/// accel
			r._writeBuffer[j].len = 5;
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
int YARPEsdCanDeviceDriver::definePosition (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::definePositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::setCommand (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_COMMAND_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double (LATER: to be optimized).
int YARPEsdCanDeviceDriver::setCommands (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::getRefPositions (void *cmd)
{
	return _readDWordArray (CAN_GET_DESIRED_POSITION, (double *)cmd);
}

/// cmd is a SingleAxis pointer with double arg
int YARPEsdCanDeviceDriver::getRefPosition (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	int value = 0;
	if (_readDWord (CAN_GET_DESIRED_POSITION, axis, value) == YARP_OK)
		*((double *)(tmp->parameters)) = double (value);
	else
		return YARP_FAIL;

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPEsdCanDeviceDriver::setSpeed (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	_ref_speeds[axis] = *((double *)(tmp->parameters));
	//const short s = S_16(_ref_speeds[axis]);
	//return _writeWord16 (CAN_SET_DESIRED_VELOCITY, axis, s);
	return YARP_OK;
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::setSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		_ref_speeds[i] = tmp[i];
		//if (_writeWord16 (CAN_SET_DESIRED_VELOCITY, i, S_16(tmp[i])) != YARP_OK)
		//	return YARP_FAIL;
	}

	return YARP_OK;
}


/// cmd is an array of double
int YARPEsdCanDeviceDriver::getSpeeds (void *cmd)
{
	return _readWord16Array (CAN_GET_DESIRED_VELOCITY, (double *)cmd);
}

/// cmd is an array of double (LATER: to be optimized).
int YARPEsdCanDeviceDriver::getRefSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::setAcceleration (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	_ref_accs[axis] = *((double *)tmp->parameters);
	const short s = S_16(_ref_accs[axis]);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}

/// cmd is an array of double (LATER: to be optimized, WARNING: doesn't skip disabled joints).
int YARPEsdCanDeviceDriver::setAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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

/// cmd is an array of double (LATER: to be optimized).
int YARPEsdCanDeviceDriver::getRefAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::setOffset (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_OFFSET, axis, s);
}

/// cmd is an array of double (LATER: to be optimized).
int YARPEsdCanDeviceDriver::setOffsets (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::setIntegratorLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_ILIM_GAIN, axis, s);
}

/// cmd is an array of double (LATER: to be optimized, WARNING: doesn't check disabled cards).
int YARPEsdCanDeviceDriver::setIntegratorLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
/// LATER: can be optimized.
int YARPEsdCanDeviceDriver::setPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
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
int YARPEsdCanDeviceDriver::getPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
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
int YARPEsdCanDeviceDriver::enableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPEsdCanDeviceDriver::disableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPEsdCanDeviceDriver::controllerRun (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);
}

/// cmd is an int *
int YARPEsdCanDeviceDriver::controllerIdle (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

/// cmd is an array of double
/// LATER: can be optimized, we can also be reading current back.
int YARPEsdCanDeviceDriver::getTorques (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::getTorque (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
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
int YARPEsdCanDeviceDriver::getPidError (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	short value;

	if (_readWord16 (CAN_GET_PID_ERROR, axis, value) != YARP_OK)
	{
		*((double *)tmp->parameters) = 0;
		return YARP_FAIL;
	}

	*((double *)tmp->parameters) = double(value);
	return YARP_OK;
}

/// cmd is a SingleAxis poitner with 1 double arg
int YARPEsdCanDeviceDriver::setCurrentLimit (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_CURRENT_LIMIT, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
/// LATER: can be optimized.
int YARPEsdCanDeviceDriver::setCurrentLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r.getJoints(); i++)
	{
		if (_writeDWord (CAN_SET_CURRENT_LIMIT, i, S_32(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a pointer to SingleAxisParameters struct with no argument.
int YARPEsdCanDeviceDriver::readBootMemory (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	return _writeNone (CAN_READ_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPEsdCanDeviceDriver::writeBootMemory (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	return _writeNone (CAN_WRITE_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPEsdCanDeviceDriver::setSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a pointer to SingleAxisParameters struct with a single double arg.
int YARPEsdCanDeviceDriver::setSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a SingleAxis pointer with double arg
int YARPEsdCanDeviceDriver::getSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MIN_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with double arg
int YARPEsdCanDeviceDriver::getSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	int value;

	_readDWord (CAN_GET_MAX_POSITION, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}


/// cmd is a SingleAxis pointer with double arg
int YARPEsdCanDeviceDriver::setTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_TLIM, axis, s);
}

/// cmd is an array of double
/// LATER: can be optimized.
int YARPEsdCanDeviceDriver::setTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::getTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	short value;

	_readWord16 (CAN_GET_TLIM, axis, value);
	*((double *)tmp->parameters) = double(value);

	return YARP_OK;
}

/// cmd is an array of double
/// LATER: can be optimized.
int YARPEsdCanDeviceDriver::getTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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
int YARPEsdCanDeviceDriver::getErrorStatus (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
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

/// check motion done, single axis.
int YARPEsdCanDeviceDriver::checkMotionDoneSingle (void *cmd)
{
    SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	short value;
	double *out = (double *)tmp->parameters;

    if (_readWord16 (CAN_MOTION_DONE, axis, value) == YARP_OK)
    {
        *out = double(value);
    }
    else
    {
        *out = 0;
        return YARP_FAIL;
    }

    return YARP_OK;
}

/// cmd is a pointer to a bool
int YARPEsdCanDeviceDriver::checkMotionDone (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value;
	bool *out = (bool *) cmd;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (CAN_MOTION_DONE, i);
		}
	}

	if (r._writeMessages < 1)
		return YARP_FAIL;

	_writerequested = true;
	_noreply = false;
	_mutex.Post();

	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		*out = false;
		return YARP_FAIL;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			CMSG& m = r._replyBuffer[j];
			if (m.id != 0xffff)
			{
				value = *((short *)(m.data+1));
				if (!value)
				{
					*out = false;
					return YARP_OK;
				}
			}
			j++;
		}
	}

	*out = true;
	return YARP_OK;
}


/// sets the broadcast policy for a given board (don't need to be called twice).
/// the parameter is a 32-bit integer: bit X = 1 -> message X = active
/// e.g. 0x02 activates the broadcast of position information
///		 0x04 activates the broadcast of velocity ...
///
int YARPEsdCanDeviceDriver::setBCastMessages (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	return _writeDWord (CAN_SET_BCAST_POLICY, axis, S_32(*((double *)tmp->parameters)));
}

///
/// reads an array of double from the broadcast message position buffer.
/// LATER: add a check of timing/error message.
///
int YARPEsdCanDeviceDriver::getBCastPosition (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastPositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;
    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._position);
	}
    _mutex.Post();
	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastPIDOutput (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._pid_value);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastPIDOutputs (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;
    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._pid_value);
	}
    _mutex.Post();
	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastCurrent (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._current);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastCurrents (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;
    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._current);
	}
    _mutex.Post();
	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastFaults (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short *tmp = (short *)cmd;

    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = short(r._bcastRecvBuffer[i]._fault);
	}
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastPositionError (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position_error);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdCanDeviceDriver::getBCastPositionErrors (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;

    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._position_error);
	}
    _mutex.Post();

	return YARP_OK;
}


///
/// helper functions.
///
///
///

/// sends a message without parameters
int YARPEsdCanDeviceDriver::_writeNone (int msg, int axis)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

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

int YARPEsdCanDeviceDriver::_readWord16 (int msg, int axis, short& value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

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

	value = *((short *)(r._replyBuffer[0].data+1));
	return YARP_OK;
}

/// reads an array.
int YARPEsdCanDeviceDriver::_readWord16Array (int msg, double *out)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value = 0;

	_mutex.Wait();
	r.startPacket ();

	for(i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (msg, i);
		}
		else
			out[i] = 0;
	}

	if (r._writeMessages < 1)

	{

		_mutex.Post();
		return YARP_FAIL;
	}


	_writerequested = true;
	_noreply = false;
	_mutex.Post();

	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		memset (out, 0, sizeof(double) * r.getJoints());
		return YARP_FAIL;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			CMSG& m = r._replyBuffer[j];
			if (m.id == 0xffff)
				out[i] = 0;
			else
				out[i] = *((short *)(m.data+1));
			j++;
		}
	}

	return YARP_OK;
}

/// to send a Word16.
int YARPEsdCanDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	
	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].data+1)) = s;
	r._writeBuffer[0].len = 3;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	/// hopefully ok...
	return YARP_OK;
}

/// write a DWord
int YARPEsdCanDeviceDriver::_writeDWord (int msg, int axis, int value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((int*)(r._writeBuffer[0].data+1)) = value;
	r._writeBuffer[0].len = 5;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

/// two shorts in a single Can message (both must belong to the same control card).
int YARPEsdCanDeviceDriver::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	EsdCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);
	ACE_ASSERT ((axis % 2) == 0);	/// axis is even.

	if (!ENABLED(axis))
		return YARP_OK;

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r._writeBuffer[0].data+1)) = s1;
	*((short *)(r._writeBuffer[0].data+3)) = s2;
	r._writeBuffer[0].len = 5;

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
int YARPEsdCanDeviceDriver::_readDWord (int msg, int axis, int& value)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

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

	value = *((int *)(r._replyBuffer[0].data+1));
	return YARP_OK;
}

/// reads an array of double words.
int YARPEsdCanDeviceDriver::_readDWordArray (int msg, double *out)
{
	EsdCanResources& r = RES(system_resources);
	int i, value = 0;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			r.addMessage (msg, i);
		}
		else
			out[i] = 0;
	}

	if (r._writeMessages < 1)
	{
		_mutex.Post();
		return YARP_FAIL;
	}

	_writerequested = true;
	_noreply = false;
	_mutex.Post();

	_done.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		memset (out, 0, sizeof(double) * r.getJoints());
		return YARP_FAIL;
	}

	int j;
	for (i = 0, j = 0; i < r.getJoints(); i++)
	{
		if (ENABLED(i))
		{
			CMSG& m = r._replyBuffer[j];
			if (m.id == 0xffff)
				out[i] = 0;
			else
				out[i] = *((int *)(m.data+1));
			j++;
		}
	}

	return YARP_OK;
}

