
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/name.h>
#include <sys/kernel.h>

#include <fcntl.h>

#include "meid.h"
#include "meid-msg.h"



int16 meid_error;  /* our version of the "error code global variable" */



/* initialization */
int meid_open(char *meid_name)
{
  return open(meid_name, O_RDWR);
}


int meid_close(int meifd)
{
  return close(meifd);
}



/* "mldsp.c" */

int16 meid_dsp_reset(int meifd)
{
  msg_t sbuf;
  int16 rbuf;
  
  sbuf = MEID_MSG_CLASS | MEID_DSP_RESET;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_sample_rate(int meidx, unsigned16 rate)
{
  struct _meid_set_sample_rate sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_SAMPLE_RATE;
  sbuf.rate = rate;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_dac_output(int meidx, int16 axis, int16 voltage)
{
  struct _meid_set_int16 sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_DAC_OUTPUT;
  sbuf.axis = axis;
  sbuf.value = voltage;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_frames_to_execute(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_FRAMES_TO_EXECUTE;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_dsp_encoder(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_ENCODER;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_fifo_space(int meifd)
{
    int16 axis = -1;
    struct _meid_axis sbuf;
    struct _meid_get_int16_reply rbuf;
  
    sbuf.type = MEID_MSG_CLASS | MEID_DSP_ENCODER;
    sbuf.axis = axis;
    if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
        meid_error = MEID_ERR_SYS;
    else
        meid_error = rbuf.mstatus;
    return rbuf.value;
}


/* "mlpos.c" */

int16 meid_get_command(int meifd, int16 axis, double *command)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_COMMAND;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *command = rbuf.value;
  }
  return meid_error;
}

int16 meid_set_command(int meidx, int16 axis, double position)
{
  struct _meid_set_double sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_COMMAND;
  sbuf.axis = axis;
  sbuf.value = position;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_get_position(int meifd, int16 axis, double *pos)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_POSITION;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *pos = rbuf.value;
  }
  return meid_error;
}

int16 meid_set_position(int meidx, int16 axis, double position)
{
  struct _meid_set_double sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_POSITION;
  sbuf.axis = axis;
  sbuf.value = position;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_get_velocity(int meifd, int16 axis, double *vel)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_VELOCITY;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *vel = rbuf.value;
  }
  return meid_error;
}

int16 meid_set_velocity(int meifd, int16 axis, double vel)
{
  struct _meid_set_double sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_VELOCITY;
  sbuf.axis = axis;
  sbuf.value = vel;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else 
    meid_error = rbuf;
  return meid_error;
}

int16 meid_get_accel(int meifd, int16 axis, double *accel)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_ACCEL;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *accel = rbuf.value;
  }
  return meid_error;
}

int16 meid_get_jerk(int meifd, int16 axis, double *jerk)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_JERK;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *jerk = rbuf.value;
  }
  return meid_error;
}

int16 meid_get_error(int meifd, int16 axis, double *err)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_ERROR;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *err = rbuf.value;
  }
  return meid_error;
}


/* "mlmove.c" */
  
int16 meid_start_move(int meifd, int16 axis,
		      double position, double velocity, double acceleration)
{
  struct _meid_start_move sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_START_MOVE;
  sbuf.axis = axis;
  sbuf.position = position;
  sbuf.velocity = velocity;
  sbuf.acceleration = acceleration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_start_move_all(int meifd, int16 len, int16 *axes,
			  double *position, double *velocity,
			  double *acceleration)
{
  struct _meid_start_move_all sbuf;
  int16 rbuf;
  struct _mxfer_entry smx[5], rmx[1];

  sbuf.type = MEID_MSG_CLASS | MEID_START_MOVE_ALL;
  sbuf.len = len;
  _setmx(&smx[0], &sbuf, sizeof(sbuf) - sizeof(sbuf.data));
  _setmx(&smx[1], axes, len * sizeof(int16));
  _setmx(&smx[2], position, len * sizeof(double));
  _setmx(&smx[3], velocity, len * sizeof(double));
  _setmx(&smx[4], acceleration, len * sizeof(double));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf));
  if (Sendfdmx(meifd, 5, 1, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_move_all(int meifd, int16 len, int16 *axes,
			  double *position, double *velocity,
			  double *acceleration)
{
  struct _meid_start_move_all sbuf;
  int16 rbuf;
  struct _mxfer_entry smx[5], rmx[1];

  sbuf.type = MEID_MSG_CLASS | MEID_MOVE_ALL;
  sbuf.len = len;
  _setmx(&smx[0], &sbuf, sizeof(sbuf) - sizeof(sbuf.data));
  _setmx(&smx[1], axes, len * sizeof(int16));
  _setmx(&smx[2], position, len * sizeof(double));
  _setmx(&smx[3], velocity, len * sizeof(double));
  _setmx(&smx[4], acceleration, len * sizeof(double));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf));
  if (Sendfdmx(meifd, 5, 1, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}


/* "mlconfig.c" */

int16  meid_set_positive_sw_limit(int meifd,int16 axis, double position, int16 action)
{ 
	struct _meid_set_double_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_POSITIVE_SW_LIMIT;
	sbuf.axis = axis;
	sbuf.value_dbl = position;
	sbuf.value_i16 = action;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16  meid_set_negative_sw_limit(int meifd,int16 axis, double position, int16 action) 
{

	struct _meid_set_double_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_NEGATIVE_SW_LIMIT;
	sbuf.axis = axis;
	sbuf.value_dbl = position;
	sbuf.value_i16 = action;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}

int16  meid_set_positive_limit(int meifd,int16 axis, int16 action) 
{
  
	struct _meid_set_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_POSITIVE_LIMIT;
	sbuf.axis = axis;
	sbuf.value=action;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}

int16  meid_set_positive_level(int meifd,int16 axis, int16 level) 
{
	struct _meid_set_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_POSITIVE_LEVEL;
	sbuf.axis = axis;
	sbuf.value=level;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}


int16  meid_set_negative_limit(int meifd,int16 axis, int16 action) 
{

	struct _meid_set_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_NEGATIVE_LIMIT;
	sbuf.axis = axis;
	sbuf.value=action;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}

int16  meid_set_negative_level(int meifd,int16 axis, int16 level)
{
	struct _meid_set_int16 sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_NEGATIVE_LEVEL;
	sbuf.axis = axis;
	sbuf.value=level;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;


}

int16  meid_set_error_limit(int meifd, int16 axis, double limit, int16 action)
{
	struct _meid_set_error_limit sbuf;
	int16 rbuf;

	sbuf.type = MEID_MSG_CLASS | MEID_SET_ERROR_LIMIT;
	sbuf.axis = axis;
	sbuf.limit = limit;
	sbuf.action = action;

  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_in_position(int meifd, int16 axis, double pos)
{
  struct _meid_set_double sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_IN_POSITION;
  sbuf.axis = axis;
  sbuf.value = pos;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_get_in_position(int meifd, int16 axis, double *pos)
{
  struct _meid_axis sbuf;
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_IN_POSITION;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *pos = rbuf.value;
  }
  return meid_error;
}

int16 meid_set_amp_enable_level(int meifd, int16 axis, int16 level)
{
  struct _meid_set_int16 sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_AMP_ENABLE_LEVEL;
  sbuf.axis = axis;
  sbuf.value = level;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else 
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_filter(int meifd, int16 axis, int16 *coeffs)
{
  struct _meid_set_filter sbuf;
  int16 rbuf;
  struct _mxfer_entry smx[2], rmx[1];

  sbuf.type = MEID_MSG_CLASS | MEID_SET_FILTER;
  sbuf.axis = axis;
  _setmx(&smx[0], &sbuf, sizeof(sbuf) - sizeof(sbuf.data));
  _setmx(&smx[1], coeffs, COEFFICIENTS * sizeof(coeffs[0]));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf));
  if (Sendfdmx(meifd, 2, 1, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_get_filter(int meifd, int16 axis, int16 *coeffs)
{
  struct _meid_axis sbuf;
  struct _meid_get_filter_reply rbuf;
  struct _mxfer_entry smx[1], rmx[2];

  sbuf.type = MEID_MSG_CLASS | MEID_GET_FILTER;
  sbuf.axis = axis;
  _setmx(&smx[0], &sbuf, sizeof(sbuf));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf) - sizeof(rbuf.data));
  _setmx(&rmx[1], coeffs, COEFFICIENTS * sizeof(coeffs[0]));
  if (Sendfdmx(meifd, 1, 2, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return meid_error;
}

int16 meid_set_dual_loop(int meidx, int16 axis, int16 vel_axis, int16 dual)
{
  struct _meid_set_dual_loop sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_DUAL_LOOP;
  sbuf.axis = axis;
  sbuf.vel_axis = vel_axis;
  sbuf.dual = dual;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_feedback(int meidx, int16 axis, int16 device)
{
  struct _meid_set_int16 sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_FEEDBACK;
  sbuf.axis = axis;
  sbuf.value = device;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_analog_channel(int meidx, int16 axis, int16 channel, int16 differential, 
int16 bipolar)
{
  struct _meid_set_analog_channel sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_ANALOG_CHANNEL;
  sbuf.axis = axis;
  sbuf.channel = channel;
  sbuf.differential = differential;
  sbuf.bipolar = bipolar;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_axis_analog(int meidx, int16 axis, int16 state)
{
  struct _meid_set_int16 sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_SET_AXIS_ANALOG;
  sbuf.axis = axis;
  sbuf.value = state;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_axis(int meifd, int16 axis, int16 state)
{
  struct _meid_set_int16 sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_AXIS;
  sbuf.axis = axis;
  sbuf.value = state;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

/* "mlio.c" */

int16 meid_init_analog(int meidx, int16 channel, int16 diff, int16 bipolar)
{
  struct _meid_set_dual_loop sbuf;
  int16 rbuf;
  
  sbuf.type = MEID_MSG_CLASS | MEID_INIT_ANALOG;
  sbuf.axis = channel;
  sbuf.vel_axis = diff;
  sbuf.dual = bipolar;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_pos_switch(int meifd, int16 axis)
{
	struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_POS_SWITCH;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}


int16 meid_neg_switch(int meifd, int16 axis)
{
	struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_NEG_SWITCH;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_get_analog(int meidx, int16 axis, int16 *value)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_ANALOG;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *value = rbuf.value;
  }
  return meid_error;
}

int16 meid_read_axis_analog(int meidx, int16 axis, int16 *value)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_READ_AXIS_ANALOG;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
    meid_error = rbuf.mstatus;
    *value = rbuf.value;
  }
  return meid_error;
}

int16 meid_enable_amplifier(int meidx, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_ENABLE_AMPLIFIER;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_disable_amplifier(int meidx, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_ENABLE_AMPLIFIER;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}


/* "mlstatus.c" */

int16 meid_set_e_stop(int meidx, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_E_STOP;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_set_stop(int meidx, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_STOP;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_controller_idle(int meidx, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_CONTROLLER_IDLE;
  sbuf.axis = axis;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_clear_status(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_CLEAR_STATUS;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return meid_error;
}

int16 meid_frames_left(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_FRAMES_LEFT;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_axis_done(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_AXIS_DONE;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_axis_status(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_AXIS_STATUS;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_axis_state(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_AXIS_STATE;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}

int16 meid_axis_source(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  struct _meid_get_int16_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_AXIS_SOURCE;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return rbuf.value;
}



/* "mllink.c" */

int16 meid_mei_link(int meifd, int16 master, int16 slave,
		    double ratio, int16 source)
{
  struct _meid_mei_link sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_MEI_LINK;
  sbuf.master = master;
  sbuf.slave = slave;
  sbuf.ratio = ratio;
  sbuf.source = source;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}
   
int16 meid_endlink(int meifd, int16 slave)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_ENDLINK;
  sbuf.axis = slave;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}




/* mlef.c */
int16  meid_set_e_stop_rate(int meifd, int16 axis, double rate)
{
	struct _meid_set_double sbuf;
	int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_E_STOP_RATE;
  sbuf.axis = axis;
  sbuf.value = rate;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

/* "mlsetvel.c" */

int16 meid_v_move(int meidx, int16 axis, double vel, double acc)
{
  struct _meid_v_move sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_V_MOVE;
  sbuf.axis = axis;
  sbuf.velocity = vel;
  sbuf.acceleration = acc;
  if (Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}


/* "mldspf.c" */

int16 meid_dsp_position(int meifd, int16 axis, double value, double duration)
{
  struct _meid_set_frame sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_POSITION;
  sbuf.axis = axis;
  sbuf.value = value;
  sbuf.duration = duration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_dsp_velocity(int meifd, int16 axis, double value, double duration)
{
  struct _meid_set_frame sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_VELOCITY;
  sbuf.axis = axis;
  sbuf.value = value;
  sbuf.duration = duration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_dsp_accel(int meifd, int16 axis, double value, double duration)
{
  struct _meid_set_frame sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_ACCEL;
  sbuf.axis = axis;
  sbuf.value = value;
  sbuf.duration = duration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_dsp_jerk(int meifd, int16 axis, double value, double duration)
{
  struct _meid_set_frame sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_JERK;
  sbuf.axis = axis;
  sbuf.value = value;
  sbuf.duration = duration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_dsp_dwell(int meifd, int16 axis, double duration)
{
  struct _meid_set_double sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_DWELL;
  sbuf.axis = axis;
  sbuf.value = duration;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}

int16 meid_dsp_end_sequence(int meifd, int16 axis)
{
  struct _meid_axis sbuf;
  int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_DSP_END_SEQUENCE;
  sbuf.axis = axis;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}





/* "bconfig.c" */

unsigned	meid_mei_checksum(int meifd)
{

  struct _meid_axis sbuf; //dummy
  struct _meid_get_double_reply rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_MEI_CHECKSUM;
 
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else {
		meid_error = rbuf.mstatus;
		}
  return (unsigned) rbuf.value;

}
int16		meid_set_boot_io(int meifd,int16 port, int16 value) 
{

	struct _meid_set_int16 sbuf;
	int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_SET_BOOT_IO;
  sbuf.axis = port;
  sbuf.value = value;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}
int16		meid_init_boot_io(int meifd,int16 port, int16 direction) 
{

	struct _meid_set_int16 sbuf;
	int16 rbuf;

  sbuf.type = MEID_MSG_CLASS | MEID_INIT_BOOT_IO;
  sbuf.axis = port;
  sbuf.value = direction;
  if (Sendfd(meifd, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf)))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;

}

int16 meid_get_boot_filter(int meidx, int16 axis, int16 *coeffs)
{
  struct _meid_axis sbuf;
  struct _meid_get_filter_reply rbuf;
  struct _mxfer_entry smx[1], rmx[2];

  sbuf.type = MEID_MSG_CLASS | MEID_GET_BOOT_FILTER;
  sbuf.axis = axis;
  _setmx(&smx[0], &sbuf, sizeof(sbuf));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf) - sizeof(rbuf.data));
  _setmx(&rmx[1], coeffs, COEFFICIENTS * sizeof(coeffs[0]));
  if (Sendfdmx(meidx, 1, 2, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf.mstatus;
  return meid_error;
}


int16 meid_set_boot_filter(int meidx, int16 axis, int16 *coeffs)
{
  struct _meid_set_filter sbuf;
  int16 rbuf;
  struct _mxfer_entry smx[2], rmx[1];

  sbuf.type = MEID_MSG_CLASS | MEID_SET_BOOT_FILTER;
  sbuf.axis = axis;
  _setmx(&smx[0], &sbuf, sizeof(sbuf) - sizeof(sbuf.data));
  _setmx(&smx[1], coeffs, COEFFICIENTS * sizeof(coeffs[0]));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf));
  if (Sendfdmx(meidx, 2, 1, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}



/* CogLand extended functions */

int16 meidx_multi_dsp_position(int meifd, int count,
			       int16 *axes, double *values, double *durations)
{
  struct _meidx_multi_dsp_position sbuf;
  int16 rbuf;
  struct _mxfer_entry smx[4], rmx[1];

  sbuf.type = MEID_MSG_CLASS | MEIDX_MULTI_DSP_POSITION;
  sbuf.count = count;

  _setmx(&smx[0], &sbuf, sizeof(sbuf) - sizeof(sbuf.data));
  _setmx(&smx[1], axes, count * sizeof(axes[0]));
  _setmx(&smx[2], values, count * sizeof(values[0]));
  _setmx(&smx[3], durations, count * sizeof(durations[0]));
  _setmx(&rmx[0], &rbuf, sizeof(rbuf));
  if (Sendfdmx(meifd, 4, 1, &smx, &rmx))
    meid_error = MEID_ERR_SYS;
  else
    meid_error = rbuf;
  return meid_error;
}



#if 0
int16 meid_get_all_analog(int meidx, int *values)
{
  struct _meid_get_all_analog sbuf;
  struct _meid_get_all_analog_reply rbuf;
  int status, i;

  sbuf.type = MEID_MSG_CLASS | MEID_GET_ALL_ANALOG;
  status = Sendfd(meidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  if (status) return -1;
  for(i=0;i<8;i++)
    values[i] = rbuf.values[i];
  return rbuf.mstatus;
}
#endif

