#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include "TinySocket.h"

#include "YARPSocket.h"


//#define DBG if (1)
#define DBG if (0)

/*
static string GetBase(const char *name)
{
  string work = name;
  int pos = work.find("|",0);
  if (pos != string::npos)
    {
      if (pos>0)
	{
	  work = work.substr(0,pos);
	}
      else
	{
	  work = "";
	}
    }
  else
    {
      work = "";
    }
  return work;
}
*/

/*
static int GetPort(const char *name)
{
  string work = name;
  int pos = work.find("|",0);
  int len = work.size()-pos-1;
  if (pos != string::npos)
    {
      work = work.substr(pos+1,len);
    }
  //DBG printf("Port part is from %d, length %d (%s)\n", 
  //pos, len, work.c_str());
  return atoi(work.c_str());
}
*/

class ISData
{
public:
  SocketOwner owner;
};

ISData& ISDATA(void *x)
{
  assert(x!=NULL);
  return *((ISData*)x);
}

YARPInputSocket::YARPInputSocket()
{ 
  system_resources = NULL; 
  identifier = -1; 
  system_resources = new ISData;
  assert(system_resources!=NULL);
}

YARPInputSocket::~YARPInputSocket()
{
  if (system_resources!=NULL)
    {
      delete ((ISData*)system_resources);
      system_resources = NULL;
    }
}


int YARPInputSocket::Register(const char *name)
{
  int port = GetPort(name);
  DBG printf ("Registering port %d\n", port);
  ISDATA(system_resources).owner.Connect(port);
}


int YARPInputSocket::PollingReceiveBegin(char *buffer, int buffer_length, 
					 int *reply_id = NULL)
{
  return ISDATA(system_resources).owner.PollingRead(buffer,buffer_length,
						    reply_id);
}


int YARPInputSocket::ReceiveBegin(char *buffer, int buffer_length, 
				  int *reply_id = NULL)
{
  return ISDATA(system_resources).owner.Read(buffer,buffer_length,reply_id);
}


int YARPInputSocket::ReceiveContinue(int reply_id, char *buffer, 
				     int buffer_length)
{
  return ISDATA(system_resources).owner.ReceiveMore(reply_id,buffer,buffer_length);
}

int YARPInputSocket::ReceiveReplying(int reply_id, char *reply_buffer,
				     int reply_buffer_length)
{
  return ISDATA(system_resources).owner.BeginReply(reply_id,reply_buffer,
						   reply_buffer_length);
}


int YARPInputSocket::ReceiveEnd(int reply_id, char *reply_buffer, 
				int reply_buffer_length)
{
  return ISDATA(system_resources).owner.Reply(reply_id,reply_buffer,
					      reply_buffer_length);
}


class OSData
{
public:
  TinySocket sock;
};

OSData& OSDATA(void *x)
{
  assert(x!=NULL);
  return *((OSData*)x);
}

YARPOutputSocket::YARPOutputSocket()
{ 
  system_resources = NULL;
  identifier = -1;
  system_resources = new OSData;
  assert(system_resources!=NULL);
}

YARPOutputSocket::~YARPOutputSocket()
{
  if (system_resources!=NULL)
    {
      delete ((OSData*)system_resources);
      system_resources = NULL;
    }
}


int YARPOutputSocket::Connect(const char *name)
{
  string machine = GetBase(name);
  int port = GetPort(name);
  DBG printf ("Connecting to port %d on %s\n", port, machine.c_str());
  OSDATA(system_resources).sock.Connect(machine.c_str(),port);
  identifier = OSDATA(system_resources).sock.GetSocketPID();
}


int YARPOutputSocket::SendBegin(char *buffer, int buffer_length)
{
  return SendBlock(OSDATA(system_resources).sock,buffer,buffer_length);
}


int YARPOutputSocket::SendContinue(char *buffer, int buffer_length)
{
  return SendBlock(OSDATA(system_resources).sock,buffer,buffer_length,0);
}

int YARPOutputSocket::SendReceivingReply(char *reply_buffer, 
					 int reply_buffer_length)
{
  return ::SendBeginEnd(OSDATA(system_resources).sock,reply_buffer,
			reply_buffer_length);
}


int YARPOutputSocket::SendEnd(char *reply_buffer, int reply_buffer_length)
{
  return ::SendEnd(OSDATA(system_resources).sock,reply_buffer,
		   reply_buffer_length);
}



void YARPOutputSocket::InhibitDisconnect()
{
  OSDATA(system_resources).sock.InhibitDisconnect();
}

int YARPOutputSocket::GetIdentifier()
{
  return OSDATA(system_resources).sock.GetSocketPID();
}

void YARPOutputSocket::SetIdentifier(int n_identifier)
{
  OSDATA(system_resources).sock.ForcePID(n_identifier);
  identifier = n_identifier;
}


#include "YARPTime.h"

void server(const char *name)
{
  YARPInputSocket s;
  s.Register(name);
  while (1)
    {
      char buf[1000] = "not set";
      int reply_id = -1;
      int len = s.ReceiveBegin(buf,sizeof(buf),&reply_id);
      printf("Get data (%d): %s\n", len, buf);
      char reply[1000];
      sprintf(reply,"I acknowledge --> %s", buf);
      s.ReceiveEnd(reply_id,reply,strlen(reply)+1);
    }
}

void client(const char *name)
{
  YARPOutputSocket s;
  s.Connect(name);

  char buf[1000];
  char buf2[1000];

  for (int i=1; i<=20; i++)
    {
      sprintf(buf, "hello there time #%d", i);
      int len = strlen(buf)+1;
      //int len2 = Send(c,buf,len,buf2,sizeof(buf2));
      s.SendBegin(buf,len);
      int len2 = s.SendEnd(buf2,sizeof(buf2));
      if (len2>0)
	{
	  printf("Got response (%d): %s\n", len2, buf2);
	}
      else
	{
	  printf("Got response (%d)\n", len2);
	}
      YARPTime::DelayInSeconds(2.0);
    }
}

/*
int main(int argc, char *argv[])
{
  //    TinySocket::Verbose();

    if (argc == 2)
      {
	if (argv[1][0] == '+')
	  {
	    server(argv[1]+1);
	  }
	else
	  {
	    client(argv[1]);
	  }
      }
    return 0;
}

*/

