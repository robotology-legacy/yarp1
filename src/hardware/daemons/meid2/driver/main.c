/*
 * dmain.c -- MEI DSP Driver/Proxy
 *
 * created.  maddog 19980223
 *
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/kernel.h>
#include <stdio.h>

#include <errno.h>

#include "meid-int.h"
#include "meid-msg.h"


/* lame global variables */
short int verbose = 0;       /* verbosity level    */
short int debug = 0;         /* random debug level */
drv_info di;                 /* configuration data */


void parse_options(int argc, char *argv[], drv_info *di)
{
  int opt;

  di->priority   = DEF_PRIORITY;
  di->base_addr  = DEF_BASE_ADDRESS;
  di->irq        = 0;
  while ((opt = getopt(argc,argv,"p:V:b:i:d")) != -1) {
    switch(opt) {
    case 'p': di->priority = atoi(optarg); break;
    case 'b': di->base_addr = strtol(optarg, NULL, 0); break;
    case 'i': di->irq = atoi(optarg); break;
    case 'V': verbose = atoi(optarg); break;
    case 'd': debug++; break;
    default:  break;
    }
  }
  {
    char name[256];
    sprintf(name, "%s0x%x", DEF_DEVICE_BASENAME, di->base_addr);
    di->devicename = strdup(name);
  }

  if (di->irq != 0) {
    fprintf(stderr, "IRQ are not yet supported!\n", di->irq);
    exit(-1);
  }
  if (verbose) {
    fprintf(stderr, "  device name: %s\n", di->devicename);
    fprintf(stderr, "MEI base addr: 0x%x\n", di->base_addr);
  }
}



void main(int argc, char *argv[])
{
  pid_t pid;
  char reply;
  union msg msg;
  
  /* parse the command line */
  parse_options(argc, argv, &di);
  /* initialize the resource manager */
  manager_init(&di);
  /* do any hardware setup required when initializing */
  hardware_init(&di); 
  
  /**************** Message processing loop. ****************************/
  for(;;) {
    pid = Receive(0, &msg, sizeof(msg));

    if (verbose > 5) print_event(msg.type, pid);

    if (((msg.type & ~0xFF) == MEID_MSG_CLASS) &&
	((msg.type & 0xFF) < MEID_MSG_COUNT)) {
      /* super-quick dispatch for motor control messages */
#if 0 /* NO CHECKING WHATSOEVER!  BWAH-HAH-HAH! */
      if (pid != di.cntrl_pid) {
	msg.status = EBADF;  
	reply = REPLY;
      } else
#endif
	reply = (di.dispatch[msg.type & 0xFF])(&di, pid, &msg);
    } else {
      /* ol' fashioned dispatch for I/O messages */
      switch(msg.type) {
      case _SYSMSG: /* when a 'sin ver' is run this message comes in */
	reply = sys_msg(pid, &msg); break;
	
	/* standard IO manager messages */
      case _IO_OPEN:   
	reply = open_msg(&di, pid, &msg);     break;
      case _IO_CLOSE:   
	reply = close_msg(&di, pid, &msg);    break;
      case _IO_FSTAT:
	reply = fstat_msg(&di, pid, &msg);    break;
      case _IO_STAT:
	reply = stat_msg(&di, pid, &msg);     break;
      case _IO_READ:
	reply = read_msg(&di, pid, &msg);     break;
      case _IO_DUP:
	reply = dup_msg(&di, pid, &msg);      break;
#if 0
      case _IO_WRITE:
	reply = write_msg(&di, pid, &msg);    break;
      case _IO_QIOCTL:
	reply = qioctl_msg(&di, pid, &msg);   break;
#endif
#if 0
      case _IO_HANDLE:
	reply = handle_msg(pid);
	break;
      case _IO_CHMOD:
	reply = chmod_msg(pid);
	break;
      case _IO_CHOWN:
	reply = chown_msg(pid);
	break;
      case _IO_UTIME:
	reply = utime_msg(pid);
	break;
#endif
#if 0
      case _IO_CHDIR:
      case _IO_READDIR:
      case _IO_REWINDDIR:
      case _IO_LSEEK:             
      case _IO_RENAME:
      case _IO_GET_CONFIG:
      case _IO_FLAGS:
      case _IO_LOCK:
#endif
	
      default:
	if (verbose > 2) 
	  fprintf(stderr, "main loop:  %d unsupported!\n", msg.type);
	msg.status = ENOSYS;
	reply = REPLY;
	break;
      }
    }
    if (reply) Reply(pid, &msg, sizeof(msg));
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

  if ((msgtype & ~0xFF) == MEID_MSG_CLASS)
    fprintf(stderr,"Received MEI message from %d. Type = 0x%x\n", pid, 
	    msgtype);
  else
    fprintf(stderr,"Received IO message from %d. Type = %s\n", pid, 
	    &msg[msgtype&0xff][0]);
}

