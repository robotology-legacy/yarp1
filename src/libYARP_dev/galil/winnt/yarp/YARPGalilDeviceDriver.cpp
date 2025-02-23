/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
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
//
// $Id: YARPGalilDeviceDriver.cpp,v 1.6 2007-03-14 16:52:33 beltran Exp $
//

#include "YARPGalilDeviceDriver.h"

#	include <windows.h>
#	include "../dd_orig/include/DMCCom.h"
#	include "../dd_orig/include/DMCMLIB.h"

YARPGalilDeviceDriver::YARPGalilDeviceDriver() :
YARPDeviceDriver<YARPNullSemaphore, YARPGalilDeviceDriver>(CBNCmds)
{
	// fill function pointer table
	m_cmds[CMDSetSpeed] 		= &YARPGalilDeviceDriver::set_speed;
	m_cmds[CMDSetAcceleration] 	= &YARPGalilDeviceDriver::set_acceleration;
	m_cmds[CMDSetPosition] 		= &YARPGalilDeviceDriver::set_position;
	m_cmds[CMDSetPID] 			= &YARPGalilDeviceDriver::set_pid;
	m_cmds[CMDGetPosition] 		= &YARPGalilDeviceDriver::get_position;
	m_cmds[CMDSetPortValue] 	= &YARPGalilDeviceDriver::set_output_port;
	m_cmds[CMDSetOutputBit] 	= &YARPGalilDeviceDriver::set_output_bit;
	m_cmds[CMDClearOutputBit] 	= &YARPGalilDeviceDriver::clear_output_bit;
	m_cmds[CMDSetOffset] 		= &YARPGalilDeviceDriver::set_offset;
	m_cmds[CMDSetOffsets] 		= &YARPGalilDeviceDriver::set_offsets;
	m_cmds[CMDSetSpeeds] 		= &YARPGalilDeviceDriver::set_speeds;
	m_cmds[CMDSetAccelerations] = &YARPGalilDeviceDriver::set_accelerations;
	m_cmds[CMDSetPositions] 	= &YARPGalilDeviceDriver::set_positions;
	m_cmds[CMDGetPositions] 	= &YARPGalilDeviceDriver::get_positions;
	m_cmds[CMDDefinePositions] 	= &YARPGalilDeviceDriver::define_positions;
	m_cmds[CMDDefinePosition] 	= &YARPGalilDeviceDriver::define_position;
	m_cmds[CMDStopAxes] 		= &YARPGalilDeviceDriver::stop_axes;
	//m_cmds[CMDReadSwitches] 	= &YARPGalilDeviceDriver::read_switches;
	m_cmds[CMDGetSpeeds] 		= &YARPGalilDeviceDriver::get_speeds;
	m_cmds[CMDGetRefSpeeds] 	= &YARPGalilDeviceDriver::get_ref_speeds;
	m_cmds[CMDGetRefAccelerations] = &YARPGalilDeviceDriver::get_ref_accelerations;
	m_cmds[CMDGetRefPositions] 	= &YARPGalilDeviceDriver::get_ref_positions;
	m_cmds[CMDGetPID] 			= &YARPGalilDeviceDriver::get_pid;
	m_cmds[CMDGetTorques] 		= &YARPGalilDeviceDriver::get_torques;
	m_cmds[CMDSetIntegratorLimits] = &YARPGalilDeviceDriver::set_int_limits;
	m_cmds[CMDSetTorqueLimits] 	= &YARPGalilDeviceDriver::set_torque_limits;
	m_cmds[CMDGetTorqueLimits] 	= &YARPGalilDeviceDriver::get_torque_limits;
	m_cmds[CMDGetPIDErrors] 	= &YARPGalilDeviceDriver::get_errors;
	m_cmds[CMDResetController] 	= &YARPGalilDeviceDriver::reset_controller;
	m_cmds[CMDSetErrorLimit]	= &YARPGalilDeviceDriver::error_limit;
	m_cmds[CMDOffOnError]		= &YARPGalilDeviceDriver::off_on_error; 
	m_cmds[CMDVMove] 			= &YARPGalilDeviceDriver::set_safe_jogs;
	m_cmds[CMDCheckMotionDone]	= &YARPGalilDeviceDriver::check_motion_done2;
	m_cmds[CMDControllerIdle]	= &YARPGalilDeviceDriver::controller_idle;
	m_cmds[CMDSetPositiveLimit] = &YARPGalilDeviceDriver::set_positive_limit;
	m_cmds[CMDSetNegativeLimit]	= &YARPGalilDeviceDriver::set_negative_limit;
	m_cmds[CMDSetCommands]		= &YARPGalilDeviceDriver::set_commands;
	m_cmds[CMDSetCommand]		= &YARPGalilDeviceDriver::set_command;
	m_cmds[CMDSetHomingBehavior] = &YARPGalilDeviceDriver::set_index_search;

	m_question_marks = NULL;
	m_temp_int_array = NULL;
	m_temp_double_array = NULL;
}

int YARPGalilDeviceDriver::open(void *d)
{
	// temporarly removed
	long rc = 0;
	
	GalilOpenParameters *p = (GalilOpenParameters *)d;

	m_all_axes = p->mask;
	m_njoints  = p->nj;

	rc = DMCOpen(p->device_id, NULL, (HANDLEDMC *)&m_handle);

    // Reset control board
    reset_controller(NULL);

    // Setting motor type
    int i;
    for (i=0; i < m_njoints; i++){
        SingleAxisParameters cmd;
        cmd.axis = i;
        double my_motor_type = p->motor_type[i];
        cmd.parameters = &my_motor_type;
        motor_type(&cmd);
    }

	m_question_marks = new char [2*m_njoints];
	ACE_ASSERT (m_question_marks != NULL);
	m_temp_int_array = new int[m_njoints];
	ACE_ASSERT (m_temp_int_array != NULL);
	m_temp_double_array = new double[m_njoints];
	ACE_ASSERT (m_temp_double_array != NULL);
	_current_positions = new double [m_njoints];
	ACE_ASSERT (_current_positions != NULL);
	_current_vel	   = new double [m_njoints];
	ACE_ASSERT (_current_vel != NULL);
	_current_accel	   = new double [m_njoints];
	ACE_ASSERT (_current_accel != NULL);

    _index_search = false;

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

	double *output = (double *) j;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// tell position
	buff = _append_cmd((char) 0xD9, buff);		//TP
	buff = _append_cmd((char) 0x00, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	buff = _append_cmd((char) 0x00, buff);		
	
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 4,
							m_buffer_in, buff_length);
	
	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int YARPGalilDeviceDriver::get_ref_positions(void *j)
{
	long rc = 0;

	double *output = (double *) j;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// reference position
	buff = _append_cmd((char) 0xD8, buff);		//RP
	buff = _append_cmd((char) 0x00, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	buff = _append_cmd((char) 0x00, buff);		
	
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 4,
							m_buffer_in, buff_length);
	
	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int YARPGalilDeviceDriver::get_errors(void *errs)
{
	long rc = 0;

	double *output = (double *) errs;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// tell error
	buff = _append_cmd((char) 0xDA, buff);		//TE
	buff = _append_cmd((char) 0x00, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	buff = _append_cmd((char) 0x00, buff);		
	
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 4,
							m_buffer_in, buff_length);
	
	_ascii_to_binary(m_buffer_in, output);

	return rc;
}

int YARPGalilDeviceDriver::get_torques(void *trqs)
{
	long rc = 0;

	double *output = (double *) trqs;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set TT
	buff = _append_cmd((char) 0xDE, buff);		//TT
	buff = _append_cmd((char) 0x00, buff);		//00
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	buff = _append_cmd((char) 0x00, buff);		
	
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 4,
							m_buffer_in, buff_length);
	
	_ascii_to_binary(m_buffer_in, output);

	return rc;

}

int YARPGalilDeviceDriver::get_speeds(void *spds)
{
	long rc = 0;

	double *output = (double *) spds;
	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// set SP
	buff = _append_cmd((char) 0xDC, buff);		//TV
	buff = _append_cmd((char) 0x00, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement

	buff = _append_cmd((char) 0x00, buff);		
	
	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 4,
							m_buffer_in, buff_length);
	
	_ascii_to_binary(m_buffer_in, output);

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

/*
int YARPGalilDeviceDriver::set_commands (void *param) 
{
	double max_speeds[6] = {1000000,1000000,1000000,2000,2000,2000}; 
	double max_accel[6]  = {1000000,1000000,1000000,2000,2000,2000};
	set_speeds(max_speeds);
	set_accelerations(max_accel);
	return set_positions (param); 
}
*/

int YARPGalilDeviceDriver::set_commands (void *param) 
{
	long rc = 0;
	int cmd_length = 0;
	//double max_speeds[6] = {1000000,1000000,1000000,2000,2000,2000}; 
	//double max_accel[6]  = {1000000,1000000,1000000,2000,2000,2000};
	bool motion_done = false;
	///double _jog_vels[6] = {0,0,0,0,0,0};
	double * positions_double = (double *) param;	

	///set_speeds(max_speeds);
	///set_accelerations(max_accel);
	///set_jogs(_jog_vels);
	///check_motion_done(&motion_done);

	get_ref_positions(_current_positions);
	///get_positions(_current_positions);

	for (int i = 0; i <  m_njoints; i++)
	//{
			m_temp_double_array[i] = positions_double[i] - _current_positions[i];
	//		if (m_temp_double_array[i] > 0)
	//			_jog_vels[i] = 1;
	//		else
	//			_jog_vels[i] = -1;
	//}

	//set jog mode. This is necesary to run the IP command in the adecuate mode
	double_to_int(m_temp_int_array, m_temp_double_array);
	sprintf(m_buffer_out,"IP %d,%d,%d,%d,%d,%d",m_temp_int_array[0]
											   ,m_temp_int_array[1]
											   ,m_temp_int_array[2]
											   ,m_temp_int_array[3]
											   ,m_temp_int_array[4]
											   ,m_temp_int_array[5]);
	//if(motion_done)
	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	return rc;
}

//TODO: int to double transformation
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

	// setting the encoders requires a servo_here command (set the ref position).
	buff = _append_cmd((char) 0xAA, buff);		//SH
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	cmd_length += 4;

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

	// setting the encoders requires a servo_here command (set the ref position).
	buff = _append_cmd((char) 0xAA, buff);		//SH
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);
	buff = _append_cmd((char) 0x00, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8+4,
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
	if (abort_axes (NULL) == YARP_OK)
		return servo_here (NULL);
	else
		return YARP_FAIL;

#if 0
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
#endif
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
// however these methods are executed sporadically I didn't
// want to waste time (someone would say I'm lazy :) )
//
// YES, Lorenzo, you've been lazy on this!
//
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

    /**
     * Note: This is likely to be a bug. BG is only needed the first time you issue JG. It
     * is like the firs JG changes the mode, then a BG is issued and successive
     * JG's change the velocity of the joint on the fly. To stop you can issue
     * a ST. See example 7 velocity control in pag 31 DMC-1700/1800 Manual Rev
     * 1.2.h
     */
	return rc;
}

int YARPGalilDeviceDriver::set_safe_jogs (void *spds) 
{
	long rc = 0;
	// NOT IMPLEMENTED YET !!
    // TODO: Implement adequate save jogs in the galil
    // redirecting to normal set_jogs command
    set_jogs(spds);
	return rc;
}

int YARPGalilDeviceDriver::check_frames_left (void *flag) 
{
	long rc = 0;
	// NOT IMPLEMENTED YET !!
	ACE_ASSERT(0);
	return rc;
}

int YARPGalilDeviceDriver::wait_for_frames_left(void *cmd) 
{
	long rc = 0;
	// NOT IMPLEMENTED YET !!
	ACE_ASSERT(0);
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

// FIX: This must be improved! The axes added to the _BG Operand should be variable
// Now I am checking all the axis in the control card. The axis that are not being used should return 
// "motion done" because no motion was ordered on them, therefore only the axis participating in a movement
// should contribute to the command. 
int YARPGalilDeviceDriver::check_motion_done(void *flag)
{
	long rc = 0;

	bool *tmp = (bool *)flag;
	*tmp = true;
	bool subflag = false;

	for (int i = 0; i < m_njoints; i++)
	{
		rc = check_motion_done(&subflag,i);
		*tmp = *tmp && subflag;
	}

	return rc;
/*

	long rc = 0;

	bool *tmp = (bool *) flag;
	
	char cmd[] = "MG _BGABCDEFGH";
	char *buff = m_buffer_out;

	memcpy(buff, cmd, sizeof(cmd)-1);
	buff+=(sizeof(cmd)-1);

	// close command
	buff = _append_cmd('\0', buff);
	
	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	if (atoi(m_buffer_in))
		*tmp = false;
	else	*tmp = true;
	//_ascii_to_binary(m_buffer_in, tmp);

	return rc;

*/
}

//
// flag returns true if motion on all joints has been completed.
// WARNING: improper use of the m_buffer_in as a temporary buffer.
int YARPGalilDeviceDriver::check_motion_done2 (void *flag)
{
	long rc = 0;
	rc = read_switches (m_buffer_in);
	bool _moving = false;
	int i;
	for(i = 0; i < m_njoints; i++)
		_moving = _moving || ((char) 0x80 & m_buffer_in[i]);

	*(bool *)flag = _moving;
	return rc;
}

//
//
int YARPGalilDeviceDriver::check_motion_done(void *flag, int axis)
{
	char axis_names[] = "ABCDEFGH";
	long rc = 0;
	bool *tmp = (bool *) flag;
	char cmd[] = "MG _BG ";		//<-- an space left for the axis
	cmd[6] = axis_names[axis];	//Adding the axis letter.
	char *buff = m_buffer_out;

	memcpy(buff, cmd, sizeof(cmd)); //we include also the \0 in cmd 
	buff+=(sizeof(cmd));

	// close command
	// buff = _append_cmd('\0', buff);

	rc = DMCCommand((HANDLEDMC) m_handle,
					m_buffer_out,
					m_buffer_in, buff_length);

	if (atoi(m_buffer_in))	// '1' from the card means motion running
		*tmp = false;
	else	
		*tmp = true;	// '0' means motion complete

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
