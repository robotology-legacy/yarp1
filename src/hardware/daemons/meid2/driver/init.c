#include <stdlib.h>
#include <signal.h>
#include <i86.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/psinfo.h>
#include <sys/osinfo.h>
#include <sys/sched.h>
#include <sys/types.h>

#include <sys/prfx.h>
#include <sys/fd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>

#include <stdio.h>

#include <conio.h>

#include <sys/name.h>

#include <mei/pcdsp.h>
#include "meid-int.h"




/****************************************************************************/
/*Function terminate() -This function is called when a terminate signal is */
/*			sent to the driver.  This function will detach the */
/*			device and the prefix. 				    */
/*Parameters Passed : none						    */
/****************************************************************************/

void terminate()
{
  qnx_device_detach(di.devno);
  qnx_prefix_detach(di.devicename);
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
  struct _osinfo osdata;


  /* Boost our priority. */
  setprio(0, di->priority);  
  /* Allow only SIGTERM for graceful termination, ignore all others. */
  for( i = _SIGMIN; i <= _SIGMAX; ++i) 
    if( i != SIGTERM) signal( i, SIG_IGN);
  /* Setup the terminate signal handler function. */
  signal(SIGTERM, &terminate);
  /* Get system messages. */
  if (qnx_pflags(~0, _PPF_SERVER, NULL, NULL)) {
    perror("pflags");
    exit(-1);
  }
  /* Attach a device number to our I/O manager */
  if ((di->devno = qnx_device_attach()) == -1) {
    perror("qnx_device_attach");
    exit(-1);
  }

  if (qnx_osinfo(0, &osdata) == -1) {
    perror("qnx_osinfo");
    exit(-1);
  }  
  /* Get a pointer to system time segment. */
  di->timep = MK_FP(osdata.timesel, offsetof(struct _timesel, seconds));

  /* Attach prefix for device */
  if (qnx_prefix_attach(di->devicename, NULL, 0)) {
    perror("qnx_prefix_attach");
    exit(-1);
  }

  di->fd_count = 0;
  di->active = 0;
  if (init_dispatch_table(di)) {
    fprintf(stderr, "COULDN'T MAKE DISPATCH TABLE!!!\n");
    exit(1);
  }
  if ((di->fd_ctrl = __init_fd(getpid())) == NULL) {
    perror("__init_fd");
    exit(1);
  }


  /* Initialize individual stat structure. */
  di->fstat.st_ino = 1;
  /* Major and Minor Device number w/node ID. */
  di->fstat.st_rdev = (dev_t) ((getnid() << 16L) + (di->devno << 10L)); 
  /* really IFCHR, or IFREG?? */
  di->fstat.st_mode = 
    S_IFCHR | 
    S_IRUSR | S_IWUSR |
    S_IRGRP | S_IWGRP |
    S_IROTH | S_IWOTH;
  di->fstat.st_nlink = 1;           /* number of links.*/  
  di->fstat.st_ftime = *(di->timep); /* Time of creation.       */
  di->fstat.st_atime = *(di->timep); /* Time of last access.*/
  di->fstat.st_mtime = *(di->timep); /* Time of last modification.      */
  di->fstat.st_ctime = *(di->timep); /* Time of last status change.*/
  di->fstat.st_uid = 0;
  di->fstat.st_gid = 0;
  di->fstat.st_status = _FILE_USED;
  di->fstat.st_size = 1;
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
  int16 err;
  char buffer[MAX_ERROR_LEN];
#if 1
  err = dsp_init(di->base_addr);
  if (err) {
    error_msg(err, buffer);
    fprintf(stderr, "dsp_init failed! `%s' (%d)\n", buffer, err);
    exit(1);
  }

  if (verbose > 1)
    fprintf(stderr, "dsp_init ok; version %f%c\n",
	    (double)dsp_version() / 1600.0, (dsp_version() & 0xF) + 'A');
#endif  
}



