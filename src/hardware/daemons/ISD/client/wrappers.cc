#include <stdlib.h>

#include <sys/name.h>
#include <sys/kernel.h>

#include "isd.h"
#include "isd-msg.h"



/* initialization */
int isd_initialize(char *isd_name, int *isdx)
{
  pid_t pid;
  
  pid = qnx_name_locate(0, isd_name, 0, NULL);
  if (pid == -1) {
    *isdx = errno;
    return(-1);
  } else {
    *isdx = pid;
    return(0);
  }
}


/* message-passing wrappers */
int isd_poll_all(int isdx, float *yaw, float *pitch, float *roll)
{
  struct _isd_poll_all sbuf;
  struct _isd_poll_all_reply rbuf;
  int status;
   
  sbuf.type = ISD_POLL_ALL;
  status = Send(isdx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  if (status) return -1;
  *yaw = rbuf.yaw;
  *pitch = rbuf.pitch;
  *roll = rbuf.roll;
  return rbuf.status;
}

int isd_boresight(int isdx)
{
  struct _isd_boresight sbuf;
  struct _isd_boresight_reply rbuf;
  int status;
   
  sbuf.type = ISD_BORESIGHT;
  status = Send(isdx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  return rbuf.status;
}

int isd_unboresight(int isdx)
{
  struct _isd_unboresight sbuf;
  struct _isd_unboresight_reply rbuf;
  int status;
   
  sbuf.type = ISD_UNBORESIGHT;
  status = Send(isdx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  return rbuf.status;
}

int isd_set_output_mode(int isdx, int is_polled, int is_ascii)
{
  struct _isd_set_output_mode sbuf;
  struct _isd_set_output_mode_reply rbuf;
  int status;

  sbuf.type = ISD_SET_OUTPUT_MODE;
  sbuf.is_polled = is_polled;
  sbuf.is_ascii = is_ascii;
  status = Send(isdx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  return rbuf.status;
}

int isd_get_output_mode(int isdx, int *is_polled, int *is_ascii)
{
  struct _isd_get_output_mode sbuf;
  struct _isd_get_output_mode_reply rbuf;
  int status;

  sbuf.type = ISD_GET_OUTPUT_MODE;
  status = Send(isdx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  *is_polled = rbuf.is_polled;
  *is_ascii = rbuf.is_ascii;
  return rbuf.status;
}
  
