
#ifndef _MEID_MSG_H_
#define _MEID_MSG_H_



#include <sys/sys_msg.h>  /* for _SYSMSG stuff */
#include <sys/io_msg.h>   /* for _IO*MSG stuff */

#define NO_PROTOTYPES
#include <mei/pcdsp.h>
#undef NO_PROTOTYPES

/* message type codes -- remember that 0 is reserved for _SYSMSG */
/* direct MEI library functions */

#define MEID_MSG_CLASS 0x6900

typedef enum {
  MEID_DSP_RESET = 0x00,
  MEID_SET_DAC_OUTPUT,
  MEID_FRAMES_TO_EXECUTE,
  MEID_GET_COMMAND,
  MEID_SET_COMMAND,
  MEID_GET_POSITION,
  MEID_SET_POSITION,
  MEID_GET_VELOCITY,
  MEID_SET_VELOCITY,
  MEID_GET_ACCEL,
  MEID_GET_JERK,
  MEID_GET_ERROR,
  MEID_START_MOVE,
  MEID_START_MOVE_ALL,
  MEID_SET_IN_POSITION,
  MEID_GET_IN_POSITION,
  MEID_SET_FILTER,
  MEID_GET_FILTER,
  MEID_SET_DUAL_LOOP,
  MEID_SET_FEEDBACK,
  MEID_SET_ANALOG_CHANNEL,
  MEID_SET_AXIS_ANALOG,
  MEID_GET_ANALOG,
  MEID_READ_AXIS_ANALOG,
  MEID_CONTROLLER_IDLE,
  MEID_FRAMES_LEFT,
  MEID_AXIS_DONE,
  MEID_AXIS_STATUS,
  MEID_AXIS_STATE,
  MEID_AXIS_SOURCE,
  MEID_MEI_LINK,
  MEID_ENDLINK,
  MEID_V_MOVE,
  MEID_DSP_END_SEQUENCE,
  MEID_DSP_DWELL,
  MEID_DSP_JERK,
  MEID_DSP_ACCEL,
  MEID_DSP_VELOCITY,
  MEID_DSP_POSITION,
  MEID_GET_BOOT_FILTER,
  MEID_SET_BOOT_FILTER,
  MEID_SET_AXIS,
  MEID_SET_AMP_ENABLE_LEVEL,
  MEID_ENABLE_AMPLIFIER,
  MEID_DISABLE_AMPLIFIER,
  MEID_SET_SAMPLE_RATE,
  MEID_DSP_ENCODER,
  MEID_SET_POSITIVE_SW_LIMIT,
  MEID_SET_NEGATIVE_SW_LIMIT,
  MEID_SET_POSITIVE_LIMIT,
  MEID_SET_POSITIVE_LEVEL,
  MEID_SET_NEGATIVE_LIMIT,
  MEID_SET_NEGATIVE_LEVEL,
  MEID_POS_SWITCH,
  MEID_NEG_SWITCH,
  MEID_SET_E_STOP_RATE,
  MEID_CLEAR_STATUS,
  MEID_MEI_CHECKSUM,
  MEID_SET_BOOT_IO,
  MEID_INIT_BOOT_IO,
  MEID_INIT_ANALOG,
  MEID_SET_E_STOP,
  MEID_SET_STOP,
  MEIDX_MULTI_DSP_POSITION,

  MEID_SET_ERROR_LIMIT,
  MEID_MOVE_ALL,
  
  MEID_FIFO_SPACE,

  MEID_MSG_COUNT /* this MUST come last */
} meid_msgtype_t;




/*
  many "get_" commands have a single parameter:  the axis
  the reply structures vary with the result types, though.
*/

struct _meid_axis {
  msg_t type;
  int16 axis;
};

struct _meid_get_int16_reply {
  int16 mstatus;
  int16 value;
};

struct _meid_get_double_reply {
  int16 mstatus;
  double value;
};

struct _meid_get_filter_reply {
  int16 mstatus;
  char data[1];
  /* follows:  int16 coeffs[COEFFICIENTS] */
};


/*
  "set_" commands have a greater variety of parameters.
  however, the replies are mostly just "mstatus".
*/

struct _meid_set_double {
  msg_t type;
  int16 axis;
  double value;
};

struct _meid_set_double_int16 {
  msg_t type;
  int16 axis;
  double value_dbl;
  int16 value_i16;

};

struct _meid_set_int16 {
  msg_t type;
  int16 axis;
  int16 value;
};


struct _meid_set_frame {
  msg_t type;
  int16 axis;
  double value;
  double duration;
};

struct _meid_set_dual_loop {
  msg_t type;
  int16 axis;
  int16 vel_axis;
  int16 dual;
};

struct _meid_set_analog_channel {
  msg_t type;
  int16 axis;
  int16 channel;
  int16 differential;
  int16 bipolar;
};

struct _meid_set_filter {
  msg_t type;
  int16 axis;
  char data[1];
  /* follows:  int16 coeffs[COEFFICIENTS] */
};

struct _meid_mei_link {
  msg_t type;
  int16 master;
  int16 slave;
  int16 source;
  double ratio;
};

struct _meid_v_move {
  msg_t type;
  int16 axis;
  double velocity;
  double acceleration;
};

struct _meid_start_move {
  msg_t type;
  int16 axis;
  double position;
  double velocity;
  double acceleration;
};

struct _meid_start_move_all {
  msg_t type;
  int16 len;
  char data[1];
  /* following this header are:
     int16 axes[len]
     double pos[len]
     double vel[len]
     double accel[len]
     */
};

struct _meid_set_sample_rate {
  msg_t type;
  unsigned16 rate;
};

struct _meid_set_error_limit {
	msg_t type;
	int16 axis;
	double limit;
	int16 action;
};

struct _meidx_multi_dsp_position {
  msg_t type;
  int16 count;
  char data[1];
  /* following this header are:
     int16 axes[count]
     double values[count]
     double durations[count]
     */
};

#if 0
struct _meid_get_all_analog_reply {
  int16 mstatus;
  int values[8];
};
#endif







union msg
{
  /* every received message has a type first... */
  msg_t type;
  /* ...and every reply has a status first. */
  msg_t status;

  /* system messages (i.e. "sin ver") */
  struct _sysmsg_hdr sysmsg;
  struct _sysmsg_hdr_reply sysmsg_reply;

  /* I/O manager messages */
  struct _io_open open; /* IO_HANDLE uses this one also.        */
  struct _io_open_reply open_reply;
  struct _io_dup dup;   /* Used for fork, exec, dup, dup2()'s. */
  struct _io_dup_reply dup_reply;
  struct _io_close close;
  struct _io_close_reply close_reply;
  struct _io_read read;
  struct _io_read_reply read_reply;
  struct _io_write write;
  struct _io_write_reply write_reply;
  struct _io_qioctl qioctl;
  struct _io_qioctl_reply qioctl_reply;
  struct _io_utime utime;
  struct _io_utime_reply utime_reply;
  struct _io_chmod chmod;
  struct _io_chmod_reply chmod_reply;
  struct _io_chown chown;
  struct _io_chown_reply chown_reply;
  struct _io_fstat fstat;
  struct _io_fstat_reply fstat_reply;
  struct _io_readdir readdir;
  struct _io_readdir_reply readdir_reply;
  struct _io_rewinddir rewinddir;
  struct _io_rewinddir_reply rewinddir_reply;   

  /* finally, meid native messages */
  int16 mstatus; /* (all replies start with an mstatus) */
  struct _meid_axis axis;
  struct _meid_get_double_reply get_double_reply;
  struct _meid_get_int16_reply get_int16_reply;
  struct _meid_get_filter_reply get_filter_reply;

  struct _meid_set_double set_double;
  struct _meid_set_double_int16 set_double_int16;
  struct _meid_set_int16 set_int16;
  struct _meid_set_frame set_frame;
  struct _meid_set_filter set_filter;
  struct _meid_set_dual_loop set_dual_loop;
  struct _meid_set_analog_channel set_analog_channel;
  struct _meid_mei_link mei_link;
  struct _meid_v_move v_move;
  struct _meid_start_move start_move;
  struct _meid_start_move_all start_move_all;
  struct _meid_set_sample_rate set_sample_rate;
  struct _meid_set_error_limit set_error_limit;
  struct _meidx_multi_dsp_position x_multi_dsp_position;

  /*  struct _meid_get_all_analog_reply get_all_analog_reply;*/

};



#endif /* _MEID_MSG_H_ */

