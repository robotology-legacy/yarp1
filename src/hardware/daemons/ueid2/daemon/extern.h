
#ifndef _DEXTERN_H_
#define _DEXTERN_H_

/* global variables (lame but convenient) */
/*   (main.c) */
extern short int verbose;
extern short int debug;
extern drv_info di;

/* initialization and exit functions (init.c) */
void manager_init(drv_info *di);
void hardware_init(drv_info *di);
void terminate();

/* system message handler (sys_msg.c) */
int sys_msg(pid_t pid, union msg *io_msg);

/* simple UEI wrapper handlers (wraps.c) */
int do_init(drv_info *di, pid_t pid, union msg *io_msg);
int do_get_samples(drv_info *di, pid_t pid, union msg *io_msg);
int do_sw_cl_start(drv_info *di, pid_t pid, union msg *io_msg);

/* composite functions (wraps.c) */
int do_calibrate(drv_info *di, pid_t pid, union msg *io_msg);

#endif /* _DEXTERN_H_ */

