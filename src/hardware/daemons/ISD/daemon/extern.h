
#ifndef _DEXTERN_H_
#define _DEXTERN_H_

#include <stdlib.h>
#include <signal.h>
#include <i86.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dev.h>
#include <sys/psinfo.h>
#include <sys/sched.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <stdio.h>
#include <conio.h>
#include <sys/name.h>
#include <errno.h>

/* global variables (lame but convenient) */
/*   (main.c) */
extern short int verbose;
extern short int debug;
extern drv_info di;
extern char _isd_readbuf[100]; // should be a max of the record sizes
extern int output_is_polled;
extern int output_is_ascii;

/* initialization and exit functions (init.c) */
void manager_init(drv_info *di);
void hardware_init(drv_info *di);
void terminate();

/* system message handler (sys_msg.c) */
int sys_msg(pid_t pid, union msg *io_msg);
void isd_send_msg(char *msg);

/* InterSense wrapper handlers (wraps.c) */
int do_poll_all(drv_info *di, pid_t pid, union msg *io_msg);
int do_boresight(drv_info *di, pid_t pid, union msg *io_msg);
int do_unboresight(drv_info *di, pid_t pid, union msg *io_msg);
int do_set_output_mode(drv_info *di, pid_t pid, union msg *io_msg);
int do_get_output_mode(drv_info *di, pid_t pid, union msg *io_msg);

#define ISD_STATUS_MSG "S"
#define ISD_POLL_MSG   "P"
#define ISD_BORESIGHT_MSG "MB1\x0D\x0A"
#define ISD_UNBORESIGHT_MSG "Mb1\x0D\x0A"
#define ISD_COMPASS_USE_FULL_MSG "MH1,2\x0D\x0A"
#define ISD_COMPASS_USE_PARTIAL_MSG "MH1,1\x0D\x0A"
#define ISD_COMPASS_USE_NONE_MSG "MH1,0\x0D\x0A"
#define ISD_OUTPUT_POLLED_MSG "c"
#define ISD_OUTPUT_CONTINUOUS_MSG  "C" 
#define ISD_OUTPUT_ASCII_MSG "F"
#define ISD_OUTPUT_BINARY_MSG  "f"

#define ISD_ASCII_RECORD_SIZE   47 // (3+7*6+2)
#define ISD_BINARY_RECORD_SIZE   29 // (3+4*6+2)
#define ISD_STATUS_RECORD_SIZE 55

#endif /* _DEXTERN_H_ */

