#include "isd-msg.h"
#include "isd-int.h"
#include "isd-err.h"
#include "extern.h"

/*
 * Utilities
 */

void isd_send_msg(char *msg)
{
  write(di.fd, msg, strlen(msg));
  if (verbose > 5) 
    fprintf(stderr, "wrote msg <%s>\n", msg);
}

void isd_read_msg(int len)
{
  int n, i;
  
  n = dev_read(di.fd, &_isd_readbuf, len, len, 0, 0, 0, 0 );
  if (verbose > 5)
    {
      fprintf(stderr, "read msg <");
      for(i=0;i<n-2;i++)
	fprintf(stderr, "%c", _isd_readbuf[i]);
      fprintf(stderr, ">\n");
    }
}

/*
 * Wrap functions
 */

int do_poll_all(drv_info *di, pid_t pid, union msg *io_msg)
{
  float *p1, *p2, *p3;
  float f1, f2, f3;
  
  isd_send_msg(ISD_POLL_MSG);
  if (output_is_ascii)
    {
      isd_read_msg(ISD_ASCII_RECORD_SIZE);
      sscanf(&_isd_readbuf[24], "%f %f %f", &f1, &f2, &f3);
    }
  else
    {
      isd_read_msg(ISD_BINARY_RECORD_SIZE);
      p1 = (float *) &_isd_readbuf[15];
      p2 = (float *) &_isd_readbuf[19];
      p3 = (float *) &_isd_readbuf[23];
      f1 = *p1;
      f2 = *p2;
      f3 = *p3;
    }
  io_msg->poll_all_reply.yaw = f1;
  io_msg->poll_all_reply.pitch = f2;
  io_msg->poll_all_reply.roll = f3;
  io_msg->poll_all_reply.status = ISD_OK;
  return(REPLY);
}

int do_boresight(drv_info *di, pid_t pid, union msg *io_msg)
{
  isd_send_msg(ISD_BORESIGHT_MSG);
  io_msg->boresight_reply.status = ISD_OK;
  return(REPLY);
}

int do_unboresight(drv_info *di, pid_t pid, union msg *io_msg)
{
  isd_send_msg(ISD_UNBORESIGHT_MSG);
  io_msg->unboresight_reply.status = ISD_OK;
  return(REPLY);
}

int do_set_output_mode(drv_info *di, pid_t pid, union msg *io_msg)
{
  int is_polled, is_ascii;

  is_ascii = io_msg->set_output_mode.is_ascii;
  is_polled = io_msg->set_output_mode.is_polled;
  if (is_polled)
    {
      isd_send_msg(ISD_OUTPUT_POLLED_MSG);
      output_is_polled = TRUE;
    }
  else
    {
      isd_send_msg(ISD_OUTPUT_CONTINUOUS_MSG);
      output_is_polled = FALSE;
    }
  
  if (is_ascii)
    {
      isd_send_msg(ISD_OUTPUT_ASCII_MSG);
      output_is_ascii = TRUE;
    }
  else
    {
      isd_send_msg(ISD_OUTPUT_BINARY_MSG);
      output_is_ascii = FALSE;
    }
  
  
  io_msg->set_output_mode_reply.status = ISD_OK;
  return(REPLY);
}


int do_get_output_mode(drv_info *di, pid_t pid, union msg *io_msg)
{
  io_msg->get_output_mode_reply.is_ascii = output_is_ascii;
  io_msg->get_output_mode_reply.is_polled = output_is_polled;
  io_msg->get_output_mode_reply.status = ISD_OK;
  return(REPLY);
}
