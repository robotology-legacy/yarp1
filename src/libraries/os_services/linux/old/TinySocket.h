#ifndef TINYSOCKET_INC
#define TINYSOCKET_INC

#include <string>
using namespace std;

#include "YARPThread.h"

string GetBase(const char *name);
int GetPort(const char *name);

class TinySocket
{
private:
  int pid;
  int auto_close;

  int MakeClient(const char *host,int port);

public:
  TinySocket(int n_pid=-1)  { pid = n_pid;  auto_close = 1; }
  virtual ~TinySocket() { if (auto_close) { Close(); } }

  void InhibitDisconnect()
    {
      auto_close = 0;
    }

  int ForcePID(int n_pid)
    {
      pid = n_pid;
    }

  int Connect(const char *host,int port)
  { return MakeClient(host,port); }

  int Close();

  int Write(const char *buf, int len);
  int Read(char *buf, int len, int exact=1);

  int IsData();

  int WaitForData();

  static void Verbose(int flag=1);

  int GetSocketPID() { return pid; }
};


class SocketOwner : public YARPThread
{
private:
  void *system_resources;
  int pid;
  int MakeServer(int port);
public:
  SocketOwner()  { pid = -1;  system_resources = NULL; }
  virtual ~SocketOwner();

  int Connect(int port);

  void AddSocket();

  int Close();

  void DeclareDataAvailable();
  void DeclareDataWritten();

  virtual void Body()
    {
      while (1)
	{
	  AddSocket();
	}
    }

  int Read(char *buf, int len, int *reply_pid = NULL);
  int PollingRead(char *buf, int len, int *reply_pid = NULL);
  int BeginReply(int reply_pid, char *buf, int len);
  int Reply(int reply_pid, char *buf, int len);
  
  // this demands exact number of bytes
  int ReceiveMore(int reply_pid, char *buf, int len);
};

int Receive(SocketOwner& owner, char *buf, int len, int *reply_pid = NULL);

int Reply(SocketOwner& owner, int pid, char *buf, int len);

int ReceiveMore(SocketOwner& owner, int pid, char *buf, int len);

int SendBlock(TinySocket& c, char *msg, int msg_len, int header=1);

int SendBeginEnd(TinySocket& c, char *in_msg, int in_msg_len);

int SendEnd(TinySocket& c, char *in_msg, int in_msg_len);


#endif
