// $Id: YARPGalilDeviceDriver.cpp,v 1.5 2003-05-21 14:30:05 beltran Exp $

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
	m_cmds[CMDGetErrors] 		= &YARPGalilDeviceDriver::get_errors;

	m_cmds[CMDReadInput] 		= &YARPGalilDeviceDriver::read_input;
	
	m_cmds[CMDResetController] 	= &YARPGalilDeviceDriver::reset_controller;
	m_cmds[CMDErrorLimit]		= &YARPGalilDeviceDriver::error_limit;
	m_cmds[CMDOffOnError]		= &YARPGalilDeviceDriver::off_on_error; 
	
	m_cmds[CMDVMove] 			= &YARPGalilDeviceDriver::set_jogs;
	
	m_cmds[CMDControllerIdle]	= &YARPGalilDeviceDriver::controller_idle;
	
	m_cmds[CMDSetPositiveLimit] = &YARPGalilDeviceDriver::set_positive_limit;
	m_cmds[CMDSetNegativeLimit]	= &YARPGalilDeviceDriver::set_negative_limit;
			
	m_cmds[CMDDummy] 			= &YARPGalilDeviceDriver::dummy;
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
   /* If you have more than 1 Galil PCI bus controller, use the serial
      number to identify the controller you wish to connect to */
	controllerinfo.ulSerialNumber = 0;
	controllerinfo.pid = getpid();
	
	DMCInitLibrary();
#endif

	GalilOpenParameters *p = (GalilOpenParameters *)d;

	m_all_axes = p->mask;
	m_njoints = p->nj;
#ifdef __QNX6__
	rc = DMCOpen(&controllerinfo,(HANDLEDMC *)&m_handle);
#else
	rc = DMCOpen(p->device_id, NULL, (HANDLEDMC *)&m_handle);
#endif

	m_question_marks = new char [2*m_njoints];
	m_temp_int_array = new int[m_njoints];

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

	delete [] m_question_marks;
	delete [] m_temp_int_array;

	return rc;
}

int YARPGalilDeviceDriver::set_speed(void *cmd) 
{
	long rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int *speed = (int *) tmp->parameters;

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
	buff = _append_cmd(*speed, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	
	return rc;

}

int YARPGalilDeviceDriver::set_acceleration(void *cmd) 
{
	long rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int *acceleration = (int *) tmp->parameters;

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
	buff = _append_cmd(*acceleration, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	
	return rc;

}

int YARPGalilDeviceDriver::set_position(void *cmd) 
{
	long rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int *position = (int *) tmp->parameters;

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
	buff = _append_cmd(*position, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	begin_motion(NULL);
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

	int *output = (int *) j;
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

	int *output = (int *) j;
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

	int *output = (int *) errs;
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

	int *output = (int *) spds;
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
	
	///int *speeds = (int *) spds;
	
	double * speeds_double = (double *) spds;	
	//int * speeds = new int [m_njoints];
	
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
	//int * positions = new int [m_njoints];
	
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

int YARPGalilDeviceDriver::define_positions (void *param) 
{
	long rc = 0;

	int cmd_length = 0;
	
	int *positions = (int *) param;

	char *buff = m_buffer_out;

	///////////////////////////////////////////////////////////////////
	// define position
	buff = _append_cmd((char) 0x97, buff);		//DP
	buff = _append_cmd((char) 0x04, buff);		//04 long format
	buff = _append_cmd((char) 0x00, buff);		//00 no coordinated movement
	
	// axes
	buff = _append_cmd((char) m_all_axes, buff);
	
	// values
	int n = _append_values(positions, buff);

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
	int *position = (int *) tmp->parameters;

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
	buff = _append_cmd(*position, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	
	return rc;
}

int YARPGalilDeviceDriver::set_accelerations (void *param) 
{
	long rc = 0;

	int cmd_length = 0;
	
	///int *accelerations = (int *) param;
	
	double * accelerations_double = (double *) param;	
	//int * accelerations = new int [m_njoints];
	
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
	buff = _append_cmd((char) 0xE8, buff);		//PO
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
// want to waste time (someone would say I'm lazy :)
int YARPGalilDeviceDriver::get_ref_speeds(void *spds)
{
	long rc = 0;

	int *output = (int *) spds;
	
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

	int *output = (int *) accs;
	
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
	int *error = (int *) tmp->parameters;
	
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
	buff = _append_cmd(*error, buff);

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
	int *value = (int *) tmp->parameters;
	
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
	buff = _append_cmd(*value, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	
	return rc;
}

int YARPGalilDeviceDriver::set_jogs (void *spds) 
{
	long rc = 0;

	int cmd_length = 0;
	
	///int *speeds = (int *) spds;
	
	double * speeds_double = (double *) spds;	
	//int * speeds = new int [m_njoints];
	
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
	rc = begin_motion(NULL);
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
	int *value = (int *) tmp->parameters;
	
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
	buff = _append_cmd(*value, buff);

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
	int *value = (int *) tmp->parameters;
	
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
	buff = _append_cmd(*value, buff);

	rc = DMCBinaryCommand((HANDLEDMC) m_handle,
							(unsigned char *) m_buffer_out, 8,
							m_buffer_in, buff_length);
	
	return rc;
}



