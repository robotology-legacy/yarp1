


#ifndef _MEID_H_
#define _MEID_H_

#ifdef __cplusplus
extern "C" {
#endif
    

#ifndef _MEID_ERR_CODES_
#define _MEID_ERR_CODES_

/* meid-specific error codes (others inherited from MEI lib) */
#define MEID_ERR_SYS   -1
#define MEID_ERR_NOMEM -2
#define MEID_ERR_INVAL -3

#endif /* _MEID_ERR_CODES_ */


#ifndef _MEID_NO_PROTOTYPES_  /* we do same thing!  see below... */

/* we want all the constants and keywords from the MEI lib header file,
    but we don't want any function prototypes ('cause we don't call any
    of those functions!)
   conventiently, those MEI guys did put a switch in there for us.
   (if you do happen to want the prototypes (daemon code), make sure
    that <mei/pcdsp.h> precedes any instance of "meid.h".)
*/
#define NO_PROTOTYPES
#include <mei/pcdsp.h>
#undef NO_PROTOTYPES



extern int16 meid_error;




/* initialization */
int meid_open(char *meid_name);
int meid_close(int meidx);

/* "mldsp.c" */
int16 meid_dsp_reset(int meidx);
int16 meid_set_sample_rate(int meidx, unsigned16 rate) ;
int16 meid_set_dac_output(int meidx, int16 axis, int16 voltage);
int16 meid_frames_to_execute(int meidx, int16 axis);
int16 meid_dsp_encoder(int meidx, int16 axis);
int16 meid_fifo_space(int meidx);

/* "mlpos.c" */
int16 meid_get_command(int meidx, int16 axis, double *command);
int16 meid_set_command(int meidx, int16 axis, double position);
int16 meid_get_position(int meidx, int16 axis, double *position);
int16 meid_set_position(int meidx, int16 axis, double position);
int16 meid_get_velocity(int meidx, int16 axis, double *velocity);
int16 meid_set_velocity(int meidx, int16 axis, double vel);
int16 meid_get_accel(int meidx, int16 axis, double *accel);
int16 meid_get_jerk(int meidx, int16 axis, double *jerk);
int16 meid_get_error(int meidx, int16 axis, double *error);

/* "mlmove.c" */
int16 meid_start_move(int meidx, int16 axis, 
		      double position, double vel, double accel);
int16 meid_start_move_all(int meidx, int16 len, int16 *axes,
			  double *position, double *velocity,
			  double *acceleration);
int16 meid_move_all(int meidx, int16 len, int16 *axes,
			  double *position, double *velocity,
			  double *acceleration);

/* "mlconfig.c" */

int16  meid_set_positive_sw_limit(int meidx,int16 axis, double position, int16 action) ;
int16  meid_set_negative_sw_limit(int meidx,int16 axis, double position, int16 action) ;
int16  meid_set_positive_limit(int meidx,int16 axis, int16 action) ;
int16  meid_set_positive_level(int meidx,int16 axis, int16 level) ;		/* TRUE or FALSE */
int16  meid_set_negative_limit(int meidx,int16 axis, int16 action) ;
int16  meid_set_negative_level(int meidx,int16 axis, int16 level) ;
int16  meid_set_error_limit(int meidx, int16 axis, double limit, int16 action);

int16 meid_set_in_position(int meidx, int16 axis, double limit);
int16 meid_get_in_position(int meidx, int16 axis, double *limit);
int16 meid_set_amp_enable_level(int meidx, int16 axis, int16 level);
int16 meid_set_filter(int meidx, int16 axis, int16 *coeffs);
int16 meid_get_filter(int meidx, int16 axis, int16 *coeffs);
int16 meid_set_dual_loop(int meidx, int16 axis, int16 vel_axis, int16 dual);
int16 meid_set_feedback(int meidx, int16 axis, int16 device);
int16 meid_set_analog_channel(int meidx, int16 axis, int16 channel,
			      int16 differential, int16 bipolar);
int16 meid_set_axis_analog(int meidx, int16 axis, int16 state);
int16 meid_set_axis(int meidx, int16 axis, int16 enable);

/* "mlio.c" */
int16 meid_init_analog(int meidx,int16 channel, int16 diff, int16 bipolar);
int16  meid_pos_switch(int meidx, int16 axis);
int16  meid_neg_switch(int meidx, int16 axis);
int16 meid_get_analog(int meidx, int16 axis, int16 *value);
int16 meid_read_axis_analog(int meidx, int16 axis, int16 *value);
int16 meid_enable_amplifier(int meidx, int16 axis);
int16 meid_disable_amplifier(int meidx, int16 axis);

/* "mlstatus.c" */
int16 meid_set_stop(int meidx, int16 axis);
int16 meid_set_e_stop(int meidx,int16 axis);
int16 meid_controller_idle(int meidx, int16 axis);
int16 meid_clear_status(int meidx, int16 axis);
int16 meid_frames_left(int meidx, int16 axis);
int16 meid_axis_done(int meidx, int16 axis);
int16 meid_axis_status(int meidx, int16 axis);
int16 meid_axis_state(int meidx, int16 axis);
int16 meid_axis_source(int meidx, int16 axis);

  
/* "mllink.c" */
int16 meid_mei_link(int meidx, int16 master, int16 slave,
		    double ratio, int16 source);
int16 meid_endlink(int meidx, int16 slave);

/* mlef.c */
int16  meid_set_e_stop_rate(int meidx, int16 axis, double rate);


/* "mlsetvel.c" */
int16 meid_v_move(int meidx, int16 axis, double vel, double acc);

/* "mldspf.c" */
int16 meid_dsp_end_sequence(int meidx, int16 axis);
int16 meid_dsp_dwell(int meidx, int16 axis, double duration);
int16 meid_dsp_jerk(int meidx, int16 axis,
		    double jerk, double duration);
int16 meid_dsp_accel(int meidx, int16 axis,
		     double accel, double duration);
int16 meid_dsp_velocity(int meidx, int16 axis,
			double velocity, double duration);
int16 meid_dsp_position(int meidx, int16 axis,
			double position, double duration);

/* "bconfig.c" */
unsigned	meid_mei_checksum(int meidx) ;
int16		meid_set_boot_io(int meidx,int16 port, int16 value) ;
int16		meid_init_boot_io(int meidx,int16 port, int16 direction) ;
int16 meid_get_boot_filter(int meidx, int16 axis, int16 *coeffs);
int16 meid_set_boot_filter(int meidx, int16 axis, int16 *coeffs);



/* CogLand extended functions */

  int16 meidx_multi_dsp_position(int meifd, int count,
			       int16 *axes, double *values, double *durations);


/* utility/glue */
void meid_print_pid_filter(int meidx, int axis);
void meid_print_axis(int meidx, int axis);
void meid_error_check(int error_code);






/******** stuff from apocalypse **********/
int16 meid_get_all_analog(int meidx, int *values);


#endif /* _MEID_NO_PROTOTYPES_ */

#ifdef __cplusplus
}
#endif

#endif /* _MEID_H_ */

