
#ifndef _UEID_INT_H
#define _UEID_INT_H


#define VERSION_NAME "UEI Daemon"
#define VERSION_NUMBER 001
#define VERSION_LETTER 'A'
#define VERSION_DATE __DATE__

#define DEF_PROCESS_NAME  "/yarp/uei"
#define DEF_PRIORITY     10
#define DEF_BASE_ADDRESS 0x700

typedef struct drv_info {
  char *procname;    /* registered name of the driver process */
  int priority;      /* priority of driver process */
  int base_addr;     /* card's I/O base address */
  int name_id;       /* name_id returned by name_attach */
} drv_info;


#define NO_REPLY     0
#define REPLY	     1

#define TRUE         1
#define FALSE        0


#endif /* _UEID_INT_H */
