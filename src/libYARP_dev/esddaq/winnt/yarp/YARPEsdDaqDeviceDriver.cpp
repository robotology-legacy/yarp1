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
/// $Id: YARPEsdDaqDeviceDriver.cpp,v 1.11 2006-06-13 12:31:46 gmetta Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPADCUtils.h>
#include <yarp/YARPScheduler.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

/// specific to this device driver.
#include "YARPEsdDaqDeviceDriver.h"
#include "../dd_orig/include/ntcan.h"

/// can bus buffer size.
const int BUF_SIZE = 2047;
/// number of channels on the pic-daq card
const int MAX_CHANNELS = 32;

typedef int (*PV) (const char *fmt, ...);


//
// a buffer element.
//
class BCastBufferElement
{
public:
	short _value;
	double _update;

	BCastBufferElement () { zero (); }

	void zero (void)
	{
		_value = 0;
		_update = .0;
    }
};

///
///
///
class EsdDaqResources
{
public:
	EsdDaqResources ();
	~EsdDaqResources ();

	int initialize (const EsdDaqOpenParameters& parms);
	int uninitialize ();
	int read ();
	bool error (const CMSG& m);

	int startPacket ();
	int addMessage (int msg_id);
	int writePacket ();

	int printMessage (const CMSG& m);
	int dumpBuffers (void);
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
	int _scanSequence;							/// the sequence of channel to sample (bitmap).
    bool _broadcast;                            /// whether to broadcast replies.
	BCastBufferElement _bcastRecvBuffer[MAX_CHANNELS];		/// local storage for bcast reply messages.

	unsigned char _my_address;					/// local can bus address.
	unsigned char _remote_address;				/// remote can bus address.

	int _error_status;							/// error status of the last packet.

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	
	char _printBuffer[16384];
};

EsdDaqResources::EsdDaqResources ()
{
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;						/// default 1Mbit/s
	_networkN = 0;

	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

	_my_address = 0;
	_remote_address = 0;
    _broadcast = false;
	_p = NULL;

	_readMessages = 0;
	_msg_lost = 0;
	_writeMessages = 0;
	_error_status = YARP_OK;
	_scanSequence = 0;
}

EsdDaqResources::~EsdDaqResources () 
{ 
	uninitialize(); 
}

int EsdDaqResources::initialize (const EsdDaqOpenParameters& parms)
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

	_my_address = parms._my_address;
	_remote_address = parms._remote_address;
	_polling_interval = parms._polling_interval;
	_timeout = parms._timeout;
	_p = parms._p;
	_broadcast = parms._broadcast;

	_txQueueSize = parms._txQueueSize;
	_rxQueueSize = parms._rxQueueSize;
	_txTimeout = parms._txTimeout;
	_rxTimeout = parms._rxTimeout;

	/// clean up buffers.
	memset (_readBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_writeBuffer, 0, sizeof(CMSG)*BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(CMSG)*BUF_SIZE);

    ACE_OS::memset (_bcastRecvBuffer, 0, sizeof(BCastBufferElement) * MAX_CHANNELS);

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

	/// sets all message ID's (only certain messages need to be included).
	int i;
	for (i = 0x200; i < 0x2ff; i++)
		canIdAdd (_handle, i);
	
	return YARP_OK;
}


int EsdDaqResources::uninitialize ()
{
	if (_handle != ACE_INVALID_HANDLE)
	{
		int res = canClose (_handle);
		if (res != NTCAN_SUCCESS)
			return YARP_FAIL;
		_handle = ACE_INVALID_HANDLE;
	}
	
	return YARP_OK;
}


int EsdDaqResources::read ()
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _readBuffer, &messages); 
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	_readMessages = messages;
	return YARP_OK;
}

int EsdDaqResources::startPacket ()
{
	_writeMessages = 0;
	return YARP_OK;
}

/*
 * LATER: check the message ID's class, it might be different for the pic-daq board!
 */
int EsdDaqResources::addMessage (int msg_id)
{
	CMSG x;
	memset (&x, 0, sizeof(CMSG));

	x.id = 0x200;
	x.id |= (_my_address << 4);
	x.id |= (_remote_address & 0x0f);

	x.len = 1;
	x.data[0] = msg_id;

	_writeBuffer[_writeMessages] = x;
	_writeMessages ++;

	return YARP_OK;
}

int EsdDaqResources::writePacket ()
{
	if (_writeMessages < 1)
		return YARP_FAIL;

	long len = _writeMessages;

	int res = canSend (_handle, _writeBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _writeMessages)
		return YARP_FAIL;

	return YARP_OK;
}

int EsdDaqResources::printMessage (const CMSG& m)
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


int EsdDaqResources::dumpBuffers (void)
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

bool EsdDaqResources::error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

///
///
///
inline EsdDaqResources& RES(void *res) { return *(EsdDaqResources *)res; }


YARPEsdDaqDeviceDriver::YARPEsdDaqDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPEsdDaqDeviceDriver>(ADCCmds), _mutex(1), _done(0)
{
	system_resources = (void *) new EsdDaqResources;
	ACE_ASSERT (system_resources != NULL);

	m_cmds[CMDScanSetup] = &YARPEsdDaqDeviceDriver::scanSetup;
	m_cmds[CMDBroadcastSetup] = &YARPEsdDaqDeviceDriver::broadcastSetup;
	m_cmds[CMDAIReadScan] = &YARPEsdDaqDeviceDriver::aiReadScan;
	m_cmds[CMDAIReadChannel] = &YARPEsdDaqDeviceDriver::aiReadChannel;
	m_cmds[CMDGetMaxChannels] = &YARPEsdDaqDeviceDriver::getMaxChannels;
	m_cmds[CMDAISetDebugPrintFunction] = &YARPEsdDaqDeviceDriver::setDebugPrintFunction;
}


YARPEsdDaqDeviceDriver::~YARPEsdDaqDeviceDriver ()
{
	if (system_resources != NULL)
		delete (EsdDaqResources *)system_resources;
	system_resources = NULL;
}


int YARPEsdDaqDeviceDriver::open (void *p)
{
	if (p == NULL)
		return YARP_FAIL;

	_mutex.Wait();

	EsdDaqOpenParameters& parms = *(EsdDaqOpenParameters *)p;
	EsdDaqResources& r = RES (system_resources);
	
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

    //scanSetup (&r._scanSequence);

	return YARP_OK;
}

int YARPEsdDaqDeviceDriver::close (void)
{
	EsdDaqResources& d = RES(system_resources);

	End ();	/// stops the thread first (joins too).

	int ret = d.uninitialize ();
	return ret;
}


///
/// LATER: check the class of messages to be received. IMPORTANT!
///
void YARPEsdDaqDeviceDriver::Body (void)
{
	EsdDaqResources& r = RES (system_resources);

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

        //
        // broadcast messages.
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

                // class = 2, remote was addressed earlier, message is a broadcast [48, 48+32).
                // broadcast flag must be true.
                //
			    if ((m.id & 0x700) == 0x200 && (((m.id & 0x0f0) >> 4) == r._remote_address) &&
                    (m.data[0] >= MPH_BCAST_CHANNEL_0 && m.data[0] < MPH_BCAST_CHANNEL_0 + MAX_CHANNELS) &&
                    r._broadcast == true) 
			    {
				    // 4 next bits = source address, next 4 bits = dest address (alw. 0).
                    // data[0] = channel number + 0x30.
				    // need an array here for storing the messages on a per-channel basis.

                    int j = m.data[0] - MPH_BCAST_CHANNEL_0;
                    if (j >= 0 && j < MAX_CHANNELS)
                    {
                        if (m.len == 7 && j <= MAX_CHANNELS-3)
                        {
					        r._bcastRecvBuffer[j]._value = *((short *)(m.data+1));
					        r._bcastRecvBuffer[j]._update = before;
					        r._bcastRecvBuffer[j+1]._value = *((short *)(m.data+3));
					        r._bcastRecvBuffer[j+1]._update = before;
					        r._bcastRecvBuffer[j+2]._value = *((short *)(m.data+5));
					        r._bcastRecvBuffer[j+2]._update = before;
                        }
                        else
                        if (m.len == 5 && j >= 30)
                        {
					        r._bcastRecvBuffer[j]._value = *((short *)(m.data+1));
					        r._bcastRecvBuffer[j]._update = before;
					        r._bcastRecvBuffer[j+1]._value = *((short *)(m.data+3));
					        r._bcastRecvBuffer[j+1]._update = before;
                        }
                    }
                }
        }

        //
        // other messages, non-broadcast.
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

				if ((m.id & 0x700) == 0x200)
					r.printMessage (m);

				if (!noreply) /// this requires a reply.
				{
					if (((m.id & 0x700) == 0x200) &&
						((m.id & 0x00f) == r._my_address) &&
						(((m.id & 0x0f0) >> 4) == r._remote_address) &&
                        (m.data[0] < MPH_BCAST_CHANNEL_0)
					   )
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




///
///
/// specific messages that change the driver behavior.

int YARPEsdDaqDeviceDriver::setDebugPrintFunction (void *cmd)
{
	_mutex.Wait();
	EsdDaqResources& r = RES(system_resources);
	_p = (int (*) (const char *fmt, ...))cmd;
	_mutex.Post();

	return YARP_OK;
}


///
/// 
/// control card commands.
///

/// cmd is a pointer to a 32-bit map (1- read, 0- don't read channel).
int YARPEsdDaqDeviceDriver::scanSetup (void *cmd)
{
	_mutex.Wait();
	EsdDaqResources& r = RES(system_resources);
    r._broadcast = false;
	r._scanSequence = *((int *)cmd);
    _mutex.Post();

    if (_writeDWord (MPH_SET_SEQUENCE, 0, *((int *)cmd)) != YARP_OK)
	{
		return YARP_FAIL;
    }

	return YARP_OK;
}


/// cmd is a pointer to a 32-bit map (1- read, 0- don't read channel).
int YARPEsdDaqDeviceDriver::broadcastSetup (void *cmd)
{
	_mutex.Wait();
	EsdDaqResources& r = RES(system_resources);
	r._scanSequence = *((int *)cmd);
    r._broadcast = true;
    _mutex.Post();

    if (_writeDWord (MPH_SET_BCAST_SEQUENCE, 0, *((int *)cmd)) != YARP_OK)
	{
		return YARP_FAIL;
	}

	return YARP_OK;
}


/// cmd is an array of double (LATER: verify this).
int YARPEsdDaqDeviceDriver::aiReadScan (void *cmd)
{
    EsdDaqResources& r = RES(system_resources);
    if (!r._broadcast)
    	return _readWord16Array (MPH_READ_CHANNEL_0, (short *)cmd);

    // bcast version.
	int i;
	short *tmp = (short *)cmd;
    _mutex.Wait();
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		tmp[i] = short(r._bcastRecvBuffer[i]._value);
	}
    _mutex.Post();
	return YARP_OK;

	/// rationale:
	/// - sends a single packet to start the acquisition
	/// - replies multiple messages (each containing 3 channels)
	/// 
}

/// returns the maximum number of channels.
int YARPEsdDaqDeviceDriver::getMaxChannels (void *cmd)
{
	*((int *)cmd) = MAX_CHANNELS;
	return YARP_OK;
}

/// cmd is a pointer to an integer.
int YARPEsdDaqDeviceDriver::aiReadChannel (void *cmd)
{
	/// prepare can message.
	const int channel = *((int *)cmd);
	ACE_ASSERT (channel >= 0 && channel < MAX_CHANNELS);

	short value;
	if (_readWord16 (MPH_READ_CHANNEL_0+channel, 0, value) == YARP_OK)
	{
		*((int *)cmd) = int(value);
		return YARP_OK;
	}
	
	*((int *)cmd) = 0;
	return YARP_FAIL;
}


///
/// helper functions.
///
///
///

/// sends a message without parameters
int YARPEsdDaqDeviceDriver::_writeNone (int msg, int axis)
{
	EsdDaqResources& r = RES(system_resources);
	ACE_ASSERT (axis == 0);

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);

	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

int YARPEsdDaqDeviceDriver::_readWord16 (int msg, int axis, short& value)
{
	EsdDaqResources& r = RES(system_resources);
	ACE_ASSERT (axis == 0);

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);
		
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

// reads an array up to MAX_CHANNELS (WARNING: <out> must be preallocated of
// length MAX_CHANNELS).
// this should probably be part of the configuration of the 
// scan sequence.
//
int YARPEsdDaqDeviceDriver::_readWord16Array (int msg, short *out)
{
	EsdDaqResources& r = RES(system_resources);
	int i;
	short value = 0;

	_mutex.Wait();
	r.startPacket ();

	int val = r._scanSequence;

	for(i = 0; i < MAX_CHANNELS; i++)
	{
		if (val & 0x00000001)
		{
			r.addMessage (msg + MAX_CHANNELS - 1 - i);
		}

		val >>= 1;
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
		ACE_OS::memset (out, 0, sizeof(short) * MAX_CHANNELS);
		return YARP_FAIL;
	}

	ACE_OS::memset (out, 0, sizeof(short) * MAX_CHANNELS);
	for (i = 0; i < r._writeMessages; i++)
	{
		CMSG& m = r._replyBuffer[i];
		if (m.id == 0xffff)
			out[r._writeMessages-1-i] = 0;
		else
			out[r._writeMessages-1-i] = *((short *)(m.data+1));
	}

	return YARP_OK;
}

/// to send a Word16.
int YARPEsdDaqDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	EsdDaqResources& r = RES(system_resources);
	ACE_ASSERT (axis == 0);
	
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);

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
int YARPEsdDaqDeviceDriver::_writeDWord (int msg, int axis, int value)
{
	EsdDaqResources& r = RES(system_resources);
	ACE_ASSERT (axis == 0);

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);

	*((int*)(r._writeBuffer[0].data+1)) = value;
	r._writeBuffer[0].len = 5;
		
	_writerequested = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_done.Wait();

	return YARP_OK;
}

/// two shorts in a single Can message.
int YARPEsdDaqDeviceDriver::_writeWord16Ex (int msg, int axis, short s1, short s2)
{
	/// prepare Can message.
	EsdDaqResources& r = RES(system_resources);

	ACE_ASSERT (axis == 0);

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);

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

//
// sends a message and gets a dword back.
// 
int YARPEsdDaqDeviceDriver::_readDWord (int msg, int axis, int& value)
{
	EsdDaqResources& r = RES(system_resources);
	ACE_ASSERT (axis == 0);

	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg);
		
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

// reads an array of double words.
//
int YARPEsdDaqDeviceDriver::_readDWordArray (int msg, int *out)
{
	EsdDaqResources& r = RES(system_resources);
	int i, value = 0;

	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < MAX_CHANNELS; i++)
	{
		r.addMessage (msg + i);
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
		memset (out, 0, sizeof(double) * MAX_CHANNELS);
		return YARP_FAIL;
	}

	int j;
	for (i = 0, j = 0; i < MAX_CHANNELS; i++)
	{
		CMSG& m = r._replyBuffer[j];
		if (m.id == 0xffff)
			out[i] = 0;
		else
			out[i] = *((int *)(m.data+1));
		j++;
	}

	return YARP_OK;
}
