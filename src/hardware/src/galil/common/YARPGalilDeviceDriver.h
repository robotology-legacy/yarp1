//
//
// Wrapper for the Galil motor board
//
// feb 2003 -- by nat and pasa
//
// win32: link dmcmlib.lib and dmc32.lib
// $Id: YARPGalilDeviceDriver.h,v 1.7 2003-10-10 08:23:39 beltran Exp $

#ifndef __YARP_GALIL_DEVICE_DRIVER__
#define __YARP_GALIL_DEVICE_DRIVER__

#include <conf/YARPConfig.h>
#include <YARPDeviceDriver.h>
#include <YARPControlBoardUtils.h>
#include <YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>
// abstract, SYNC is the semaphore type.
//

const int buff_length = 128;

struct GalilOpenParameters
{
	GalilOpenParameters()
	{
		hwnd = 0;
		nj = 8;
		mask = (char) 0xFF;
	}

	void *hwnd;
	int nj;
	char mask;		//tells which axes are really connected
	int device_id;
};

class YARPGalilDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPGalilDeviceDriver> 
{
public:
	YARPGalilDeviceDriver();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

	//  functions
	int set_speed(void *sp);
	int set_position(void *pos);
	int set_command(void *pos);
	int set_acceleration(void *acc);
	int set_pid(void *cmd);
	int get_position(void *j);

	int set_offset(void *cmd);
	
	int set_output_port(void *word);
	int set_output_bit(void *bit);
	int clear_output_bit(void *bit);

	int set_speeds(void *spds);
	int set_positions(void *pos);
	int set_commands(void *pos);
	int set_accelerations(void *acc);
	int get_positions(void *j);
	int set_offsets(void *offs);

	int define_positions(void *pos);
	int define_position(void *cmd);

	int begin_motion(void *cmd);
	int begin_motions(void *d);

	int stop_axes(void *p);		// decelerate
	int abort_axes(void *p);	// instantaneous stop
	int read_switches(void *p);

	int servo_here(void *p);

	int get_speeds(void *sps);
	
	int get_ref_speeds(void *sps);
	int get_ref_accelerations(void *accs);
	int get_ref_positions(void *pos);
	int get_pid(void *par);

	int get_torques(void *trqs);

	int set_torque_limits(void *trqs);
	int get_torque_limits(void *trqs);
	int get_errors(void *errs);
	int set_int_limits(void *lmts);

	int read_input(void *input);
	
	int reset_controller(void *input);
	int error_limit(void *input);
	int off_on_error(void *input);
	
	int set_jogs(void *input);
	
	int controller_idle(void *input);
		
	int set_positive_limit(void * input);
	int set_negative_limit(void * input);

	int motor_type(void * input);
	int get_motor_type(void * input);
		
	int check_motion_done(void *flag);

	int dummy(void *d);	// dummy function, for debug purpose

protected:
	int m_njoints;
	char m_buffer_in[buff_length];
	char m_buffer_out[buff_length];
	int * m_temp_int_array;

	char *m_question_marks;

	unsigned char m_all_axes;

	inline char * _append_cmd(char data, char *buf);
	inline char * _append_cmd(const char *cmd, char *buf);
	inline char * _append_cmd(double data, char *buffer);
	inline char * _append_cmd_as_int(double data, char *buffer);
	inline char * _append_cmd(int data, char *buffer);
	inline char * _append_cmd(short data, char *buffer);
	inline int	_append_values(int *values, char *buffer);
	inline int	_append_values(double *values, char *buffer);
	inline int	_append_values_as_int(double *values, char *buffer);
	inline char * _append_question_marks(char *buffer);
	inline char * _append_question_mark(char *buffer, int axis);
	inline char * _append_question_marks(char *buffer, char mask);
	inline void _ascii_to_binary(const char *in, int *out);
	inline void _ascii_to_binary(const char *in, char *out);
	inline void _ascii_to_binary(const char *in, double *out);
	inline short _convert_float_to_bin (float f);
	inline short _convert_float_to_bin (double d);
	inline void double_to_int(int * int_array, double * double_array);
};

char *YARPGalilDeviceDriver::_append_cmd(char data, char *buf)
{
	*buf = data;

	return buf+1;
}

int YARPGalilDeviceDriver::_append_values(int *values, char *buff)
{
	char mask = 0x01;
	int i;
	int j;
	for (i = 0, j = 0; i < m_njoints; i++)
	{
		if (mask & m_all_axes)
		{
			buff = _append_cmd(values[j], buff);
			j++;
		}

		mask = mask << 1;
	}

	return j;
}

int YARPGalilDeviceDriver::_append_values(double *values, char *buff)
{
	char mask = 0x01;
	int i;
	int j;
	for (i = 0, j = 0; i < m_njoints; i++)
	{
		if (mask & m_all_axes)
		{
			buff = _append_cmd(values[j], buff);
			j++;
		}

		mask = mask << 1;
	}

	return j;
}

int YARPGalilDeviceDriver::_append_values_as_int(double *values, char *buff)
{
	char mask = 0x01;
	int i;
	int j;
	for (i = 0, j = 0; i < m_njoints; i++)
	{
		if (mask & m_all_axes)
		{
			buff = _append_cmd_as_int(values[j], buff);
			j++;
		}

		mask = mask << 1;
	}

	return j;
}

char *YARPGalilDeviceDriver::_append_cmd(const char *cmd, char *buf)
{
	strcpy(buf, cmd);
	buf+=strlen(cmd);

	return buf;
}

char *YARPGalilDeviceDriver::_append_cmd_as_int(double data, char *buf)
{
	// convert double to int
	// don't care about loss of information, commands must be integral
	int tmp = (int) data;
	char *t = (char *) &tmp;

	for(int i = 0; i < 4; i++)
	{
		buf[i] = t[3 - i];
	}

	buf+=4;

	return buf;
}

char *YARPGalilDeviceDriver::_append_cmd(double data, char *buf)
{
	// Galil uses 1's complement
	if (data < 0)
		data = data-1;

	int i_tmp = (int) data;

	char *t = (char *) &i_tmp;

	int i;
	for(i = 0; i < 4; i++)
	{
		buf[i] = t[3 - i];
	}

	buf+=4;

	short s_tmp = _convert_float_to_bin(data);
	t = (char *) &s_tmp;
	
	for(i = 0; i < 2; i++)
	{
		buf[i] = t[1 - i];
	}
	
	buf+=2;

	return buf;
}

char *YARPGalilDeviceDriver::_append_cmd(int data, char *buf)
{
	char *t = (char *) &data;

	for(int i = 0; i < 4; i++)
	{
		buf[i] = t[3 - i];
	}

	buf+=4;

	return buf;
}

char *YARPGalilDeviceDriver::_append_cmd(short data, char *buf)
{
	char *t = (char *) &data;

	for(int i = 0; i < 2; i++)
	{
		buf[i] = t[1 - i];
	}

	buf+=2;

	return buf;
}

// append question marks
char *YARPGalilDeviceDriver::_append_question_marks(char *buffer)
{
	memcpy(buffer, m_question_marks, 2*m_njoints-1);
	buffer+=2*m_njoints-1;

	return buffer;
}

// same as above: axis specify axis number
char *YARPGalilDeviceDriver::_append_question_mark(char *buffer, int axis)
{
	// add question marks
	for(int i = 0; i<m_njoints && i < axis; i++)
	{
		buffer = _append_cmd(',', buffer);
	}

	buffer = _append_cmd('?', buffer);

	return buffer;
}

short YARPGalilDeviceDriver::_convert_float_to_bin (float f)
{
	return (short) (f*65536);
}

short YARPGalilDeviceDriver::_convert_float_to_bin (double d)
{
	return (short) (d*65536);
}

void YARPGalilDeviceDriver::_ascii_to_binary(const char *in, int *out)
{
	char mask = 0x01;

	while(*in != '\n')
	{
		if (*in == ' ')
		{
			// read an integer
			// skip non valid axes
			if (m_all_axes & mask)
			{
				*out = atoi(in);
				// check vector boundaries ?
				out++;
			}
			// shift mask
			mask = mask << 1;
		}
				
		in++;
	}
}

void YARPGalilDeviceDriver::_ascii_to_binary(const char *in, char *out)
{
	char mask = 0x01;

	while(*in != '\n')
	{
		if (*in == ' ')
		{
			// skip blank
			// skip non valid axes
			if (m_all_axes & mask)
			{
				*out = atoi(in);
				// check vector boundaries ?
				out++;
			}
			// shift mask
			mask = mask << 1;
		}
				
		in++;
	}
}

void YARPGalilDeviceDriver::_ascii_to_binary(const char *in, double *out)
{
	char mask = 0x01;

	while(*in != '\n')
	{
		if (*in == ' ')
		{
			// skip blank
			// skip non valid axes
			if (m_all_axes & mask)
			{
				*out = atof(in);
				// check vector boundaries ?
				out++;
			}
			// shift mask
			mask = mask << 1;
		}
				
		in++;
	}
}

void YARPGalilDeviceDriver::double_to_int(int * int_array, double * double_array)
{
	for (int i = 0; i < m_njoints; i++)
	{
		*(int_array + i) = (int) *(double_array + i);
	}
}

#endif // .h

