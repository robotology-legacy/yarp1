/*
 * firmware/controller application.
 *
 */

#include "dsp56f807.h"
#include "asc.h"
#include "can1.h"
#include "qd0.h"
#include "qd1.h"
#include "pwmc0.h"
#include "pwmc1.h"
#include "ti1.h"

#include "controller.h"
#include "messages.h"

/*
 * prototypes.
 */
void isrIRQA ();
int get_flash_addr (void);

/*
 * test irs.
 */
#pragma interrupt saveall
void isrIRQA ()
{
	AS1_sendCharSafe('*');
}

/* stable global data */
bool _calibrated[JN] = { false, false };
bool _pad_enabled[JN] = { false, false };
bool _verbose = false;
byte _control_mode[JN] = { MODE_IDLE, MODE_IDLE };
										/* control mode (e.g. position, velocity, etc.) */

Int32 _position[JN] = { 0, 0 };			/* encoder position */
Int32 _position_old[JN] = { 0, 0 };		/* do I need to add bits for taking into account multiple rotations */
Int32 _speed[JN] = { 0, 0 };			/* encoder speed */

Int32 _desired[JN] = { 0, 0 };			/* PID ref value, computed by the trajectory generator */
Int32 _set_point[JN] = { 0, 0 };		/* set point for position [user specified] */

Int32 _min_position[JN] = { -DEFAULT_MAX_POSITION, -DEFAULT_MAX_POSITION };
Int32 _max_position[JN] = { DEFAULT_MAX_POSITION, DEFAULT_MAX_POSITION };
										/* software position limits */
										
Int16  _desired_vel[JN] = { 0, 0 };		/* speed reference value, computed by the trajectory gen. */
Int16  _set_vel[JN] = { DEFAULT_VELOCITY, DEFAULT_VELOCITY };	
										/* set point for velocity [user specified] */
Int16  _max_vel[JN] = { DEFAULT_MAX_VELOCITY, DEFAULT_MAX_VELOCITY };
										/* assume this limit is symmetric */
										
Int16  _set_acc[JN] = { DEFAULT_ACCELERATION, DEFAULT_ACCELERATION };
										/* set point for acceleration [too low!] */
Int16  _integral[JN] = { 0, 0 };		/* store the sum of the integral component */
Int16  _integral_limit[JN] = { 0x7fff, 0x7fff };

Int16  _error[JN] = { 0, 0 };			/* actual feedback error */
Int16  _error_old[JN] = { 0, 0 };		/* error at t-1 */

Int16  _pid[JN] = { 0, 0 };				/* pid result */
Int16  _pid_limit[JN] = { 100, 100 };	/* pid limit */

Int16  _kp[JN] = { 10, 10 };			/* PID gain */
Int16  _kd[JN] = { 40, 40 };
Int16  _ki[JN] = { 0, 0 };
Int16  _ko[JN] = { 0, 0 };				/* offset */
Int16  _kr[JN] = { 3, 3 };				/* scale factor (negative power of two) */

Int16 _counter = 0;						/* used to count cycles, it resets now and then */
										/* to generate periodic events */

/*
 * version specifi global variables.
 */
#if VERSION == 0x0113
Int32  _other_position[JN] = { 0, 0 };	/* the position of the synchronized card */
Int32  _adjustment[JN] = { 0, 0 };		/* the actual adjustment (compensation) */
Int32  _delta_adj[JN] = { 0, 0 };		/* velocity over the adjustment */

bool _pending_request = false;			/* whether a request to another card is pending */
Int16  _timeout = 0;					/* used to timeout requests */
#endif

/* CAN bus communication global vars */
canmsg_t can_fifo[CAN_FIFO_LEN];
Int16 write_p = 0;
Int16 read_p = -1;					/* -1 means empty, last_read == fifo_ptr means full */
canmsg_t _canmsg;					/* buffer to prepare messages for send */

byte	_board_ID = DEFAULT_BOARD_ID;	/* */
byte	_general_board_error = ERROR_NONE;

volatile bool _wait = true;				/* wait on timer variable */

extern bool _ended[];					/* trajectory completed flag */
#define IS_DONE(jj) (_ended[jj])

/* Local prototypes */
Int16 compute_pid2(byte j);
void print_version(void);

#if VERSION == 0x0113
void can_send_request(void);
#endif

/*
 * compute PID control (integral is not yet implemented).
 */
Int16 compute_pid2(byte j)
{
	Int32 ProportionalPortion, PIDoutput;
	Int16 InputError;

	/* the error @ previous cycle */
	_error_old[j] = _error[j];

	PIDoutput = L_sub(_desired[j], _position[j]);
	
	if (PIDoutput > MAX_16)
		InputError = MAX_16;
	else
	if (PIDoutput < MIN_16) 
		InputError = MIN_16;
	else
	{
		InputError = extract_l(PIDoutput);
	}
	
	ProportionalPortion = L_mult(_kp[j], InputError) >> _kr[j];
	_error[j] = InputError;

	PIDoutput = L_sub(L_deposit_l(InputError), L_deposit_l(_error_old[j]));
	
	if (PIDoutput > MAX_16)
		InputError = MAX_16;
	else
	if(PIDoutput < MIN_16)  
		InputError = MIN_16;
	else
	{
		InputError = extract_l(PIDoutput);
	}
	
	PIDoutput = L_mult(_kd[j], InputError) >> _kr[j];
	PIDoutput = L_add(PIDoutput, ProportionalPortion);

	if (PIDoutput > _pid_limit[j])
    	_pid[j] = _pid_limit[j];
	else
    if (PIDoutput < -_pid_limit[j])
		_pid[j] =  -_pid_limit[j];
	else
	{
		_pid[j] = extract_l(PIDoutput);
	}
		
	return 0; //(extract_l(PIDoutput));
}


/*
 * 
 */
Int32 step_velocity (byte jj)
{
	Int32 u0;
	Int16 dv, da;
	
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

#if 0
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
		AS1_printStringEx ("Error while writing to flash memory, pls try again\r\n");
			
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
#endif


dword BYTE_C(byte x4, byte x3, byte x2, byte x1)
{
	dword ret;
	word *p = (word *)&ret;
	*p++ = __shl(x3,8) | x4;
	*p++ = __shl(x1,8) | x2;
	return ret;
}

#define DUTYCYCLE(axis, ch, value) \
 if (axis == 0) \
	PWMC0_setDutyPercent (ch, value); \
 else \
	PWMC1_setDutyPercent (ch, value);
 
#define LOADDUTYCYCLE(axis) \
 if (axis == 0) \
	PWMC0_load(); \
 else \
	PWMC1_load();

/*
 * helper function to generate PWM values according to controller status.
 */
void generatePwm (byte i)
{
	Int32 cd;
	
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
		compute_pid2 (i);
		
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

void print_version(void)
{
	AS1_printStringEx ("\r\n\n");
	AS1_printStringEx ("Firmware - ver ");
#if VERSION == 0x0111
	AS1_printStringEx ("1.11");
#elif VERSION == 0x0112
	AS1_printStringEx ("1.12");
#elif VERSION == 0x0113
	AS1_printStringEx ("1.13");
#else
#	error "No valid version specified"
#endif
	AS1_printStringEx ("\r\n");
}

#if VERSION == 0x0113
void can_send_request(void)
{
	const byte _neighbor = 11;

	if (_counter == 0)
	{
		if (!_pending_request)
		{
			_timeout = 0;
			_canmsg.CAN_messID &= 0xfffff800;
			_canmsg.CAN_messID |= (_neighbor << 7);
			_canmsg.CAN_messID |= CAN_GET_ACTIVE_ENCODER_POSITION;
			_canmsg.CAN_data[0] = _board_ID;
			
			//while (CAN1_GetStateTX () == 0) ;
			
			_canmsg.CAN_length = 1;
			_canmsg.CAN_frameType = DATA_FRAME;
			if (CAN1_sendFrame (1, _canmsg.CAN_messID, _canmsg.CAN_frameType, _canmsg.CAN_length, _canmsg.CAN_data) != ERR_OK)
				AS1_printStringEx("send err\r\n");
			_pending_request = true;
		}
	}
	
	_timeout ++;
	if (_timeout > 10) // these are 10 ms.
	{
		_timeout = 0;
		_pending_request = false;
		//if (_verbose) 
			AS1_printStringEx ("timeout\r\n");
	}
}
#endif

/* defined by the linker */
extern _data_ROM2_addr;
asm int get_flash_addr (void)
{
	move #_data_ROM2_addr, y0
	rts
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
	Int32 acceptance_code = 0x0;
	Int16 flash_addr = get_flash_addr();	
	
	/* enable interrupts */
	setReg(SYS_CNTL, 0);
	setRegBits(IPR, 0xfe12);	/* enable all interrupts and IRQA, IRQB */
	__ENIGROUP (52, 4);
	__ENIGROUP (53, 4);
	__ENIGROUP (50, 4);
	__ENIGROUP (51, 4);
	__ENIGROUP (14, 4);
	__ENIGROUP (15, 4);
	__ENIGROUP (42, 4);
		
	AS1_init ();
	CAN1_init ();
	QD0_init ();
	QD1_init ();	
	PWMC0_init ();
	PWMC1_init ();
	TI1_init ();
		
	__EI();
	
	print_version ();
	AS1_printStringEx ("\r\n");
	
	/* initialization */
	//PWMC0_enable ();
	//PWMC1_enable ();  
	QD0_initPosition ();
	QD1_initPosition ();

	/* reads the PID parameters from flash memory */
	//readFromFlash ();

	/* CAN masks/filters init */
	CAN1_setAcceptanceMask (0xffffff0f);
	acceptance_code = L_deposit_l (_board_ID) << 4;
	CAN1_setAcceptanceCode (acceptance_code);
	
	/* reset encoders, LATER: should do something more than this */
	calibrate(0);
	calibrate(1);

	/* reset trajectory generation */
	abort_trajectory (0, 0);
	abort_trajectory (1, 0);
	
	/* main control loop */
	for(_counter = 0;; _counter ++) 
	{
		if (_counter >= CAN_SYNCHRO_STEPS) _counter = 0;
		while (_wait) ;
		
		/* read commands from CAN or serial board */
		serial_interface ();
		can_interface ();
		
#if VERSION == 0x0113
		/* used to ask information about neighbor cards */
		can_send_request ();
#endif

		/* instructions are executed for both axes and only the PWM isn't 
		   used if the specific axis is not used/calibrated
		   we're supposed to guarantee a constant loop period 
		   the serial line and other devices shouldn't be doing
		   much processing in case they receive external commands */

		/* read encoders, 32 bit values */
		_position_old[0] = _position[0];
		_position_old[1] = _position[1];
		QD0_getPosition ((dword *)_position);
		QD1_getPosition ((dword *)(_position+1));

#if VERSION == 0x0112
		/* (de)couple encoder readings */
		_position[0] = L_sub(_position[0], _position[1]);
		
#ifdef DEBUG_TRAJECTORY
		if (_verbose && _counter == 0)
		{
			AS1_printDWordAsCharsDec (_position[0]);
			AS1_printStringEx (" ");
			AS1_printDWordAsCharsDec (_position[1]);
			AS1_printStringEx ("\r\n");
		}
#endif

#elif VERSION == 0x0113
#ifdef DEBUG_TRAJECTORY
		if (_verbose && _counter == 0)
		{
			AS1_printDWordAsCharsDec (_position[0]);
			AS1_printStringEx (" ");
			AS1_printDWordAsCharsDec (_adjustment[0]);
			AS1_printStringEx (" ");
			AS1_printDWordAsCharsDec (_adjustment[1]);
			AS1_printStringEx ("\r\n");
		}
#endif
		
		/* beware of the first cycle when _old has no meaning */		
		_position[0] = L_add(_position[0], _adjustment[0] >> 1);
		_position[0] = L_sub(_position[0], _adjustment[1] >> 2);  // last >>2 must be 11/41
				
		_adjustment[0] = L_add(_adjustment[0], _delta_adj[0]);
		_adjustment[1] = L_add(_adjustment[1], _delta_adj[1]);
#endif

		/* this can be useful to estimate speed later on */
		_speed[0] = _position[0] - _position_old[0]; /// divide by CONTROLLER_PERIOD
		_speed[1] = _position[1] - _position_old[1];
		
		/* read ADC or other ports */
		/* to be inserted here */

		/*
		 * 
		 */		  
		generatePwm (0);
		generatePwm (1);

		if (_verbose)
		{
			AS1_printDWordAsCharsDec (_desired[0]);
			AS1_printStringEx (" ");
			AS1_printWord16AsChars (_pid[0]);
			AS1_printStringEx ("\r\n");
		}
		
		
		/* do extra functions, communicate, etc. */
		/* LATER */
		
		/* tells that the control cycle is completed */
		_wait = true;	
		
	} /* end for(;;) */
}


/* this function might not be required */
byte calibrate (byte jnt)
{
	if (jnt == 0)
	{
		_desired[0] = 0;
		_set_point[0] = 0;
		if (QD0_initPosition() == ERR_OK)
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
		if (QD1_initPosition() == ERR_OK)
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
	CAN_TEMP16 = (word)extract_l(x); \
	if (((CAN_TEMP16 & 0x0780) >> 7) != _board_ID) \
	{ \
		return ERR_OK; \
	} \
	switch (CAN_TEMP16 & 0x7f) \
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
	AS1_printString (&c, 1);
	AS1_printStringEx (": ");
	
	for (i = 0; i < len; i++)
	{
		AS1_printByteAsChars (data[i]);
		AS1_printStringEx (" ");
	}
	AS1_printStringEx ("\r\n");
}

#define SEND_BOOL(x) ((x) ? AS1_printStringEx("1") : AS1_printStringEx("0"))

void print_can_error(CAN1_TError *e)
{
	AS1_printStringEx ("f: ");
	SEND_BOOL(e->errName.BusOff);
	SEND_BOOL(e->errName.TxPassive);
	SEND_BOOL(e->errName.RxPassive);
	SEND_BOOL(e->errName.TxWarning);
	SEND_BOOL(e->errName.RxWarning);
	SEND_BOOL(e->errName.OverRun);
	AS1_printStringEx ("\r\n");
}

CAN1_TError err;

/* */
byte can_interface (void)
{
	bool done = false;
	word i;
	
	CAN_DI;
	if (read_p != -1)
	{
		CAN_EI;
		while (!done)
		{
			canmsg_t *p;
			CAN_DI;	
			p = can_fifo + read_p;
			
			/* makes a private copy of the message */
			_canmsg.CAN_data[0] = p->CAN_data[0];
			_canmsg.CAN_data[1] = p->CAN_data[1];
			_canmsg.CAN_data[2] = p->CAN_data[2];
			_canmsg.CAN_data[3] = p->CAN_data[3];
			_canmsg.CAN_data[4] = p->CAN_data[4];
			_canmsg.CAN_data[5] = p->CAN_data[5];
			_canmsg.CAN_data[6] = p->CAN_data[6];
			_canmsg.CAN_data[7] = p->CAN_data[7];
			_canmsg.CAN_messID = p->CAN_messID;
			_canmsg.CAN_frameType = p->CAN_frameType;
			_canmsg.CAN_frameFormat = p->CAN_frameFormat;
			_canmsg.CAN_length = p->CAN_length;
			
			if (read_p == write_p)
			{
				read_p = -1;	/* empty */
				done = true;
			}
			else
			{
				read_p ++;
				if (read_p >= CAN_FIFO_LEN)
					read_p = 0;
				//done = true;
			}
			CAN_EI;
			
#ifdef DEBUG_CAN_MSG
		if (_verbose)
		{
			AS1_printStringEx ("id: ");
			AS1_printWord16AsChars (read_p);
			AS1_printStringEx (" ");
			AS1_printDWordAsChars (_canmsg.CAN_messID);
			AS1_printStringEx (" ");
			print_can (_canmsg.CAN_data, _canmsg.CAN_length, 'i');
			CAN1_getError (&err);
			print_can_error (&err);
		}
#endif

#define CAN_DATA _canmsg.CAN_data
#define CAN_FRAME_TYPE _canmsg.CAN_frameType
#define CAN_FRAME_FMT _canmsg.CAN_frameFormat
#define CAN_LEN _canmsg.CAN_length
#define CAN_ID _canmsg.CAN_messID
#define CAN_TEMP16 i

			/* interpret the messages */
			//BEGIN_SPECIAL_MSG_TABLE (CAN_data)
			//HANDLE_MSG (CAN_SET_BOARD_ID, CAN_SET_BOARD_ID_HANDLER)
			//END_SPECIAL_MSG_TABLE
			
			BEGIN_MSG_TABLE (_canmsg.CAN_messID)
			HANDLE_MSG (CAN_NO_MESSAGE, CAN_NO_MESSAGE_HANDLER)
			HANDLE_MSG (CAN_CONTROLLER_RUN, CAN_CONTROLLER_RUN_HANDLER)
			HANDLE_MSG (CAN_CONTROLLER_IDLE, CAN_CONTROLLER_IDLE_HANDLER)
			HANDLE_MSG (CAN_TOGGLE_VERBOSE, CAN_TOGGLE_VERBOSE_HANDLER)
			HANDLE_MSG (CAN_CALIBRATE_ENCODER, CAN_CALIBRATE_ENCODER_HANDLER)
			HANDLE_MSG (CAN_ENABLE_PWM_PAD, CAN_ENABLE_PWM_PAD_HANDLER)
			HANDLE_MSG (CAN_DISABLE_PWM_PAD, CAN_DISABLE_PWM_PAD_HANDLER)
			HANDLE_MSG (CAN_GET_CONTROL_MODE, CAN_GET_CONTROL_MODE_HANDLER)
			HANDLE_MSG (CAN_MOTION_DONE, CAN_MOTION_DONE_HANDLER)
			
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
			HANDLE_MSG (CAN_GET_PID_ERROR, CAN_GET_PID_ERROR_HANDLER)
			
			HANDLE_MSG (CAN_SET_MIN_POSITION, CAN_SET_MIN_POSITION_HANDLER)
			HANDLE_MSG (CAN_GET_MIN_POSITION, CAN_GET_MIN_POSITION_HANDLER)
			HANDLE_MSG (CAN_SET_MAX_POSITION, CAN_SET_MAX_POSITION_HANDLER)
			HANDLE_MSG (CAN_GET_MAX_POSITION, CAN_GET_MAX_POSITION_HANDLER)
			HANDLE_MSG (CAN_SET_MAX_VELOCITY, CAN_SET_MAX_VELOCITY_HANDLER)
			HANDLE_MSG (CAN_GET_MAX_VELOCITY, CAN_GET_MAX_VELOCITY_HANDLER)
		
			HANDLE_MSG (CAN_SET_TLIM, CAN_SET_TLIM_HANDLER)
			HANDLE_MSG (CAN_GET_TLIM, CAN_GET_TLIM_HANDLER)
			HANDLE_MSG (CAN_GET_ERROR_STATUS, CAN_GET_ERROR_STATUS_HANDLER)
			
			HANDLE_MSG (CAN_GET_ACTIVE_ENCODER_POSITION, CAN_GET_ACTIVE_ENCODER_POSITION_HANDLER)
#if VERSION == 0x0113
			HANDLE_MSG (CAN_SET_ACTIVE_ENCODER_POSITION, CAN_SET_ACTIVE_ENCODER_POSITION_HANDLER)
#endif
			END_MSG_TABLE		

#ifdef DEBUG_CAN_MSG
			if (_verbose)
			{
				AS1_printStringEx ("id: ");
				AS1_printDWordAsChars (_canmsg.CAN_messID);
				AS1_printStringEx (" ");
				print_can (_canmsg.CAN_data, _canmsg.CAN_length, 'o'); 
			}
#endif
	
		} /* end of while() */
		
	} /* end of if () */
	else
	{
		CAN_EI;
	}
			
	return ERR_OK;
}

byte c = 0;

/* test/debug serial port interface (on AS1) */
byte serial_interface (void)
{
	Int32 acceptance_code;
	byte d = 0;
	char buffer[SMALL_BUFFER_SIZE];
	int  iretval = 0;
	
	if (c == 0)
		AS1_recvChar(&c);
	
	switch (c)
	{
		default:
			c = 0;
			break;
			
		case 'h':
		case 'H':
		case '\r':
			print_version ();
			AS1_printStringEx ("h, H: help\r\n");
			AS1_printStringEx ("a, set card address\r\n");
			AS1_printStringEx ("b, print card address\r\n");
			AS1_printStringEx ("w2, write control params to FLASH mem\r\n");
			AS1_printStringEx ("w3, read control params from FLASH mem\r\n");
			AS1_printStringEx ("v, toggle verbose flag\r\n");
			
			AS1_printStringEx ("e, enable controller\r\n");
			AS1_printStringEx ("g, set pid gain\r\n");
			AS1_printStringEx ("s, show pid gain\r\n");
			AS1_printStringEx ("x1, start trajectory generation\r\n");
			AS1_printStringEx ("x2, stop trajectory generation\r\n");
			AS1_printStringEx ("x3, enable/disable PWM\r\n");
						
			c = 0;
			break;

		case 's':
			AS1_printStringEx ("gain, p= ");
			AS1_printWord16AsChars (_kp[0]);
			AS1_printStringEx (" d= ");
			AS1_printWord16AsChars (_kd[0]);
			AS1_printStringEx (" scale= ");
			AS1_printWord16AsChars (_kr[0]);
			AS1_printStringEx ("\r\n");
			c = 0;
			break;
			
		case 'e':
			if (_control_mode[0] == MODE_IDLE)
			{
				_control_mode[0] = MODE_POSITION;
				_calibrated[0] = true;
				AS1_printStringEx ("mode = position\r\n");
			}
			else
			{
				_control_mode[0] = MODE_IDLE;
				_calibrated[0] = false;
				AS1_printStringEx ("mode = idle\r\n");
			}
			
			c = 0;
			break;
			
		case 'x':
			if (AS1_recvChar(&d) == ERR_OK)
			{
				if (d == '1')
				{
					AS1_printStringEx ("position: ");
					AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true);
					iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); 
					
					AS1_printStringEx ("move: ");
					_set_point[0] = 0;
					_set_point[0] = L_deposit_l(iretval);
					AS1_printDWordAsCharsDec (iretval);
					_set_vel[0] = 10;
					_set_acc[0] = 0;
					AS1_printStringEx (" 10\r\n");
					init_trajectory (0, _position[0], _set_point[0], _set_vel[0]);
				}
				else
				if (d == '2')
				{
					abort_trajectory (0, _position[0]);
					AS1_printStringEx ("trajectory aborted\r\n");
				}
				else
				if (d == '3')
				{
					if (_pad_enabled[0] == true)
						PWMC0_outputPadDisable();
					else
						PWMC0_outputPadEnable();
						
					_pad_enabled[0] = !_pad_enabled[0];
				}

				c = 0;
			}
		
			break;
			
		case 'g':
			AS1_printStringEx ("p gain [");
			AS1_printWord16AsChars (_kp[0]);
			AS1_printStringEx ("] : ");
			AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true);
			iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); 
			
			_kp[0] = iretval;
			
			AS1_printStringEx ("d gain [");
			AS1_printWord16AsChars (_kd[0]);
			AS1_printStringEx ("] : ");
			AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true);
			iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); 

			_kd[0] = iretval;

			AS1_printStringEx ("scale factor [");
			AS1_printWord16AsChars (_kr[0]);
			AS1_printStringEx ("] : ");
			
			AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true);
			iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); 

			_kr[0] = iretval;
			
			c = 0;
			break;
			
		case 'v':
			_verbose = !_verbose;
			if (_verbose)
				AS1_printStringEx ("verbose is now ON\r\n");
			else
				AS1_printStringEx ("verbose is now OFF\r\n");
				
			c = 0;
			break;
			
		case 'a':
			AS1_printStringEx ("address [1-15]: ");
			AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true);
			iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); 
			AS1_printStringEx ("address is ");
			AS1_printWord16AsChars (iretval);
			AS1_printStringEx ("\r\n");
			
			if (iretval >= 1 && iretval <= 15)
				_board_ID = iretval & 0x0f;

			/* CAN masks/filters init */
			CAN1_setAcceptanceMask (0xffffff0f);
			acceptance_code = 0;
			acceptance_code = L_deposit_l (_board_ID) << 4;
			CAN1_setAcceptanceCode (acceptance_code);
			
			c = 0;
			break;

		case 'b':
			AS1_printStringEx ("address is ");
			iretval = BYTE_W(_board_ID, 0);
			AS1_printWord16AsChars (iretval);
			AS1_printStringEx ("\r\n");
			
			c = 0;
			break;
		
		case 'w':
			if (AS1_recvChar(&d) == ERR_OK)
			{
				if (d == '0')
				{
					if (_calibrated[0])
					{
						if (!_pad_enabled[0])
						{
							PWMC0_outputPadEnable();
							AS1_printStringEx ("pad channel 0 enabled\r\n");
						}
						else
						{
							PWMC0_outputPadDisable();
							AS1_printStringEx ("pad channel 0 disabled\r\n");
						}
						_pad_enabled[0] = !_pad_enabled[0];
					}
					else
						AS1_printStringEx ("need to calibrate channel 0 first\r\n");
				}
				else
				if (d == '1')
				{
					if (_calibrated[1])
					{
						if (!_pad_enabled[1])
						{
							PWMC1_outputPadEnable();
							AS1_printStringEx ("pad channel 1 enabled\r\n");
						}
						else
						{
							PWMC1_outputPadDisable();
							AS1_printStringEx ("pad channel 1 disabled\r\n");
						}
						_pad_enabled[1] = !_pad_enabled[1];
					}
					else
						AS1_printStringEx ("need to calibrate channel 1 first\r\n");
				}
				else
				if (d == '2')
				{
					AS1_printStringEx ("writing to FLASH mem\r\n");
					//writeToFlash ();
					AS1_printStringEx ("done!\r\n");
				}
				else
				if (d == '3')
				{
					AS1_printStringEx ("reading from FLASH mem\r\n");
					//readFromFlash ();
					AS1_printStringEx ("done!\r\n");
				}

				c = 0;
			}
			break;
		
	}	/* end switch/case */
}


