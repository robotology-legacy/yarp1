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
/// $Id: YARPEsdDeviceDriver.cpp,v 1.5 2006-05-16 22:59:43 babybot Exp $
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
#include "YARPEsdDeviceDriver.h"
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
	short _velocity;
	double _update_v;
	short _acceleration;

	// msg 3
	short _fault;
	double _update_e;

	// msg 4
	short _current;
	short _controlvalue;
	double _update_c;

	BCastBufferElement () { zero (); }

	void zero (void)
	{
		_position = 0;
		_velocity = 0;
		_acceleration = 0;
		_current = 0;
		_fault = 0;
		_controlvalue = 0;

		_update_p = .0;
		_update_v = .0;

		_update_e = .0;
		_update_c = .0;
	}
};


///
///
///
class EsdResources
{
public:
	EsdResources ();
	~EsdResources ();

	int initialize (const EsdOpenParameters& parms);
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
	unsigned char _destinations[EsdOpenParameters::ESD_MAX_CARDS];	/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).

	int _error_status;							/// error status of the last packet.

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	
	char _printBuffer[16384];
};

EsdResources::EsdResources ()
{
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;						/// default 1Mbit/s
	_networkN = 0;

	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	memset (_destinations, 0, sizeof(unsigned char) * EsdOpenParameters::ESD_MAX_CARDS);

	_my_address = 0;
	_njoints = 0;
	_p = NULL;

	_readMessages = 0;
	_msg_lost = 0;
	_writeMessages = 0;
	_bcastRecvBuffer = NULL;

	_error_status = YARP_OK;
}

EsdResources::~EsdResources () 
{ 
	uninitialize(); 
}

int EsdResources::initialize (const EsdOpenParameters& parms)
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

	memcpy (_destinations, parms._destinations, sizeof(unsigned char)*EsdOpenParameters::ESD_MAX_CARDS);
	_my_address = parms._my_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_njoints = parms._njoints;	
	_p = parms._p;

	_txQueueSize = parms._txQueueSize;
	_rxQueueSize = parms._rxQueueSize;
	_txTimeout = parms._txTimeout;
	_rxTimeout = parms._rxTimeout;

	_bcastRecvBuffer = new BCastBufferElement[_njoints];
	ACE_ASSERT (_bcastRecvBuffer != NULL);

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

	//canIdAdd (_handle, 0x1bc);
	//canIdAdd (_handle, 0x1cb);

	return YARP_OK;
}


int EsdResources::uninitialize ()
{
	if (_bcastRecvBuffer != NULL) 
    {
        delete[] _bcastRecvBuffer;
        _bcastRecvBuffer = NULL;
    }

	if (_handle != ACE_INVALID_HANDLE)
	{
		int res = canClose (_handle);
		if (res != NTCAN_SUCCESS)
			return YARP_FAIL;
		_handle = ACE_INVALID_HANDLE;
	}
	
	return YARP_OK;
}


int EsdResources::read ()
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _readBuffer, &messages); 
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	_readMessages = messages;
	return YARP_OK;
}

int EsdResources::startPacket ()
{
	_writeMessages = 0;
	return YARP_OK;
}

int EsdResources::addMessage (int msg_id, int joint)
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

int EsdResources::writePacket ()
{
	if (_writeMessages < 1)
		return YARP_FAIL;

	long len = _writeMessages;

	int res = canSend (_handle, _writeBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _writeMessages)
		return YARP_FAIL;

	return YARP_OK;
}

int EsdResources::printMessage (const CMSG& m)
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


int EsdResources::dumpBuffers (void)
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

bool EsdResources::error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

///
///
///
inline EsdResources& RES(void *res) { return *(EsdResources *)res; }


YARPEsdDeviceDriver::YARPEsdDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPEsdDeviceDriver>(CBNCmds), _mutex(1), _done(0)
{
	system_resources = (void *) new EsdResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[CMDGetPosition] = &YARPEsdDeviceDriver::getBCastPosition;
	m_cmds[CMDGetPositions] = &YARPEsdDeviceDriver::getBCastPositions;
	m_cmds[CMDGetSpeeds] = &YARPEsdDeviceDriver::getBCastVelocities;
	m_cmds[CMDGetAccelerations] = &YARPEsdDeviceDriver::getBCastAccelerations;
	m_cmds[CMDGetTorque] = &YARPEsdDeviceDriver::getBCastCurrent;
	m_cmds[CMDGetTorques] = &YARPEsdDeviceDriver::getBCastCurrents;
	m_cmds[CMDGetAnalogChannel] = &YARPEsdDeviceDriver::getBCastCurrent;
	m_cmds[CMDGetAnalogChannels] = &YARPEsdDeviceDriver::getBCastCurrents;
	m_cmds[CMDGetFaults] = &YARPEsdDeviceDriver::getBCastFaults;
	m_cmds[CMDSetDebugPrintFunction] = &YARPEsdDeviceDriver::setDebugPrintFunction;
}


YARPEsdDeviceDriver::~YARPEsdDeviceDriver ()
{
	if (system_resources != NULL)
		delete (EsdResources *)system_resources;
	system_resources = NULL;
}


int YARPEsdDeviceDriver::open (void *p)
{
	if (p == NULL)
		return YARP_FAIL;

	_mutex.Wait();

	EsdOpenParameters& parms = *(EsdOpenParameters *)p;
	EsdResources& r = RES (system_resources);
	
	if (r.initialize (parms) != YARP_OK)
	{
		_mutex.Post();
		return YARP_FAIL;
	}

	Begin ();
	_done.Wait ();

	/// used for printing debug messages.
	_p = parms._p;
	_writerequested = false;
	_noreply = false;

	_mutex.Post ();

	return YARP_OK;
}

int YARPEsdDeviceDriver::close (void)
{
	EsdResources& d = RES(system_resources);
	End ();	/// stops the thread first (joins too).
	int ret = d.uninitialize ();

	return ret;
}


///
///
///
void YARPEsdDeviceDriver::Body (void)
{
	EsdResources& r = RES (system_resources);

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
				for (j = 0; j < EsdOpenParameters::ESD_MAX_CARDS; j++)
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

				case CAN_BCAST_VELOCITY:
					r._bcastRecvBuffer[j]._velocity = *((short *)(m.data));
					r._bcastRecvBuffer[j]._update_v = before;
					r._bcastRecvBuffer[j]._acceleration = *((short *)(m.data+4));

					j++;
					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._velocity = *((short *)(m.data+2));
						r._bcastRecvBuffer[j]._update_v = before;
						r._bcastRecvBuffer[j]._acceleration = *((short *)(m.data+6));
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

					r._bcastRecvBuffer[j]._controlvalue = *((short *)(m.data+4));
					r._bcastRecvBuffer[j]._update_c = before;
					j++;
					if (j < r.getJoints())
					{
						r._bcastRecvBuffer[j]._current = *((short *)(m.data+2));

						r._bcastRecvBuffer[j]._controlvalue = *((short *)(m.data+6));
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
					((m.data[0] & 0x7f) < NUM_OF_MESSAGES))
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


int YARPEsdDeviceDriver::setDebugPrintFunction (void *cmd)
{
	_mutex.Wait();
	EsdResources& r = RES(system_resources);
	_p = (int (*) (const char *fmt, ...))cmd;
	_mutex.Post();

	return YARP_OK;
}


///
/// 
/// control card commands.
///
inline bool YARPEsdDeviceDriver::ENABLED (int axis)
{
	EsdResources& r = RES(system_resources);
	return ((r._destinations[axis/2] & ESD_CAN_SKIP_ADDR) == 0) ? true : false;
}


/// sets the broadcast policy for a given board (don't need to be called twice).
/// the parameter is a 32-bit integer: bit X = 1 -> message X = active
/// e.g. 0x02 activates the broadcast of position information
///		 0x04 activates the broadcast of velocity ...
///
int YARPEsdDeviceDriver::setBCastMessages (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);

	return _writeDWord (CAN_SET_BCAST_POLICY, axis, S_32(*((double *)tmp->parameters)));
}

///
/// reads an array of double from the broadcast message position buffer.
/// LATER: add a check of timing/error message.
///
int YARPEsdDeviceDriver::getBCastPosition (void *cmd)
{
	EsdResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());

    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._position);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdDeviceDriver::getBCastPositions (void *cmd)
{
	EsdResources& r = RES(system_resources);
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

int YARPEsdDeviceDriver::getBCastVelocities (void *cmd)
{
	EsdResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;
    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._velocity);
	}
    _mutex.Post();
	return YARP_OK;
}

int YARPEsdDeviceDriver::getBCastAccelerations (void *cmd)
{
	EsdResources& r = RES(system_resources);
	int i;
	double *tmp = (double *)cmd;
    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = double(r._bcastRecvBuffer[i]._acceleration);
	}
    _mutex.Post();
	return YARP_OK;
}

int YARPEsdDeviceDriver::getBCastCurrent (void *cmd)
{
	EsdResources& r = RES(system_resources);
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= r.getJoints());
	
    _mutex.Wait();
	*((double *)tmp->parameters) = double(r._bcastRecvBuffer[axis]._current);
    _mutex.Post();

	return YARP_OK;
}

int YARPEsdDeviceDriver::getBCastCurrents (void *cmd)
{
	EsdResources& r = RES(system_resources);
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

int YARPEsdDeviceDriver::getBCastFaults (void *cmd)
{
	EsdResources& r = RES(system_resources);
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

int YARPEsdDeviceDriver::getBCastControlValues (void *cmd)
{
	EsdResources& r = RES(system_resources);
	int i;
	int *tmp = (int *)cmd;

    _mutex.Wait();
	for (i = 0; i < r.getJoints(); i++)
	{
		tmp[i] = int(r._bcastRecvBuffer[i]._controlvalue);
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
int YARPEsdDeviceDriver::_writeNone (int msg, int axis)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);

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

int YARPEsdDeviceDriver::_readWord16 (int msg, int axis, short& value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);

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
int YARPEsdDeviceDriver::_readWord16Array (int msg, double *out)
{
	EsdResources& r = RES(system_resources);
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
int YARPEsdDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);
	
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
int YARPEsdDeviceDriver::_writeDWord (int msg, int axis, int value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);

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
int YARPEsdDeviceDriver::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	EsdResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);
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
int YARPEsdDeviceDriver::_readDWord (int msg, int axis, int& value)
{
	EsdResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (EsdOpenParameters::ESD_MAX_CARDS-1)*2);

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
int YARPEsdDeviceDriver::_readDWordArray (int msg, double *out)
{
	EsdResources& r = RES(system_resources);
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
