#include <stdlib.h>

#include <sys/name.h>
#include <sys/kernel.h>

#include "ueid.h"
#include "ueid-msg.h"



/* initialization */
int ueid_initialize(char *ueid_name, int *ueidx)
{
   pid_t pid;

   pid = qnx_name_locate(0, ueid_name, 0, NULL);
   if (pid == -1) {
      *ueidx = errno;
      return(-1);
   } else {
      *ueidx = pid;
      return(0);
   }
}

/* message-passing wrappers */
int ueid_init(int ueidx)
{
   struct _ueid_init sbuf;
   struct _ueid_init_reply rbuf;
   int status;
   
   sbuf.type = UEID_INIT;
   status = Send(ueidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
   if (status) return -1;
   return rbuf.status;
}

int ueid_get_samples(int ueidx, int board, int max_samples, u16 *buffer)
{
  struct _ueid_get_samples sbuf;
  struct _ueid_get_samples_reply rbuf;
  int status, i;

  sbuf.type = UEID_GET_SAMPLES;
  sbuf.board = board;
  //sbuf.max_samples = max_samples;
  sbuf.max_samples = SAMPLES;

  status = Send(ueidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));

  // shouldn't it copy max_samples elements only?
  // for(i=0;i<SAMPLES;i++)
  for(i=0;i<max_samples;i++)
    buffer[i] = rbuf.buffer[i];
  //  *buffer = rbuf.buffer;

  if (status) return -1;
  return rbuf.status;  
}

int ueid_sw_cl_start(int ueidx, int board)
{
  struct _ueid_sw_cl_start sbuf;
  struct _ueid_sw_cl_start_reply rbuf;
  int status;
  
  sbuf.type = UEID_SW_CL_START;
  sbuf.board = board;
  status = Send(ueidx, &sbuf, &rbuf, sizeof(sbuf), sizeof(rbuf));
  if (status) return -1;
  return rbuf.status;  
}  



