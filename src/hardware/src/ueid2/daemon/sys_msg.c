#include <sys/kernel.h>
#include <errno.h>

#include "ueid-int.h"
#include "ueid-msg.h"


/****************************************************************************/
/*Function sys_msg() -	This function is required to respond to system 	    */
/*      messages.  At this time it only supports the system                 */
/*	message subtype _SYSMSG_SUBTYPE_VERSION which is 	            */
/*	issued by the sin utility.				      	    */
/*Parameters Passed:			 			            */
/*pid :	Process ID of calling application.			      	    */
/****************************************************************************/

int sys_msg(pid_t pid, union msg *io_msg)
{
   int reply;
   struct _mxfer_entry mx[2];
   struct _sysmsg_version_reply version = {
      VERSION_NAME,
      VERSION_DATE,
      VERSION_NUMBER,
      VERSION_LETTER,
      0 };
   
   switch(io_msg->sysmsg.subtype) {
      /* Send driver version back to sin utility. */
    case _SYSMSG_SUBTYPE_VERSION:	
      io_msg->sysmsg_reply.status = EOK;
      _setmx(&mx[0], io_msg, sizeof(io_msg->sysmsg_reply));
      _setmx(&mx[1], &version, sizeof(version));
      Replymx(pid, 2, &mx);
      reply = NO_REPLY;
      break;
      /* The following system messages are not supported by this driver. */
    case _SYSMSG_SUBTYPE_DEATH:
    case _SYSMSG_SUBTYPE_SIGNAL:
    case _SYSMSG_SUBTYPE_TRACE:
      io_msg->sysmsg_reply.status = ENOSYS;
      reply = REPLY;
      break;
   }
   return(reply);
}
