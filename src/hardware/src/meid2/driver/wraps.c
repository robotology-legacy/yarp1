
#include <stdlib.h>
#include <stdio.h>
#include <sys/kernel.h>

#include <errno.h>

#include <mei/pcdsp.h>

#define _MEID_NO_PROTOTYPES_
#include "meid.h"

#include "meid-msg.h"
#include "meid-int.h"



/* "mldsp.c" */

int do_dsp_reset(drv_info *di, pid_t pid, union msg *msg)
{

  msg->mstatus = dsp_reset();
  return REPLY;
}

int do_set_sample_rate(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_sample_rate(msg->set_sample_rate.rate);

  return REPLY;

}

int do_set_dac_output(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_dac_output(msg->set_int16.axis,
			msg->set_int16.value);
  return REPLY;
}

int do_frames_to_execute(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = frames_to_execute(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}

int do_dsp_encoder(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = dsp_encoder(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}

int do_fifo_space(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = fifo_space();
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}




/* "mlpos.c" */

int do_get_command(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_command(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_set_command(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_command(msg->set_double.axis,
			       msg->set_double.value); 
  return REPLY;
}

int do_get_position(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_position(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_set_position(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_position(msg->set_double.axis,
				msg->set_double.value); 
  return REPLY;
}

int do_get_velocity(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_velocity(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_set_velocity(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_velocity(msg->set_double.axis, 
				msg->set_double.value);
  return REPLY;
}

int do_get_accel(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus =
    get_accel(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_get_jerk(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_jerk(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_get_error(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_error(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}



/* "mlmove.c" */

int do_start_move(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = start_move(msg->start_move.axis,
			      msg->start_move.position,
			      msg->start_move.velocity,
			      msg->start_move.acceleration);
  return REPLY;
}

int do_start_move_all(drv_info *di, pid_t pid, union msg *msg)
{
  int num;
  int len;
  int16 *axes;
  double *pos, *vel, *accel;
  struct _mxfer_entry mx[4];

  len = msg->start_move_all.len;
  axes = malloc(len * sizeof(axes[0]));
  pos = malloc(len * sizeof(pos[0]));
  vel = malloc(len * sizeof(vel[0]));
  accel = malloc(len * sizeof(accel[0]));
  if ( (axes == NULL) || (pos == NULL) || (vel == NULL) || (accel == NULL) ) {
    msg->mstatus = MEID_ERR_NOMEM;
    return REPLY;
  }
  _setmx(&mx[0], axes, len * sizeof(axes[0]));
  _setmx(&mx[1], pos, len * sizeof(pos[0]));
  _setmx(&mx[2], vel, len * sizeof(vel[0]));
  _setmx(&mx[3], accel, len * sizeof(accel[0]));
  num = Readmsgmx(pid, 
		  sizeof(msg->start_move_all) - 
		  sizeof(msg->start_move_all.data),
		  4, &mx);
  if (num == -1) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
   if (debug) {
      int i;
      for (i=0; i<len; i++) 
	fprintf(stderr, "dsma: %d-> %f, %f, %f\n", 
		axes[i], pos[i], vel[i], accel[i]);
      fprintf(stderr, "\n");
   }
  msg->mstatus = start_move_all(len, axes, pos, vel, accel);
  free(axes);
  free(pos);
  free(vel);
  free(accel);
  return REPLY;
}
  
int do_move_all(drv_info *di, pid_t pid, union msg *msg)
{
  int num;
  int len;
  int16 *axes;
  double *pos, *vel, *accel;
  struct _mxfer_entry mx[4];

  len = msg->start_move_all.len;
  axes = malloc(len * sizeof(axes[0]));
  pos = malloc(len * sizeof(pos[0]));
  vel = malloc(len * sizeof(vel[0]));
  accel = malloc(len * sizeof(accel[0]));
  if ( (axes == NULL) || (pos == NULL) || (vel == NULL) || (accel == NULL) ) {
    msg->mstatus = MEID_ERR_NOMEM;
    return REPLY;
  }
  _setmx(&mx[0], axes, len * sizeof(axes[0]));
  _setmx(&mx[1], pos, len * sizeof(pos[0]));
  _setmx(&mx[2], vel, len * sizeof(vel[0]));
  _setmx(&mx[3], accel, len * sizeof(accel[0]));
  num = Readmsgmx(pid, 
		  sizeof(msg->start_move_all) - 
		  sizeof(msg->start_move_all.data),
		  4, &mx);
  if (num == -1) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
   if (debug) {
      int i;
      for (i=0; i<len; i++) 
	fprintf(stderr, "dsma: %d-> %f, %f, %f\n", 
		axes[i], pos[i], vel[i], accel[i]);
      fprintf(stderr, "\n");
   }
  msg->mstatus = move_all(len, axes, pos, vel, accel);
  free(axes);
  free(pos);
  free(vel);
  free(accel);
  return REPLY;
}


/* "mlconfig.c" */

int  do_set_positive_sw_limit(drv_info *di, pid_t pid, union msg *msg) 
{
 msg->mstatus = set_positive_sw_limit(msg->set_double_int16.axis, 
				   msg->set_double_int16.value_dbl, msg->set_double_int16.value_i16);

 return REPLY;

}
int  do_set_negative_sw_limit(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_negative_sw_limit(msg->set_double_int16.axis, 
				   msg->set_double_int16.value_dbl, msg->set_double_int16.value_i16);

return REPLY;

}
int  do_set_positive_limit(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_positive_limit(msg->set_int16.axis, 
				   msg->set_int16.value);
    
	return REPLY;

}
int  do_set_positive_level(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_positive_level(msg->set_int16.axis, 
				   msg->set_int16.value);

	return REPLY;

}
int  do_set_negative_limit(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_negative_limit(msg->set_int16.axis, 
				   msg->set_int16.value);


  
	return REPLY;

}

int  do_set_negative_level(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_negative_level(msg->set_int16.axis, 
				   msg->set_int16.value);

	return REPLY;

}

int  do_set_error_limit(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_error_limit(msg->set_error_limit.axis, 
				   msg->set_error_limit.limit, msg->set_error_limit.action);

	return REPLY;
}

int do_set_in_position(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_in_position(msg->set_double.axis, 
				   msg->set_double.value);
  return REPLY;
}

int do_get_in_position(drv_info *di, pid_t pid, union msg *msg)
{
  double value;
  msg->get_double_reply.mstatus = 
    get_in_position(msg->axis.axis, &value);
  msg->get_double_reply.value = value;
  return REPLY;
}

int do_set_amp_enable_level(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_amp_enable_level(msg->set_int16.axis,
				      msg->set_int16.value);
  return REPLY;
}

int do_set_filter(drv_info *di, pid_t pid, union msg *msg)
{
  int num;
  int16 coeffs[COEFFICIENTS];

  num = Readmsg(pid, 
		sizeof(msg->set_filter) - sizeof(msg->set_filter.data),
		coeffs, sizeof(coeffs));
  if (num != sizeof(coeffs)) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
  msg->mstatus = set_filter(msg->set_filter.axis, coeffs);
  return REPLY;
}
  
int do_get_filter(drv_info *di, pid_t pid, union msg *msg)
{
  int16 coeffs[COEFFICIENTS];

  msg->get_filter_reply.mstatus = get_filter(msg->axis.axis, coeffs);
  if (msg->get_filter_reply.mstatus == DSP_OK) {
    struct _mxfer_entry mx[2];
    _setmx(&mx[0], msg,
	   sizeof(msg->get_filter_reply) - 	   
	   sizeof(msg->get_filter_reply.data));
    _setmx(&mx[1], coeffs, sizeof(coeffs));
    Replymx(pid, 2, &mx);
    return NO_REPLY;
  }
  return REPLY;
}
  
int do_set_dual_loop(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_dual_loop(msg->set_dual_loop.axis,
				 msg->set_dual_loop.vel_axis,
				 msg->set_dual_loop.dual);
  return REPLY;
}

int do_set_feedback(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_feedback(msg->set_int16.axis,
				msg->set_int16.value);
  return REPLY;
}

int do_set_analog_channel(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_analog_channel(msg->set_analog_channel.axis,
				      msg->set_analog_channel.channel,
				      msg->set_analog_channel.differential,
				      msg->set_analog_channel.bipolar);
  return REPLY;
}

int do_set_axis_analog(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_axis_analog(msg->set_int16.axis,
				   msg->set_int16.value);
  return REPLY;
}

int do_set_axis(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_axis(msg->set_int16.axis, msg->set_int16.value);
  return REPLY;
}



/* "mlio.c" */

 

int do_init_analog(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus =init_analog(msg->set_dual_loop.axis, 
			    msg->set_dual_loop.vel_axis, 
			    msg->set_dual_loop.dual);
  return REPLY;

	
}

int do_pos_switch(drv_info *di, pid_t pid, union msg *msg)
{

  msg->get_int16_reply.value = 
    pos_switch(msg->axis.axis);
  
  return REPLY;

	
}

int do_neg_switch(drv_info *di, pid_t pid, union msg *msg)
{

  msg->get_int16_reply.value = 
    neg_switch(msg->axis.axis);
  
  return REPLY;

	
}


int do_get_analog(drv_info *di, pid_t pid, union msg *msg)
{
  int16 value;
  msg->get_int16_reply.mstatus = 
    get_analog(msg->axis.axis, &value);
  msg->get_int16_reply.value = value;
  return REPLY;
}

int do_read_axis_analog(drv_info *di, pid_t pid, union msg *msg)
{
  int16 value;
  msg->get_int16_reply.mstatus = 
    read_axis_analog(msg->axis.axis, &value);
  msg->get_int16_reply.value = value;
  return REPLY;
}

int do_enable_amplifier(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = enable_amplifier(msg->axis.axis);
  return REPLY;
}

int do_disable_amplifier(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = disable_amplifier(msg->axis.axis);
  return REPLY;
}

/* "mlstatus.c" */

int do_set_e_stop(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_e_stop(msg->axis.axis);
  return REPLY;
}

int do_set_stop(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = set_stop(msg->axis.axis);
  return REPLY;
}


int do_controller_idle(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = controller_idle(msg->axis.axis);
  return REPLY;
}

int do_clear_status(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = clear_status(msg->axis.axis);
  return REPLY;
}

int do_frames_left(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = frames_left(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}

int do_axis_done(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = axis_done(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}

int do_axis_status(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = axis_status(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  
  return REPLY;
}

int do_axis_state(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = axis_state(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}

int do_axis_source(drv_info *di, pid_t pid, union msg *msg)
{
  msg->get_int16_reply.value = axis_source(msg->axis.axis);
  msg->get_int16_reply.mstatus = dsp_error;
  return REPLY;
}



/* "mllink.c" */

int do_mei_link(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = mei_link(msg->mei_link.master,
			    msg->mei_link.slave,
			    msg->mei_link.ratio,
			    msg->mei_link.source);
  return REPLY;
}

int do_endlink(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = endlink(msg->axis.axis);
  return REPLY;
}


/* mlef.c */
int  do_set_e_stop_rate(drv_info *di, pid_t pid, union msg *msg)
{
	msg->mstatus = set_e_stop_rate(msg->set_double.axis, msg->set_double.value);
	return REPLY;
}



/* "mlsetvel.c" */

int do_v_move(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = v_move(msg->v_move.axis,
			  msg->v_move.velocity,
			  msg->v_move.acceleration);
  return REPLY;
}



/* "mldspf.c" */

int do_dsp_position(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_position(msg->set_frame.axis,
				msg->set_frame.value,
				msg->set_frame.duration);
  return REPLY;
}

int do_dsp_velocity(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_velocity(msg->set_frame.axis,
				msg->set_frame.value,
				msg->set_frame.duration);
  return REPLY;
}

int do_dsp_accel(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_accel(msg->set_frame.axis,
			     msg->set_frame.value,
			     msg->set_frame.duration);
  return REPLY;
}

int do_dsp_jerk(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_jerk(msg->set_frame.axis,
			    msg->set_frame.value,
			    msg->set_frame.duration);
  return REPLY;
}

int do_dsp_dwell(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_dwell(msg->set_double.axis,
			     msg->set_double.value);
  return REPLY;
}

int do_dsp_end_sequence(drv_info *di, pid_t pid, union msg *msg)
{
  msg->mstatus = dsp_end_sequence(msg->axis.axis);
  return REPLY;
}



/* "bconfig.c" */

int	do_mei_checksum(drv_info *di, pid_t pid, union msg *msg)
{
	msg->get_double_reply.value = mei_checksum();
  return REPLY;

}


int		do_set_boot_io(drv_info *di, pid_t pid, union msg *msg) 
{
	msg->mstatus = set_boot_io(msg->set_int16.axis,	msg->set_int16.value);
  return REPLY;
}

int		do_init_boot_io(drv_info *di, pid_t pid, union msg *msg) 
{

	msg->mstatus = init_boot_io(msg->set_int16.axis,msg->set_int16.value);
  return REPLY;

}

int do_get_boot_filter(drv_info *di, pid_t pid, union msg *msg)
{
  int16 coeffs[COEFFICIENTS];

  msg->get_filter_reply.mstatus = get_boot_filter(msg->axis.axis, coeffs);
  if (msg->get_filter_reply.mstatus == DSP_OK) {
    struct _mxfer_entry mx[2];
    _setmx(&mx[0], msg,
           sizeof(msg->get_filter_reply) -         
           sizeof(msg->get_filter_reply.data));
    _setmx(&mx[1], coeffs, sizeof(coeffs));
    Replymx(pid, 2, &mx);
    return NO_REPLY;
  }
  return REPLY;
}
  
int do_set_boot_filter(drv_info *di, pid_t pid, union msg *msg)
{
  int num;
  int16 coeffs[COEFFICIENTS];

  num = Readmsg(pid, 
                sizeof(msg->set_filter) - 
                sizeof(msg->set_filter.data),
                coeffs, sizeof(coeffs));
  if (num != sizeof(coeffs)) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
  msg->mstatus = set_boot_filter(msg->set_filter.axis, coeffs);
  return REPLY;
}





int do_x_multi_dsp_position(drv_info *di, pid_t pid, union msg *msg)
{
  int num;
  int count;
  int16 axes[PCDSP_MAX_AXES];
  double values[PCDSP_MAX_AXES];
  double durations[PCDSP_MAX_AXES];
  struct _mxfer_entry mx[3];
  int i;

  count = msg->x_multi_dsp_position.count;
  if (count > PCDSP_MAX_AXES) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
  _setmx(&mx[0], axes, count * sizeof(axes[0]));
  _setmx(&mx[1], values, count * sizeof(values[0]));
  _setmx(&mx[2], durations, count * sizeof(durations[0]));
  num = Readmsgmx(pid, 
		  sizeof(msg->x_multi_dsp_position) - 
		  sizeof(msg->x_multi_dsp_position.data),
		  3, &mx);
  if (num == -1) {
    msg->mstatus = MEID_ERR_INVAL;
    return REPLY;
  }
  msg->mstatus = 0;
  for (i = 0; i < count; i++) {
    msg->mstatus = dsp_position(axes[i], values[i], durations[i]);
    if (msg->mstatus) return REPLY;
  }
  return REPLY;
}



#if 0
int do_get_all_analog(drv_info *di, pid_t pid, union msg *msg)
{
  int stat, i;
  int16 value;
  
  stat = 0;
  for(i=0;i<8;i++)
    {
      stat |= get_analog(i, &value);
      msg->get_all_analog_reply.values[i] = value;
    }
  msg->get_all_analog_reply.mstatus = stat;
  
  return REPLY;
}
#endif









/* this could be done with a simple array initialization,
   but this is less prone to error (by offset, etc) as
   functions get added
*/
int init_dispatch_table(drv_info *di)
{
  msgfunc_t *table;

  if (MEID_MSG_COUNT > 256) {
    fprintf(stderr,
	    "OOOPS!  There are >256 message types defined.\n"
	    "Someone is going to have to hack the code.\n");
    exit(10);
  }

  if ((table = calloc(MEID_MSG_COUNT, sizeof(table[0]))) == NULL)
    return -1;
  di->dispatch = table;

  table[MEID_DSP_RESET]          = do_dsp_reset;
  table[MEID_SET_DAC_OUTPUT]     = do_set_dac_output;  
  table[MEID_FRAMES_TO_EXECUTE]  = do_frames_to_execute;
  table[MEID_GET_COMMAND]        = do_get_command;
  table[MEID_SET_COMMAND]        = do_set_command;  
  table[MEID_GET_POSITION]       = do_get_position;
  table[MEID_SET_POSITION]       = do_set_position;  
  table[MEID_GET_VELOCITY]       = do_get_velocity;
  table[MEID_SET_VELOCITY]       = do_set_velocity;
  table[MEID_GET_ACCEL]          = do_get_accel;
  table[MEID_GET_JERK]           = do_get_jerk;
  table[MEID_GET_ERROR]          = do_get_error;
  table[MEID_START_MOVE]         = do_start_move;
  table[MEID_START_MOVE_ALL]     = do_start_move_all;
  table[MEID_MOVE_ALL]			 = do_move_all;
  table[MEID_SET_IN_POSITION]    = do_set_in_position;
  table[MEID_GET_IN_POSITION]    = do_get_in_position;
  table[MEID_SET_FILTER]         = do_set_filter;
  table[MEID_GET_FILTER]         = do_get_filter;
  table[MEID_SET_DUAL_LOOP]      = do_set_dual_loop;  
  table[MEID_SET_FEEDBACK]       = do_set_feedback;  
  table[MEID_SET_ANALOG_CHANNEL] = do_set_analog_channel;  
  table[MEID_SET_AXIS_ANALOG]    = do_set_axis_analog;  
  table[MEID_GET_ANALOG]         = do_get_analog;  
  table[MEID_READ_AXIS_ANALOG]   = do_read_axis_analog;  
  table[MEID_CONTROLLER_IDLE]    = do_controller_idle;  
  table[MEID_FRAMES_LEFT]        = do_frames_left;
  table[MEID_AXIS_DONE]          = do_axis_done;
  table[MEID_AXIS_STATUS]        = do_axis_status;
  table[MEID_AXIS_STATE]         = do_axis_state;
  table[MEID_AXIS_SOURCE]        = do_axis_source;
  table[MEID_MEI_LINK]           = do_mei_link;
  table[MEID_ENDLINK]            = do_endlink;
  table[MEID_V_MOVE]             = do_v_move;  
  table[MEID_DSP_END_SEQUENCE]   = do_dsp_end_sequence;  
  table[MEID_DSP_DWELL]          = do_dsp_dwell;  
  table[MEID_DSP_JERK]           = do_dsp_jerk;  
  table[MEID_DSP_ACCEL]          = do_dsp_accel;  
  table[MEID_DSP_VELOCITY]       = do_dsp_velocity;
  table[MEID_DSP_POSITION]       = do_dsp_position;  
  table[MEID_GET_BOOT_FILTER]    = do_get_boot_filter;  
  table[MEID_SET_BOOT_FILTER]    = do_set_boot_filter;  
  table[MEID_SET_AXIS]           = do_set_axis;
  table[MEID_SET_AMP_ENABLE_LEVEL] = do_set_amp_enable_level;
  table[MEID_ENABLE_AMPLIFIER]   = do_enable_amplifier;
  table[MEID_DISABLE_AMPLIFIER]  = do_disable_amplifier;
  table[MEID_SET_SAMPLE_RATE]    = do_set_sample_rate;
  table[MEID_DSP_ENCODER]		 = do_dsp_encoder;
  table[MEID_SET_POSITIVE_SW_LIMIT]    = do_set_positive_sw_limit;
  table[MEID_SET_NEGATIVE_SW_LIMIT]    = do_set_negative_sw_limit;
  table[MEID_SET_POSITIVE_LIMIT]    = do_set_positive_limit;
  table[MEID_SET_POSITIVE_LEVEL]    = do_set_positive_level;
  table[MEID_SET_NEGATIVE_LIMIT]    = do_set_negative_limit;
  table[MEID_SET_NEGATIVE_LEVEL]    = do_set_negative_level;
  table[MEID_SET_ERROR_LIMIT]    = do_set_error_limit;
  table[MEID_POS_SWITCH]		 = do_pos_switch;
  table[MEID_NEG_SWITCH]		 = do_neg_switch;
  table[MEID_SET_E_STOP_RATE]    = do_set_e_stop_rate;
  table[MEID_CLEAR_STATUS]		 = do_clear_status;
  table[MEID_MEI_CHECKSUM]		 = do_mei_checksum;
  table[MEID_INIT_BOOT_IO]		 = do_init_boot_io;
  table[MEID_SET_BOOT_IO]		 = do_set_boot_io;
  table[MEID_INIT_ANALOG]		 = do_init_analog;
  table[MEID_SET_E_STOP]		 = do_set_e_stop;
  table[MEID_SET_STOP]			 = do_set_stop;
  table[MEIDX_MULTI_DSP_POSITION]        = do_x_multi_dsp_position;
  table[MEID_FIFO_SPACE]                 = do_fifo_space;

  return 0;
}



