#include <assert.h>
#include <stdio.h>

#include "YARPSyncComm.h"

#include "YARPSocket.h"

#include "ThreadInput.h"

#define DBG(x) if ((x)>=10) 
//#define DBG(x) if (0) 


int YARPSyncComm::Send(YARPLinkID dest, char *buffer, 
				 int buffer_length,
				 char *return_buffer, int return_buffer_length)
{
  YARPOutputSocket os;
  os.SetIdentifier(dest);
  os.InhibitDisconnect();
  os.SendBegin(buffer,buffer_length);
  return os.SendEnd(return_buffer,return_buffer_length);
}


YARPLinkID YARPSyncComm::BlockingReceive(YARPLinkID src, char *buffer, 
				    int buffer_length)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  int id = -1;
  int ct = ts->ReceiveBegin(buffer,buffer_length, &id);
  return id;
}


YARPLinkID YARPSyncComm::PollingReceive(YARPLinkID src, char *buffer, 
				   int buffer_length)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  int id = -1;
  int ct = ts->PollingReceiveBegin(buffer,buffer_length, &id);
  return id;
}


int YARPSyncComm::Reply(YARPLinkID src, char *buffer, 
				  int buffer_length)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  return ts->ReceiveEnd(src,buffer,buffer_length);
}


int YARPSyncComm::Send(YARPLinkID dest, YARPMultipartMessage& msg,
				 YARPMultipartMessage& return_msg)
{
  YARPOutputSocket os;
  os.SetIdentifier(dest);
  os.InhibitDisconnect();
  int send_parts = msg.GetParts();
  int return_parts = return_msg.GetParts();
  assert(send_parts>=1);
  assert(return_parts>=1);
  int i;

  DBG(5) printf("Get %d send_parts, %d return_parts\n", send_parts, 
		return_parts);

  os.SendBegin(msg.GetBuffer(0),msg.GetBufferLength(0));
  for (i=1;i<send_parts; i++)
    {
      os.SendContinue(msg.GetBuffer(i),msg.GetBufferLength(i));      
    }
  for (i=0;i<return_parts-1; i++)
    {
      os.SendReceivingReply(return_msg.GetBuffer(i),
			    return_msg.GetBufferLength(i));
    }
  int result = os.SendEnd(return_msg.GetBuffer(return_parts-1),
			  return_msg.GetBufferLength(return_parts-1));
  return result;
}


YARPLinkID YARPSyncComm::BlockingReceive(YARPLinkID src, 
					 YARPMultipartMessage& msg)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  int id = -1;
  int receive_parts = msg.GetParts();
  assert(receive_parts>=1);
  int ct = ts->ReceiveBegin(msg.GetBuffer(0),msg.GetBufferLength(0), &id);
  if (id!=-1)
    {
      for (int i=1; i<receive_parts; i++)
	{
	  int ct2 = ts->ReceiveContinue(id,msg.GetBuffer(i),msg.GetBufferLength(i));
	  DBG(5) printf("^^^ additional receive of %d bytes\n", ct2);
	}
    }
  return id;
}


YARPLinkID YARPSyncComm::PollingReceive(YARPLinkID src, 
					YARPMultipartMessage& msg)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  int id = -1;
  int receive_parts = msg.GetParts();
  assert(receive_parts>=1);
  int ct = ts->PollingReceiveBegin(msg.GetBuffer(0),msg.GetBufferLength(0), 
				   &id);
  if (id!=-1)
    {
      for (int i=1; i<receive_parts; i++)
	{
	  ts->ReceiveContinue(id,msg.GetBuffer(i),msg.GetBufferLength(i));
	}
    }
  return id;
}


int YARPSyncComm::Reply(YARPLinkID src, YARPMultipartMessage& msg)
{
  YARPInputSocket *ts = GetThreadSocket();
  assert(ts!=NULL);
  int reply_parts = msg.GetParts();
  assert(reply_parts>=1);

  for (int i=0; i<reply_parts-1; i++)
    {
      ts->ReceiveReplying(src,msg.GetBuffer(i),msg.GetBufferLength(i));
    }

  return ts->ReceiveEnd(src,msg.GetBuffer(reply_parts-1),
			msg.GetBufferLength(reply_parts-1));
}




