#include "isd-int.h"
#include "extern.h"
#include<sys/qioctl.h>

int output_is_ascii = TRUE;
int output_is_polled = TRUE;

/****************************************************************************/
/*Function terminate() -This function is called when a terminate signal is */
/*			sent to the driver.  This function will detach the */
/*			device and the prefix. 				    */
/*Parameters Passed : none						    */
/****************************************************************************/

void terminate()
{
  qnx_name_detach(0, di.name_id);
  close(di.fd);
  close(1);
  exit(0);
}


/****************************************************************************/
/*Function manager_init()                                                   */
/*    -	This function is called from the main() during the 		 */
/*	initialization of the driver.  This function will 		 */
/*	register this application as a device driver, notify the */
/*	kernel that we are a server so that	we will receive 	 */
/*	system messages.						 */
/* Parameters Passed: char *devicename, int priority                        */
/****************************************************************************/
void manager_init(drv_info *di)
{
  int i;

  /* Boost our priority. */
  setprio(0, di->priority);  
  /* Allow only SIGTERM for graceful termination, ignore all others. */
  /* Ignore signals all signal except SIGTERM. */
  for( i = _SIGMIN; i <= _SIGMAX; ++i) 
    if( i != SIGTERM) signal( i, SIG_IGN);
  /* Setup the terminate signal handler function. */
  signal(SIGTERM, &terminate);
  /* Get system messages. */
  if (qnx_pflags(~0, _PPF_SERVER, NULL, NULL)) {
    perror("pflags");
    exit(-1);
  }
  /* Attach name for our process */
  di->name_id = qnx_name_attach(0, di->procname);
  if (di->name_id == -1) {
    perror("qnx_prefix_attach");
    exit(-1);
  }
}



/****************************************************************************/
/*Function hardware_init()                                                  */
/*    -This function is called from the main() during the 		    */
/*     initialization of the driver.  This function will 		    */
/*     preform any hardware specific operations before any                  */
/*     open/close/read/write requests are processed.       	            */
/*Parameters Passed: none						    */
/****************************************************************************/
void hardware_init(drv_info *di)
{
  int j;
  struct _dev_info_entry info;
  long io_st[2] = {0L, 0L};
  long io_rtn;
  int cts_ready = FALSE;
  
  di->fd = open( di->serialport, O_RDWR );
  
  if (verbose > 1)
    fprintf(stderr, "%s Number %d Version %c (%s)\n",
	    VERSION_NAME, VERSION_NUMBER, VERSION_LETTER, VERSION_DATE);
  
  if (verbose > 3)
    {
      j = dev_info(di->fd, &info);
      fprintf(stderr, "Serial Port info (from dev_info):\n");
      fprintf(stderr, "  tty %d unit %d node %d pid_driver %d\n", info.tty, info.unit,
	      info.nid, info.driver_pid);
      fprintf(stderr, "  nature %d attributes %d capabilities %d driver_type<%s>\n", 
	      info.nature, info.attributes, info.capabilities, info.driver_type);
      fprintf(stderr, "  tty_name<%s>\n", info.tty_name);
    }
  
  while (!cts_ready)
    {
      if (verbose > 2) fprintf(stderr, "Waiting for CTS ready...\n");
      qnx_ioctl(di->fd, QCTL_DEV_CTL, io_st, 8, &io_rtn, 4);
      if (io_rtn & (1 << 20))
	cts_ready = TRUE;
      else
	delay(200);
    }
  
  if (verbose > 2)
    fprintf(stderr, "CTS shows ready... on with the show\n");

  output_is_polled = TRUE;
  isd_send_msg(ISD_OUTPUT_POLLED_MSG);
  
  output_is_ascii = TRUE;
  isd_send_msg(ISD_OUTPUT_ASCII_MSG);

  switch(di->compass_mode)
    {
    case 0:
      isd_send_msg(ISD_COMPASS_USE_NONE_MSG);
      break;
    case 1:
      isd_send_msg(ISD_COMPASS_USE_PARTIAL_MSG);
      break;
    case 2:
      isd_send_msg(ISD_COMPASS_USE_FULL_MSG);
      break;
    default:
      break;
    }
      
  if (verbose > 2)
    {
      fprintf(stderr, "InterSense: Checking diagnostics\n");
      j = write(di->fd, ISD_STATUS_MSG, strlen(ISD_STATUS_MSG));
      j = dev_read(di->fd, &_isd_readbuf, ISD_STATUS_RECORD_SIZE, 
		   ISD_STATUS_RECORD_SIZE, 0, 0, 0, 0 );
      fprintf(stderr, "InterSense diagnostics report : (read %d chars)\n", j);
      fprintf(stderr, "    <%53.53s>\n", _isd_readbuf);
    }
  
  
}
   

