/*
 * main.c -- InterSense Daemon main loop
 *
 * created.  scaz 11/13/99
 *
 */

#include "isd-int.h"
#include "isd-msg.h"
#include "extern.h"

/* lame global variables */
short int verbose = 0;       /* verbosity level    */
short int debug = 0;         /* random debug level */
drv_info di;                 /* configuration data */
char _isd_readbuf[100]; // should be a max of the record sizes

void parse_options(int argc, char *argv[], drv_info *di)
{
  int opt;
  
  di->procname   = NULL;
  di->priority   = DEF_PRIORITY;
  di->serialport  = DEF_SERIAL_PORT;
  di->compass_mode = 2; // default is FULL compass mode
  while ((opt = getopt(argc,argv,"n:p:V:s:c:d")) != -1) {
    switch(opt) {
    case 'n': di->procname = strdup(optarg); break;
    case 'p': di->priority = atoi(optarg); break;
    case 'c': di->compass_mode = atoi(optarg); break;
    case 's': di->serialport = strdup(optarg); break;
    case 'V': verbose = atoi(optarg); break;
    case 'd': debug++; break;
    default:  break;
    }
  }

  if (di->procname == NULL) 
    di->procname = strdup(DEF_PROCESS_NAME);
  
  if (verbose) {
    fprintf(stderr, "ISD process name: %s\n", di->procname);
    fprintf(stderr, "ISD serial port: %s\n", di->serialport);
    fprintf(stderr, "ISD priority %d\n", di->priority);
    fprintf(stderr, "ISD compass mode %d\n", di->compass_mode);
    fprintf(stderr, "ISD verbose level %d\n", verbose);
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
    
    switch(io_msg.type) 
      {
      case _SYSMSG: /* when a 'sin ver' is run this message comes in */
	reply = sys_msg(pid, &io_msg); break;
      case ISD_POLL_ALL:
	reply = do_poll_all(&di, pid, &io_msg); break;
      case ISD_BORESIGHT:
	reply = do_boresight(&di, pid, &io_msg); break;
      case ISD_UNBORESIGHT:
	reply = do_unboresight(&di, pid, &io_msg); break;
      case ISD_GET_OUTPUT_MODE:
	reply = do_get_output_mode(&di, pid, &io_msg); break;
      case ISD_SET_OUTPUT_MODE:
	reply = do_set_output_mode(&di, pid, &io_msg); break;
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

