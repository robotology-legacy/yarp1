/** ###################################################################
**     Filename  : Controller.C
**     Project   : Controller
**     Processor : 56F807
**     Version   : Driver 01.03
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 4/20/2004, 10:44 AM
**     Abstract  :
**         Main module. 
**         Here is to be placed user's code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2002
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE Controller */

/* Including used modules for compilling procedure */
#include "Cpu.h"
#include "Events.h"
#include "AS1.h"
#include "QD1.h"
#include "QD2.h"
#include "PWMC1.h"
#include "PWMC2.h"
#include "TI1.h"
#include "IFsh1.h"
#include "CAN1.h"
#include "MC1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* my app includes */
#include "DSP807_serial.h"
#include "Controller.h"
#include "Messages.h"

/* stable global data */
bool _calibrated[JN] = { false, false };
bool _pad_enabled[JN] = { false, false };
bool _verbose = false;
byte _control_mode[JN] = { MODE_IDLE, MODE_IDLE };
										/* control mode (e.g. position, velocity, etc.) */

long _position[JN] = { 0, 0 };			/* encoder position */
long _position_old[JN] = { 0, 0 };		/* do I need to add bits for taking into account multiple rotations */
long _speed[JN] = { 0, 0 };				/* encoder speed */

long _desired[JN] = { 0, 0 };			/* PID ref value, computed by the trajectory generator */
long _set_point[JN] = { 0, 0 };			/* set point for position [user specified] */

long _min_position[JN] = { -DEFAULT_MAX_POSITION, -DEFAULT_MAX_POSITION };
long _max_position[JN] = { DEFAULT_MAX_POSITION, DEFAULT_MAX_POSITION };
										/* software position limits */
										
int  _desired_vel[JN] = { 0, 0 };		/* speed reference value, computed by the trajectory gen. */
int  _set_vel[JN] = { DEFAULT_VELOCITY, DEFAULT_VELOCITY };	
										/* set point for velocity [user specified] */
int  _max_vel[JN] = { DEFAULT_MAX_VELOCITY, DEFAULT_MAX_VELOCITY };
										/* assume this limit is symmetric */
										
int  _set_acc[JN] = { DEFAULT_ACCELERATION, DEFAULT_ACCELERATION };
										/* set point for acceleration [too low!] */
int  _integral[JN] = { 0, 0 };			/* store the sum of the integral component */
int  _integral_limit[JN] = { 0x7fff, 0x7fff };

int  _error[JN] = { 0, 0 };				/* actual feedback error */
int  _error_old[JN] = { 0, 0 };			/* error at t-1 */

int  _pid[JN] = { 0, 0 };				/* pid result */
int  _pid_limit[JN] = { 100, 100 };		/* pid limit */

int  _kp[JN] = { 10, 10 };				/* PID gain */
int  _kd[JN] = { 40, 40 };
int  _ki[JN] = { 0, 0 };
int  _ko[JN] = { 0, 0 };				/* offset */
int  _kr[JN] = { 3, 3 };				/* scale factor (negative power of two) */

/* CAN bus communication global vars */
byte 	CAN_data[8];					/* CAN bus message */
dword 	CAN_messID = 0;					/* arbitration */
byte 	CAN_frameType;
byte 	CAN_frameFormat;
byte 	CAN_length;						/* len of the data */

byte	_board_ID = DEFAULT_BOARD_ID;	/* */
byte	_general_board_error = ERROR_NONE;

volatile bool _wait = true;				/* wait on timer variable */

extern _ended[];						/* trajectory completed flag */
#define IS_DONE(jj) (_ended[jj])

/*
 *	inline functions
 */
long saturate;
int u0, u1, ud;

/* compute PID macro: j can only be 0 or 1 */
/* requires global var saturate */

/* 
 * PID is:
 * 	_pid[j] = _kp[j] * _error[j] + _kd[j] * (_error[j] - _error_old[j]) + _ki[j] * _integral[j];
 *  _pid[j] >> _kr[j]; 
 *  _pid[j] += _ko[j];
 *
 */
#define compute_pid(j) \
{ \
	/* compute control values */ \
	_error_old[j] = _error[j]; \
	saturate = _desired[j] - _position[j]; \
	if (saturate > 32767) \
		_error[j] = 32767; \
	else \
	if (saturate < -32768) \
		_error[j] = -32768; \
	else \
		_error[j] = (int)saturate; \
	\
	u1 = __shr (_error[j], 8); \
	u0 = (_error[j] & 0x00ff); \
		\
	/*_integral[j] += _error[j]; */ \
	/* if (_integral[j] > _integral_limit[j]) _integral[j] = _integral_limit[j]; */ \
	/*if (_integral[j] < -_integral_limit[j]) _integral[j] = -_integral_limit[j]; */ \
		\
	_pid[j] = _kp[j] * u1; \
	_pid[j] = __shl (_pid[j], 8-_kr[j]); \
	u0 *= _kp[j]; \
	_pid[j] += __shr (u0, _kr[j]); \
	\
	saturate = (long)_error[j] - (long)_error_old[j]; \
	if (saturate > 32767) \
		ud = 32767; \
	else \
	if (saturate < -32768) \
		ud = -32768; \
	else \
		ud = (int)saturate; \
	\
	u1 = __shr (ud, 8); \
	u0 = (ud & 0x00ff); \
	u1 *= _kd[j]; \
	_pid[j] += __shl (u1, 8-_kr[j]); \
	u0 *= _kd[0]; \
	_pid[j] += __shr (u0, _kr[j]); \
	\
	_pid[j] += _ko[j]; \
	\
	if (_pid[j] < -_pid_limit[j]) \
		_pid[j] = -_pid_limit[j]; \
	else \
	if (_pid[j] > _pid_limit[j]) \
		_pid[j] = _pid_limit[j]; \
}	
/* end of macro */

/*
 * 
 */
long step_velocity (byte jj)
{
	long u0;
	int dv, da;
	
	/* dv is a signed 16 bit value, need to be checked for overflow */
	if (_set_vel[jj] < -_max_vel[jj])
		_set_vel[jj] = -_max_vel[jj];
	else
	if (_set_vel[jj] > _max_vel[jj])
		_set_vel[jj] = _max_vel[jj];
	
	dv = _set_vel[jj] - _desired_vel[jj];
	da = _set_acc[jj] * CONTROLLER_PERIOD;
	
	if (__abs(dv) < da)
	{
		_desired_vel[jj] = _set_vel[jj];
	}
	else
	if (dv > 0)
	{
		_desired_vel[jj] += da;
	}
	else
	{
		_desired_vel[jj] -= da;
	}
	
	u0 = _desired[jj] + _desired_vel[jj] * CONTROLLER_PERIOD;
	return u0;
}

/*
 * flash memory functions.
 * LATER: add all relevant variables.
 */
byte writeToFlash (void)
{
	dword ptr = FLASH_END_ADDR;
	byte i, err;
	word tmp;
	bool gerr = false;
	
	for (i = 0; i < JN; i++)
	{
		err = IFsh1_SetWordFlash(ptr, _kp[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
		err = IFsh1_SetWordFlash(ptr, _kd[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
//		err = IFsh1_SetWordFlash(ptr, _ki[i]);
		err = IFsh1_SetWordFlash(ptr, 0);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
		err = IFsh1_SetWordFlash(ptr, _ko[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
		err = IFsh1_SetWordFlash(ptr, _kr[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
//		err = IFsh1_SetWordFlash(ptr, _integral_limit[i]);
		err = IFsh1_SetWordFlash(ptr, 0);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
		err = IFsh1_SetWordFlash(ptr, _pid_limit[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,2);
		
		err = IFsh1_SetLongFlash(ptr, _min_position[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,4);
		err = IFsh1_SetLongFlash(ptr, _max_position[i]);
		gerr |= (err != ERR_OK);
		ADP(ptr,4);
	}

	tmp = BYTE_W(_board_ID, 0);
	err = IFsh1_SetWordFlash(ptr, tmp);
	gerr |= (err != ERR_OK);
	//ptr ++;		

	if (gerr)
		DSP_SendDataEx ("Error while writing to flash memory, pls try again\r\n");
			
	return ERR_OK;
}

byte readFromFlash (void)
{
	dword ptr = FLASH_END_ADDR;
	word tmp;
	int i;

	for (i = 0; i < JN; i++)
	{
		IFsh1_GetWordFlash(ptr, (word *)(_kp+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_kd+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_ki+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_ko+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_kr+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_integral_limit+i));
		ADP(ptr,2);
		IFsh1_GetWordFlash(ptr, (word *)(_pid_limit+i));
		ADP(ptr,2);
		
		IFsh1_GetLongFlash(ptr, (dword *)(_min_position+i));
		ADP(ptr,4);
		IFsh1_GetLongFlash(ptr, (dword *)(_max_position+i));
		ADP(ptr,4);
	}

	IFsh1_GetWordFlash(ptr, &tmp);
	_board_ID = BYTE_H(tmp);
	//ptr ++;
	
	return ERR_OK;
}

long BYTE_C(byte x4, byte x3, byte x2, byte x1)
{
	long ret;
	short *p = (short *)&ret;
	*p++ = __shl(x3,8) | x4;
	*p++ = __shl(x1,8) | x2;
	return ret;
}

#define SPIKEREMOVE(i) \
	if (_speed[i] < -32768) \
	{ \
		_position[i] += 65536; \
		_speed[i] += 65536; \
	} \
	else \
	if (_speed[i] > 32767) \
	{ \
		_position[i] -= 65535; \
		_speed[i] -= 65535; \
	}
	

#define DUTYCYCLE(axis, ch, value) \
 if (axis == 0) \
	PWMC1_SetDutyPercent (ch, value); \
 else \
	PWMC2_SetDutyPercent (ch, value);
 
#define LOADDUTYCYCLE(axis) \
 if (axis == 0) \
	PWMC1_Load(); \
 else \
	PWMC2_Load();

/*
 * helper function to generate PWM values according to controller status.
 */
void generatePwm (byte i)
{
	long cd;
	
	if (_control_mode[i] != MODE_IDLE)
	{
		cd = _desired[i];
		
		/* compute trajectory and control mode */
		switch (_control_mode[i])
		{
		case MODE_POSITION:
			_desired[i] = step_trajectory (i);
			break;
			
		case MODE_VELOCITY:
			_desired[i] = step_velocity (i);
			if (_desired[i] < _min_position[i] && (_desired[i] - cd) < 0) 
			{
				_desired[i] = _min_position[i];
				_set_vel[i] = 0;
			}
			else
			if (_desired[i] > _max_position[i] && (_desired[i] - cd) > 0)
			{
				_desired[i] = _max_position[i];
				_set_vel[i] = 0;
			}
			break;
		}
			
		/* computes PID control */
		compute_pid (i);
					
		/* set PWM, _pid becomes the PWM value */
		if (_calibrated[i])
		{
			if (_pid[i] >= 0)
			{
				if (_pid[i] > 100) 
					_pid[i] = 100;
				DUTYCYCLE (i, 0, (unsigned char)(_pid[i] & 0x00ff));
				DUTYCYCLE (i, 2, 0);
				DUTYCYCLE (i, 4, 0);
			}
			else
			{
				_pid[i] = -_pid[i];
				if (_pid[i] > 100) 
					_pid[i] = 100;
				DUTYCYCLE (i, 0, 0);
				DUTYCYCLE (i, 2, (unsigned char)(_pid[i] & 0x00ff));
				DUTYCYCLE (i, 4, 0);
			}
			
			LOADDUTYCYCLE(i);
		}
	} /* end of !IDLE */
	else
	{
		DUTYCYCLE (i, 0, 0);
		DUTYCYCLE (i, 2, 0);
		DUTYCYCLE (i, 4, 0);

		LOADDUTYCYCLE(i);
	}
}


/* 
	This is the main controller loop.
	sequences of operations:
		- reads from CAN bus or serial port 1.
		- reads encoders (or ADC).
		- computes the control value (a PID in this version).
		- checks limits and other errors.
		- sets PWM
		- does extra functions (e.g. communicate with neighboring cards).
*/
void main(void)
{
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* initialization */
	DSP_SendDataEx ("\r\n\n");
	DSP_SendDataEx ("Firmware - ver 1.0\r\n");
	
	PWMC1_Enable ();
	PWMC2_Enable ();  
	QD1_InitPosition ();
	QD2_InitPosition ();

	/* reads the PID parameters from flash memory */
	readFromFlash ();
	
	/* reset encoders, LATER: should do something more than this */
	calibrate(0);
	calibrate(1);

	/* reset trajectory generation */
	abort_trajectory (0, 0);
	abort_trajectory (1, 0);
	
	/* main control loop */
	for(;;) 
	{
		while (_wait) ;

		/* read commands from CAN or serial board */
		serial_interface ();
		can_interface ();
		
		/* instructions are executed for both axes and only the PWM isn't 
		   used if the specific axis is not used/calibrated
		   we're supposed to guarantee a constant loop period 
		   the serial line and other devices shouldn't be doing
		   much processing in case the receive external commands */

		/* read encoders, 32 bit values */
		_position_old[0] = _position[0];
		_position_old[1] = _position[1];
		QD1_GetPosition ((dword *)_position);
		QD2_GetPosition ((dword *)(_position+1));

		/* this can be useful to estimate speed later on */
		_speed[0] = _position[0] - _position_old[0]; /// divide by CONTROLLER_PERIOD
		_speed[1] = _position[1] - _position_old[1];
		
		/* adjust zero crossing @ position counter */
		SPIKEREMOVE (0);
		SPIKEREMOVE (1);

		/* read ADC or other ports */
		/* to be inserted here */

		/*
		 * 
		 */		  
		generatePwm (0);
		generatePwm (1);
		
		/* do extra functions, communicate, etc. */
		/* LATER */
		
		/* tells that the control cycle is completed */
		_wait = true;	
		
	} /* end for(;;) */
}


/* this function might not be required */
byte calibrate (int jnt)
{
	if (jnt == 0)
	{
		_desired[0] = 0;
		_set_point[0] = 0;
		if (QD1_InitPosition() == ERR_OK)
			_calibrated[0] = true;
		else
			_calibrated[0] = false;
		return ERR_OK;
	}
	else
	if (jnt == 1)
	{
		_desired[1] = 0;
		_set_point[1] = 0;
		if (QD2_InitPosition() == ERR_OK)
			_calibrated[1] = true;
		else
			_calibrated[1] = false;
		return ERR_OK;
	}
	
	/* need to change this return val */
	return ERR_SPEED;
}

#define BEGIN_SPECIAL_MSG_TABLE(x) \
	switch (x[1] & 0x7F) \
	{ \
		default: \
			break;

#define END_SPECIAL_MSG_TABLE \
	}
	
/* message table macros */
#define BEGIN_MSG_TABLE(x) \
	if ((x[0] & 0x0f) != _board_ID) \
	{ \
		/* DSP_SendDataEx ("it wasn't my message\r\n"); */ \
		return ERR_OK; \
	} \
	switch (x[1] & 0x7F) \
	{ \
		default: \
			return ERR_OK; \
			break;

#define HANDLE_MSG(x, y) \
		case x: \
			y(CAN_DATA) ; \
			break;
	
#define END_MSG_TABLE \
	}

	
void print_can (byte data[], byte len, char c)
{
	int i;
	DSP_SendData (&c, 1);
	DSP_SendDataEx (": ");
	
	for (i = 0; i < len; i++)
	{
		DSP_SendByteAsChars (data[i]);
		DSP_SendDataEx (" ");
	}
	DSP_SendDataEx ("\r\n");
}

#define SEND_BOOL(x) ((x) ? DSP_SendDataEx("1") : DSP_SendDataEx("0"))

void print_can_error(CAN1_TError *e)
{
	DSP_SendDataEx ("f: ");
	SEND_BOOL(e->errName.BusOff);
	SEND_BOOL(e->errName.TxPassive);
	SEND_BOOL(e->errName.RxPassive);
	SEND_BOOL(e->errName.TxWarning);
	SEND_BOOL(e->errName.RxWarning);
	SEND_BOOL(e->errName.OverRun);
	DSP_SendDataEx ("\r\n");
}

CAN1_TError err;

/* */
byte can_interface (void)
{
	if (CAN1_GetStateRX () != 0)
	{
		CAN1_ReadFrame (&CAN_messID, &CAN_frameType, &CAN_frameFormat, &CAN_length, CAN_data);
///		print_can (CAN_data, CAN_length, 'i');
///		CAN1_GetError (&err);
///		print_can_error (&err);
		
#define CAN_DATA CAN_data
#define CAN_FRAME_TYPE CAN_frameType
#define CAN_FRAME_FMT CAN_frameFormat
#define CAN_LEN CAN_length
#define CAN_ID CAN_messID

		/* interpret the messages */
		BEGIN_SPECIAL_MSG_TABLE (CAN_data)
		HANDLE_MSG (CAN_SET_BOARD_ID, CAN_SET_BOARD_ID_HANDLER)
		END_SPECIAL_MSG_TABLE
		
		BEGIN_MSG_TABLE (CAN_data)
		HANDLE_MSG (CAN_NO_MESSAGE, CAN_NO_MESSAGE_HANDLER)
		HANDLE_MSG (CAN_CONTROLLER_RUN, CAN_CONTROLLER_RUN_HANDLER)
		HANDLE_MSG (CAN_CONTROLLER_IDLE, CAN_CONTROLLER_IDLE_HANDLER)
		HANDLE_MSG (CAN_TOGGLE_VERBOSE, CAN_TOGGLE_VERBOSE_HANDLER)
		HANDLE_MSG (CAN_CALIBRATE_ENCODER, CAN_CALIBRATE_ENCODER_HANDLER)
		HANDLE_MSG (CAN_ENABLE_PWM_PAD, CAN_ENABLE_PWM_PAD_HANDLER)
		HANDLE_MSG (CAN_DISABLE_PWM_PAD, CAN_DISABLE_PWM_PAD_HANDLER)
		HANDLE_MSG (CAN_GET_CONTROL_MODE, CAN_GET_CONTROL_MODE_HANDLER)
		
		HANDLE_MSG (CAN_WRITE_FLASH_MEM, CAN_WRITE_FLASH_MEM_HANDLER)
		HANDLE_MSG (CAN_READ_FLASH_MEM, CAN_READ_FLASH_MEM_HANDLER)
		
		HANDLE_MSG (CAN_GET_ENCODER_POSITION, CAN_GET_ENCODER_POSITION_HANDLER)
		HANDLE_MSG (CAN_SET_DESIRED_POSITION, CAN_SET_DESIRED_POSITION_HANDLER)
		HANDLE_MSG (CAN_GET_DESIRED_POSITION, CAN_GET_DESIRED_POSITION_HANDLER)
		HANDLE_MSG (CAN_SET_DESIRED_VELOCITY, CAN_SET_DESIRED_VELOCITY_HANDLER)
		HANDLE_MSG (CAN_GET_DESIRED_VELOCITY, CAN_GET_DESIRED_VELOCITY_HANDLER)
		HANDLE_MSG (CAN_SET_DESIRED_ACCELER, CAN_SET_DESIRED_ACCELER_HANDLER)
		HANDLE_MSG (CAN_GET_DESIRED_ACCELER, CAN_GET_DESIRED_ACCELER_HANDLER)

		HANDLE_MSG (CAN_SET_ENCODER_POSITION, CAN_SET_ENCODER_POSITION_HANDLER)
		HANDLE_MSG (CAN_GET_ENCODER_VELOCITY, CAN_GET_ENCODER_VELOCITY_HANDLER)
		HANDLE_MSG (CAN_SET_COMMAND_POSITION, CAN_SET_COMMAND_POSITION_HANDLER)
		
		HANDLE_MSG (CAN_SET_P_GAIN, CAN_SET_P_GAIN_HANDLER)
		HANDLE_MSG (CAN_GET_P_GAIN, CAN_GET_P_GAIN_HANDLER)
		HANDLE_MSG (CAN_SET_D_GAIN, CAN_SET_D_GAIN_HANDLER)
		HANDLE_MSG (CAN_GET_D_GAIN, CAN_GET_D_GAIN_HANDLER)
		HANDLE_MSG (CAN_SET_I_GAIN, CAN_SET_I_GAIN_HANDLER)
		HANDLE_MSG (CAN_GET_I_GAIN, CAN_GET_I_GAIN_HANDLER)
		HANDLE_MSG (CAN_SET_ILIM_GAIN, CAN_SET_ILIM_GAIN_HANDLER)
		HANDLE_MSG (CAN_GET_ILIM_GAIN, CAN_GET_ILIM_GAIN_HANDLER)
		HANDLE_MSG (CAN_SET_OFFSET, CAN_SET_OFFSET_HANDLER)
		HANDLE_MSG (CAN_GET_OFFSET, CAN_GET_OFFSET_HANDLER)
		HANDLE_MSG (CAN_SET_SCALE, CAN_SET_SCALE_HANDLER)
		HANDLE_MSG (CAN_GET_SCALE, CAN_GET_SCALE_HANDLER)

		HANDLE_MSG (CAN_POSITION_MOVE, CAN_POSITION_MOVE_HANDLER)
		HANDLE_MSG (CAN_VELOCITY_MOVE, CAN_VELOCITY_MOVE_HANDLER)
	
		HANDLE_MSG (CAN_GET_PID_OUTPUT, CAN_GET_PID_OUTPUT_HANDLER)
		
		HANDLE_MSG (CAN_SET_MIN_POSITION, CAN_SET_MIN_POSITION_HANDLER)
		HANDLE_MSG (CAN_GET_MIN_POSITION, CAN_GET_MIN_POSITION_HANDLER)
		HANDLE_MSG (CAN_SET_MAX_POSITION, CAN_SET_MAX_POSITION_HANDLER)
		HANDLE_MSG (CAN_GET_MAX_POSITION, CAN_GET_MAX_POSITION_HANDLER)
		HANDLE_MSG (CAN_SET_MAX_VELOCITY, CAN_SET_MAX_VELOCITY_HANDLER)
		HANDLE_MSG (CAN_GET_MAX_VELOCITY, CAN_GET_MAX_VELOCITY_HANDLER)
	
		HANDLE_MSG (CAN_SET_TLIM, CAN_SET_TLIM_HANDLER)
		HANDLE_MSG (CAN_GET_TLIM, CAN_GET_TLIM_HANDLER)
		HANDLE_MSG (CAN_GET_ERROR_STATUS, CAN_GET_ERROR_STATUS_HANDLER)
		
		END_MSG_TABLE		

///		print_can (CAN_data, CAN_length, 'o'); 

///		if (_general_board_error != ERROR_NONE)
///		{
///			DSP_SendDataEx ("error in processing message\r\n");
///			_general_board_error = ERROR_NONE;
///		}	
	}
			
	return ERR_OK;
}

AS1_TComData c = 0;

/* test/debug serial port interface (on AS1) */
byte serial_interface (void)
{
	AS1_TComData d = 0;
	char buffer[SMALL_BUFFER_SIZE];
	int  iretval = 0;
	
	if (c == 0)
		AS1_RecvChar(&c);
	
	switch (c)
	{
		default:
			c = 0;
			break;
			
		case 'h':
		case 'H':
		case '\r':
			DSP_SendDataEx ("\r\n\n");
			DSP_SendDataEx ("Firmware - ver 1.0\r\n");
			DSP_SendDataEx ("h, H: help\r\n");
			
			DSP_SendDataEx ("a, set card address\r\n");
			DSP_SendDataEx ("b, print card address\r\n");
			
			DSP_SendDataEx ("w2, write control params to FLASH mem\r\n");
			DSP_SendDataEx ("w3, read control params from FLASH mem\r\n");
						
			c = 0;
			break;

		case 'a':
			DSP_SendDataEx ("address [1-15]: ");
			DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
			iretval = DSP_atoi (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE)); 
			DSP_SendDataEx ("address is ");
			DSP_SendWord16AsChars (iretval);
			DSP_SendDataEx ("\r\n");
			
			if (iretval >= 1 && iretval <= 15)
				_board_ID = iretval & 0x0f;
			
			c = 0;
			break;

		case 'b':
			DSP_SendDataEx ("address is ");
			iretval = BYTE_W(_board_ID, 0);
			DSP_SendWord16AsChars (iretval);
			DSP_SendDataEx ("\r\n");
			
			c = 0;
			break;
						
		case 'w':
			if (AS1_RecvChar(&d) == ERR_OK)
			{
				if (d == '0')
				{
					if (_calibrated[0])
					{
						if (!_pad_enabled[0])
						{
							PWMC1_OutputPadEnable();
							DSP_SendDataEx ("pad channel 0 enabled\r\n");
						}
						else
						{
							PWMC1_OutputPadDisable();
							DSP_SendDataEx ("pad channel 0 disabled\r\n");
						}
						_pad_enabled[0] = !_pad_enabled[0];
					}
					else
						DSP_SendDataEx ("need to calibrate channel 0 first\r\n");
				}
				else
				if (d == '1')
				{
					if (_calibrated[1])
					{
						if (!_pad_enabled[1])
						{
							PWMC2_OutputPadEnable();
							DSP_SendDataEx ("pad channel 1 enabled\r\n");
						}
						else
						{
							PWMC2_OutputPadDisable();
							DSP_SendDataEx ("pad channel 1 disabled\r\n");
						}
						_pad_enabled[1] = !_pad_enabled[1];
					}
					else
						DSP_SendDataEx ("need to calibrate channel 1 first\r\n");
				}
				else
				if (d == '2')
				{
					DSP_SendDataEx ("writing to FLASH mem\r\n");
					writeToFlash ();
					DSP_SendDataEx ("done!\r\n");
				}
				else
				if (d == '3')
				{
					DSP_SendDataEx ("reading from FLASH mem\r\n");
					readFromFlash ();
					DSP_SendDataEx ("done!\r\n");
				}

				c = 0;
			}
			break;
		
	}	/* end switch/case */
}


/* END Controller */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 03.32 for 
**     the Motorola 56800 series of microcontrollers.
**
** ###################################################################
*/
