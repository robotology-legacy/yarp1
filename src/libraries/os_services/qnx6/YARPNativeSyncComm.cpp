/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, /// 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, carlos, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNativeSyncComm.cpp,v 1.2 2003-05-01 22:51:20 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include "YARPNativeSyncComm.h"
#include "YARPNameID_defs.h"
#include "YARPThread.h"

#include <sys/neutrino.h>

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

// only one reader per thread, a the raw QNX protocol, so can use thread-global
// variable
YARPThreadSpecific<TSData> ts_data;


/* MIG4NTO             Send Use MsgSend() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
int YARPNativeSyncComm::Send(YARPNameID dest, char *buffer, int buffer_length, char *return_buffer, int return_buffer_length)
{
	ACE_ASSERT (dest.isConsistent(YARP_QNET));
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


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, char *buffer, int buffer_length)
{
	ACE_ASSERT (src.isConsistent(YARP_QNET));
	//  printf("^^^^ blocking receive for id %d\n", src.GetRawIdentifier());
	///int result =  __receive(src.GetRawIdentifier(),buffer,buffer_length);

	int result =  MsgReceive(src.GetRawIdentifier(),buffer,buffer_length,NULL);
	ts_data.Content().offset = buffer_length; //no way to know real transfer :(

	return YARPNameID(YARP_QNET, result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, char *buffer, int buffer_length)
{ 
	struct sigevent         event;//+QNX6+
	ACE_ASSERT (src.isConsistent(YARP_QNET));
/* MIG4NTO         Creceive Use MsgReceive() preceeded immediately by: */
/* MIG4NTO                  event.sigev_notify = SIGEV_UNBLOCK; */
/* MIG4NTO                  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, */
/* MIG4NTO                  &event, NULL, NULL ); */
  //int result = ::Creceive(src.GetRawIdentifier(),buffer,buffer_length);
	event.sigev_notify = SIGEV_UNBLOCK;//+QNX6+
	TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE,&event, NULL, NULL );//+QNX6+
	int result = MsgReceive(src.GetRawIdentifier(),buffer,buffer_length,NULL);//+QNX6+
	ts_data.Content().offset = buffer_length; //no way to know real transfer :(
	return YARPNameID(YARP_QNET, result);
}

int YARPNativeSyncComm::ContinuedReceive(YARPNameID src, char *buffer, int buffer_length)
{
	ACE_ASSERT (src.isConsistent(YARP_QNET));
	int result = -1;
	if (src.isValid())
	{
		int offset = ts_data.Content().offset;
		/* MIG4NTO          Readmsg Use MsgRead() instead.  Note that this takes the */
		/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
		/* MIG4NTO                  process id. See the migration guide as a starting */
		/* MIG4NTO                  place for more information on this. */
		///int more = Readmsg(src.GetRawIdentifier(),offset,buffer,buffer_length);
		int more = MsgRead(src.getRawIdentifier(), buffer, buffer_length,offset);
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
int YARPNativeSyncComm::Reply(YARPNameID src, char *buffer, int buffer_length)
{
	ACE_ASSERT (src.isConsistent(YARP_QNET));
	///return __reply(src.GetRawIdentifier(),buffer,buffer_length);
	return MsgReply(src.getRawIdentifier(), 0, (const void*)buffer,buffer_length);
}


/* MIG4NTO             Send Use MsgSend() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
int YARPNativeSyncComm::Send(YARPNameID dest, YARPMultipartMessage& msg, YARPMultipartMessage& return_msg)
{
	ACE_ASSERT (dest.isConsistent(YARP_QNET));
/* MIG4NTO           Sendmx Use MsgSendv() instead. Note that this takes a */
/* MIG4NTO                  connection id (coid) instead of a process id. This */
/* MIG4NTO                  coid can be obtained from open() (a coid is a file */
/* MIG4NTO                  descriptor) or ConnectAttach(..., */
/* MIG4NTO                  _NTO_SIDE_CHANNEL, ...). See the migration guide */
/* MIG4NTO                  as a starting place for more information on this. */
  ///return Sendmx(dest.GetRawIdentifier(), msg.GetParts(), return_msg.GetParts(),
  ///		(struct _mxfer_entry*) msg.GetRawBuffer(),
  ///		(struct _mxfer_entry*) return_msg.GetRawBuffer());
  
  return MsgSendv(dest.getRawIdentifier(), 
  				  (const iov_t*) msg.GetRawBuffer(),
  				  msg.GetParts(), 
  				  (const iov_t*) return_msg.GetRawBuffer(),
  				  return_msg.GetParts());			
}


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
	ACE_ASSERT (src.isConsistent(YARP_QNET));
/* MIG4NTO        Receivemx Use MsgReceivev() instead.  Note that this takes a */
/* MIG4NTO                  channel id returned from ChannelCreate() instead */
/* MIG4NTO                  of a process id. See the migration guide as a */
/* MIG4NTO                  starting place for more information on this. */
  ///int result =  Receivemx(src.GetRawIdentifier(), msg.GetParts(), 
  ///			  (struct _mxfer_entry*) msg.GetRawBuffer());
	int result =  MsgReceivev(src.getRawIdentifier(), 
  						   (const iov_t*) msg.GetRawBuffer(),
  							msg.GetParts(),
  							NULL); 
	return YARPNameID(YARP_QNET, result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
	struct sigevent event; //+QNX6+
	ACE_ASSERT (src.isConsistent(YARP_QNET));
	/* MIG4NTO       Creceivemx Use MsgReceivev() preceeded immediately by: */
	/* MIG4NTO                  event.sigev_notify = SIGEV_UNBLOCK; */
	/* MIG4NTO                  TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, */
	/* MIG4NTO                  &event, NULL, NULL ); */
	/*
	int result = Creceivemx(src.GetRawIdentifier(), msg.GetParts(), 
	(struct _mxfer_entry*) msg.GetRawBuffer());
	*/
	event.sigev_notify = SIGEV_UNBLOCK; //+QNX6+
	TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, &event, NULL, NULL);	//+QNX6+

	//+QNX6+
	int result = MsgReceivev( 
			src.getRawIdentifier(),
			(const iov_t *) msg.GetRawBuffer(),
			msg.GetParts(),
			NULL);
	return YARPNameID(YARP_QNET,result);
}


/* MIG4NTO            Reply Use MsgReply() instead.  Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
int YARPNativeSyncComm::Reply(YARPNameID src, YARPMultipartMessage& msg)
{
	ACE_ASSERT (src.isConsistent(YARP_QNET));
/* MIG4NTO          Replymx Use MsgReplyv() instead. Note that this takes the */
/* MIG4NTO                  receive id returned from MsgReceive() instead of a */
/* MIG4NTO                  process id. See the migration guide as a starting */
/* MIG4NTO                  place for more information on this. */
  ///return Replymx(src.GetRawIdentifier(), msg.GetParts(), 
  ///		 (struct _mxfer_entry*) msg.GetRawBuffer());
  
	return MsgReplyv(src.getRawIdentifier(),
				0,
				(const iovec *) msg.getRawBuffer(),
				msg.GetParts());  				 
}



