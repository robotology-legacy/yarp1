#include <stdlib.h>
#include <sys/fd.h>
#include <errno.h>

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <conio.h>

#include <fcntl.h>
#include <malloc.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <i86.h>

#include <string.h>

#include <unix.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/psinfo.h>

#include "meid-int.h"
#include "meid-msg.h"



ocb_t *get_ocb(drv_info *di, pid_t pid, int fd)
{
  unsigned handle;

  handle = __get_fd(pid, fd, di->fd_ctrl);
  if ((handle == -1) || (handle == 0)) 
    return NULL;
  return (ocb_t *)handle;
}




mode_t check_access(drv_info *di, pid_t pid, int oflag, struct stat *sp)
{
  mode_t mode, reqmode;
  int accflag;
  uid_t uid;
  gid_t gid;
  struct _psinfo3 psinfo3;

  __get_pid_info(pid, &psinfo3, di->fd_ctrl);
  uid = psinfo3.euid;
  gid = psinfo3.egid;

  /* first, assign desired mode */
  accflag = oflag & O_ACCMODE;
  reqmode =
    (accflag == O_RDONLY) ? S_IREAD :
    (accflag == O_WRONLY) ? S_IWRITE :
    (accflag == O_RDWR) ? (S_IREAD | S_IWRITE) : 0;
  if (verbose > 5) 
    fprintf(stderr, "reqmode = 0%o\n", reqmode);
  /* then, check calling process has permissions for it (mask it) */
  mode = reqmode &
    ( (uid == 0) ? S_IRWXU :
      (uid == sp->st_uid) ? (sp->st_mode & S_IRWXU) :
      (gid == sp->st_gid) ? (sp->st_mode & S_IRWXG) << 3 :
      (sp->st_mode & S_IRWXO) << 6 );
  if (verbose > 5) 
    fprintf(stderr, "mode = 0%o\n", mode);
  return(mode);
}





/***********************************************************************/
/*Function close_msg() -This function is called when a IO_CLOSE message is */
/*			received.  This message is sent every time a 	   */
/*			application issues a close() on the file 	   */
/*			descriptor associated with the driver.  This 	   */
/*			function will reset the adapter and allow another  */
/*			application to open the driver.			   */
/*Parameters Passed :							   */
/*pid :	Process ID of application calling close().			   */
/***************************************************************************/

int close_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  ocb_t *ocb;

  if ((ocb = get_ocb(di, pid, io_msg->close.fd)) == NULL) {
    io_msg->status = EBADF;
    return(REPLY);
  }

  if (--ocb->count <= 0) {
    if (ocb->msg != NULL) 
      free(ocb->msg);
    if (ocb->mode & S_IWRITE) di->active = 0;
    free(ocb);
  }
  di->fd_count--;

  /* Have to map a zero where this ocb was. */
  qnx_fd_attach(pid, io_msg->close.fd, 0, 0, 0, 0, 0);
  io_msg->close_reply.status = EOK;
  return(REPLY);
}





/****************************************************************************/
/*Function open_msg() - This function is called when the IO_OPEN message    */
/*		is received.  This function will check access 	            */
/*		rights of the application opening the driver as             */
/*		well as make sure the driver is not already open.           */
/*		It will then associate a OCB with the file 		    */
/*		descriptor which is sent with the message. 		    */
/*Parameters Passed:							    */
/*	pid :	Process ID of calling application.			    */
/****************************************************************************/

int open_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  mode_t mode;
  ocb_t *ocb;
  
  mode = check_access(di, pid, io_msg->open.oflag, &(di->fstat));

  if ((mode & S_IRWXU) == 0) {
    io_msg->status = EPERM;
    return REPLY;
  }
  if ((mode & S_IWRITE) && (di->active)) {
    io_msg->status = EBUSY;
    return REPLY;
  }
  if ((ocb = calloc(1, sizeof(*ocb))) == NULL) {
    io_msg->status = ENOMEM;
    return REPLY;
  }
  ocb->mode = mode;
  ocb->count = 1;   /* ????? */
  ocb->offset = 0;
  ocb->msg = NULL;

  if (qnx_fd_attach(pid, io_msg->open.fd, 0, 0, 0, 0, (unsigned)ocb) == -1) {
    perror("fd_attach");
    io_msg->status = errno;
    free(ocb);
    return REPLY;
  }

  di->fd_count++;
  if (mode & S_IWRITE) di->active = 1;

  fprintf(stderr, "fd:  %d   pid:  %d   ocb: 0x%p\n",
	  io_msg->open.fd, pid, ocb);
  io_msg->open_reply.status = EOK;
  return(REPLY);
}


char themessage[] =
"\nI am the MEI driver.  If you feed me and clean me and care for me,\n"
"I will grow up big and strong and happy and I will always be your\n"
"friend.\n\n";


int read_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  struct _mxfer_entry mx[2];
  int num_bytes;
  ocb_t *ocb;
  size_t remaining;
  
  if ((ocb = get_ocb(di, pid, io_msg->read.fd)) == NULL) {
    io_msg->status = EBADF;
    return(REPLY);
  }
  /* Check access modes. */
  if(!(ocb->mode & S_IREAD)) {
    io_msg->status = EBADF;
    return(REPLY);
  }

  if (ocb->msg == NULL) {
    if ((ocb->msg = calloc(256, sizeof(ocb->msg[0]))) == NULL) {
      io_msg->status = ENOMEM;
      return REPLY;
    }
    snprintf(ocb->msg, 256, 
	     "MEIDv2\n"
	     "base address: 0x%x\n"
	     "open fd's: %d\n"
	     "controller active? %s\n"
	     "mode - 0x%x\n",
	     di->base_addr, di->fd_count, 
	     (di->active == 0) ? "no" : "yes",
	     ocb->mode);
    ocb->offset = 0;
    ocb->msglen = strlen(ocb->msg);
  }
  
  /*num_bytes = io_msg->read.nbytes;*/
  /*  remaining = sizeof(themessage) - ocb->offset;*/
  remaining = ocb->msglen - ocb->offset;
  num_bytes = 
    (remaining > io_msg->read.nbytes) ? io_msg->read.nbytes :
    remaining;

  io_msg->read_reply.status = EOK;
  io_msg->read_reply.nbytes = num_bytes;
  
  _setmx(&mx[0], io_msg, 
	 sizeof(io_msg->read_reply) - sizeof(io_msg->read_reply.data));
  _setmx(&mx[1], ocb->msg + ocb->offset, num_bytes);
  Replymx(pid, 2, &mx);

  ocb->offset += num_bytes;

  return(NO_REPLY);
}




/****************************************************************************/
/*Function fstat_msg() - This function is call when the IO_FSTAT message is */
/*                        received. This function will reply with the fstat  */
/*                        structure which is kept updated throughout the     */
/*                        driver in the appropriate functions.               */
/*Parameters Passed:                                                   */
/*   pid :  Process ID of calling application.				    */
/******************************************************************************/
int fstat_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  if (get_ocb(di, pid, io_msg->read.fd) == NULL) {
    io_msg->status = EBADF;
    return(REPLY);
  }
  memcpy(&(io_msg->fstat_reply.stat), &(di->fstat), sizeof(struct stat));
  io_msg->fstat_reply.status = EOK;
  return(REPLY);
}


/****************************************************************************/
/*Function stat_msg() - This function is called when the IO_STAT message    */
/*		  is received.  This function will reply with the 	  */
/*		  fstat_reply structure to the calling application.	  */
/*Parameters Passed:						          */
/* pid :	Process ID of calling application.			   */
/******************************************************************** ****/
int stat_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  if(io_msg->open.path[0] == NULL) {
    memcpy(&(io_msg->fstat_reply.stat), &(di->fstat), sizeof(struct stat));
    io_msg->fstat_reply.status = EOK;
  } else {
    io_msg->status = ENOENT;
  }
  return(REPLY);
}



/*****************************************************************************/
/*Function dup_msg() - This function is called when a IO_DUP message is sent.*/
/*                     This message is sent either when a application 	     */
/*			 directly calls the dup() function or indirectly     */
/*			 during a fork() operation.	This function will   */
/*                       associate the OCB currently setup for the parent to */
/*                       the child process.                                  */
/*Parameters Passed :				                             */
/*	pid :	Process ID of process calling dup() or PID of child process. */
/*****************************************************************************/
int dup_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  ocb_t *ocb;

  if ((ocb = get_ocb(di,
		     io_msg->dup.src_pid ? io_msg->dup.src_pid : pid,
		     io_msg->dup.src_fd)) == NULL) {
    io_msg->status = EBADF;
    return(REPLY);
  }

  if (qnx_fd_attach(pid, io_msg->dup.dst_fd,
		    0, 0, 0, 0, (unsigned)ocb) == -1) {
    perror("fd_attach");
    io_msg->status = errno;
    return REPLY;
  }
  ocb->count++;
  di->fd_count++;
  io_msg->dup_reply.status = EOK;
  return REPLY;
}


#if 0
int write_msg(drv_info *di, pid_t pid, union msg *io_msg)
{
  char *buffer = NULL;
  int i;

  if (!(verify_fd(di, pid, io_msg->write.fd))) {
    io_msg->status = EBADF;
    return(REPLY);
  }

  /* Check access modes. */
  if(!(di->mode & S_IWRITE)) {
    io_msg->write_reply.status = EBADF;
    return(REPLY);
  }

  /* note that this buffer could be a '(far *)' buffer as well */
  if ((buffer = (char *)malloc(io_msg->write.nbytes)) == NULL)  {
    io_msg->status = ENOMEM;
    return(REPLY);
  }
  
  io_msg->write_reply.nbytes =
    Readmsg(pid, sizeof(io_msg->write) - sizeof(io_msg->write.data), 
	    buffer, io_msg->write.nbytes);
  if ( io_msg->write_reply.nbytes == -1 ) {
    io_msg->status = errno;
    io_msg->write_reply.status = errno;
    if ( buffer ) free(buffer);
    return(REPLY);
  }
  
  /*	
   * 	Now that the data to be written has been obtain from the application
   *	you could now setup the device and start writing this data to it.
   *  e.g. write 'io_msg.write.nbytes' of data from 'buffer' ...
   *
   *  If verbose is turned on, we write the data to stderr as a test 
   */
  if (verbose > 5) {
    fprintf(stderr,"write_msg:  writing %d bytes of data out... \n",
	    io_msg->write.nbytes);
/*    write(1, buffer, io_msg->write.nbytes);*/
  }

  io_msg->status = EOK;
  io_msg->write_reply.status = EOK;
  if ( buffer ) free(buffer);
  return(REPLY);
}
#endif






#if 0


#include <sys/fd.h>
#include "struct.h"

/***************************************************************************/
/*Function chmod_msg() - This function is called when the IO_CHMOD message  */
/*		   is receive.  this function will change the read,   */
/*		   write and execute permissions of the driver to 	  */
/*		   what is received in the message.			  */
/*Parameters Passed:							  */
/*pid :	Process ID of calling application.			       */
/******************************************************************************/
int chmod_msg(pid_t pid)
{
  struct ocb *ocb;
  uid_t uid;
  gid_t gid;
  
   if ((ocb = get_ocb(pid, io_msg.chmod.fd)) == -1)
     {
       io_msg.status = EBADF;
       return(REPLY);
     }
  // Get group and user id of calling process.
    get_ids( pid, &uid, &gid );
  /*
   * ROOT  anytime
   * OWNER anytime
   * GROUP never
   * OTHER never
   */
  if( uid == 0  ||  uid == daq_stat.st_uid) 
    {
      daq_stat.st_mode = daq_stat.st_mode & ~0777 | (io_msg.chmod.mode & 0777) ;   /* Change access modes. */
      daq_stat.st_ctime = *timep;	// Time of last status change.
	io_msg.status = EOK;
    }
  else
    io_msg.status = EACCES;
  return(REPLY);
}
#include <sys/fd.h>
#include "struct.h"
#include "proto.h"

/*****************************************************************************/
/*Function chown_msg() -This function is called when the IO_CHOWN message */
/*			is received.  This function will change the 	  */
/*			ownership of the driver to what is received in the*/
/*			message. 		 			  */
/*Parameters Passed:	 					  */
/* pid :	Process ID of calling application.			]*/
/******************************************************************************/
int chown_msg(pid_t pid)
{
  uid_t uid;
  gid_t gid;
  struct ocb  *ocb;
  
  if ((ocb = get_ocb(pid, io_msg.chown.fd)) == -1)
    {
      io_msg.status = EBADF;
      return(REPLY);
    }
  
  get_ids( pid, &uid, &gid );
  /*
   * ROOT  anytime
   * OWNER (can change group, but not owner)
   * GROUP never
   * OTHER never
   */
  if (uid == 0  ||
      (uid == daq_stat.st_uid  &&  io_msg.chown.uid == daq_stat.st_uid)) {
      daq_stat.st_uid = io_msg.chown.uid; /* Change user ID.  */
      daq_stat.st_gid = io_msg.chown.gid; /* Change group ID. */
      daq_stat.st_ctime = *timep;	  /* Time of last status change.*/
      io_msg.status = EOK;
  } else
    io_msg.status = EACCES;
  return(REPLY);
}
#include <stdlib.h>
#include "struct.h"

/*****************************************************************************/
/*Function handle_msg() -This function is called when the IO_HANDLE message */
/*                       is called.                                         */
/*Parameters Passed:				  		   */
/*	pid :	Process ID of calling application.			   */
/****************************************************************************/

int handle_msg(pid_t pid)
{
  uid_t uid;
  gid_t gid;
  struct ocb  *ocb;
  
  get_ids(pid, &uid, &gid);	/* Get egid and euid of process pid.*/
  io_msg.status = EOK;
  
  switch(io_msg.open.oflag) {
  case _IO_HNDL_CHANGE:
    if((uid != 0) && (uid != daq_stat.st_uid)) {
      io_msg.status = EPERM; /* Must be super user or have equivalent euid */
      break;
    }
    
    if ((ocb = (struct ocb *)calloc(1, sizeof(struct ocb))) == NULL) {
      io_msg.status = ENOMEM;
      return(REPLY);
    }
    
    if(map_ocb(pid, io_msg.open.fd, ocb) == -1)
      io_msg.status = errno;
    else
      ocb->count++;
    break;
    
  case _IO_HNDL_INFO:
  case _IO_HNDL_UTIME:
    if((check_access(pid, &daq_stat) & S_IRWXU) == 0) {
      io_msg.status = EPERM;
      break;
    }
    if ((ocb = (struct ocb *)calloc(1, sizeof(struct ocb))) == NULL) {
      io_msg.status = ENOMEM;
      return(REPLY);
    }
    
    if(map_ocb(pid, io_msg.open.fd, ocb) == -1)
      io_msg.status = errno;
    else
      ocb->count++;
    break;
    
  case _IO_HNDL_RDDIR:
  case _IO_HNDL_LOAD:
  case _IO_HNDL_CLOAD:
    io_msg.status = ENOSYS;
    break;
  }
  return(REPLY);
}

#include <fcntl.h>
#include <sys/fd.h>
#include "struct.h"
#include "define.h"

/******************************************************************************/
/*	Function utime_msg() -	This function is called when the touch command is */
/*							the driver  (/dev/___). It will update the access */
/*							time stamp to the current time.	                  */
/* 		Parameters Pass :													  */
/*				pid :	PID of calling process.								  */
/******************************************************************************/
int utime_msg(pid_t pid)
{
   	struct ocb  *ocb;
   
   	if ((ocb = get_ocb(pid, io_msg.utime.fd)) == -1)
	{
		io_msg.status = EBADF;
		return(REPLY);
	}
					/* Calling process must have write permission. */
   	if(!(ocb->mode & S_IWRITE) || ((ocb->oflags & O_ACCMODE) == O_WRONLY))
   	{
		io_msg.utime_reply.status = EBADF;
		return(REPLY);
	}
	
	if(io_msg.utime.cur_flag)	/* Set to current time.	*/
	{
		daq_stat.st_atime = *timep;
		daq_stat.st_mtime = *timep;
		io_msg.fstat_reply.stat.st_atime = *timep;
		io_msg.fstat_reply.stat.st_mtime = *timep;
	}
	else								/* Set to time passed with message.	*/
	{
		daq_stat.st_mtime = io_msg.utime.modtime;
		daq_stat.st_atime = io_msg.utime.actime;
		io_msg.fstat_reply.stat.st_mtime = io_msg.utime.modtime;
		io_msg.fstat_reply.stat.st_atime = io_msg.utime.actime;
	}
	io_msg.utime_reply.status = EOK;
	return(REPLY);
}



#endif
