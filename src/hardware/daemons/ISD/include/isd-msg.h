
#ifndef _ISD_MSG_H_
#define _ISD_MSG_H_

#include <sys/sys_msg.h>  /* for _SYSMSG stuff */

/* message type codes -- remember that 0 is reserved for _SYSMSG */
#define ISD_POLL_ALL               0x101
#define ISD_BORESIGHT              0x102
#define ISD_UNBORESIGHT            0x103
#define ISD_SET_OUTPUT_MODE        0x104
#define ISD_GET_OUTPUT_MODE        0x105

/* reply message codes */
#define ISD_OK 0

/* message structures */
struct _isd_poll_all {
  msg_t type;
};

struct _isd_poll_all_reply {
  msg_t status;
  float yaw;
  float pitch;
  float roll;
};

struct _isd_boresight {
  msg_t type;
};

struct _isd_boresight_reply {
  msg_t status;
};

struct _isd_unboresight {
  msg_t type;
};

struct _isd_unboresight_reply {
  msg_t status;
};

struct _isd_set_output_mode {
  msg_t type;
  int is_polled;
  int is_ascii;
};

struct _isd_set_output_mode_reply {
  msg_t status;
};
struct _isd_get_output_mode {
  msg_t type;
};

struct _isd_get_output_mode_reply {
  msg_t status;
  int is_polled;
  int is_ascii;
};

/*
 This union encompasses all the possible message types that the IO manager
 will receive. The replies are also contained in this union.
 */

union msg
{
  short unsigned type;
  short unsigned status;
  struct _sysmsg_hdr sysmsg;
  struct _sysmsg_hdr_reply sysmsg_reply;
  struct _isd_poll_all poll_all;
  struct _isd_poll_all_reply poll_all_reply;
  struct _isd_boresight boresight;
  struct _isd_boresight_reply boresight_reply;
  struct _isd_unboresight unboresight;
  struct _isd_unboresight_reply unboresight_reply;
  struct _isd_set_output_mode set_output_mode;
  struct _isd_set_output_mode_reply set_output_mode_reply;
  struct _isd_get_output_mode get_output_mode;
  struct _isd_get_output_mode_reply get_output_mode_reply;
};





#endif /* _ISD_MSG_H_ */

