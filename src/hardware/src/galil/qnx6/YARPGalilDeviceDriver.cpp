// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPGalilDeviceDriver.cpp
// 
//     Description:  
// 
//         Version:  $Id: YARPGalilDeviceDriver.cpp,v 1.18 2004-01-07 13:42:46 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "YARPGalilDeviceDriver.h"

#ifdef __WIN32__
#include <windows.h>
#include <sys/DMCCom.h>
#include <sys/DMCMLIB.h>
#else
#include <dmcqnx.h>
#endif

YARPGalilDeviceDriver::YARPGalilDeviceDriver() :
YARPDeviceDriver<YARPNullSemaphore, YARPGalilDeviceDriver>(CBNCmds)
{
	// fill function pointer table
	m_cmds[CMDSetSpeed] 		= &YARPGalilDeviceDriver::set_speed;
	m_cmds[CMDSetAcceleration] 	= &YARPGalilDeviceDriver::set_acceleration;
	m_cmds[CMDSetPosition] 		= &YARPGalilDeviceDriver::set_position;
	m_cmds[CMDSetPID] 			= &YARPGalilDeviceDriver::set_pid;
	m_cmds[CMDGetPosition] 		= &YARPGalilDeviceDriver::get_position;

	m_cmds[CMDSetOutputPort] 	= &YARPGalilDeviceDriver::set_output_port;
	m_cmds[CMDSetOutputBit] 	= &YARPGalilDeviceDriver::set_output_bit;
	m_cmds[CMDClearOutputBit] 	= &YARPGalilDeviceDriver::clear_output_bit;

	m_cmds[CMDSetOffset] 		= &YARPGalilDeviceDriver::set_offset;
	m_cmds[CMDSetOffsets] 		= &YARPGalilDeviceDriver::set_offsets;

	m_cmds[CMDSetSpeeds] 		= &YARPGalilDeviceDriver::set_speeds;
	m_cmds[CMDSetAccelerations] = &YARPGalilDeviceDriver::set_accelerations;
	m_cmds[CMDSetPositions] 	= &YARPGalilDeviceDriver::set_positions;
	m_cmds[CMDGetPositions] 	= &YARPGalilDeviceDriver::get_positions;

	m_cmds[CMDBeginMotion] 		= &YARPGalilDeviceDriver::begin_motion;
	m_cmds[CMDBeginMotions] 	= &YARPGalilDeviceDriver::begin_motions;

	m_cmds[CMDDefinePositions] 	= &YARPGalilDeviceDriver::define_positions;
	m_cmds[CMDDefinePosition] 	= &YARPGalilDeviceDriver::define_position;

	m_cmds[CMDStopAxes] 		= &YARPGalilDeviceDriver::stop_axes;
	m_cmds[CMDAbortAxes]		= &YARPGalilDeviceDriver::abort_axes;
	m_cmds[CMDReadSwitches] 	= &YARPGalilDeviceDriver::read_switches;

	m_cmds[CMDServoHere] 		= &YARPGalilDeviceDriver::servo_here;

	m_cmds[CMDGetSpeeds] 		= &YARPGalilDeviceDriver::get_speeds;

	m_cmds[CMDGetRefSpeeds] 	= &YARPGalilDeviceDriver::get_ref_speeds;
	m_cmds[CMDGetRefAccelerations] = &YARPGalilDeviceDriver::get_ref_accelerations;
	m_cmds[CMDGetRefPositions] 	= &YARPGalilDeviceDriver::get_ref_positions;
	m_cmds[CMDGetPID] 			= &YARPGalilDeviceDriver::get_pid;
	m_cmds[CMDGetTorques] 		= &YARPGalilDeviceDriver::get_torques;

	m_cmds[CMDSetIntegratorLimits] = &YARPGalilDeviceDriver::set_int_limits;
	m_cmds[CMDSetTorqueLimits] 	= &YARPGalilDeviceDriver::set_torque_limits;
	m_cmds[CMDGetTorqueLimits] 	= &YARPGalilDeviceDriver::get_torque_limits;
	m_cmds[CMDGetErrors] 		= &YARPGalilDeviceDriver::get_errors;

	m_cmds[CMDReadInput] 		= &YARPGalilDeviceDriver::read_input;

	m_cmds[CMDResetController] 	= &YARPGalilDeviceDriver::reset_controller;
	m_cmds[CMDErrorLimit]		= &YARPGalilDeviceDriver::error_limit;
	m_cmds[CMDOffOnError]		= &YARPGalilDeviceDriver::off_on_error; 
	
	m_cmds[CMDVMove] 			= &YARPGalilDeviceDriver::set_jogs;
	m_cmds[CMDSafeVMove]		= &YARPGalilDeviceDriver::set_safe_jogs;

	m_cmds[CMDCheckMotionDone]	= &YARPGalilDeviceDriver::check_motion_done;
	m_cmds[CMDWaitForMotionDone] = &YARPGalilDeviceDriver::waitForMotionDone;

	m_cmds[CMDControllerIdle]	= &YARPGalilDeviceDriver::controller_idle;

	m_cmds[CMDSetPositiveLimit] = &YARPGalilDeviceDriver::set_positive_limit;
	m_cmds[CMDSetNegativeLimit]	= &YARPGalilDeviceDriver::set_negative_limit;
	m_cmds[CMDAbortAxes]		= &YARPGalilDeviceDriver::abort_axes;

	m_cmds[CMDMotorType]		= &YARPGalilDeviceDriver::motor_type;
	
	m_cmds[CMDGetMotorType]		= &YARPGalilDeviceDriver::get_motor_type;

	m_cmds[CMDSetCommands]		= &YARPGalilDeviceDriver::set_commands;

	m_cmds[CMDSetCommand]		= &YARPGalilDeviceDriver::set_command;

	m_cmds[CMDCheckFramesLeft]  = &YARPGalilDeviceDriver::check_frames_left;
	m_cmds[CMDWaitForFramesLeft] = &YARPGalilDeviceDriver::wait_for_frames_left;
	m_cmds[CMDSetDR]			= &YARPGalilDeviceDriver::set_dr;
	m_cmds[CMDIndexSearch]		= &YARPGalilDeviceDriver::set_index_search;

	m_cmds[CMDDummy] 			= &YARPGalilDeviceDriver::dummy;

	m_question_marks = NULL;
	m_temp_int_array = NULL;

	m_temp_double_array = NULL;
}

int YARPGalilDeviceDriver::open(void *d)
{
	// temporarly removed
	long rc = 0;

#ifdef __QNX6__
	CONTROLLERINFO controllerinfo;

	memset(&controllerinfo, '\0', sizeof(controllerinfo));

	controllerinfo.cbSize = sizeof(controllerinfo);
	controllerinfo.usModelID = MODEL_1800;
	controllerinfo.fControllerType = ControllerTypePCIBus;
	controllerinfo.ulTimeout = 2000;
	controllerinfo.ulDelay = 0;

	/* If you have more than 1 Galil PCI bus controller, use the serial
	   number to identify the controller you wish to connect to */

	controllerinfo.ulSerialNumber = 0;
	controllerinfo.pid = getpid();
	//This is needed to use the second FIFO
	controllerinfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO; 

	DMCInitLibrary();

	DMCDiagnosticsOn((HANDLEDMC) m_handle, "TEST.LOG", 0); //This is necesary for the 2 FIFO to work
	//Why is so, it is a nice mistery.

#endif
	GalilOpenParameters *p = (GalilOpenParameters *)d;

	m_all_axes = p->mask;
	m_njoints = p->nj;
#ifdef __QNX6__
	rc = DMCOpen(&controllerinfo,(HANDLEDMC *)&m_handle);
#else
#ifdef __WIN32__
	rc = DMCOpen(p->device_id, NULL, (HANDLEDMC *)&m_handle);
#endif
#endif

	m_question_marks = new char [2*m_njoints];
	m_temp_int_array = new int[m_njoints];
	m_temp_double_array = new double[m_njoints];
	data = new long[m_njoints]; 

	_last_ordered_positions= new double [m_njoints];
	_current_positions = new double [m_njoints];
	_current_vel	   = new double [m_njoints];
	_current_accel	   = new double [m_njoints];

	m_max_vel = false;
	_index_search = false;

	int i;
	int j;
	for (i=0,j=0; i<m_njoints; i++)
	{
		m_question_marks[j] = '?';
		j++;
		m_question_marks[j] = ',';
		j++;
	}
	// close the string; overwrite the last ',' with a '\0'
	m_question_marks[j-1] = '\0';

	return rc;
}

int YARPGalilDeviceDriver::close(void) 
{
	long rc = 0;

	rc = DMCClose((HANDLEDMC) m_handle);
	m_handle = NULL;

	if (m_question_marks != NULL)
		delete [] m_question_marks;
	if (m_temp_int_array != NULL)
		delete [] m_temp_int_array;
	if (m_temp_double_array != NULL)
		delete [] m_temp_double_array;
	if (data != NULL)
		delete [] data;
	if (_last_ordered_positions != NULL)
		delete [] _last_ordered_positions;
	if (_current_positions != NULL)
		delete [] _current_positions;
	if (_current_vel != NULL)
		delete [] _current_vel;
	if (_current_accel != NULL)
		delete [] _current_accel;

	return rc;
}

int YARPGalilDeviceDriver::set_speed(void *cmd) 
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	// int *speed = (int *) tmp->parameters;
	double *speed = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x92, buff);		//SP
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	buff = _append_cmd_as_int(*speed, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;

}

int YARPGalilDeviceDriver::set_acceleration(void *cmd) 
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *acceleration = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set acceleration
	buff = _append_cmd((char) 0x90, buff);		//AC
	buff = _append_cmd((char) 0x04, buff);		//04 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd_as_int(*acceleration, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_position(void *cmd) 
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *position = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set PA
	buff = _append_cmd((char) 0xA6, buff);		//PA
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd_as_int(*position, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);
	begin_motion(NULL);
	return rc;
}

/* TO BE REVISED */

int YARPGalilDeviceDriver::set_command(void *cmd) 
{
	long rc = 0;
	bool motion_done=false;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *position = (double *) tmp->parameters;
	double m_position = *position;

	//The position passed to the set_command is in absolute coordinates
	//we need to transform it (into relative) in order to use the IP command	

	get_ref_positions(_current_positions);

	m_position = m_position - _current_positions[tmp->axis];

	//set jog mode. This is necesary to run the IP command in the adecuate mode
	set_jog_mode();

	//The IP command is cumulative so we have to check if it has finished
	//check_motion_done(&motion_done,tmp->axis);

	char *buff = m_buffer_out;
	char *buff_ini = m_buffer_out;

	//A binary version of the IP command does not seem to exist. I am using the
	//ASCII version. This will slow down the command, but, do we have any other 
	//alternative??

	buff = _append_cmd("IP",buff);
	buff = _append_commas(buff, tmp->axis);
	buff = _append_cmd(itoa((int) m_position,m_aux_buffer,10),buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	return rc;
}


int YARPGalilDeviceDriver::set_pid(void *cmd)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set KP
	buff = _append_cmd((char) 0x81, buff);		//KP
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd(pid->KP, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 10,
						  m_buffer_in, buff_length);

	///////////////////////////////////////////////////////////////////
	// set KI
	buff = m_buffer_out;

	buff = _append_cmd((char) 0x82, buff);		//KI
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis, use the same as before
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd(pid->KI, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 10,
						  m_buffer_in, buff_length);

	///////////////////////////////////////////////////////////////////
	// set KD
	buff = m_buffer_out;

	buff = _append_cmd((char) 0x83, buff);		//KD
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis, use the same as before
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd(pid->KD, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 10,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::get_positions(void *j)
{
	long rc = 0;
	unsigned short data_type;

	double *output = (double *)j;

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisMotorPosition,i+1,&data_type,data + i);
		*(output+i) = *(data+i);
	}

	return rc;
}

int YARPGalilDeviceDriver::get_ref_positions(void *j)
{
	long rc = 0;
	unsigned short data_type;

	double *output = (double *) j;


	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisReferencePosition,i+1,&data_type,data + i);
		*(output+i) = *(data+i);
	}

	return rc;
}

int YARPGalilDeviceDriver::get_errors(void *errs)
{
	long rc = 0;
	unsigned short data_type;

	double *output = (double *) errs;

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisPositionError,i+1,&data_type,data + i);
		*(output+i) = *(data+i);
	}

	return rc;
}

int YARPGalilDeviceDriver::get_torques(void *trqs)
{
	long rc = 0;
	unsigned short data_type;

	double *output = (double *) trqs;

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisTorque,i+1,&data_type,data + i);
		*(output+i) = *(data+i);
		//This is to transform in the 0.0 - 9.9 format. 
		*(output+i) *= 0.000307;
	}

	return rc;
}

int YARPGalilDeviceDriver::get_speeds(void *spds)
{
	long rc = 0;
	unsigned short data_type;

	double *output = (double *) spds;

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisVelocity,i+1,&data_type,data + i);
		*(output+i) = *(data+i);
	}

	return rc;
}

int YARPGalilDeviceDriver::get_position(void *j)
{
	long rc = 0;

	// not implemented yet !

	rc = -1;

	return rc;
}

int YARPGalilDeviceDriver::set_speeds (void *spds) 
{
	long rc = 0;
	int cmd_length = 0;
	double * speeds_double = (double *) spds;	
	double_to_int(m_temp_int_array, speeds_double);
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x92, buff);		//SP
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values (m_temp_int_array, buff);
	cmd_length = 4 + 4*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	return rc;
}

int YARPGalilDeviceDriver::set_positions (void *param) 
{
	long rc = 0;
	int cmd_length = 0;
	double * positions_double = (double *) param;	
	double_to_int(m_temp_int_array, positions_double);
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set pos
	buff = _append_cmd((char) 0xA6, buff);		//
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values(m_temp_int_array, buff);

	cmd_length = 4 + 4*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	rc = begin_motion(NULL);							
	return rc;
}

int YARPGalilDeviceDriver::set_commands (void *param) 
{

	double max_speeds[6] = {1000000,1000000,1000000,2000,2000,2000}; 
	double max_accel[6]  = {1000000,1000000,1000000,2000,2000,2000};

	if (!m_max_vel)
	{
		set_speeds(max_speeds);
		set_accelerations(max_accel);
		m_max_vel = true;
	}

	return set_positions (param); 
}

int YARPGalilDeviceDriver::define_positions (void *param) 
{
	long rc = 0;

	int cmd_length = 0;

	double *positions = (double *) param;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// define position
	buff = _append_cmd((char) 0x97, buff);		//DP
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values_as_int(positions, buff);

	cmd_length = 4 + 4*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	return rc;
}

int YARPGalilDeviceDriver::define_position (void *cmd) 
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *position = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// define position
	buff = _append_cmd((char) 0x97, buff);		//DP
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd_as_int(*position, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_accelerations (void *param) 
{
	long rc = 0;

	int cmd_length = 0;

	double * accelerations_double = (double *) param;	

	double_to_int(m_temp_int_array, accelerations_double);

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set acc
	buff = _append_cmd((char) 0x90, buff);		//AC
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values(m_temp_int_array, buff);

	cmd_length = 4 + 4*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	return rc;
}

// word is a short
// bit 1-8 is IOG2, 9-16 is IOG1
// e.g. 0 = all clear, 65535 = all set
int YARPGalilDeviceDriver::set_output_port(void *cmd)
{
	long rc = 0;
	IOParameters *par = (IOParameters *) cmd;
	// port number is ignored; output bits are controlled with a 16 bit word
	// see comment above
	short sTmp = par->value;
	int cmd_length = 0;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set output port
	// The OP command sends data to the output ports of the controller. You can use
	// the output port to control external switches and relays
	//////////////////////////////////////////////////////////////////
	buff = _append_cmd((char) 0xE8, buff);		//OP
	buff = _append_cmd((char) 0x04, buff);		//04 two words
	buff = _append_cmd((char) 0x00, buff);

	// axes, it has to be 0x01 (don't know why) 
	buff = _append_cmd((char) 0x01, buff);

	buff = _append_cmd((short) 0, buff);	// first word
	buff = _append_cmd(sTmp, buff);			// output value

	cmd_length = 8;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

// Set n-th bit. n is a byte where
// 1 to 8 is IOG2
// 9 to 16 is IOG1
int YARPGalilDeviceDriver::set_output_bit(void *n)
{
	char *cTmp;
	long rc = 0;
	cTmp = (char *) n;
	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set output bit
	buff = _append_cmd((char) 0xEA, buff);		//SB
	buff = _append_cmd((char) 0x01, buff);		//01 one byte
	buff = _append_cmd((char) 0x00, buff);

	// axes, don't know why we need 0x01 ! 
	buff = _append_cmd((char) 0x01, buff);

	buff = _append_cmd(*cTmp, buff);	// output value

	cmd_length = 5;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

// Clear n-th bit. n is a byte where
// 1 to 8 is IOG2
// 9 to 16 is IOG1
int YARPGalilDeviceDriver::clear_output_bit(void *n)
{
	char *cTmp;
	long rc = 0;
	cTmp = (char *) n;
	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set output bit
	buff = _append_cmd((char) 0xEB, buff);		//CB
	buff = _append_cmd((char) 0x01, buff);		//01 one byte
	buff = _append_cmd((char) 0x00, buff);

	// axes, don't know why we need 0x01 ! 
	buff = _append_cmd((char) 0x01, buff);

	buff = _append_cmd(*cTmp, buff);	// output value

	cmd_length = 5;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_offset(void *cmd)
{
	long rc = 0;
	int cmd_length = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *off = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set offset
	buff = _append_cmd((char) 0x99, buff);		//OF
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// offset value
	buff = _append_cmd(*off, buff);

	cmd_length = 10;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_offsets(void *offs)
{
	long rc = 0;
	int cmd_length = 0;
	double *offsets = (double *) offs;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set offset
	buff = _append_cmd((char) 0x99, buff);		//OF
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values(offsets, buff);

	cmd_length = 4 + 6*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_torque_limits(void *trqs)
{
	long rc = 0;
	int cmd_length = 0;
	double *torques = (double *) trqs;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set torque limits
	buff = _append_cmd((char) 0x8A, buff);		//TL
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values(torques, buff);

	cmd_length = 4 + 6*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	return rc;
}

int YARPGalilDeviceDriver::get_torque_limits(void *trqs)
{
	long rc = 0;
	// NOT IMPL YET !
	return rc;
}

int YARPGalilDeviceDriver::set_int_limits(void *lmts)
{
	long rc = 0;
	int cmd_length = 0;
	double *int_limits= (double *) lmts;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set intergrator limits
	buff = _append_cmd((char) 0x89, buff);		//IL
	buff = _append_cmd((char) 0x06, buff);		//06 real format
	buff = _append_cmd((char) 0x00, buff);

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values(int_limits, buff);

	cmd_length = 4 + 6*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	return rc;
}

int YARPGalilDeviceDriver::read_input(void *input)
{
	long rc = 0;	
	/*
	   long *in = (long *) input;
	   USHORT usDataType;
	   rc = DMCRefreshDataRecord((HANDLEDMC) m_handle,0);
	   rc = DMCGetDataRecordByItemId((HANDLEDMC) m_handle,
	   DRIdGeneralInput0, 0,
	   &usDataType, in);
	   */
	return rc;
}

int YARPGalilDeviceDriver::dummy(void *d)
{
	long rc = 0;

	char command[] = "QR X;";
	int cmd_length = strlen(command);
	memcpy(m_buffer_out, command, cmd_length);

	// unsigned long l;

	/*
	   DMCDATARECORDQR MyDataRecordQR;
	   USHORT RecordSize;
	   rc = DMCGetDataRecordSize((HANDLEDMC) m_handle, &RecordSize);

	   USHORT tmp;
	   tmp = sizeof(MyDataRecordQR);

	   if (sizeof(MyDataRecordQR) >= RecordSize)
	   rc = DMCCommand((HANDLEDMC) m_handle,
	   "QR\r",
	   (char *) &MyDataRecordQR,
	   sizeof(MyDataRecordQR));
	   */
	/*

	   rc = DMCCommand_AsciiToBinary((HANDLEDMC) m_handle,
	   (char *) m_buffer_out, cmd_length,
	   (unsigned char *) m_buffer_in, buff_length, &l);

	   rc = DMCBinaryCommand((HANDLEDMC) m_handle,
	   (unsigned char *) m_buffer_in, l ,
	   m_buffer_out, buff_length);
	   */

	return rc;
}

int YARPGalilDeviceDriver::begin_motion(void *cmd)
{
	return begin_motions(cmd);
}

int YARPGalilDeviceDriver::begin_motions(void *cmd)
{
	long rc = 0;

	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// BG
	buff = _append_cmd((char) 0xA0, buff);		//BG
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

// decelerate to a stop
int YARPGalilDeviceDriver::stop_axes(void *par)
{
	long rc = 0;

	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// ST
	buff = _append_cmd((char) 0xA1, buff);		//ST
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

// instantaneous stop (no deceleration)
int YARPGalilDeviceDriver::abort_axes(void *par)
{
	long rc = 0;

	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// AB
	buff = _append_cmd((char) 0xA2, buff);		//AB
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

//TODO: This functions needs to be revised to use the DMCGetDataRecordByItemId

int YARPGalilDeviceDriver::read_switches(void *switches)
{
	long rc = 0;
	int cmd_length = 0;
	char *buff = m_buffer_out;
	char *output = (char *) switches;

	///////////////////////////////////////////////////////////////////
	// ST
	buff = _append_cmd((char) 0xDF, buff);		//TS
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int YARPGalilDeviceDriver::servo_here(void *par)
{
	long rc = 0;

	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// SH
	buff = _append_cmd((char) 0xAA, buff);		//SH
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;
}

// these commands are ascii; I KNOW this is probably slow...
// however these are methods which are executed once in a while and I didn't
// want to waste time (someone would say I'm lazy :) )
int YARPGalilDeviceDriver::get_ref_speeds(void *spds)
{
	long rc = 0;

	// int *output = (int *) spds;
	double *output = (double *) spds;

	char cmd[] = "SP";
	char *buff = m_buffer_out;

	memcpy(buff, cmd, sizeof(cmd)-1);
	buff+=(sizeof(cmd)-1);

	buff = _append_question_marks(buff);

	// close command
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

//TODO: This would be necesary to do it in ascii mode (DMCCommand)

int YARPGalilDeviceDriver::get_ref_accelerations(void *accs)
{
	long rc = 0;

	double *output = (double *) accs;

	char cmd[] = "AC";
	char *buff = m_buffer_out;

	memcpy(buff, cmd, sizeof(cmd)-1);
	buff+=(sizeof(cmd)-1);

	buff = _append_question_marks(buff);

	// close command
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int YARPGalilDeviceDriver::get_pid(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	char *buff;

	////////////////////////////////////////
	// KP
	buff = _append_cmd("KP", m_buffer_out);
	buff = _append_question_mark(buff, tmp->axis);
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	// output should be just one value here
	pid->KP = atof(m_buffer_in);

	///////////////////////////////////////
	// KD
	buff = _append_cmd("KD", m_buffer_out);
	buff = _append_question_mark(buff, tmp->axis);
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	// output should be just one value here
	pid->KD = atof(m_buffer_in);

	////////////////////////////////////////
	// KI
	buff = _append_cmd("KI", m_buffer_out);
	buff = _append_question_mark(buff, tmp->axis);
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	// output should be just one value here
	pid->KI = atof(m_buffer_in);

	return rc;
}

int
YARPGalilDeviceDriver::reset_controller(void *par)
{
	long rc = 0;

	char cmd[] = "RS";
	char *buff = m_buffer_out;

	memcpy(buff, cmd, sizeof(cmd)-1);
	buff+=(sizeof(cmd)-1);

	// close command
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	//_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int
YARPGalilDeviceDriver::error_limit(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	double *error = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x88, buff);		//ER
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	buff = _append_cmd_as_int(*error, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int
YARPGalilDeviceDriver::off_on_error(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	double *value = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x8D, buff);		//OE
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	buff = _append_cmd_as_int(*value, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int YARPGalilDeviceDriver::set_jog_mode()
{

	long rc = 0;

	int cmd_length = 0;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0xA8, buff);		//JG
	buff = _append_cmd((char) 0x00, buff);		//0 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement
	buff = _append_cmd((char) 0x00, buff);		//00 no axis

	cmd_length = 4;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);

	return rc;

}

int YARPGalilDeviceDriver::set_jogs (void *spds) 
{
	long rc = 0;
	int cmd_length = 0;
	double * speeds_double = (double *) spds;	
	double_to_int(m_temp_int_array, speeds_double);
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0xA8, buff);		//JG
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axes
	buff = _append_cmd((char) m_all_axes, buff);

	// values
	int n = _append_values (m_temp_int_array, buff);
	cmd_length = 4 + 4*n;

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, cmd_length ,
						  m_buffer_in, buff_length);
	//----------------------------------------------------------------------
	// If the seach index procedure has been activated it is necesary to send
	// the FI command. This is done so because of the particular procedure
	// used by the galil card to search indexes. JG->FI->BG  
	//----------------------------------------------------------------------
	if ( _index_search )
	{
		find_index();
		_index_search = false;
	}

	rc = begin_motion(NULL);
	return rc;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGalilDeviceDriver
//      Method:  set_safe_jogs()
// Description:  It is not clear how to implement this kind of control in the Galil card
// 				 For the moment I am liking the safe_velocity movement into the normal
// 				 velocity movement. This should be corrected when the correct implementation
// 				 of the safe_velocity movement is fully understood in the galil.
//--------------------------------------------------------------------------------------
int 
YARPGalilDeviceDriver::set_safe_jogs (void *spds) 
{
	long rc = 0;
	// TODO: Implement adequate save jogs in the galil
	
	set_jogs(spds);
	
	return rc;
}

int 
YARPGalilDeviceDriver::check_frames_left (void *flag) 
{
	long rc = 0;
	// NOT IMPLEMENTED YET !!
	return rc;
}

int 
YARPGalilDeviceDriver::wait_for_frames_left(void *cmd) 
{
	long rc = 0;
	// NOT IMPLEMENTED YET !!
	return rc;
}

int
YARPGalilDeviceDriver::controller_idle(void *j)
{
	long rc = 0;
	int *axis = (int *) j;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0xA9, buff);		//MO disable motor amplifiers (motor off)
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= *axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int
YARPGalilDeviceDriver::set_positive_limit(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	double *value = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x8E, buff);		//FL Forward software limit
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	buff = _append_cmd_as_int(*value, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int
YARPGalilDeviceDriver::set_negative_limit(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	double *value = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set velocity
	buff = _append_cmd((char) 0x8F, buff);		//BL Reverse software limit
	buff = _append_cmd((char) 0x04, buff);		//4 byte format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// value
	buff = _append_cmd_as_int(*value, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);

	return rc;
}

int 
YARPGalilDeviceDriver::motor_type(void * cmd)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *type = (double *) tmp->parameters;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set MT
	buff = _append_cmd((char) 0x8B, buff);		//MT
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	// axis
	unsigned char dummy = 0x01;	//bit
	dummy <<= tmp->axis;
	// axis
	buff = _append_cmd((char) dummy, buff);
	// PID value
	buff = _append_cmd_as_int(*type, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 8,
						  m_buffer_in, buff_length);
	return rc;
}

int YARPGalilDeviceDriver::get_motor_type(void *par)
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) par;
	double *type = (double *) tmp->parameters;

	char *buff;

	////////////////////////////////////////
	// KP
	buff = _append_cmd("MT", m_buffer_out);
	buff = _append_question_mark(buff, tmp->axis);
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	// output should be just one value here
	*type = atof(m_buffer_in);

	return rc;
}

int YARPGalilDeviceDriver::check_motion_done(void *flag)
{
	long rc = 0;
	bool *tmp = (bool *) flag;
	unsigned short data_type;
	long status;

	*tmp = true; //A priory we suppose that the motion is done

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);

	for (int i = 0; i < m_njoints; i++)
	{
		DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisStatus,i+1,&data_type,&status);

		//The 15th bit contains the motion status
		if (status & 0x8000)  //One motion in progress detected
		{
			*tmp = false;
			break;
		}
	}

	return rc;
}

int YARPGalilDeviceDriver::check_motion_done(void *flag, int axis)
{
	long rc = 0;
	bool *tmp = (bool *) flag;
	unsigned short data_type;
	long status;

	DMCRefreshDataRecord((HANDLEDMC) m_handle, 0);
	DMCGetDataRecordByItemId((HANDLEDMC) m_handle,DRIdAxisStatus,axis,&data_type,&status );

	//The 15th bit contains the motion status
	if ( (status & 0x8000) == 0)	//No move
		*tmp = true;
	else							//Motion in progress
		*tmp = false;

	return rc;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGalilDeviceDriver
//      Method:  waitForMotionDone 
// Description:  This method wait for a motion to be completed. A time command can
// 				 be used in order to avoid a continuous polling 
//--------------------------------------------------------------------------------------
int 
YARPGalilDeviceDriver::waitForMotionDone(void *cmd)
{
	int rc = 0;
	bool _motion_flag = false;
	int time = *((int *) cmd);

	check_motion_done(&_motion_flag);
	if (time != 0){ 
		while(!_motion_flag){
			ACE_OS::sleep(ACE_Time_Value(0,time));
			check_motion_done(&_motion_flag);
		}
	}
	else {
		while(!_motion_flag)
			check_motion_done(&_motion_flag);
	}

	return rc;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGalilDeviceDriver
//      Method:  set_dr
// Description:  The DR command is necessary to activate the secondary communications
// 				 channel in the galil device driver. The parameter accepted controls
// 				 the frequency used to update the information in the secondary buffer.
// 				 The ascii version is used because no binary version exist
//--------------------------------------------------------------------------------------
int
YARPGalilDeviceDriver::set_dr(void * value)
{
	int rc = 0;
	int * _value = (int *) value;

	char *buff = m_buffer_out;

	buff = _append_cmd("DR",buff);
	buff = _append_cmd(itoa((int)(* _value),m_aux_buffer,10),
					   buff);
	buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, 
					buff_length);
	return rc;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGalilDeviceDriver
//      Method:  activate_index_search
// Description:  This method set a boolean variable that it used by the set_jogs method
// 				 to detect if indexes have to be searched.  
//--------------------------------------------------------------------------------------
int
YARPGalilDeviceDriver::set_index_search(void * cmd)
{
	bool *tmp = (bool *) cmd;
	_index_search = *tmp; 
	return 0;	
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGalilDeviceDriver
//      Method:  find_index 
// Description:  This method activates de FI command in all the axis. It is not available 
// 				 externally. This is because the find index procedure has to follow a 
// 				 particular sequence (JG,FI,BG). Therefore, in upper levels the activate_index_search
// 				 is used.
//--------------------------------------------------------------------------------------
int
YARPGalilDeviceDriver::find_index()
{
	int rc = 0;
	char *buff = m_buffer_out;

	buff = _append_cmd((char) 0xA5, buff);		//FI
	buff = _append_cmd((char) 0x00, buff);		//00 no data fields
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement
	buff = _append_cmd((char) 0x00, buff);		//00 for all the axis

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
						  (unsigned char *) m_buffer_out, 
						  4,
						  m_buffer_in, 
						  buff_length);
	return rc;
}
