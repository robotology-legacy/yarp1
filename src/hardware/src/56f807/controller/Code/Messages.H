/*
 *
 *
 *
 *
 */
 
#ifndef __MessagesH__
#define __MessagesH__

/* 
 *
 * message handlers 
 *
 */

#define CAN_NO_MESSAGE_HANDLER(x) \
{ \
	_general_board_error = ERROR_UNSPECIFIED; \
}

#define CAN_CONTROLLER_RUN_HANDLER(x) \
{ \
	if (_control_mode == MODE_IDLE) \
	{ \
		_control_mode = MODE_POSITION; \
	} \
	else \
		_general_board_error = ERROR_MODE; \
}

#define CAN_CONTROLLER_IDLE_HANDLER(x) \
{ \
	if (_control_mode != MODE_IDLE) \
	{ \
		_control_mode = MODE_IDLE; \
	} \
	else \
		_general_board_error = ERROR_MODE; \
}

#define CAN_TOGGLE_VERBOSE_HANDLER(x) \
{ \
	_verbose = !_verbose; \
}

#define CAN_CALIBRATE_ENCODER_HANDLER(x) \
{ \
	if (CHANNEL(x[1]) == 0) \
		calibrate (0); \
	else \
		calibrate (1); \
}

#define CAN_ENABLE_PWM_PAD_HANDLER(x) \
{ \
	if (CHANNEL(x[1]) == 0) \
	{ \
		if (_calibrated[0]) \
		{ \
			PWMC1_OutputPadEnable(); \
			_pad_enabled[0] = true; \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
	else \
	{ \
		if (_calibrated[1]) \
		{ \
			PWMC2_OutputPadEnable(); \
			_pad_enabled[1] = true; \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
}

#define CAN_DISABLE_PWM_PAD_HANDLER(x) \
{ \
	if (CHANNEL(x[1]) == 0) \
	{ \
		if (_calibrated[0]) \
		{ \
			PWMC1_OutputPadDisable(); \
			_pad_enabled[0] = false; \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
	else \
	{ \
		if (_calibrated[1]) \
		{ \
			PWMC2_OutputPadDisable(); \
			_pad_enabled[1] = false; \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
}

#define CAN_GET_CONTROL_MODE_HANDLER(x) \
{ \
	if (CAN1_GetStateTX () != 0) \
	{ \
		CAN_LEN = 3; \
		CAN_DATA[0] = CAN_DATA[0] >> 4; \
		CAN_DATA[0] |= (_board_ID << 4); \
		/* CAN_DATA[1] untouched */ \
		CAN_DATA[2] = _control_mode; \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_WRITE_FLASH_MEM_HANDLER(x) \
{ \
	writeToFlash (); \
}

#define CAN_READ_FLASH_MEM_HANDLER(x) \
{ \
	readFromFlash (); \
}

#define CAN_GET_ENCODER_POSITION_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 8; \
		CAN_DATA[2] = BYTE_4(_position[i]); \
		CAN_DATA[3] = BYTE_3(_position[i]); \
		CAN_DATA[4] = BYTE_2(_position[i]); \
		CAN_DATA[5] = BYTE_1(_position[i]); \
		CAN_DATA[6] = BYTE_2(_speed[i]); \
		CAN_DATA[7] = BYTE_1(_speed[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_ENCODER_POSITION_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	int value; \
	if (CAN_LEN == 6) \
	{ \
		value = BYTE_C(CAN_DATA[2], CAN_DATA[3], CAN_DATA[4], CAN_DATA[5]); \
		if (i == 0) \
			QD1_SetPosition (value); \
		else \
			QD2_SetPosition (value); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_SET_COMMAND_POSITION_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	int value; \
	if (CAN_LEN == 6) \
		_desired[i] = BYTE_C(CAN_DATA[2], CAN_DATA[3], CAN_DATA[4], CAN_DATA[5]); \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_SET_DESIRED_POSITION_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 6) \
	{ \
		_set_point[i] = BYTE_C(CAN_DATA[2], CAN_DATA[3], CAN_DATA[4], CAN_DATA[5]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_POSITION_MOVE_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 8) \
	{ \
		if (_control_mode != MODE_IDLE && IS_DONE(i)) \
		{ \
			_control_mode = MODE_POSITION; \
			_set_point[i] = BYTE_C(CAN_DATA[2], CAN_DATA[3], CAN_DATA[4], CAN_DATA[5]); \
			_set_vel[i] = BYTE_W(CAN_DATA[6], CAN_DATA[7]); \
			init_trajectory (i, _position[i], _set_point[i], _set_vel[i]); \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_VELOCITY_MOVE_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 6) \
	{ \
		if (_control_mode != MODE_IDLE && IS_DONE(i)) \
		{ \
			_control_mode = MODE_VELOCITY; \
			_set_vel[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
			_set_acc[i] = BYTE_W(CAN_DATA[4], CAN_DATA[5]); \
		} \
		else \
			_general_board_error = ERROR_MODE; \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_DESIRED_POSITION_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 6; \
		CAN_DATA[2] = BYTE_4(_set_point[i]); \
		CAN_DATA[3] = BYTE_3(_set_point[i]); \
		CAN_DATA[4] = BYTE_2(_set_point[i]); \
		CAN_DATA[5] = BYTE_1(_set_point[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_DESIRED_VELOCITY_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_set_vel[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_DESIRED_VELOCITY_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_set_vel[i]); \
		CAN_DATA[3] = BYTE_L(_set_vel[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_GET_ENCODER_VELOCITY_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_speed[i]); \
		CAN_DATA[3] = BYTE_L(_speed[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_DESIRED_ACCELER_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_set_acc[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_DESIRED_ACCELER_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_set_acc[i]); \
		CAN_DATA[3] = BYTE_L(_set_acc[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_P_GAIN_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_kp[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_P_GAIN_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_kp[i]); \
		CAN_DATA[3] = BYTE_L(_kp[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_D_GAIN_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_kd[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_D_GAIN_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_kd[i]); \
		CAN_DATA[3] = BYTE_L(_kd[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_I_GAIN_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_ki[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_I_GAIN_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_ki[i]); \
		CAN_DATA[3] = BYTE_L(_ki[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_ILIM_GAIN_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_integral_limit[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_ILIM_GAIN_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_integral_limit[i]); \
		CAN_DATA[3] = BYTE_L(_integral_limit[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_OFFSET_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_ko[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_OFFSET_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_ko[i]); \
		CAN_DATA[3] = BYTE_L(_ko[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}

#define CAN_SET_SCALE_HANDLER(x) \
{ \
	int i = CHANNEL(x[1]); \
	if (CAN_LEN == 4) \
	{ \
		_kr[i] = BYTE_W(CAN_DATA[2], CAN_DATA[3]); \
	} \
	else \
		_general_board_error = ERROR_FMT; \
}

#define CAN_GET_SCALE_HANDLER(x) \
{ \
	CAN_DATA[0] = CAN_DATA[0] >> 4; \
	CAN_DATA[0] |= (_board_ID << 4); \
	if (CAN1_GetStateTX () != 0) \
	{ \
		int i = CHANNEL(x[1]); \
		CAN_LEN = 4; \
		CAN_DATA[2] = BYTE_H(_kr[i]); \
		CAN_DATA[3] = BYTE_L(_kr[i]); \
		CAN1_SendFrame (0, CAN_ID, CAN_FRAME_TYPE, CAN_LEN, CAN_DATA); \
	} \
	else \
		_general_board_error = ERROR_SEND; \
}


/* end of messages */
#endif

