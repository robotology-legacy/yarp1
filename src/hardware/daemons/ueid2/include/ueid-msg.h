
#ifndef _UEID_MSG_H_
#define _UEID_MSG_H_

#include <sys/sys_msg.h>  /* for _SYSMSG stuff */

#include "win2qnx.h" //for u16

/* message type codes -- remember that 0 is reserved for _SYSMSG */
/* direct UEI library functions */
#define UEID_INIT              0x101
#define UEID_GET_SAMPLES       0x102
#define UEID_SW_CL_START       0x103

#define SAMPLES 64

/* message structures */

struct _ueid_init {
  msg_t type;
};

struct _ueid_init_reply {
  msg_t status;
};

struct _ueid_get_samples {
  msg_t type;
  int board;
  int max_samples;
};

struct _ueid_get_samples_reply {
  msg_t status;
  u16 buffer[SAMPLES]; //HACK this sucks ass, but ok
};

struct _ueid_sw_cl_start {
  msg_t type;
  int board;
};

struct _ueid_sw_cl_start_reply {
  msg_t status;
};


/* This union encompasses all the possible message types that the IO manager
   will receive. The replies are also contained in this union.
   The first paragraph of structures (up to and including 'fstat_reply') 
   will ALWAYS be the same for all IO managers.
   You can add other message types in this union if you'd like. In this
   example I've added a extra message type for an incoming proxy.
*/

union msg
{
  short unsigned type;
  short unsigned status;
  struct _sysmsg_hdr sysmsg;
  struct _sysmsg_hdr_reply sysmsg_reply;
  struct _ueid_init init;
  struct _ueid_init_reply init_reply;
  struct _ueid_get_samples get_samples;
  struct _ueid_get_samples_reply get_samples_reply;
  struct _ueid_sw_cl_start sw_cl_start ;
  struct _ueid_sw_cl_start_reply sw_cl_start_reply;

};





#endif /* _UEID_MSG_H_ */

