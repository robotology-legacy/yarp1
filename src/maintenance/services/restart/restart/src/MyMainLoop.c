/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/kernel.h>
#include <dirent.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "abimport.h"
#include "proto.h"

void Setup()
{
  DIR *dirp;
  struct dirent *direntp;
  char *str;
  char *str2;
  int index = 1;

  char *items[2];

  dirp = opendir( "." );
  if( dirp != NULL ) {
    for(;;) {
      direntp = readdir( dirp );
      if( direntp == NULL ) break;
      str = strstr(direntp->d_name,".run");
      str2 = strstr(direntp->d_name,"~");
      if (str!=NULL && str2!=NULL)
	{
	  *str = '\0';  // bad, bad thing to do
	  //printf( "%s\n", direntp->d_name );
	  items[0] = direntp->d_name;
	  PtListAddItems(ABW_ProcessList,items,1,index);
	  index++;
	}
    }
    closedir( dirp );
  }
}


static void UserAppProcessEvent( PtAppContext_t app );
static void UserAppMainLoop( PtAppContext_t app );

void
MyMainLoop( void )

	{
	  
	  Setup();

   	UserAppMainLoop(PtDefaultAppContext());

	}

static void
UserAppMainLoop( PtAppContext_t app )
{

   /* arm Photon */
   PhEventArm( );

   for( ;; ) {
      UserAppProcessEvent(app);
   }

}

                                  
#ifdef __QNXNTO__
static void
UserAppProcessEvent(PtAppContext_t app)
{
	extern struct _Ph_ctrl *_Ph_;
	struct _Ph_msg_hdr_reply r;
	iov_t		iov[3];
	struct Pt_work_proc_list_el	*proc;

	// temp code!!!
	if (app->event == NULL && (app->event = malloc(app->event_size)) == NULL)
		return;

	PtAppCallSignalProc(app);

	SETIOV(iov + 1, &r, sizeof r);
	SETIOV(iov + 2, app->event, app->event_size);
	if ( _Ph_->channel == 0 ) {
		if ( (proc = app->work_procs.tail) != NULL ) {
			if ( _readxv(_Ph_->fd, iov, 3, _IO_XFLAG_NONBLOCK | _IO_XTYPE_NONE, 0, 0) <= 0 ) {
				/* AB background process handling */
				if ( (*proc->proc.func)(proc->proc.data) != Pt_CONTINUE && proc == app->work_procs.tail )
					PtAppRemoveWorkProc(app, app->work_procs.tail->proc.id);
				return;
			}	}
		else
			if ( readv( _Ph_->fd, iov + 1, 2 ) == -1 )
				return;
		}
	else {
		struct _msg_info info;
		pid_t pid, rcvid;
		if ( ( proc = app->work_procs.tail ) != NULL ) {
			static const struct sigevent event = { SIGEV_UNBLOCK };
			TimerTimeout( _NTO_TIMEOUT_RECEIVE, &event, NULL, NULL );
			}
		switch ( rcvid = MsgReceivev( _Ph_->channel, iov+2, 1, &info ) ) {
			case -1 :
				switch ( errno ) {
					case ETIMEDOUT :
						if	(	proc->proc.func(proc->proc.data) != Pt_CONTINUE
							&&	proc == app->work_procs.tail
								)
							PtAppRemoveWorkProc(app, app->work_procs.tail->proc.id);
					case EINTR :
						break;
					default :
//						Trace1( _TRACE_PHOTON_CRECEIVE, _TRACE_CRITICAL, errno );
						break;
					}
				return;
			case 0 : /* Pulse */
				{	struct _pulse *pulse = (void*) app->event;
					if	(	( rcvid = pulse->value.sival_int )
						==	_NOTIFY_COND_INPUT
							)
						pid = 0;
					else
						if ( app->inputs.tail )
						 	pid = rcvid | ~_NOTIFY_DATA_MASK;
						else
							return;
					}
				break;
			default :
				if ( app->inputs.tail == NULL ) {
					static const msg_t reply = ENOSYS;
					SETIOV( iov, &reply, sizeof(reply) );
					MsgReplyv( rcvid, &iov, 1 );
					}
				pid = info.pid;	/* Assume it's > 0 */
			}
		if ( pid == 0 ) {
		read_again:
			if ( _readxv(_Ph_->fd, iov, 3, _IO_XFLAG_NONBLOCK | _IO_XTYPE_NONE, 0, 0) <= 0 )
				return;
			}
		else {
			struct Pt_input_list_el *el;
			PtInputRec_t *input_proc = NULL;

			/* Search for last proc registered for the pid */
			if (app->inputs.tail->proc.pid == pid)
				input_proc = &app->inputs.tail->proc;
			else
				for (el = app->inputs.head; el != app->inputs.tail; el = el->next)
					if (el->proc.pid == pid)
						input_proc = &el->proc;

			if (input_proc && input_proc->func)	{
				if ((*input_proc->func)(input_proc->data, rcvid, app->event, app->event_size))
					PtAppRemoveInput(app, input_proc->id);
				}
			else {
				/* If a specific input procedure is not found, invoke
				 * each of the non-specific procedures in turn
				 */
				el = app->inputs.head;
				do	{
					struct Pt_input_list_el	*temp_el;
					temp_el = el->next;
					if (el->proc.func && !el->proc.pid )
						if ((*el->proc.func)(el->proc.data, rcvid, app->event, app->event_size))
							PtAppRemoveInput(app, el->proc.id);
					el = temp_el;
					} while ( el );
				}
			return;
		}	}
	switch( r.type ) {
		case 0: 					/* non-photon events */
			break;

		case Ph_EVENT_MSG:			/* photon event */
			app->event->processing_flags = 0;
			PtEventHandler( app->event );
			break;

		case Ph_RESIZE_MSG:
			app->event_size = *(unsigned long *)app->event;
			if( !( app->event = realloc( app->event, app->event_size ) ) ) {
//				Trace1( _TRACE_PHOTON_EVENT_REALLOC, _TRACE_CRITICAL, errno );
			}
			SETIOV(iov + 2, app->event, app->event_size);
			goto read_again;

		case _Ph_CLOSE_MSG:
			/* These only arrive if you are a registered		*/
			/* Photon manager. (Graphics drivers, gwm, etc.) 	*/
			break;

		case -1:					/* error */
			if ( errno == EINTR || errno == ENOMSG )
				return;
//			Trace1( _TRACE_PHOTON_EVENT_READ, _TRACE_CRITICAL, errno );
			break;
	}
}

#else
static void
UserAppProcessEvent(PtAppContext_t app)
{
	extern struct _Ph_ctrl *_Ph_;
	pid_t		pid;
	struct Pt_work_proc_list_el	*proc;
	
	/* Do we want a PtMalloc? */
	/* allocate buffer for event */
	if (app->event == NULL && (app->event = malloc(app->event_size)) == NULL) {
//		Trace1( _TRACE_PHOTON_EVENT_MALLOC, _TRACE_CRITICAL, errno );
		return;
	}

	PtAppCallSignalProc(app);

	if((proc = app->work_procs.tail) == NULL ) {
		if(-1 == ( pid = Receive( 0, app->event, app->event_size))) {
			if(errno == EINTR)
	            return;
//			Trace1( _TRACE_PHOTON_RECEIVE, _TRACE_CRITICAL, errno );
			return;
		}	}
	else
		if ( -1 == (pid = Creceive(0, app->event, app->event_size))) {
			/* AB background process handling */
			if(errno == ENOMSG )
				if	(	(*proc->proc.func)(proc->proc.data) != Pt_CONTINUE
					&&	proc == app->work_procs.tail
						)
					PtAppRemoveWorkProc(app, proc->proc.id);
			else
				if ( errno != EINTR )
//					Trace1( _TRACE_PHOTON_CRECEIVE, _TRACE_CRITICAL, errno );	 	
					;
			return;
			}

	/* process event */
loop:
	switch( PhEventRead( pid, app->event, app->event_size ) ) {
		case 0: 					/* non-photon events */
			if ( pid == _Ph_->proxy )
				return;
			if (app->inputs.tail) {
				struct Pt_input_list_el *el;
				PtInputRec_t *input_proc = NULL;

				/* Search for last proc registered for the pid */
				if (app->inputs.tail->proc.pid == pid)
					input_proc = &app->inputs.tail->proc;
				else {
					for (el = app->inputs.head; el != app->inputs.tail; el = el->next)
						if (el->proc.pid == pid)
							input_proc = &el->proc;
				}

				if (input_proc && input_proc->func)	{
					if ((*input_proc->func)(input_proc->data, pid, app->event, app->event_size))
						PtAppRemoveInput(app, input_proc->id);
				} else {
					/* If a specific input procedure is not found, invoke
					 * each of the non-specific procedures in turn
					 */
					el = app->inputs.head;
					do {
						struct Pt_input_list_el	*temp_el;
						temp_el = el->next;
						if (el->proc.func && !el->proc.pid )
							if ((*el->proc.func)(el->proc.data, pid, app->event, app->event_size))
								PtAppRemoveInput(app, el->proc.id);
						el = temp_el;
					} while ( el );
				}
			} else {
				msg_t reply = ENOSYS;
				Reply( pid, &reply, sizeof( reply ) );
			}
			break;

		case Ph_EVENT_MSG:			/* photon event */
			app->event->processing_flags = 0;
			PtEventHandler( app->event );
			break;

		case Ph_RESIZE_MSG:
			app->event_size = *(unsigned long *)app->event;
			if( !( app->event = realloc( app->event, app->event_size ) ) ) {
//				Trace1( _TRACE_PHOTON_EVENT_REALLOC, _TRACE_CRITICAL, errno );
			}
			goto loop;

		case _Ph_CLOSE_MSG:
			/* These only arrive if you are a registered		*/
			/* Photon manager. (Graphics drivers, gwm, etc.) 	*/
			break;

		case -1:					/* error */
			if ( errno == EINTR || errno == ENOMSG )
				return;
//			Trace1( _TRACE_PHOTON_EVENT_READ, _TRACE_CRITICAL, errno );
			break;
	}
}
#endif

#if 0
void
UserAppProcessEvent(PtAppContext_t app)
{
	pid_t		pid;
	struct Pt_input_list_el	*temp_el;
	
   /* allocate buffer for event */
   if (app->event == NULL && (app->event = malloc(app->event_size)) == NULL)
   {
   		Trace1( _TRACE_PHOTON_EVENT_MALLOC, _TRACE_CRITICAL, errno );
      return;
   }

   if ( app->work_procs.tail == NULL ) {
      if ( -1 == ( pid = Receive( 0, app->event, app->event_size ) ) ) {
         if ( errno == EINTR )
            return;
			Trace1( _TRACE_PHOTON_RECEIVE, _TRACE_CRITICAL, errno );
         return;
      }
   }
   else {
      if ( -1 == ( pid = Creceive( 0, app->event, app->event_size ) ) ) {
         /* AB background process handling */
         if ( errno == ENOMSG ) {
            if ( ( *app->work_procs.tail->proc.func )( app->work_procs.tail->proc.data ) )
               PtAppRemoveWorkProc(app, &app->work_procs.tail->proc.id);
            return;
         }
         else if ( errno == EINTR )
            return;
         else {
			Trace1( _TRACE_PHOTON_CRECEIVE, _TRACE_CRITICAL, errno );	 	
            return;
         }
      }
   }

   /* process event */
loop:
   switch( PhEventRead( pid, app->event, app->event_size ) ) {
      case 0: 					/* non-photon events */
         if (app->inputs.tail)
         {
            struct Pt_input_list_el *el;
            PtInputRec_t *input_proc = NULL;

            /* Search for last proc registered for the pid */
            if (app->inputs.tail->proc.pid == pid)
               input_proc = &app->inputs.tail->proc;
            else
            {
               for (el = app->inputs.head; el != app->inputs.tail; el = el->next)
                  if (el->proc.pid == pid)
                     input_proc = &el->proc;
            }

            if (input_proc && input_proc->func)
            {
               if ((*input_proc->func)(input_proc->data, pid, app->event, app->event_size))
                  PtAppRemoveInput(app, &input_proc->id);
            }
            else
            {
               /* If a specific input procedure is not found, invoke
                * each of the non-specific procedures in turn
                */
               el = app->inputs.head;
               do
               {
				  temp_el = el->next;
                  if (el->proc.func && !el->proc.pid )
                     if ((*el->proc.func)(el->proc.data, pid, app->event, app->event_size))
                        PtAppRemoveInput(app, &el->proc.id);
                  el = temp_el;
               }
               while ( el );
            }
         }
         break;

      case Ph_EVENT_MSG:			/* photon event */
         PtEventHandler( app->event );
         break;

      case Ph_RESIZE_MSG:
         app->event_size = *(unsigned long *)app->event;
         if( !( app->event = realloc( app->event, app->event_size ) ) ) {
			Trace1( _TRACE_PHOTON_EVENT_REALLOC, _TRACE_CRITICAL, errno );
         }
		 goto loop;

      case _Ph_CLOSE_MSG:
         /* These only arrive if you are a registered		*/
         /* Photon manager. (Graphics drivers, gwm, etc.) 	*/
         break;

      case -1:					/* error */
         if ( errno == EINTR || errno == ENOMSG )
            return;
			Trace1( _TRACE_PHOTON_EVENT_READ, _TRACE_CRITICAL, errno );
         break;
   }
}
#endif
