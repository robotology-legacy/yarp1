
#include <stdlib.h>
#include <stdio.h>
#include <sys/kernel.h>

#include "win2qnx.h"
#include "powerdaq.h"
#include "pd-int.h"
#include "pdfw_def.h"
#include "pdfw_if.h"

#include "ueid-msg.h"
#include "ueid-int.h"
#include "ueid-err.h"
#include "extern.h"

int do_init(drv_info *di, pid_t pid, union msg *io_msg)
{
  //i do nothing
  io_msg->init_reply.status = 0; //or whatever
  return(REPLY);
}

int do_get_samples(drv_info *di, pid_t pid, union msg *io_msg)
{
  int status, board, max_samples;
  u16 *buffer;

  board = io_msg->get_samples.board;
  max_samples = io_msg->get_samples.max_samples;

  buffer = &(io_msg->get_samples_reply.buffer);
  status = pd_ain_get_samples(board, max_samples, buffer);
  
  status = pd_ain_sw_cl_start(board);
  
  io_msg->get_samples_reply.status = status;

  if(verbose > 4)
    fprintf(stderr, "(wraps.c)get_samples: buffer[0] = %d | status = %d\n", buffer[0], status);

  return(REPLY);
}

int do_sw_cl_start(drv_info *di, pid_t pid, union msg *io_msg)
{
  int board, status;

  board = io_msg->sw_cl_start.board;
    
  if(verbose >4)
    fprintf(stderr, "(wraps.c) sw_cl_start, board = %d\n", board);
  
  status = pd_ain_sw_cl_start(board);
  io_msg->sw_cl_start_reply.status = status;
  
  if(verbose > 4)
    fprintf(stderr, "(wraps.c) return sw_cl_start, status= %d\n", status);
  
  return(REPLY);
}
