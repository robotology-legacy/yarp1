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
/// $Id: YARPValueCanDeviceDriver.cpp,v 1.4 2004-05-02 22:25:12 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPValueCanDeviceDriver.h"
#include <YARPControlBoardUtils.h>
#include <mCan.h>

/// get the message types from the DSP code.
#define __ONLY_DEF
#include "../56f807/controller/Code/Controller.h"

// Message property
#define TX_MESSAGE 1<<1
#define EXTENDED_FRAME 1<<2
#define REMOTE_FRAME 1<<3
// Message Errors
#define INCOMPLETE_FRAME_ERR 1<<6
#define UNDEF_ERR 1<<8
#define CAN_BUS_OFF_ERR 1<<9
#define HARDWARE_ERR 1<<16
#define FRAME_LENGHT_ERR 1<<17

///
///
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
	
	icsSpyMessage _canMsg[BUF_SIZE];
	icsSpyMessage _cmdBuffer;
	long _arbitrationID;

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
	}

	~ValueCanResources () { _uninitialize(); }

	int _read (void);
	int _read_np (void);
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
int ValueCanResources::_read_np (void)
{
	int messages = 0;
	int errors = 0;

	int res = canGetMessages (_portHandle, _canMsg, &messages, &errors);
	if (res == 0)
		return errors;

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
		canClosePort (_portHandle, &nOfErrors);
		canFreeObject (_portHandle);
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

	bool _dllLoaded = canInitLibrary();
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
				57600, ///115200
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
	m_cmds[CMDGetRefPositions] = &YARPValueCanDeviceDriver::getRefPositions;
	m_cmds[CMDSetPosition] = &YARPValueCanDeviceDriver::setPosition;
	m_cmds[CMDSetSpeed] = &YARPValueCanDeviceDriver::setSpeed;
	m_cmds[CMDSetAcceleration] = &YARPValueCanDeviceDriver::setAcceleration;
	m_cmds[CMDSetPID] = &YARPValueCanDeviceDriver::setPid;
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

	Begin ();

	return ret;
}

int YARPValueCanDeviceDriver::close (void)
{
	ValueCanResources& d = RES(system_resources);

	End ();	/// stops the thread first (joins too).

	int ret = d._uninitialize ();

	return ret;
}

///
///
/// OS scheduler should possibly go into hi-res mode (winnt).
void YARPValueCanDeviceDriver::Body(void)
{
	ValueCanResources& r = RES(system_resources);
	///unsigned long timestamp = 0;
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
		int n = r._read_np ();

		/// filters messages, no buffering allowed here.
		if (pending)
		{
			/// answer pending.
			int i;
			for (i = 0; i < n; i++)
			{
				icsSpyMessage& m = r._canMsg[i];
				if (r._error (m))
				{	
					ACE_DEBUG ((LM_DEBUG, "CAN: skipped a message for error\n"));
					continue;		/// skip this message.
				}

				///
				if (r.MYSELF(m.Data[0]))
				{
					/// my last sent message.
					///timestamp = m.TimeHardware;
					continue;
				}

				/// check first to see whether <m> was addressed here.
				if (((m.Data[0] & 0x0f) == r._my_address) &&
					(m.Data[1] == messagetype))
				{
					/// ok, this is my reply.
					/// write reply and signal event.
					/// ACE_DEBUG ((LM_DEBUG, "CAN: got a nicely formed message!\n"));
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
				ACE_DEBUG ((LM_DEBUG, "CAN: board %d timed out\n", (r._cmdBuffer.Data[0] & 0x0f)));
				r._cmdBuffer.Data[0] = 0;
				r._cmdBuffer.Data[1] = CAN_NO_MESSAGE;
				pending = false;
				_ev.Signal();
			}
		}
		/// else !pending, silently forget the messages...
		/// LATER: add callbacks here.

		_mutex.Wait();
		if (_request && !pending)
		{
			r._write ();
			///timestamp = 0;
			if (!_noreply) pending = true;
			messagetype = r._cmdBuffer.Data[1];
			cyclecount = 0;
			_request = false;
			_noreply = false;
			if (!pending)
				_ev.Signal ();
		}
		_mutex.Post();

		now = YARPTime::GetTimeAsSeconds();
		if ((now - before)*1000 < r._polling_interval)
		{
			YARPTime::DelayInSeconds(double(r._polling_interval)/1000.0-(now-before));
		}
		else 
		{
			ACE_DEBUG ((LM_DEBUG, "CAN: thread can't poll fast enough\n"));
		}
		before = now;
	}
}

const short MAX_SHORT = 32767;
const short MIN_SHORT = -32768;
const int MAX_INT = 0x7fffffff;
const int MIN_INT = 0x80000000;

///
/// 
/// control card commands.
///

int YARPValueCanDeviceDriver::getPosition (void *cmd)
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
	r._cmdBuffer.Data[1] = CAN_GET_ENCODER_POSITION | ((axis % 2) << 7);
	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 2;
		
	_request = true;
	_noreply = false;
	_mutex.Post();

	/// reads back position info.
	_ev.Wait();

	if (r._cmdBuffer.Data[1] == CAN_NO_MESSAGE)
	{
		*((double *)tmp->parameters) = 0.0;
		return YARP_FAIL;
	}

	*((double *)tmp->parameters) = double (*((int *)(r._cmdBuffer.Data+2)));

	return YARP_OK;
}


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
	r._cmdBuffer.Data[1] = CAN_SET_DESIRED_POSITION | ((axis % 2) << 7);

	double st = *((double *)tmp->parameters);
	int s = 0;
	if (st < double(-(MAX_INT))-1.0)
		s = MIN_INT;
	else
	if (st > double(MAX_INT))
		s = MAX_INT;
	else
		s = int(st);

	*((int*)(r._cmdBuffer.Data+2)) = s;

	r._cmdBuffer.ArbIDOrHeader = r._arbitrationID;
	r._cmdBuffer.NumberBytesData = 6;
		
	_request = true;
	_noreply = true;	/// maybe temporary?
	
	_mutex.Post();

	/// syncing.
	_ev.Wait();

	/// LATER: should also start generating the trajectory.

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

int YARPValueCanDeviceDriver::getRefPositions (void *j)
{
	ValueCanResources& r = RES(system_resources);
	double *out = (double *) j;
	int i, value = 0;

	for(i = 0; i < r._njoints; i++)
	{
		_readDWord (CAN_GET_DESIRED_POSITION, i, value);
		out[i] = double (value);
	}

	return YARP_OK;
}


///
/// helper function, to send a Word16.
///
int YARPValueCanDeviceDriver::_writeWord16 (int msg, int axis, short s)
{
	/// prepare Can message.
	ValueCanResources& r = RES(system_resources);

	ACE_ASSERT (axis >= 0 && axis <= (MAX_CARDS-1)*2);
	
	_mutex.Wait();

	/// four bits are mapped into four bit addresses through _destinations[].
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


int YARPValueCanDeviceDriver::setSpeed (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;

	double st = *((double *)tmp->parameters);
	short s = 0;
	if (st < double(-(MAX_SHORT))-1)
		s = MIN_SHORT;
	else
	if (st > double(MAX_SHORT))
		s = MAX_SHORT;
	else
		s = short(st);
	
	return _writeWord16 (CAN_SET_DESIRED_VELOCITY, axis, s);
}


int YARPValueCanDeviceDriver::setAcceleration (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;

	double st = *((double *)tmp->parameters);
	short s = 0;
	if (st < double(-(MAX_SHORT))-1)
		s = MIN_SHORT;
	else
	if (st > double(MAX_SHORT))
		s = MAX_SHORT;
	else
		s = short(st);
	
	return _writeWord16 (CAN_SET_DESIRED_ACCELER, axis, s);
}


int YARPValueCanDeviceDriver::setPid (void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
	return YARP_OK;
}

#if 0
int YARPMEIDeviceDriver::setPid(void *cmd)
{
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	/// LATER: to be completed!

	// these are stored to be used later in the setOffsets/setOffset functions
	_filter_coeffs[tmp->axis][DF_P] = (int16) round(pid->KP);
	_filter_coeffs[tmp->axis][DF_I] = (int16) round(pid->KI);
	_filter_coeffs[tmp->axis][DF_D] = (int16) round(pid->KD);
	_filter_coeffs[tmp->axis][DF_ACCEL_FF] = (int16) round (pid->AC_FF);
	_filter_coeffs[tmp->axis][DF_VEL_FF] = (int16) round(pid->VEL_FF);
	_filter_coeffs[tmp->axis][DF_I_LIMIT] = (int16) round(pid->I_LIMIT);
	_filter_coeffs[tmp->axis][DF_OFFSET] = (int16) round(pid->OFFSET);
	_filter_coeffs[tmp->axis][DF_DAC_LIMIT] = (int16) round(pid->T_LIMIT);
	_filter_coeffs[tmp->axis][DF_SHIFT] = (int16) round(pid->SHIFT);
	_filter_coeffs[tmp->axis][DF_FRICT_FF] = (int16) round(pid->FRICT_FF);
	
	rc = set_filter(tmp->axis, _filter_coeffs[tmp->axis]);

	return rc;
}
#endif
