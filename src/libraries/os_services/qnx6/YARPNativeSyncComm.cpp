#include <assert.h>
#include <stdio.h>
#include "YARPNativeSyncComm.h"
#include "YARPNameID_defs.h"
#include <sys/neutrino.h>

//#include <sys/kernel.h>

#include "YARPThread.h"

// deal with inconvenient macros
#ifdef Send
#undef Send
#undef Receive
#undef Reply
#endif

class TSData
{
public:
  int offset;
};

// only one reader per thread, a la raw QNX protocol, so can use thread-global
// variable
YARPThreadSpecific<TSData> ts_data;


/* MIG4NTO             Send Use MsgSend() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
int YARPNativeSyncComm::Send(YARPNameID dest, char *buffer, 
				 int buffer_length,
				 char *return_buffer, int return_buffer_length)
{
  assert(dest.IsConsistent(YARP_NAME_MODE_NATIVE));
  /*
  return __send(dest.GetRawIdentifier(),buffer,return_buffer,
		buffer_length,return_buffer_length);
	*/
  return MsgSend(dest.GetRawIdentifier(),
  					(const void *)buffer,
  					buffer_length,
  					(void *)return_buffer,
					return_buffer_length);
}


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, char *buffer, 
				    int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  //  printf("^^^^ blocking receive for id %d\n", src.GetRawIdentifier());
  ///int result =  __receive(src.GetRawIdentifier(),buffer,buffer_length);
  int result =  MsgReceive(src.GetRawIdentifier(),buffer,buffer_length,NULL);
  ts_data.Content().offset = buffer_length; //no way to know real transfer :(
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, char *buffer, 
				   int buffer_length)
{ 
  struct sigevent         event;//+QNX6+
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
/* MIG4NTO         Creceive Use MsgReceive() preceeded immediately by: */
/* MIG4NTO                  event.sigev_notify = SIGEV_UNBLOCK; */
/* MIG4NTO                  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, */
/* MIG4NTO                  &event, NULL, NULL ); */
  //int result = ::Creceive(src.GetRawIdentifier(),buffer,buffer_length);
  event.sigev_notify = SIGEV_UNBLOCK;//+QNX6+
  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE,&event, NULL, NULL );//+QNX6+
  int result = MsgReceive(src.GetRawIdentifier(),buffer,buffer_length,NULL);//+QNX6+
  ts_data.Content().offset = buffer_length; //no way to know real transfer :(
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}

int YARPNativeSyncComm::ContinuedReceive(YARPNameID src, char *buffer, 
					 int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  int result = -1;
  if (src.IsValid())
    {
      int offset = ts_data.Content().offset;
/* MIG4NTO          Readmsg Use MsgRead() instead.  Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
      ///int more = Readmsg(src.GetRawIdentifier(),offset,buffer,buffer_length);
      int more = MsgRead(src.GetRawIdentifier(),buffer,buffer_length,offset);
      if (more>=0)
	{
	  offset += more;
	  ts_data.Content().offset = offset;
	  result = more;
	}
    }
  return result;
}

/* MIG4NTO            Reply Use MsgReply() instead.  Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
int YARPNativeSyncComm::Reply(YARPNameID src, char *buffer, 
				  int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  ///return __reply(src.GetRawIdentifier(),buffer,buffer_length);
  return MsgReply(src.GetRawIdentifier(),0,(const void*)buffer,buffer_length);
}


/* MIG4NTO             Send Use MsgSend() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
int YARPNativeSyncComm::Send(YARPNameID dest, YARPMultipartMessage& msg,
				 YARPMultipartMessage& return_msg)
{
  assert(dest.IsConsistent(YARP_NAME_MODE_NATIVE));
/* MIG4NTO           Sendmx Use MsgSendv() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
  ///return Sendmx(dest.GetRawIdentifier(), msg.GetParts(), return_msg.GetParts(),
  ///		(struct _mxfer_entry*) msg.GetRawBuffer(),
  ///		(struct _mxfer_entry*) return_msg.GetRawBuffer());
  
  return MsgSendv(dest.GetRawIdentifier(), 
  				  (const iov_t*) msg.GetRawBuffer(),
  				  msg.GetParts(), 
  				  (const iov_t*) return_msg.GetRawBuffer(),
  				  return_msg.GetParts());
  				
  				
}


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
/* MIG4NTO        Receivemx Use MsgReceivev() instead.  Note that this takes a */
/* MIG4NTO                  channel id returned from ChannelCreate() instead */
/* MIG4NTO                  of a process id. See the migration guide as a */
/* MIG4NTO                  starting place for more information on this. */
  ///int result =  Receivemx(src.GetRawIdentifier(), msg.GetParts(), 
  ///			  (struct _mxfer_entry*) msg.GetRawBuffer());
  int result =  MsgReceivev(src.GetRawIdentifier(), 
  						   (const iov_t*) msg.GetRawBuffer(),
  						    msg.GetParts(),
  						    NULL); 
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
  struct sigevent         event; //+QNX6+
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
/* MIG4NTO       Creceivemx Use MsgReceivev() preceeded immediately by: */
/* MIG4NTO                  event.sigev_notify = SIGEV_UNBLOCK; */
/* MIG4NTO                  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, */
/* MIG4NTO                  &event, NULL, NULL ); */
  /*
  int result = Creceivemx(src.GetRawIdentifier(), msg.GetParts(), 
			  (struct _mxfer_entry*) msg.GetRawBuffer());
   */
  event.sigev_notify = SIGEV_UNBLOCK; //+QNX6+
  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE,
  &event, NULL, NULL );//+QNX6+
  
  //+QNX6+
  int result = MsgReceivev( src.GetRawIdentifier(),
							(const iov_t *) msg.GetRawBuffer(),
							msg.GetParts(),
							NULL );
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


/* MIG4NTO            Reply Use MsgReply() instead.  Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
int YARPNativeSyncComm::Reply(YARPNameID src, YARPMultipartMessage& msg)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
/* MIG4NTO          Replymx Use MsgReplyv() instead. Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
  ///return Replymx(src.GetRawIdentifier(), msg.GetParts(), 
  ///		 (struct _mxfer_entry*) msg.GetRawBuffer());
  
  return MsgReplyv(src.GetRawIdentifier(),
  				   0,
  				   (const iovec *) msg.GetRawBuffer(),
  				   msg.GetParts());
  				 
}



