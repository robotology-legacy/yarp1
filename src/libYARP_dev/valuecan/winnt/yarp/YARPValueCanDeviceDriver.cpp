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
/// $Id: YARPValueCanDeviceDriver.cpp,v 1.3 2004-09-02 22:05:46 gmetta Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

/// specific to this device driver.
#include "YARPValueCanDeviceDriver.h"
#include "../dd_orig/include/mCan.h"

/// get the message types from the DSP code.
#define __ONLY_DEF
#include "../56f807/controller/Code/Controller.h"


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
const int BUF_SIZE		= 255;


class ValueCanResources
{
public:
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
	unsigned char _destinations[MAX_CARDS];		/// list of connected cards (and their addresses).
	int _njoints;								/// number of joints (ncards * 2).
	
	icsSpyMessage _canMsg[BUF_SIZE];			/// read buffer.
	icsSpyMessage _cmdBuffer;					/// write buffer (one element only).
	long _arbitrationID;

	int (*_p) (char *fmt, ...);					///	pointer to a printf type function
												/// used to spy on can messages.
	int _filter;								/// don't print filtered messages.

public:
	int _initialize (const ValueCanOpenParameters& params);
	inline int _uninitialize (void) { return _closeAndRelease(); }
	inline int _closeAndRelease (void);

	ValueCanResources (void)
	{
		_iNumDevices = 0;
		memset (_iSerialNum, 0, MAX_CHANNELS * sizeof(int));
		memset (_iComPort, 0, MAX_CHANNELS * sizeof(int));
		memset (_iDeviceTypes, 0, MAX_CHANNELS * sizeof(int));
		memset (_bNetworkID, 0, MAX_NID * sizeof(int));
		memset (_canMsg, 0, sizeof(icsSpyMessage) * BUF_SIZE);
		memset (&_cmdBuffer, 0, sizeof(icsSpyMessage));
		_dllLoaded = false;
		_portHandle = 0;
		_my_address = 0;
		memset (_destinations, 0, sizeof(unsigned char) * MAX_CARDS);
		_polling_interval = 5;
		_arbitrationID = 0;
		_timeout = 1;
		_njoints = 0;
		_p = NULL;
		_filter = -1;
	}

	~ValueCanResources () { _uninitialize(); }

	int _read (void);
	int _read_np (int& err);
	int _write (void);
	bool _error (const icsSpyMessage& m);

	inline bool MYSELF(unsigned char x) { return (((x & 0xf0) >> 4) == _my_address) ? true : false; }
};


/// polling with busy wait. Bad!
int ValueCanResources::_read (void)
{
	int messages = 0;
	int errors = 0;

	do 
	{
		int res = canGetMessages (_portHandle, _canMsg, &messages, &errors);
		if (res == 0)
			return errors;
	} 
	while (messages == 0);

	return messages;
}

/// read, no polling.
int ValueCanResources::_read_np (int& err)
{
	int messages = 0;
	int errors = 0;

	int res = canGetMessages (_portHandle, _canMsg, &messages, &errors);
	err = errors;

	return messages;
}

/// write to can.
int ValueCanResources::_write (void)
{
	return canPutMessages (_portHandle, &_cmdBuffer, NETID_HSCAN, 1);
}

/// test message for errors.
bool ValueCanResources::_error (const icsSpyMessage& m)
{
	if (m.StatusBitField & INCOMPLETE_FRAME_ERR) return true;
	if (m.StatusBitField & UNDEF_ERR) return true;
	if (m.StatusBitField & CAN_BUS_OFF_ERR) return true;
	if (m.StatusBitField & HARDWARE_ERR) return true;
	if (m.StatusBitField & FRAME_LENGHT_ERR) return true;

	return false;
}

///
int ValueCanResources::_closeAndRelease (void)
{
	if (_dllLoaded)
	{
		int nOfErrors = 0;
		if (_portHandle)
		{
			canClosePort (_portHandle, &nOfErrors);
			canFreeObject (_portHandle);
			_portHandle = 0;
		}

		canReleaseLibrary();

		_dllLoaded = false;
	
		return nOfErrors;
	}
	
	return 0;
}

int ValueCanResources::_initialize (const ValueCanOpenParameters& params)
{
	if (_dllLoaded)
	{
		ACE_DEBUG ((LM_DEBUG, "CAN: DLL lib seems to be already in memory\n"));
		return -1;
	}

	_dllLoaded = canInitLibrary();
	if (!_dllLoaded)
	{
		ACE_DEBUG ((LM_DEBUG, "CAN: Can't load the CAN DLL\n"));
		return -1;
	}

	if (canFindAllDevices (INTREPIDCS_DRIVER_STANDARD, 
				0, 1, 1, _iDeviceTypes, _iComPort, _iSerialNum, &_iNumDevices))
	{
		if (_iNumDevices > 0)
		{    
			int i;
			for (i = 0; i < MAX_NID; i++)
				_bNetworkID[i] = i;

			int res = canOpenPort (
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
				ACE_DEBUG ((LM_DEBUG, "CAN: Can't open port\n"));
				canReleaseLibrary ();
				return -1;
			}
		}
		else
		{
			ACE_DEBUG ((LM_DEBUG, "CAN: No devices found!\n"));
			_closeAndRelease ();
			return -1;
		}
	}
	else
	{
		ACE_DEBUG ((LM_DEBUG, "CAN: Can't communicate with CAN devices\n"));
		_closeAndRelease ();
		return -1;
	}

	/// reset buffers.
	memset (_canMsg, 0, sizeof(icsSpyMessage) * BUF_SIZE);
	memset (&_cmdBuffer, 0, sizeof(icsSpyMessage));

	_polling_interval = params._polling_interval;
	_my_address = params._my_address;
	memcpy (_destinations, params._destinations, sizeof(unsigned char) * MAX_CARDS);
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
	: YARPDeviceDriver<YARPNullSemaphore, YARPValueCanDeviceDriver>(CBNCmds), _mutex(1)
{
	system_resources = (void *) new ValueCanResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[CMDGetPosition] = &YARPValueCanDeviceDriver::getPosition;
	m_cmds[CMDGetPositions] = &YARPValueCanDeviceDriver::getPositions;
	m_cmds[CMDGetRefPositions] = &YARPValueCanDeviceDriver::getRefPositions;
	m_cmds[CMDSetPosition] = &YARPValueCanDeviceDriver::setPosition;
	m_cmds[CMDSetPositions] = &YARPValueCanDeviceDriver::setPositions;
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
	ValueCanResources& d = RES(system_resources);
	int ret = d._initialize (*(ValueCanOpenParameters *)res);

	_p = ((ValueCanOpenParameters *)res)->_p;
	_filter = -1;

	_ref_positions = new double [d._njoints];		
	_ref_speeds = new double [d._njoints];
	_ref_accs = new double [d._njoints];
	ACE_ASSERT (_ref_positions != NULL && _ref_speeds != NULL && _ref_accs != NULL);

	if (ret >= 0)
		Begin ();

	return ret;
}

int YARPValueCanDeviceDriver::close (void)
{
	ValueCanResources& d = RES(system_resources);

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
void YARPValueCanDeviceDriver::_debugMsg (int n, void *msg, int (*p) (char *fmt, ...))
{
	icsSpyMessage *m = (icsSpyMessage *)msg;
	int i;

	if (p)
	{
		/// minimal debug, actual debug would require interpreting messages.
		for (i = 0; i < n; i++)
		{
			ValueCanResources& r = RES(system_resources);
			if ((m[i].Data[1] & 0x7f) != r._filter)
			{
				(*p) 
					("s: %2x d: %2x c: %1d msg: %3d x: %x %x %x %x %x %x\n", 
					  (m[i].Data[0] & 0xf0) >> 4, 
					   m[i].Data[0] & 0x0f, 
					 ((m[i].Data[1] & 0x80)==0)?0:1,
					  (m[i].Data[1] & 0x7f),
					   m[i].Data[2],
					   m[i].Data[3],
					   m[i].Data[4],
					   m[i].Data[5],
					   m[i].Data[6],
					   m[i].Data[7]);
			}
		}
	}
}

///
///
/// OS scheduler should possibly go into hi-res mode (winnt).
void YARPValueCanDeviceDriver::Body(void)
{
	ValueCanResources& r = RES(system_resources);
	unsigned char messagetype = 0;
	bool pending = false;
	double now = -1, before = -1;
	int cyclecount = 0;

	_request = false;
	_noreply = false;

	while (!IsTerminated())
	{
		before = YARPTime::GetTimeAsSeconds();

		/// reads all messages.
		int err = 0;
		int n = r._read_np (err);

		if (err != 0)
			if (r._p) (*r._p)("got %d errors\n", err);

		/// debug/print messages to console.
		if (n > 0 && r._p)
			_debugMsg (n, (void *)r._canMsg, r._p);

		/// filters messages, no buffering allowed here.
		if (pending)
		{
			int i;
			for (i = 0; i < n; i++)
			{
				icsSpyMessage& m = r._canMsg[i];
				if (m.StatusBitField & SPY_STATUS_GLOBAL_ERR)
					if (r._p) (*r._p)("CAN: troubles w/ message %x\n", m.StatusBitField);

				if (r._error (m))
				{	
					if (r._p) (*r._p)("CAN: skipped a message for error\n");
					continue;		/// skip this message.
				}

				if (m.StatusBitField & SPY_STATUS_TX_MSG)
				{
					/// my last sent message.
					if (_noreply)
					{
						_noreply = false;
						pending = false;
						_ev.Signal();
					}
					continue;
				}

				/// check first to see whether <m> was addressed here.
				if (((m.Data[0] & 0x0f) == r._my_address) &&
					(m.Data[1] == messagetype))
				{
					/// ok, this is my reply.
					/// write reply and signal event.
					memcpy (&r._cmdBuffer, &m, sizeof(icsSpyMessage));
					pending = false;
					_ev.Signal();
				}
			}

			/// requires a timeout (to signal back the caller).
			/// if doesn't get a reply, signal a problem.
			cyclecount++;
			if (cyclecount >= r._timeout)
			{
				if (r._p) (*r._p)("CAN: board %d timed out [msg type %d channel %d]\n", (r._cmdBuffer.Data[0] & 0x0f), messagetype & 0x7f, (messagetype & 0x80)?1:0);
				r._cmdBuffer.Data[0] = 0;
				r._cmdBuffer.Data[1] = CAN_NO_MESSAGE;
				pending = false;
				_ev.Signal();
			}
		}
		/// else !pending, silently forget the messages...
		/// LATER: add callbacks here.

		_mutex.Wait();
		
		/// copies buffered values.
		r._p = _p;
		r._filter = _filter;

		if (_request && !pending)
		{
			r._write ();

			pending = true;
			messagetype = r._cmdBuffer.Data[1];
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
			if (r._p) (*r._p)("CAN: thread can't poll fast enough\n");
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

	for (i = 0; i < r._njoints; i++)
	{
		if (_readDWord (CAN_GET_ENCODER_POSITION, i, value) == YARP_OK)
		{
			tmp[i] = double(value);
		}
		else
		{
			ACE_OS::memset (tmp, 0, sizeof(double) * r._njoints); //tmp[i] = 0;
			return YARP_FAIL;
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);

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

	for (i = 0; i < r._njoints; i++)
	{
		SingleAxisParameters x;
		x.axis = i;
		x.parameters = tmp+i;	
		/// speed is contained in the next entry of tmp
		if (setPosition ((void *)&x) != YARP_OK)
			return YARP_FAIL;
	}

	return YARP_OK;
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = CAN_POSITION_MOVE | ((axis % 2) << 7);

	_ref_positions[axis] = *((double *)tmp->parameters);
	*((int*)(r._cmdBuffer.Data+2)) = S_32(_ref_positions[axis]);		/// pos
	*((short*)(r._cmdBuffer.Data+6)) = S_16(_ref_speeds[axis]);			/// speed
		
	///*((short*)(r._cmdBuffer.Data+6)) = S_16(*(((double *)tmp->parameters)+1));	/// speed

	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 8;
		
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

	for (i = 0; i < r._njoints; i++)
	{
		_mutex.Wait();

		/// four bits are mapped into four bit addresses through _destinations[].
		r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
		r._cmdBuffer.Data[0] += (r._destinations[i/2] & 0x0f); 
		r._cmdBuffer.Data[1] = CAN_VELOCITY_MOVE | ((i % 2) << 7);

		_ref_speeds[i] = tmp[i];
		*((short*)(r._cmdBuffer.Data+2)) = S_16(_ref_speeds[i]);		/// speed
		*((short*)(r._cmdBuffer.Data+4)) = S_16(_ref_accs[i]);			/// accel
		///*((short*)(r._cmdBuffer.Data+4)) = S_16(*(tmp+i*2+1));		/// accel

		r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
		r._cmdBuffer.NumberBytesData = 6;
		
		_request = true;
		_noreply = true;
		
		_mutex.Post();

		/// syncing.
		_ev.Wait();
	}

	return YARP_OK;
}

/// cmd is a SingleAxis poitner with 1 double arg
int YARPValueCanDeviceDriver::definePosition (void *cmd)
{
	/// prepare can message.

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_ENCODER_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::definePositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::setCommand (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_COMMAND_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setCommands (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::getRefPositions (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::setSpeed (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
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
	for (i = 0; i < r._njoints; i++)
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
int YARPValueCanDeviceDriver::getRefSpeeds (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::setAcceleration (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);

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
	for (i = 0; i < r._njoints; i++)
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
int YARPValueCanDeviceDriver::setOffset (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_OFFSET, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setOffsets (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::setIntegratorLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_ILIM_GAIN, axis, s);
}

/// cmd is an array of double 
int YARPValueCanDeviceDriver::setIntegratorLimits (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::setPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeNone (CAN_ENABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::disableAmp (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeNone (CAN_DISABLE_PWM_PAD, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::controllerRun (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_RUN, axis);
}

/// cmd is an int *
int YARPValueCanDeviceDriver::controllerIdle (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeNone (CAN_CONTROLLER_IDLE, axis);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::getTorques (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::getTorque (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	short value;

	if (_readWord16 (CAN_GET_PID_OUTPUT, axis, value) != YARP_OK)
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);

	return _writeNone (CAN_READ_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPValueCanDeviceDriver::writeBootMemory (void *cmd)
{
	const int axis = *((int *)cmd);
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);

	return _writeNone (CAN_WRITE_FLASH_MEM, axis);
}

/// cmd is a pointer to an integer (axis number).
int YARPValueCanDeviceDriver::setSwPositiveLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MAX_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a pointer to SingleAxisParameters struct with a single double arg.
int YARPValueCanDeviceDriver::setSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	return _writeDWord (CAN_SET_MIN_POSITION, axis, S_32(*((double *)tmp->parameters)));
}

/// cmd is a SingleAxis pointer with double arg
int YARPValueCanDeviceDriver::getSwNegativeLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
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
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	const short s = S_16(*((double *)tmp->parameters));
	
	return _writeWord16 (CAN_SET_TLIM, axis, s);
}

/// cmd is an array of double
int YARPValueCanDeviceDriver::setTorqueLimits (void *cmd)
{
	ValueCanResources& r = RES(system_resources);
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
int YARPValueCanDeviceDriver::getTorqueLimit (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
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

///
/// helper functions.
///
///
///

/// sends a message without parameters
int YARPValueCanDeviceDriver::_writeNone (int msg, int axis)
{
	ValueCanResources& r = RES(system_resources);
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);

	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 2;
		
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

	/// prepare Can message.
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);
	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 2;
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	/// reads back position info.
	_ev.Wait();

	if (r._cmdBuffer.Data[1] == CAN_NO_MESSAGE)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((short *)(r._cmdBuffer.Data+2));
	return YARP_OK;
}

/// to send a Word16.
int YARPValueCanDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	ValueCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);

	*((short *)(r._cmdBuffer.Data+2)) = s;
	
	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 4;
		
	_request = true;
	_noreply = true;	/// maybe temporary?
	
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

	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);

	*((int*)(r._cmdBuffer.Data+2)) = value;

	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 6;
		
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

	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	ACE_ASSERT ((axis % 2) == 0);	/// axis is even.

	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);

	*((short *)(r._cmdBuffer.Data+2)) = s1;
	*((short *)(r._cmdBuffer.Data+4)) = s2;
	
	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 6;
		
	_request = true;
	_noreply = true;	/// maybe temporary?
	
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

	/// prepare Can message.
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
	memset (&(r._cmdBuffer), 0, sizeof(icsSpyMessage));
	r._cmdBuffer.Data[0] = ((r._my_address << 4) & 0xf0);
	r._cmdBuffer.Data[0] += (r._destinations[axis/2] & 0x0f); 
	r._cmdBuffer.Data[1] = msg | ((axis % 2) << 7);
	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 2;
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	/// reads back position info.
	_ev.Wait();

	if (r._cmdBuffer.Data[1] == CAN_NO_MESSAGE)
	{
		value = 0;
		return YARP_FAIL;
	}

	value = *((int *)(r._cmdBuffer.Data+2));
	return YARP_OK;
}
