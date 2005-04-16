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
/// $Id: YARPEsdCanDeviceDriver.cpp,v 1.2 2005-04-16 01:43:58 babybot Exp $
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


///
///
/// note that this class is just a container.
class EsdCanResources
{
public:
	enum { ESD_TIMEOUT = 20, ESD_POLLING_INTERVAL = 5 };

	HANDLE _handle;								/// the actual ddriver handle.
	int _timeout;								/// this is my thread timeout.
	
	int _polling_interval;
	int _speed;	
	int _networkN;
	int _msg_lost;
	
	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	CMSG _canMsg[BUF_SIZE];						/// read buffer.
	CMSG _cmdBuffer[BUF_SIZE];					/// write buffer.
	CMSG _replyBuffer[BUF_SIZE];				/// .

	unsigned char _my_address;					/// 
	unsigned char _destinations[ESD_MAX_CARDS];	/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).

    int _cur_packetsize;						/// size of the current packet.
	int _error_status;							/// error status of the last packet.

	int (*_p) (char *fmt, ...);					///	pointer to a printf type function
												/// used to spy on can messages.
	int _filter;								/// don't print filtered messages.

public:
	int _initialize (const EsdCanOpenParameters& params);
	inline int _uninitialize (void) { return _closeAndRelease(); }
	inline int _closeAndRelease (void);

	EsdCanResources (void)
	{
		_handle = ACE_INVALID_HANDLE;
		_timeout = ESD_TIMEOUT;
		_polling_interval = ESD_POLLING_INTERVAL;
		_speed = 0;						/// default 1Mbit/s
		_networkN = 0;
		_msg_lost = 0;

		_cur_packetsize = 0;
		_error_status = YARP_OK;
		_my_address = 0;
		memset (_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);

		_txQueueSize = BUF_SIZE;
		_rxQueueSize = BUF_SIZE;
		_txTimeout = _timeout;
		_rxTimeout = _timeout;
	
		_njoints = 0;
		_p = NULL;
		_filter = -1;
	}

	~EsdCanResources () { _uninitialize(); }

	int _startPacket (void);
	int _addMessage (int msg, int axis);
	int _writePacket (void);

	int _read (void);
	int _read_np (int& err);
	int _write (void);
	bool _error (const CMSG& m);
	
	void _getErrorsAndPrint (void);

	inline bool MYSELF(unsigned char x) { return (((x & 0xf0) >> 4) == _my_address) ? true : false; }
	inline bool _prepareHeader (CMSG& msg, int msg_type, int src, int dest, int channel);
};


/// blocking read.
int EsdCanResources::_read (void)
{
	long messages = BUF_SIZE;

	int res = canRead (_handle, _canMsg, &messages, NULL); 
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	return messages;
}

/// read, no polling.
int EsdCanResources::_read_np (int& err)
{
	long messages = BUF_SIZE;

	int res = canTake (_handle, _canMsg, &messages); 
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	return messages;
}

/// reads the error queue and print to console (if open).
void EsdCanResources::_getErrorsAndPrint (void)
{
}

/// write to can.
int EsdCanResources::_write (void)
{
	long len = 1;
	int res = canSend (_handle, _cmdBuffer, &len);
	if (res != NTCAN_SUCCESS || len != 1)
		return YARP_FAIL;

	return YARP_OK;
}

/// initializes the construction of a new packet (array) of messages.
int EsdCanResources::_startPacket (void)
{
	_cur_packetsize = 0;
	return YARP_OK;
}

/// adds a new message to the current packet.
int EsdCanResources::_addMessage (int msg, int axis)
{
	memset (&_cmdBuffer[_cur_packetsize], 0, sizeof(CMSG));
	_prepareHeader (_cmdBuffer[_cur_packetsize], msg, _my_address, _destinations[axis/2], axis % 2);
	_cmdBuffer[_cur_packetsize].len = 1;
	_cur_packetsize ++;
	return _cur_packetsize;
}

/// sends the current packet.
int EsdCanResources::_writePacket (void)
{
	if (_cur_packetsize < 1)
		return YARP_FAIL;

	long len = _cur_packetsize;

	int res = canSend (_handle, _cmdBuffer, &len);
	if (res != NTCAN_SUCCESS || len != _cur_packetsize)
		return YARP_FAIL;

	return YARP_OK;
}

/// test message for errors.
bool EsdCanResources::_error (const CMSG& m)
{
	if (m.len & NTCAN_NO_DATA) return true;
	if (m.msg_lost != 0) { _msg_lost = m.msg_lost; return true; }

	return false;
}

bool EsdCanResources::_prepareHeader (CMSG& msg, int msg_type, int src, int dest, int channel)
{
	msg.id = (src << 4) + (dest);
	msg.data[0] = (channel << 7) + (msg_type & 0x7f);
	return true;
}

///
int EsdCanResources::_closeAndRelease (void)
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

int EsdCanResources::_initialize (const EsdCanOpenParameters& params)
{
	/// general variable init.
	_handle = ACE_INVALID_HANDLE;
	_timeout = ESD_TIMEOUT;
	_polling_interval = ESD_POLLING_INTERVAL;
	_speed = 0;				/// default 1Mbit/s
	_networkN = 0;

	_cur_packetsize = 0;
	_error_status = YARP_OK;
	_my_address = 0;
	memset (_destinations, 0, sizeof(unsigned char) * ESD_MAX_CARDS);

	_txQueueSize = params._txQueueSize;
	_rxQueueSize = params._rxQueueSize;
	_txTimeout = params._txTimeout;
	_rxTimeout = params._rxTimeout;

	_njoints = 0;
	_p = NULL;
	_filter = -1;

	int res = canOpen (_networkN, 0, _txQueueSize, _rxQueueSize, _txTimeout, _rxTimeout, &_handle);
	if (res != NTCAN_SUCCESS)
		return YARP_FAIL;

	res = canSetBaudrate (_handle, _speed);
	if (res != NTCAN_SUCCESS)
	{
		canClose (_handle);
		return YARP_FAIL;
	}

	/// sets all message ID's.
	int i;
	for (i = 0; i < 0xff; i++)
		canIdAdd (_handle, i);
	
	//canIdAdd (_handle, 0x1bc);
	//canIdAdd (_handle, 0x1cb);

	_msg_lost = 0;

	_polling_interval = params._polling_interval;
	_my_address = params._my_address;
	memcpy (_destinations, params._destinations, sizeof(unsigned char) * ESD_MAX_CARDS);
	_timeout = params._timeout;
	_njoints = params._njoints;

	_p = params._p;
	_filter = -1;

	return YARP_OK;
}


///
/// actual device driver class.
///
inline EsdCanResources& RES(void *res) { return *(EsdCanResources *)res; }

YARPEsdCanDeviceDriver::YARPEsdCanDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPEsdCanDeviceDriver>(CBNCmds), _mutex(1), _pending(1)
{
	system_resources = (void *) new EsdCanResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[CMDGetPosition] = &YARPEsdCanDeviceDriver::getPosition;
	m_cmds[CMDGetPositions] = &YARPEsdCanDeviceDriver::getPositions;
	m_cmds[CMDGetRefPosition] = &YARPEsdCanDeviceDriver::getRefPosition;
	m_cmds[CMDGetRefPositions] = &YARPEsdCanDeviceDriver::getRefPositions;
	m_cmds[CMDSetPosition] = &YARPEsdCanDeviceDriver::setPosition;
	m_cmds[CMDSetPositions] = &YARPEsdCanDeviceDriver::setPositions;
	m_cmds[CMDGetPIDError] = &YARPEsdCanDeviceDriver::getError;

	m_cmds[CMDSetSpeed] = &YARPEsdCanDeviceDriver::setSpeed;
	m_cmds[CMDSetSpeeds] = &YARPEsdCanDeviceDriver::setSpeeds;
	m_cmds[CMDGetSpeeds] = &YARPEsdCanDeviceDriver::getSpeeds;
	m_cmds[CMDGetRefSpeeds] = &YARPEsdCanDeviceDriver::getRefSpeeds;
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
	m_cmds[CMDSafeVMove] = &YARPEsdCanDeviceDriver::velocityMove;

	m_cmds[CMDSetCommand] = &YARPEsdCanDeviceDriver::setCommand;
	m_cmds[CMDSetCommands] = &YARPEsdCanDeviceDriver::setCommands;
	m_cmds[CMDCheckMotionDone] = &YARPEsdCanDeviceDriver::checkMotionDone;

	m_cmds[CMDGetTorque] = &YARPEsdCanDeviceDriver::getTorque;
	m_cmds[CMDGetTorques] = &YARPEsdCanDeviceDriver::getTorques;
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

int YARPEsdCanDeviceDriver::open (void *res)
{
	_mutex.Wait(); // the thread will post this mutex after startup.
	_ev.Reset();

	EsdCanResources& d = RES(system_resources);
	int ret = d._initialize (*(EsdCanOpenParameters *)res);

	_p = ((EsdCanOpenParameters *)res)->_p;
	_filter = -1;

	_ref_positions = new double [d._njoints];		
	_ref_speeds = new double [d._njoints];
	_ref_accs = new double [d._njoints];
	ACE_ASSERT (_ref_positions != NULL && _ref_speeds != NULL && _ref_accs != NULL);

	if (ret >= 0)
	{
		Begin ();
		YARPScheduler::yield();
		//YARPTime::DelayInSeconds (.2);
	}
	else
		_mutex.Post();

	return ret;
}

int YARPEsdCanDeviceDriver::close (void)
{
	EsdCanResources& d = RES(system_resources);

	End ();	/// stops the thread first (joins too).

	if (_ref_positions != NULL) delete[] _ref_positions;
	if (_ref_speeds != NULL) delete[] _ref_speeds;
	if (_ref_accs != NULL) delete[] _ref_accs;

	int ret = d._uninitialize ();

	return ret;
}

///
///
/// decode messages from CAN to device driver protocol.
void YARPEsdCanDeviceDriver::_debugMsg (int n, void *msg, int (*p) (char *fmt, ...))
{
	CMSG *m = (CMSG *)msg;
	int i;

	if (p)
	{
		//(*p) ("nmsg: %d\n", n);
		/// minimal debug, actual debug would require interpreting messages.
		for (i = 0; i < n; i++)
		{
			EsdCanResources& r = RES(system_resources);
			if ((m[i].data[0] & 0x7f) != r._filter &&
				(m[i].id & 0x0f) == r._my_address)
			{
				(*p) 
					("class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
					  (m[i].id & 0x700) >> 8,
					  (m[i].id & 0xf0) >> 4, 
					  (m[i].id & 0x0f), 
					  ((m[i].data[0] & 0x80)==0)?0:1,
					  (m[i].data[0] & 0x7f),
					  (m[i].data[0] & 0x7f));

				if (m[i].len > 1)
				{
					(*p)("x: "); 
				}

				int j;
				for (j = 1; j < m[i].len; j++)
				{
					(*p)("%x ", m[i].data[j]);
				}

				(*p)("id: %x\n", m[i].id);
			}
		}
	}
}

///
///
///
void YARPEsdCanDeviceDriver::_printMessage (void *msg, int (*p) (char *fmt, ...))
{
	CMSG m = *(CMSG *)msg;
	EsdCanResources& r = RES(system_resources);

//	if ((m.Data[0] & 0x7f) != r._filter &&
//		(m.ArbIDOrHeader & 0x0f) == r._my_address)
	{
		(*p) 
			("class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
			  (m.id & 0x700) >> 8,
			  (m.id & 0xf0) >> 4, 
			  (m.id & 0x0f), 
			  ((m.data[0] & 0x80)==0)?0:1,
			  (m.data[0] & 0x7f),
			  (m.data[0] & 0x7f));

		if (m.len > 1)
		{
			(*p)("x: "); 
		}

		int j;
		for (j = 1; j < m.len; j++)
		{
			(*p)("%x ", m.data[j]);
		}

		(*p)("id: %x\n", m.id);
	}
}

///
///
/// OS scheduler should possibly go into hi-res mode (winnt).
/// make sure only 1 message packed at a time is processed.
///
void YARPEsdCanDeviceDriver::Body(void)
{
	EsdCanResources& r = RES(system_resources);
	double now = -1, before = -1;
	int cyclecount = 0;
	int left = 0, own_messages = 0;
	bool done = false;

	_request = false;
	_noreply = false;

	_mutex.Post();	// was decremented by the open().

	while (!IsTerminated() || left >= 1)
	{
		before = YARPTime::GetTimeAsSeconds();

		/// reads all messages.
		int err = 0;
		int n = 0;
		
		n = r._read_np (err);

		if (err != 0)
		{
			if (r._p) (*r._p)("got %d errors\n", err);
			r._getErrorsAndPrint ();
		}

		/// debug/print messages to console.
		if (n > 0 && r._p)
			_debugMsg (n, (void *)r._canMsg, r._p);

		/// filters messages, no buffering allowed here.
		if (left > 0)
		{
			int i;
			for (i = 0; i < n; i++)
			{
				CMSG& m = r._canMsg[i];

				if (r._error(m))
				{
					if (r._p) 
					{
						(*r._p)("CAN: troubles w/ message %x\n", 0);
						_printMessage ((void *)&m, r._p);
						(*r._p)("CAN: skipped a message because of errors\n");
						r._getErrorsAndPrint ();
					}

					continue;		/// skip this message.
				}


				/// can't get own messages :(
				_mutex.Wait();
				if (_noreply)
				{
					_noreply = false;
					left = 0;
					own_messages = 0;
					r._error_status = YARP_OK;
					_mutex.Post();
					_ev.Signal();
					done = true;
				}
				else
					_mutex.Post();


				/// check whether the message is a reply of any of the pending messages
				/// in current packet.
				if ((m.id & 0x0f) == r._my_address)
				{
					int j;
					for (j = 0; j < r._cur_packetsize; j++)
					{
						if (r._cmdBuffer[j].data[0] == m.data[0] &&		// same message type.
							(m.id & 0x700) == 0 &&				// class 0.
							((m.id & 0xf0) >> 4) == (r._cmdBuffer[j].id & 0x0f))
						{
							memcpy (&r._replyBuffer[j], &m, sizeof(CMSG));
							left --;
							if (left < 1)
							{
								r._error_status = YARP_OK;
								///_mutex.Post(); THE BUG!
								_ev.Signal();
								done = true;
							}
						}
					}
				}
			}

			/// requires a timeout (to signal back the caller).
			/// if doesn't get a reply then signal a problem.
			cyclecount++;
			if (cyclecount >= r._timeout)
			{
				_mutex.Wait();
				
				if (r._p) 
				{
					(*r._p)("CAN: still %d unacknoweledged messages\n", left);
					(*r._p)("CAN: still %d unsent messages\n", own_messages);
					(*r._p)("CAN: packet of %d \n", r._cur_packetsize);
				}
				
				int j;
				for (j = 0; j < r._cur_packetsize; j++)
				{
					if (r._replyBuffer[j].data[0] == CAN_NO_MESSAGE && r._p)
					{
						(*r._p)("CAN: board %d timed out [msg type %d channel %d]\n", 
								r._cmdBuffer[j].id & 0x0f, 
								r._cmdBuffer[j].data[0] & 0x7f,
								(r._cmdBuffer[j].data[0] & 0x80)?1:0);
					}
				}

				r._getErrorsAndPrint ();

				r._error_status = YARP_FAIL;
				left = 0;
				cyclecount = 0;
				_mutex.Post();
				_ev.Signal();
				done = true;
			}
		}

		/// completed prev message packet.
		if (done)
		{
			done = false;
			_pending.Post();	/// a new write is allowed.
		}

		_mutex.Wait();
		
		/// copies buffered values.
		r._p = _p;
		r._filter = _filter;

		if (_request && left < 1)
		{
			if (r._writePacket () != YARP_OK)
			{
				if (r._p) (*r._p)("problems sending message\n"); 
			}

			memset (r._replyBuffer, 0, sizeof(CMSG) * r._cur_packetsize);
			own_messages = left = r._cur_packetsize;
			r._error_status = YARP_OK;
			cyclecount = 0;
			_request = false;
		}

		_mutex.Post();

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
	_p = (int (*) (char *fmt, ...))cmd;
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
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;
	int i, value = 0;

	_pending.Wait();

	_mutex.Wait();
	r._startPacket();

	for (i = 0; i < r._njoints; i++)
	{
		if (ENABLED(i))
		{
			r._addMessage (CAN_GET_ENCODER_POSITION, i);
		}
		else
			tmp[i] = 0;
	}
	
	_request = true;
	_noreply = false;
	_mutex.Post();

	_ev.Wait();

	if (r._error_status != YARP_OK)
	{
		memset (tmp, 0, sizeof(double) * r._njoints);
		return YARP_FAIL;
	}

	int j = 0;
	for (i = 0; i < r._njoints; i++)
	{
		if (ENABLED(i))
		{
			tmp[i] = *((int *)(r._replyBuffer[j].data+1));
			j++;
		}
	}

	return YARP_OK;
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

	_pending.Wait();

	_mutex.Wait();
	r._startPacket();

	for (i = 0; i < r._njoints; i++)
	{
		if (ENABLED(i))
		{
			SingleAxisParameters x;
			x.axis = i;
			x.parameters = tmp+i;	

			r._addMessage (CAN_POSITION_MOVE, i);

			_ref_positions[i] = tmp[i];
			*((int*)(r._cmdBuffer[i].data+1)) = S_32(_ref_positions[i]);		/// pos
			*((short*)(r._cmdBuffer[i].data+5)) = S_16(_ref_speeds[i]);			/// speed
			r._cmdBuffer[i].len = 7;
		}
		else
		{
			_ref_positions[i] = tmp[i];
		}
	}

	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

	return YARP_OK;
}

inline bool YARPEsdCanDeviceDriver::ENABLED (int axis)
{
	EsdCanResources& r = RES(system_resources);
	if (r._destinations[axis/2] & ESD_CAN_SKIP_ADDR)
		return false;

	return true;
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

	_pending.Wait();

	_mutex.Wait();

	r._startPacket();
	r._addMessage (CAN_POSITION_MOVE, axis);

	_ref_positions[axis] = *((double *)tmp->parameters);
	*((int*)(r._cmdBuffer[0].data+1)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r._cmdBuffer[0].data+5)) = S_16(_ref_speeds[axis]);			/// speed
	r._cmdBuffer[0].len = 7;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();

	_mutex.Wait();
	r._startPacket();

	for (i = 0; i < r._njoints; i++)
	{
		if (ENABLED (i))
		{
			r._addMessage (CAN_VELOCITY_MOVE, i);
			_ref_speeds[i] = tmp[i];
			*((short*)(r._cmdBuffer[i].data+1)) = S_16(_ref_speeds[i]);		/// speed
			*((short*)(r._cmdBuffer[i].data+3)) = S_16(_ref_accs[i]);		/// accel
			r._cmdBuffer[i].len = 5;
		}
		else
		{
			_ref_speeds[i] = tmp[i];
		}
	}

	_request = true;
	_noreply = true;

	_mutex.Post();

	_ev.Wait();

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
	for (i = 0; i < r._njoints; i++)
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

/// cmd is an array of double
int YARPEsdCanDeviceDriver::setCommands (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
	{
		if (_writeDWord (CAN_SET_COMMAND_POSITION, i, S_32(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::getRefPositions (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i, value = 0;

	for(i = 0; i < r._njoints; i++)
	{
		if (_readDWord (CAN_GET_DESIRED_POSITION, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
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
	const short s = S_16(_ref_speeds[axis]);
	return _writeWord16 (CAN_SET_DESIRED_VELOCITY, axis, s);
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::setSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
	{
		_ref_speeds[i] = tmp[i];
		if (_writeWord16 (CAN_SET_DESIRED_VELOCITY, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::getSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r._njoints; i++)
	{
		if (_readWord16 (CAN_GET_ENCODER_VELOCITY, i, value) == YARP_OK)
			out[i] = double (value);
		else
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::getRefSpeeds (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r._njoints; i++)
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

/// cmd is an array of double
int YARPEsdCanDeviceDriver::setAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
	{
		_ref_accs[i] = tmp[i];
		if (_writeWord16 (CAN_SET_DESIRED_ACCELER, i, S_16(_ref_accs[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is an array of double
int YARPEsdCanDeviceDriver::getRefAccelerations (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r._njoints; i++)
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

/// cmd is an array of double
int YARPEsdCanDeviceDriver::setOffsets (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
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

/// cmd is an array of double 
int YARPEsdCanDeviceDriver::setIntegratorLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
	{
		if (_writeWord16 (CAN_SET_ILIM_GAIN, i, S_16(tmp[i])) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
}

/// cmd is a SingleAxis pointer with a LowLevelPID argument pointer
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
int YARPEsdCanDeviceDriver::getTorques (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r._njoints; i++)
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
int YARPEsdCanDeviceDriver::getError (void *cmd)
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
int YARPEsdCanDeviceDriver::setTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *tmp = (double *)cmd;

	int i;
	for (i = 0; i < r._njoints; i++)
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
int YARPEsdCanDeviceDriver::getTorqueLimits (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	double *out = (double *) cmd;
	int i;
	short value = 0;

	for(i = 0; i < r._njoints; i++)
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

/// cmd is a pointer to a bool
int YARPEsdCanDeviceDriver::checkMotionDone (void *cmd)
{
	EsdCanResources& r = RES(system_resources);
	int i;
	short value;
	bool *out = (bool *) cmd;

	for(i = 0; i < r._njoints; i++)
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
int YARPEsdCanDeviceDriver::_writeNone (int msg, int axis)
{
	EsdCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (ESD_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		return YARP_OK;
	}

	_pending.Wait();

	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);

	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();

	/// prepare Can message.
	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	/// reads back position info.
	_ev.Wait();

	if (r._error_status != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((short *)(r._replyBuffer[0].data+1));
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

	_pending.Wait();
	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);

	*((short *)(r._cmdBuffer[0].data+1)) = s;
	r._cmdBuffer[0].len = 3;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);

	*((int*)(r._cmdBuffer[0].data+1)) = value;
	r._cmdBuffer[0].len = 5;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);

	*((short *)(r._cmdBuffer[0].data+1)) = s1;
	*((short *)(r._cmdBuffer[0].data+3)) = s2;
	r._cmdBuffer[0].len = 5;

	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	/// prepare can packet of length 1.
	_mutex.Wait();

	r._startPacket();
	r._addMessage (msg, axis);
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	_ev.Wait();

	if (r._error_status != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((int *)(r._replyBuffer[0].data+1));
	return YARP_OK;
}
