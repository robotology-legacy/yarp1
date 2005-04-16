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
/// $Id: YARPValueCanDeviceDriver.cpp,v 1.20 2005-04-16 01:43:58 babybot Exp $
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


/// Message property.
#define TX_MESSAGE 1<<1
#define EXTENDED_FRAME 1<<2
#define REMOTE_FRAME 1<<3

/// Message errors.
#define INCOMPLETE_FRAME_ERR 1<<6
#define UNDEF_ERR 1<<8
#define CAN_BUS_OFF_ERR 1<<9
#define HARDWARE_ERR 1<<16
#define FRAME_LENGHT_ERR 1<<17

///
/// LATER: to be verified according to the value can manual.
///
const int MAX_CHANNELS	= 255;
const int MAX_NID		= 16;
const int BUF_SIZE		= 20000;
const int VALUE_CAN_SKIP_ADDR = 0x80;


typedef int (*PV) (char *fmt, ...);

///
///
/// note that this class is just a container.
class ValueCanResources
{
protected:
	int _iDeviceTypes[MAX_CHANNELS];
	int _iComPort[MAX_CHANNELS];
	int _iSerialNum[MAX_CHANNELS];
	int _iNumDevices;							/// number of valuecan devices.

	unsigned char _bNetworkID[MAX_NID];
	bool _dllLoaded;
	
	int _portHandle;
	int _polling_interval;
	int _timeout;

	unsigned char _my_address;					/// 
	unsigned char _destinations[VALUE_MAX_CARDS];		/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).
	
	icsSpyMessage _canMsg[BUF_SIZE];			/// read buffer.
	int _last_read;								/// number of messages read.

	icsSpyMessage _cmdBuffer[BUF_SIZE];			/// write buffer.
	icsSpyMessage _replyBuffer[BUF_SIZE];		/// .
	
	int _cur_packetsize;						/// size of the current packet.
	int _error_status;							/// error status of the last packet.

	PV _p;										///	pointer to a printf type function
												/// used to spy on can messages.
	int _filter;								/// don't print filtered messages.

public:
	ValueCanResources (void)
	{
		_iNumDevices = 0;
		memset (_iSerialNum, 0, MAX_CHANNELS * sizeof(int));
		memset (_iComPort, 0, MAX_CHANNELS * sizeof(int));
		memset (_iDeviceTypes, 0, MAX_CHANNELS * sizeof(int));
		memset (_bNetworkID, 0, MAX_NID * sizeof(int));
		
		memset (_canMsg, 0, sizeof(icsSpyMessage) * BUF_SIZE);
		memset (_cmdBuffer, 0, sizeof(icsSpyMessage) * BUF_SIZE);
		memset (_replyBuffer, 0, sizeof(icsSpyMessage) * BUF_SIZE);
		_cur_packetsize = 0;
		_error_status = YARP_OK;

		_dllLoaded = false;
		_portHandle = 0;
		_my_address = 0;
		memset (_destinations, 0, sizeof(unsigned char) * VALUE_MAX_CARDS);
		_polling_interval = 5;
		_timeout = 1;
		_njoints = 0;
		_p = NULL;
		_filter = -1;
		_last_read = 0;
	}

	~ValueCanResources () { uninitialize(); }

	int initialize (const ValueCanOpenParameters& params);
	inline int uninitialize (void) { return closeAndRelease(); }
	inline int closeAndRelease (void);

	inline int getJoints (void) const { return _njoints; }
	inline int getPollingInterval (void) const { return _polling_interval; }

	inline bool filter (icsSpyMessage& m) const
	{
		if ((m.Data[0] & 0x7f) != _filter &&
			(m.ArbIDOrHeader & 0x0f) == _my_address)
			return true;
		else
			return false;
	}

	
	int startPacket (void);
	int addMessage (int msg, int axis);
	inline int getPacketSize (void) const { return _cur_packetsize; }

	int writePacket (void);
	int write (void);

	int read (void);
	int nonPollingRead (int& err);
	
	inline icsSpyMessage getMessage (int i)
	{
		if (i >= 0 && _last_read > 0 && i < BUF_SIZE)
			return _canMsg[i];
		else
		{
			icsSpyMessage x;
			memset (&x, 0, sizeof(icsSpyMessage));
			x.ArbIDOrHeader = 0xffff;
			return x;
		}
	}

	inline icsSpyMessage getReplyMessage (int i)
	{
		if (i >= 0 && _cur_packetsize > 0 && i < _cur_packetsize)
			return _replyBuffer[i];
		else
		{
			icsSpyMessage x;
			memset (&x, 0, sizeof(icsSpyMessage));
			x.ArbIDOrHeader = 0xffff;
			return x;
		}
	}

	inline icsSpyMessage& getCmdBuffer (int i)
	{
		ACE_ASSERT (i >= 0 && i < _cur_packetsize);
		return _cmdBuffer[i];
	}

	bool isReply (icsSpyMessage& m);
	inline int clearReplyBuffer (void) 
	{ 
		memset (_replyBuffer, 0, sizeof(icsSpyMessage) * _cur_packetsize);
		return YARP_OK;
	}

	bool error (const icsSpyMessage& m);
	bool timeout (int count)
	{
		if (count >= _timeout)
			return true;
		else
			return false;
	}

	bool isEnabled (int axis) const 
	{
		if (_destinations[axis/2] & VALUE_CAN_SKIP_ADDR)
			return false;
		else
			return true;
	}

	inline void clearError (void) { _error_status = YARP_OK; }
	inline void setError (void) { _error_status = YARP_FAIL; } 
	inline int getErrorStatus (void) const { return _error_status; }

	int getErrorsAndPrint (void);
	int printMessage (icsSpyMessage& m);
	int printTimeout (void);
	int debugMsg (void);
	inline int setPrintFunction (PV p) { _p = p; return YARP_OK; }
	inline int setMessageFilter (int m) { _filter = m; return YARP_OK; }

	PV p (void) { return _p; }

	//inline bool MYSELF(unsigned char x) { return (((x & 0xf0) >> 4) == _my_address) ? true : false; }
	inline bool prepareHeader (icsSpyMessage& msg, int msg_type, int src, int dest, int channel);
};


///
///
int ValueCanResources::printTimeout (void)
{
	int j;
	for (j = 0; j < _cur_packetsize; j++)
	{
		if (_replyBuffer[j].Data[0] == CAN_NO_MESSAGE && _p)
		{
			(*_p)("CAN: board %d timed out [msg type %d channel %d]\n", 
					_cmdBuffer[j].ArbIDOrHeader & 0x0f, 
					_cmdBuffer[j].Data[0] & 0x7f,
					(_cmdBuffer[j].Data[0] & 0x80)?1:0);
		}
	}

	return YARP_OK;
}

///
///
bool ValueCanResources::isReply (icsSpyMessage& m)
{
	if ((m.ArbIDOrHeader & 0x0f) == _my_address)
	{
		int j;
		for (j = 0; j < _cur_packetsize; j++)
		{
			if (_cmdBuffer[j].Data[0] == m.Data[0] &&		// same message type.
				!(m.ArbIDOrHeader & 0x700) &&				// class 0.
				((m.ArbIDOrHeader & 0xf0) >> 4) == (_cmdBuffer[j].ArbIDOrHeader & 0x0f))
			{
				memcpy (&_replyBuffer[j], &m, sizeof(icsSpyMessage));
				return true;
			}
		}
	}

	return false;
}

int ValueCanResources::printMessage (icsSpyMessage& m)
{
	(*_p) 
		("class: %2d s: %2x d: %2x c: %1d msg: %3d (%x) ",
		  (m.ArbIDOrHeader & 0x700) >> 8,
		  (m.ArbIDOrHeader & 0xf0) >> 4, 
		  (m.ArbIDOrHeader & 0x0f), 
		  ((m.Data[0] & 0x80)==0)?0:1,
		  (m.Data[0] & 0x7f),
		  (m.Data[0] & 0x7f));

	if (m.NumberBytesData > 1)
	{
		(*_p)("x: "); 
	}

	int j;
	for (j = 1; j < m.NumberBytesData; j++)
	{
		(*_p)("%x ", m.Data[j]);
	}

	(*_p)("id: %x\n", m.ArbIDOrHeader);

	return YARP_OK;
}


/// polling with busy wait. Bad!
int ValueCanResources::read (void)
{
	int messages = 0;
	int errors = 0;

	do 
	{
		int res = icsneoGetMessages (_portHandle, _canMsg, &messages, &errors);
		if (res == 0)
			return errors;
	} 
	while (messages == 0);

	return messages;
}

///
///
/// decode messages from CAN to device driver protocol.
int ValueCanResources::debugMsg (void)
{
	int i;

	if (_p)
	{
		for (i = 0; i < _last_read; i++)
		{
			if (filter (_canMsg[i]))
			{
				printMessage(_canMsg[i]);
			}
		}
	}

	return YARP_OK;
}


/// reads the error queue and print to console (if open).
int ValueCanResources::getErrorsAndPrint (void)
{
	int errors;
	int errorMsgs[600];	/// 600 is a driver limit.

	if (icsneoGetErrorMessages(_portHandle, errorMsgs, &errors) != 0)
	{
		if (_p)
		{
			int i;
			for (i = 0; i < errors; i++)
			{
				char msg_short[256];
				char msg_long[256];
				int len_s = 0;
				int len_l = 0;
				int restart = 0;
				int severity = 0;

				if (icsneoGetErrorInfo (errorMsgs[i], 
										msg_short,
										msg_long,
										&len_s,
										&len_l,
										&severity,
										&restart) != 0)
				{
					(*_p) ("r: %d e%d: %s\n", restart, i, msg_short);
				}
				else
				{
					(*_p) ("CAN: can't get the error description %x\n", errorMsgs[i]);
				}
			}
		}
	}
	else
	{
		(*_p) ("CAN: can't get the error queue\n");
		return YARP_FAIL;
	}

	return YARP_OK;
}

/// read, no polling.
int ValueCanResources::nonPollingRead (int& err)
{
	int messages = 0;
	int errors = 0;

	int res = icsneoGetMessages (_portHandle, _canMsg, &messages, &errors);
	err = errors;
	_last_read = messages;

	return messages;
}

/// write to can.
int ValueCanResources::write (void)
{
	return icsneoTxMessages (_portHandle, &_cmdBuffer[0], NETID_HSCAN, 1);
}

/// initializes the construction of a new packet (array) of messages.
int ValueCanResources::startPacket (void)
{
	_cur_packetsize = 0;
	return YARP_OK;
}

/// adds a new message to the current packet.
int ValueCanResources::addMessage (int msg, int axis)
{
	memset (&_cmdBuffer[_cur_packetsize], 0, sizeof(icsSpyMessage));
	prepareHeader (_cmdBuffer[_cur_packetsize], msg, _my_address, _destinations[axis/2], axis % 2);
	_cmdBuffer[_cur_packetsize].NumberBytesData = 1;
	_cur_packetsize ++;
	return _cur_packetsize;
}

/// sends the current packet.
int ValueCanResources::writePacket (void)
{
	if (_cur_packetsize < 1)
		return YARP_FAIL;

	int i;
	for (i = 0; i < _cur_packetsize; i++)
	{
		if (icsneoTxMessages (_portHandle, _cmdBuffer+i, NETID_HSCAN, 1) == 0)
			return YARP_FAIL;
	}
	
	return YARP_OK;
}

/// test message for errors.
bool ValueCanResources::error (const icsSpyMessage& m)
{
	if (m.StatusBitField & INCOMPLETE_FRAME_ERR) return true;
	if (m.StatusBitField & UNDEF_ERR) return true;
	if (m.StatusBitField & CAN_BUS_OFF_ERR) return true;
	if (m.StatusBitField & HARDWARE_ERR) return true;
	if (m.StatusBitField & FRAME_LENGHT_ERR) return true;

	return false;
}

bool ValueCanResources::prepareHeader (icsSpyMessage& msg, int msg_type, int src, int dest, int channel)
{
	msg.ArbIDOrHeader = (src << 4) + (dest);
	msg.Data[0] = (channel << 7) + (msg_type & 0x7f);
	return true;
}

///
int ValueCanResources::closeAndRelease (void)
{
	if (_dllLoaded)
	{
		int nOfErrors = 0;
		if (_portHandle)
		{
			icsneoClosePort (_portHandle, &nOfErrors);
			icsneoFreeObject (_portHandle);
			_portHandle = 0;
		}

		_dllLoaded = false;
	
		return nOfErrors;
	}
	
	return 0;
}

int ValueCanResources::initialize (const ValueCanOpenParameters& params)
{
	if (_dllLoaded)
	{
		ACE_DEBUG ((LM_DEBUG, "CAN: DLL lib seems to be already in memory\n"));
		return -1;
	}

	/// general variable init.
	_iNumDevices = 0;
	
	memset (_iSerialNum, 0, MAX_CHANNELS * sizeof(int));
	memset (_iComPort, 0, MAX_CHANNELS * sizeof(int));
	memset (_iDeviceTypes, 0, MAX_CHANNELS * sizeof(int));
	memset (_bNetworkID, 0, MAX_NID * sizeof(int));
	
	memset (_canMsg, 0, sizeof(icsSpyMessage) * BUF_SIZE);
	memset (_cmdBuffer, 0, sizeof(icsSpyMessage) * BUF_SIZE);
	memset (_replyBuffer, 0, sizeof(icsSpyMessage) * BUF_SIZE);
	
	_cur_packetsize = 0;
	_error_status = YARP_OK;

	_portHandle = 0;
	_my_address = 0;
	memset (_destinations, 0, sizeof(unsigned char) * VALUE_MAX_CARDS);
	_polling_interval = 5;
	_timeout = 1;
	_njoints = 0;
	_p = NULL;
	_filter = -1;
	_last_read = 0;

	/// actual library/resource initialization.
	_dllLoaded = true;

	if (icsneoFindAllCOMDevices (INTREPIDCS_DRIVER_STANDARD, 
				0, 1, 1, _iDeviceTypes, _iComPort, _iSerialNum, &_iNumDevices))
	{
		if (_iNumDevices > 0)
		{    
			int i;
			for (i = 0; i < MAX_NID; i++)
				_bNetworkID[i] = i;

			YARPTime::DelayInSeconds (.2);

			int res = icsneoOpenPortEx (
				_iComPort[params._port_number], 
				NEOVI_COMMTYPE_RS232, 
				INTREPIDCS_DRIVER_STANDARD, 
				0, 
				57600, ///115200,
				1, 
				_bNetworkID, 
				&_portHandle);

			if (res == 0)
			{
				_portHandle = 0;	// it failed, must be 0, otherwise it crashes.
				ACE_DEBUG ((LM_DEBUG, "CAN: Can't open port\n"));
				_dllLoaded = false;
				return -1;
			}
		}
		else
		{
			ACE_DEBUG ((LM_DEBUG, "CAN: No devices found!\n"));
			closeAndRelease ();
			return -1;
		}
	}
	else
	{
		ACE_DEBUG ((LM_DEBUG, "CAN: Can't communicate with CAN devices\n"));
		closeAndRelease ();
		return -1;
	}

	_polling_interval = params._polling_interval;
	_my_address = params._my_address;
	memcpy (_destinations, params._destinations, sizeof(unsigned char) * VALUE_MAX_CARDS);
	_timeout = params._timeout;
	_njoints = params._njoints;

	_p = params._p;
	_filter = -1;

	return 0;
}


///
/// actual device driver class.
///
inline ValueCanResources& RES(void *res) { return *(ValueCanResources *)res; }

YARPValueCanDeviceDriver::YARPValueCanDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPValueCanDeviceDriver>(CBNCmds), _mutex(1), _pending(1)
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

int YARPValueCanDeviceDriver::open (void *res)
{
	_mutex.Wait(); // the thread will post this mutex after startup.
	_ev.Reset();

	ValueCanResources& d = RES(system_resources);
	int ret = d.initialize (*(ValueCanOpenParameters *)res);

	_p = ((ValueCanOpenParameters *)res)->_p;
	_filter = -1;

	_ref_positions = new double [d.getJoints()];		
	_ref_speeds = new double [d.getJoints()];
	_ref_accs = new double [d.getJoints()];
	ACE_ASSERT (_ref_positions != NULL && _ref_speeds != NULL && _ref_accs != NULL);

	if (ret >= 0)
	{
		Begin ();
		//YARPScheduler::yield();
		YARPTime::DelayInSeconds (.2);
	}
	else
		_mutex.Post();

	return ret;
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
/// OS scheduler should possibly go into hi-res mode (winnt).
void YARPValueCanDeviceDriver::Body(void)
{
	ValueCanResources& r = RES(system_resources);
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
		
		n = r.nonPollingRead (err);
		
		if (err != 0)
		{
			if (r.p()) (*r.p())("got %d errors\n", err);
			r.getErrorsAndPrint ();
		}
		else
		{
			r.debugMsg ();
		}

		/// filters messages, no buffering allowed here.
		if (left > 0)
		{
			int i;
			for (i = 0; i < n; i++)
			{
				icsSpyMessage m = r.getMessage (i);
				if (m.ArbIDOrHeader == 0xffff)
				{
					if (r.p())
						(*r.p())("CAN: asked for a message out of range\n");
				}

				if (m.StatusBitField & SPY_STATUS_GLOBAL_ERR)
				{
					if (r.p()) 
					{
						(*r.p())("CAN: troubles w/ message %x\n", m.StatusBitField);
						r.printMessage (m);
						(*r.p())("CAN: skipped a message because of errors\n");
						r.getErrorsAndPrint ();
					}

					continue;		/// skip this message.
				}

				if (m.StatusBitField & SPY_STATUS_TX_MSG)
				{
					_mutex.Wait();
					own_messages --;

					/// my last sent message.
					if (_noreply && own_messages < 1)
					{
						_noreply = false;
						left = 0;
						r.clearError();
						_mutex.Post();
						_ev.Signal();
						done = true;
					}
					else
						_mutex.Post();

					continue;
				}

				/// check whether the message is a reply of any of the pending messages
				/// in current packet.
				if (r.isReply (m))
				{
					/// already in reply buffer.
					left --;
					if (left < 1)
					{
						r.clearError();
						_ev.Signal();
						done = true;
					}
				}
			}

			/// requires a timeout (to signal back the caller).
			/// if doesn't get a reply then signal a problem.
			cyclecount++;
			if (r.timeout(cyclecount))
			{
				_mutex.Wait();
				
				if (r.p()) 
				{
					(*r.p())("CAN: still %d unacknoweledged messages\n", left);
					(*r.p())("CAN: still %d unsent messages\n", own_messages);
				}
				
				r.printTimeout ();
				r.getErrorsAndPrint ();

				r.setError();
				left = 0;
				cyclecount = 0;
				_mutex.Post();
				_ev.Signal();
				done = true;
			}
		}

		_mutex.Wait();

		/// completed prev message packet.
		if (done)
		{
			done = false;
			_pending.Post();	/// a new write is allowed.
		}

		/// copies buffered values.
		r.setPrintFunction (_p);
		r.setMessageFilter (_filter);

		if (_request && left < 1)
		{
			if (r.writePacket () != YARP_OK)
			{
				if (r.p()) (*r.p())("problems sending message\n"); 
			}

			r.clearReplyBuffer ();
			own_messages = left = r.getPacketSize ();
			r.clearError ();
			cyclecount = 0;
			_request = false;
		}
		
		_mutex.Post();

		now = YARPTime::GetTimeAsSeconds();
		if ((now - before)*1000 < r.getPollingInterval())
		{
			YARPTime::DelayInSeconds(double(r.getPollingInterval())/1000.0-(now-before));
		}
		else 
		{
			if (r.p()) (*r.p())("CAN: thread can't poll fast enough (time: %f)\n", now-before);
		}
		before = now;
	}
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

	_pending.Wait();
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
	
	_request = true;
	_noreply = false;
	_mutex.Post();

	_ev.Wait();

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
			icsSpyMessage& m = r.getReplyMessage(j);
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

	_pending.Wait();
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
			*((int*)(r.getCmdBuffer(i).Data+1)) = S_32(_ref_positions[i]);		/// pos
			*((short*)(r.getCmdBuffer(i).Data+5)) = S_16(_ref_speeds[i]);			/// speed
			r.getCmdBuffer(i).NumberBytesData = 7;
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

inline bool YARPValueCanDeviceDriver::ENABLED (int axis)
{
	ValueCanResources& r = RES(system_resources);
	return r.isEnabled (axis);
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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (CAN_POSITION_MOVE, axis);

	_ref_positions[axis] = *((double *)tmp->parameters);
	*((int*)(r.getCmdBuffer(0).Data+1)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r.getCmdBuffer(0).Data+5)) = S_16(_ref_speeds[axis]);			/// speed
	r.getCmdBuffer(0).NumberBytesData = 7;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();
	r.startPacket();

	for (i = 0; i < r.getJoints(); i++)
	{
		if (ENABLED (i))
		{
			r.addMessage (CAN_VELOCITY_MOVE, i);
			_ref_speeds[i] = tmp[i];
			*((short*)(r.getCmdBuffer(i).Data+1)) = S_16(_ref_speeds[i]);		/// speed
			*((short*)(r.getCmdBuffer(i).Data+3)) = S_16(_ref_accs[i]);		/// accel
			r.getCmdBuffer(i).NumberBytesData = 5;
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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	/// reads back position info.
	_ev.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((short *)(r.getReplyMessage(0).Data+1));
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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r.getCmdBuffer(0).Data+1)) = s;
	r.getCmdBuffer(0).NumberBytesData = 3;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((int*)(r.getCmdBuffer(0).Data+1)) = value;
	r.getCmdBuffer(0).NumberBytesData = 5;
		
	_request = true;
	_noreply = true;
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

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

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);

	*((short *)(r.getCmdBuffer(0).Data+1)) = s1;
	*((short *)(r.getCmdBuffer(0).Data+3)) = s2;
	r.getCmdBuffer(0).NumberBytesData = 5;

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
int YARPValueCanDeviceDriver::_readDWord (int msg, int axis, int& value)
{
	ValueCanResources& r = RES(system_resources);
	ACE_ASSERT (axis >= 0 && axis <= (VALUE_MAX_CARDS-1)*2);

	if (!ENABLED(axis))
	{
		value = 0;
		return YARP_OK;
	}

	_pending.Wait();
	_mutex.Wait();

	r.startPacket();
	r.addMessage (msg, axis);
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	_ev.Wait();

	if (r.getErrorStatus() != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((int *)(r.getReplyMessage(0).Data+1));
	return YARP_OK;
}
