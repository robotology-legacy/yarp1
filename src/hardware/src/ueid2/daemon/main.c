/*
 * main.c -- UEI A/D Driver/Proxy
 *
 * created.  scaz 19980305
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/kernel.h>
#include <stdio.h>

#include <errno.h>

#include "ueid.h"
#include "ueid-int.h"
#include "ueid-msg.h"
#include "extern.h"

/* lame global variables */
short int verbose = 0;       /* verbosity level    */
short int debug = 0;         /* random debug level */
drv_info di;                 /* configuration data */

void parse_options(int argc, char *argv[], drv_info *di)
{
   int opt;
   
   di->procname   = NULL;
   di->priority   = DEF_PRIORITY;
   di->base_addr  = DEF_BASE_ADDRESS;
   while ((opt = getopt(argc,argv,"n:p:V:b:i:d")) != -1) {
      switch(opt) {
       case 'n': di->procname = strdup(optarg); break;
       case 'p': di->priority = atoi(optarg); break;
       case 'b': di->base_addr = atoi(optarg); break;
       case 'V': verbose = atoi(optarg); break;
       case 'd': debug++; break;
       default:  break;
      }
   }
   if (di->procname == NULL) 
     di->procname = strdup(DEF_PROCESS_NAME);
   if (verbose) {
      fprintf(stderr, "UEI process name: %s\n", di->procname);
      //fprintf(stderr, "UEI base addr: 0x%x\n", di->base_addr);
   }
}



void main(int argc, char *argv[])
{
   pid_t pid;
   char reply;
   union msg io_msg;
  
   /* parse the command line */
   parse_options(argc, argv, &di);
   /* initialize the resource manager */
   manager_init(&di);
   /* do any hardware setup required when initializing */
   hardware_init(&di); 
  
   /**************** Message processing loop. ****************************/
   for(;;) {
      pid = Receive(0, &io_msg, sizeof(io_msg));

      if (verbose > 5) print_event(io_msg.type, pid);
      
      switch(io_msg.type) {
       case _SYSMSG: /* when a 'sin ver' is run this message comes in */
	 reply = sys_msg(pid, &io_msg); break;
       case UEID_INIT:
	 reply = do_init(&di, pid, &io_msg); break;
       case UEID_GET_SAMPLES:
	 reply = do_get_samples(&di, pid, &io_msg); break;
       case UEID_SW_CL_START:
	 reply = do_sw_cl_start(&di, pid, &io_msg); break;
       default:
	 if (verbose > 2) 
	   fprintf(stderr, "main loop:  %d unsupported!\n", io_msg.type);
	 io_msg.status = ENOSYS;
	 reply = REPLY;
	 break;
      }
      if (reply) Reply(pid, &io_msg, sizeof(io_msg));
   }
}



void print_event(int msgtype, pid_t pid)
{
   char msg[30][15] = {"SYS_MSG","_IO_OPEN","_IO_CLOSE","_IO_READ","_IO_WRITE",
      "_IO_LSEEK","_IO_RENAME","_IO_GET_CONFIG","_IO_DUP",
      "_IO_HANDLE","_IO_FSTAT","_IO_CHMOD","_IO_CHOWN",
      "_IO_UTIME","_IO_FLAGS","_IO_LOCK","_IO_CHDIR", "???",
      "_IO_READDIR","_IO_REWINDDIR","_IO_IOCTL","_IO_STAT",
      "_IO_SELECT"," _IO_QIOCTL"};
   
   fprintf(stderr,"Received message from %d. Type = %s\n", pid, 
	   &msg[msgtype&0xff][0]);
}

