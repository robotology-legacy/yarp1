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
byte _control_mode = 0;					/* control mode (e.g. position, velocity, etc.) */

long _position[JN] = { 0, 0 };			/* encoder position */
long _position_old[JN] = { 0, 0 };		/* do I need to add bits for taking into account multiple rotations */
long _speed[JN] = { 0, 0 };				/* encoder speed */

long _desired[JN] = { 0, 0 };			/* PID ref value, computed by the trajectory generator */
long _set_point[JN] = { 0, 0 };			/* set point for position [user specified] */
int  _desired_vel[JN] = { 0, 0 };		/* speed reference value, computed by the trajectory gen. */
int  _set_vel[JN] = { DEFAULT_VELOCITY, DEFAULT_VELOCITY };	
										/* set point for velocity [user specified] */
int  _set_acc[JN] = { DEFAULT_ACCELERATION, DEFAULT_ACCELERATION };
										/* set point for acceleration [too low!] */
int  _integral[JN] = { 0, 0 };			/* store the sum of the integral component */
int  _integral_limit[JN] = { 0x7fff, 0x7fff };

int  _error[JN] = { 0, 0 };				/* actual feedback error */
int  _error_old[JN] = { 0, 0 };			/* error at t-1 */

int  _pid[JN] = { 0, 0 };				/* pid result */

int  _kp[JN] = { 10, 0 };				/* PID gain */
int  _kd[JN] = { 40, 0 };
int  _ki[JN] = { 0, 0 };
int  _ko[JN] = { 0, 0 };				/* offset */
int  _kr[JN] = { 4, 0 };				/* scale factor (negative power of two) */

/* CAN bus communication global vars */
byte 	CAN_data[8];					/* CAN bus message */
dword 	CAN_messID = 0;						/* arbitration */
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
}	
/* end of macro */

long step_velocity (byte jj);

long step_velocity (byte jj)
{
	long u0;
	
	/* dv is a signed 16 bit value, need to be checked for overflow */
	int dv = _set_vel[jj] - _desired_vel[jj];
	int da = _set_acc[jj] * CONTROLLER_PERIOD;
	
	if (__abs(dv) < da)
		dv = 0;
	else
	if (dv > 0)
		dv = da;
	else
		dv = -da;
	
	_desired_vel[jj] += dv;
	
	u0 = _desired[jj] + _desired_vel[jj] * CONTROLLER_PERIOD;
	return u0;
}
/* end of macro */


byte writeToFlash (void)
{
	dword ptr = FLASH_START_ADDR;
	byte i, err;
	bool gerr = false;
	
	for (i = 0; i < JN; i++)
	{
		err = IFsh1_SetWordFlash(ptr, _kp[i]);
		gerr |= (err != ERR_OK);
		ptr += 2;
		err = IFsh1_SetWordFlash(ptr, _kd[i]);
		gerr |= (err != ERR_OK);
		ptr += 2;	
//		err = IFsh1_SetWordFlash(ptr, _ki[i]);
		err = IFsh1_SetWordFlash(ptr, 0);
		gerr |= (err != ERR_OK);
		ptr += 2;	
		err = IFsh1_SetWordFlash(ptr, _ko[i]);
		gerr |= (err != ERR_OK);
		ptr += 2;	
		err = IFsh1_SetWordFlash(ptr, _kr[i]);
		gerr |= (err != ERR_OK);
		ptr += 2;	
//		err = IFsh1_SetWordFlash(ptr, _integral_limit[i]);
		err = IFsh1_SetWordFlash(ptr, 0);
		gerr |= (err != ERR_OK);
		ptr += 2;
		err = IFsh1_SetByteFlash(ptr, _board_ID);
		gerr |= (err != ERR_OK);
		ptr ++;		
	}

	if (gerr)
		DSP_SendDataEx ("Error while writing to flash memory, pls try again\r\n");
			
	return ERR_OK;
}

byte readFromFlash (void)
{
	dword ptr = FLASH_START_ADDR;
	int i;

	for (i = 0; i < JN; i++)
	{
		IFsh1_GetWordFlash(ptr, (word *)(_kp+i));
		ptr +=2;
		IFsh1_GetWordFlash(ptr, (word *)(_kd+i));
		ptr +=2;
		IFsh1_GetWordFlash(ptr, (word *)(_ki+i));
		ptr +=2;
		IFsh1_GetWordFlash(ptr, (word *)(_ko+i));
		ptr +=2;
		IFsh1_GetWordFlash(ptr, (word *)(_kr+i));
		ptr +=2;
		IFsh1_GetWordFlash(ptr, (word *)(_integral_limit+i));
		ptr +=2;
		IFsh1_GetByteFlash(ptr, &_board_ID);
		ptr ++;
	}
	
	return ERR_OK;
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

	readFromFlash ();
	/* reset encoders, LATER: should do something more than this */
	calibrate(0);
	calibrate(1);

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

		/* read encoders, 32 bits should be enough */
		_position_old[0] = _position[0];
		_position_old[1] = _position[1];
		QD1_GetPosition ((dword *)_position);
		QD2_GetPosition ((dword *)(_position+1));

		/* this can be useful to estimate speed later on */
		_speed[0] = _position[0] - _position_old[0];
		_speed[1] = _position[1] - _position_old[1];
		
		/* adjust zero crossing @ position counter */
		if (_speed[0] < -32768)
		{
			_position[0] += 65536;
			_speed[0] += 65536;
		}
		else
		if (_speed[0] > 32767)
		{
			_position[0] -= 65535;
			_speed[0] -= 65535;
		}

		if (_speed[1] < -32768)
		{
			_position[1] += 65536;
			_speed[1] += 65536;
		}
		else
		if (_speed[1] > 32767)
		{
			_position[1] -= 65535;
			_speed[1] -= 65535;
		}

		/* read ADC or other ports */
		/* to be inserted here */
		  
		if (_control_mode != MODE_IDLE)
		{
			/* compute trajectory and control mode */
			switch (_control_mode)
			{
			case MODE_POSITION:
				_desired[0] = step_trajectory (0);
				break;
				
			case MODE_VELOCITY:
				_desired[0] = step_velocity (0);
				break;
			}

			/* check for trajectory limits here */
		
			compute_pid (0);
			compute_pid (1);
						
			/* check limits or errors */
			/* TO BE DONE! */

			/* set PWM, _pid becomes the PWM values */
			if (_calibrated[0])
			{
				if (_pid[0] >= 0)
				{
					if (_pid[0] > 100) 
						_pid[0] = 100;
					PWMC1_SetDutyPercent (0, (unsigned char)(_pid[0] & 0x00ff));
					PWMC1_SetDutyPercent (2, 0);
					PWMC1_SetDutyPercent (4, 0);
				}
				else
				{
					_pid[0] = -_pid[0];
					if (_pid[0] > 100) 
						_pid[0] = 100;
					PWMC1_SetDutyPercent (0, 0);
					PWMC1_SetDutyPercent (2, (unsigned char)(_pid[0] & 0x00ff));
					PWMC1_SetDutyPercent (4, 0);
				}
				
				PWMC1_Load();
			}
			
			if (_calibrated[1])
			{			
				if (_pid[1] >= 0)
				{
					if (_pid[1] > 100) 
						_pid[1] = 100;
					PWMC2_SetDutyPercent (0, (unsigned char)(_pid[1] & 0x00ff));
					PWMC2_SetDutyPercent (2, 0);
					PWMC2_SetDutyPercent (4, 0);
				}
				else
				{
					_pid[1] = -_pid[1];
					if (_pid[1] > 100) 
						_pid[1] = 100;
					PWMC2_SetDutyPercent (0, 0);
					PWMC2_SetDutyPercent (2, (unsigned char)(_pid[1] & 0x00ff));
					PWMC2_SetDutyPercent (4, 0);
				}
				PWMC2_Load();
			}
		} /* end of !IDLE */
		else
		{
			PWMC1_SetDutyPercent (0, 0);
			PWMC1_SetDutyPercent (2, 0);
			PWMC1_SetDutyPercent (4, 0);
			PWMC2_SetDutyPercent (0, 0);
			PWMC2_SetDutyPercent (2, 0);
			PWMC2_SetDutyPercent (4, 0);
		}
		
		/* do extra functions, communicate, etc. */
		
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

// x[0] != 0 && 
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
	
void print_can (byte data[], char c)
{
	DSP_SendData (&c, 1);
	DSP_SendDataEx (": ");
	DSP_SendByteAsChars (data[0]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[1]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[2]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[3]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[4]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[5]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[6]);
	DSP_SendDataEx (" ");
	DSP_SendByteAsChars (data[7]);
	DSP_SendDataEx ("\r\n");
}

/* */
byte can_interface (void)
{
	if (CAN1_GetStateRX () != 0)
	{
		CAN1_ReadFrame (&CAN_messID, &CAN_frameType, &CAN_frameFormat, &CAN_length, CAN_data);
		print_can (CAN_data, 'x');
		
#define CAN_DATA CAN_data
#define CAN_FRAME_TYPE CAN_frameType
#define CAN_FRAME_FMT CAN_frameFormat
#define CAN_LEN CAN_length
#define CAN_ID CAN_messID

		/* interpret the messages */
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

	
		END_MSG_TABLE		
		
		if (_general_board_error != ERROR_NONE)
		{
			DSP_SendDataEx ("error in processing message\r\n");
			_general_board_error = ERROR_NONE;
		}	
	}
			
	return ERR_OK;
}

AS1_TComData c = 0;

/* test/debug serial port interface (on AS1) */
byte serial_interface (void)
{
	AS1_TComData d = 0;
	char buffer[SMALL_BUFFER_SIZE];
	byte retval = 0;
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
			
			DSP_SendDataEx ("1, toggle position mode\r\n");
			DSP_SendDataEx ("2, toggle velocity mode\r\n");
			DSP_SendDataEx ("3, verbose on/off\r\n");
			
			DSP_SendDataEx ("c, calibrate encoders\r\n");
			DSP_SendDataEx ("w0, enable PWM 0\n\r");
			DSP_SendDataEx ("w1, enable PWM 1\n\r");
			DSP_SendDataEx ("p0, set desired position PWM 0\r\n");
			DSP_SendDataEx ("v0, set desired velocity PWM 1\r\n");
			DSP_SendDataEx ("p1, set desired position PWM 1\r\n");
			DSP_SendDataEx ("w2, write control params to FLASH mem\r\n");
			DSP_SendDataEx ("w3, read control params from FLASH mem\r\n");
			
			DSP_SendDataEx ("\nxp, set proportional gain channel 0\r\n");
			DSP_SendDataEx ("xd, set derivative gain channel 0\r\n");
			DSP_SendDataEx ("xo, set offset channel 0\r\n");
			DSP_SendDataEx ("xs, set scale channel 0\r\n");
			
			DSP_SendDataEx ("e, get current position 0\r\n");
			DSP_SendDataEx ("s, print gain settings\r\n");
			
			DSP_SendDataEx ("\nii, init trajectory channel 0\r\n");
			DSP_SendDataEx ("is, calculate next step trajectory 0\r\n");
			DSP_SendDataEx ("t, start test vmode channel 0\r\n");
			DSP_SendDataEx ("q, stop test vmode channel 0\r\n");
			DSP_SendDataEx ("l, invert test vmode channel 0\r\n");
			
			c = 0;
			break;
		
		case 't':
			_verbose = true;
			if (_control_mode == MODE_VELOCITY && _calibrated[0] && _pad_enabled[0])
			{
				DSP_SendDataEx ("velocity set point? ");
				retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
				_set_vel[0] = DSP_atol (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
				DSP_SendDataEx ("velocity set point= ");
				DSP_SendDWordAsChars (_set_vel[0]);
				DSP_SendDataEx ("\r\n");
			}
			else
				DSP_SendDataEx ("go to velocity mode and calibrate first\r\n");
			c = 0;
			break;

		case 'l':
			if (_control_mode == MODE_VELOCITY && _calibrated[0] && _pad_enabled[0])
				_set_vel[0] = -_set_vel[0];
			else
				DSP_SendDataEx ("go to velocity mode and calibrate first\r\n");
			c = 0;
			break;

		case 'q':
			_verbose = false;
			_set_vel[0] = 0;
			c = 0;
			break;
						
		case 'i':
			if (AS1_RecvChar(&d) == ERR_OK)
			{
				if (d == 'i')
				{
					/* speed is in ticks/ms */
					iretval = init_trajectory (0, _position[0], _set_point[0], _set_vel[0]);
					DSP_SendDataEx ("trajectory initialized\r\n");
					c = 0;
				}
				else
				if (d == 's')
				{
					DSP_SendDataEx ("s= ");
					DSP_SendDWordAsChars (step_trajectory(0));
					DSP_SendDataEx ("\r\n");
					c = 'i';
				}
				else
				{
					c = d = 0;
				}
			}
			break;
								
		case '1':
			if (_control_mode == MODE_IDLE)
			{
				_control_mode = MODE_POSITION;
				_set_vel[0] = DEFAULT_VELOCITY;
				_set_vel[1] = DEFAULT_VELOCITY;
				DSP_SendDataEx ("mode = position\r\n");
			}
			else
			if (_control_mode == MODE_POSITION)
			{
				_control_mode = MODE_IDLE;
				DSP_SendDataEx ("mode = idle\r\n");
			}
			else
				DSP_SendDataEx ("go to idle first\r\n");
			c = 0;
			break;
			
		case '2':
			if (_control_mode == MODE_IDLE)
			{
				_control_mode = MODE_VELOCITY;
				_set_vel[0] = 0;
				_set_vel[1] = 0;
				DSP_SendDataEx ("mode = velocity\r\n");
			}
			else
			if (_control_mode == MODE_VELOCITY)
			{
				_control_mode = MODE_IDLE;
				DSP_SendDataEx ("mode = idle\r\n");
			}
			else
				DSP_SendDataEx ("go to idle first\r\n");
			c = 0;
			break;

		case '3':
			_verbose = !_verbose;
			if (_verbose)
				DSP_SendDataEx ("verbose = true\r\n");
			else
				DSP_SendDataEx ("verbose = false\r\n");
			c = 0;
			break;
			
		case 'c':
			DSP_SendDataEx ("calibrating encoder 0\r\n");
			calibrate (0);
			DSP_SendDataEx ("done!\r\n");
			
			DSP_SendDataEx ("calibrating encoder 1\r\n");
			calibrate (1);
			DSP_SendDataEx ("done!\r\n");
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
				
		case 'p':
			if (AS1_RecvChar(&d) == ERR_OK)
			{
				if (d == '0')
				{
					DSP_SendDataEx ("set point? ");
					retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
					_set_point[0] = DSP_atol (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
					DSP_SendDataEx ("set point= ");
					DSP_SendDWordAsChars (_set_point[0]);
					DSP_SendDataEx ("\r\n");
					c = 0;
				}
				else
				if (d == '1')
				{
					c = 0;
				}
				else
					c = 0;
			}
			break;
			
		case 'v':
			if (AS1_RecvChar(&d) == ERR_OK)
			{
				if (d == '0')
				{
					DSP_SendDataEx ("velocity set point? ");
					retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
					_set_vel[0] = DSP_atol (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
					DSP_SendDataEx ("velocity set point= ");
					DSP_SendDWordAsChars (_set_vel[0]);
					DSP_SendDataEx ("\r\n");
					c = 0;
				}
				else
				if (d == '1')
				{
					c = 0;
				}
				else
					c = 0;
			}
			break;
				
		case 'x':
			if (AS1_RecvChar(&d) == ERR_OK)
			{
				if (d == 'p')
				{
					if (!_pad_enabled[0])
					{
						DSP_SendDataEx ("proportional gain? ");
						retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
						_kp[0] = (int)DSP_atoi (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
						DSP_SendDataEx ("proportional gain= ");
						DSP_SendWord16AsChars (_kp[0]);
						DSP_SendDataEx ("\r\n");
					}
					else
						DSP_SendDataEx ("PWM must be disabled first\r\n");					
					c = 0;
				}
				else
				if (d == 'd')
				{
					if (!_pad_enabled[0])
					{
						DSP_SendDataEx ("derivative gain? ");
						retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
						_kd[0] = (int)DSP_atoi (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
						DSP_SendDataEx ("derivative gain= ");
						DSP_SendWord16AsChars (_kd[0]);
						DSP_SendDataEx ("\r\n");
					}
					else
						DSP_SendDataEx ("PWM must be disabled first\r\n");					
					c = 0;
				}
				else
				if (d == 'o')
				{
					if (!_pad_enabled[0])
					{
						DSP_SendDataEx ("offset? ");
						retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
						_ko[0] = (int)DSP_atoi (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
						if (_ko[0] < -100)
							_ko[0] = -100;
						else
						if (_ko[0] > 100)
							_ko[0] = 100;
						DSP_SendDataEx ("offset= ");
						DSP_SendWord16AsChars (_ko[0]);
						DSP_SendDataEx ("\r\n");
					}
					else
						DSP_SendDataEx ("PWM must be disabled first\r\n");					
					c = 0;
				}
				else
				if (d == 's')
				{
					if (!_pad_enabled[0])
					{
						DSP_SendDataEx ("scale factor? ");
						retval = DSP_ReceiveDataEx (buffer, SMALL_BUFFER_SIZE, true);
						_kr[0] = (int)DSP_atoi (buffer, DSP_strlen(buffer, SMALL_BUFFER_SIZE));
						if (_kr[0] < 0)
							_kr[0] = 0;
						DSP_SendDataEx ("scale factor= ");
						DSP_SendWord16AsChars (_kr[0]);
						DSP_SendDataEx ("\r\n");
					}
					else
						DSP_SendDataEx ("PWM must be disabled first\r\n");					
					c = 0;
				}
				else
					c = 0;
			}
			break;					

		case 'e':
			DSP_SendDataEx ("position 0: ");
			DSP_SendDWordAsChars (_position[0]);
			DSP_SendDataEx ("\r\n");
			c = 0;
			break;
			
		case 's':
			DSP_SendDataEx ("\r\ngen settings\r\n");
			DSP_SendDataEx ("mode: ");
			DSP_SendDataEx (GetModeAsString(_control_mode));
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("verbose: ");
			DSP_SendByteAsChars (_verbose);
			DSP_SendDataEx ("\r\n");

			DSP_SendDataEx ("status channel 0\r\n");
			DSP_SendDataEx ("calibrated: ");
			DSP_SendByteAsChars (_calibrated[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("pad_enabled: ");
			DSP_SendByteAsChars (_pad_enabled[0]);
			DSP_SendDataEx ("\r\n");
			
			DSP_SendDataEx ("gain channel 0\r\n");
			DSP_SendDataEx ("p: ");
			DSP_SendWord16AsChars (_kp[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("d: ");
			DSP_SendWord16AsChars (_kd[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("i: ");
			DSP_SendWord16AsChars (_ki[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("i_limit: ");
			DSP_SendWord16AsChars (_integral_limit[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("offset: ");
			DSP_SendWord16AsChars (_ko[0]);
			DSP_SendDataEx ("\r\n");
			DSP_SendDataEx ("scale: ");
			DSP_SendWord16AsChars (_kr[0]);
			DSP_SendDataEx ("\r\n");
			c = 0;
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
