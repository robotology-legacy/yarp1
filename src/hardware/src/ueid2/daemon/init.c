#include <stdlib.h>
#include <signal.h>
#include <i86.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/psinfo.h>
#include <sys/sched.h>
#include <sys/types.h>

#include <stdio.h>

#include <conio.h>

#include <sys/name.h>

#include "win2qnx.h"
#include "powerdaq.h"
#include "pd-int.h"
#include "pdfw_def.h"
#include "pdfw_if.h"

#include "ueid-int.h"
#include "extern.h"

//HACK
#define CL_SIZE     48

/****************************************************************************/
/*Function terminate() -This function is called when a terminate signal is */
/*			sent to the driver.  This function will detach the */
/*			device and the prefix. 				    */
/*Parameters Passed : none						    */
/****************************************************************************/

void terminate()
{
  int ret;

  qnx_name_detach(0, di.name_id);

  //HACK
  //reset the subsystem of board 0
  ret = pd_ain_reset(0);
  //clean the uei devices list
  ret = pd_clean_devices();

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
/*     perform any hardware specific operations before any                  */
/*     open/close/read/write requests are processed.       	            */
/*Parameters Passed: none						    */
/****************************************************************************/
void hardware_init(drv_info *di)
{

  //HACK
  //this is just lifted from the sample files
  // -- assumes only one uei board (0)

  u32 dwAInCfg;
  int ret, i;
  u32 dwChList[PD_MAX_CL_SIZE];

  //board 0 is the only board ... 
  int board = 0;
  
  ret = pd_find_devices();
  if(verbose > 2)
    printf("pd_find_device returned %d\n", ret);
  
  // reset subsystem
  ret = pd_ain_reset(board);
  
  // set mode
  dwAInCfg = AIN_CV_CLOCK_CONTINUOUS | AIN_BIPOLAR | AIN_RANGE_10V;
  ret |= pd_ain_set_config(board, dwAInCfg, 0, 0);    
  
  // set channel list
  for (i = 0; i < CL_SIZE; i++)
    dwChList[i] = CHLIST_ENT(i%(CL_SIZE/2), 0, TRUE);
  
  ret |= pd_ain_set_channel_list(board, CL_SIZE, dwChList);
  
  // enable conversion
  ret |= pd_ain_set_enable_conversion(board, TRUE);
  ret |= pd_ain_sw_start_trigger(board);
  
  // clock first channel list
  ret |= pd_ain_sw_cl_start(board);
  
}    














