
#ifndef _MEID_INT_H
#define _MEID_INT_H

#include <sys/stat.h>


#define VERSION_NAME "MEI Driver"
#define VERSION_NUMBER 202
#define VERSION_LETTER 'B'
#define VERSION_DATE __DATE__

#define DEF_BASE_ADDRESS 0x320
#define DEF_DEVICE_BASENAME  "/dev/mei"
#define DEF_PRIORITY     10


typedef struct ocb_t {
  mode_t mode;
  int    count;
  off_t  offset;
  char *msg;
  size_t msglen;
} ocb_t;


/* pointer to a msg handling function */
typedef int (*msgfunc_t)();


typedef struct drv_info {
  char *devicename;  /* name of the device */
  int priority;      /* priority of driver process */
  int base_addr;     /* card's I/O base address */
  int irq;           /* card's IRQ lines; zero if IRQ not used */

  int devno;         /* device number */

  void *fd_ctrl;     /* weird ptr for __XXX_fd() functions */
  struct stat fstat; /* file status block */
  time_t __far *timep; /* ????? */
  int fd_count;      /* count of open file descriptors for device */

  int active;        /* is device open for write? */

  msgfunc_t *dispatch;

} drv_info;




#define NO_REPLY     0
#define REPLY	     1

#define TRUE         1
#define FALSE        0




/* global variables (lame but convenient) */
/*   (main.c) */
extern short int verbose;
extern short int debug;
extern drv_info di;


/* initialization and exit functions (init.c) */
void manager_init(drv_info *di);
void hardware_init(drv_info *di);
void terminate();

/* utility/glue functions (util.c) */
void mei_error_check(int error_code);

/* system message handler (sys_msg.c) */
int sys_msg(pid_t pid, union msg *io_msg);

/* io mgr functions  [io.c] */
int close_msg(drv_info *di, pid_t pid, union msg *io_msg);
int open_msg(drv_info *di, pid_t pid, union msg *io_msg);
int read_msg(drv_info *di, pid_t pid, union msg *io_msg);
int fstat_msg(drv_info *di, pid_t pid, union msg *io_msg);
int stat_msg(drv_info *di, pid_t pid, union msg *io_msg);
int write_msg(drv_info *di, pid_t pid, union msg *io_msg);
int dup_msg(drv_info *di, pid_t pid, union msg *io_msg);

/* mei dispatch [wraps.c] */
int init_dispatch_table(drv_info *di);




#endif /* _MEID_INT_H */
