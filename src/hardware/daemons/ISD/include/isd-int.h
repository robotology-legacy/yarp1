
#ifndef _ISD_INT_H
#define _ISD_INT_H


#define VERSION_NAME "InterSense Daemon"
#define VERSION_NUMBER 001
#define VERSION_LETTER 'A'
#define VERSION_DATE __DATE__

#define DEF_PROCESS_NAME  "/yarp/isd"
#define DEF_PRIORITY     10
#define DEF_SERIAL_PORT   "//3/dev/ser1"

typedef struct drv_info {
  char *procname;    /* registered name of the driver process */
  int priority;      /* priority of driver process */
  char *serialport;  /* name of the serial port, e.g. //3/dev/ser1 */
  int fd;            /* File pointer to the open serial port */
  int name_id;       /* name_id returned by name_attach */
  int compass_mode;  /* compass heading correction mode (p. 39, ISD manual) */
} drv_info;


#define NO_REPLY     0
#define REPLY	     1

#define TRUE         1
#define FALSE        0


#endif /* _ISD_INT_H */
