#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <map>
#include <list>

#include "YARPString.h"
#include "YARPSocket.h"
#include "YARPNativeNameService.h"
#include "YARPNativeSyncComm.h"
#include "YARPThread.h"
#include "YARPTime.h"

#include "wide_nameloc.h"
#include "YARPFragments.h"
#include "BlockPrefix.h"
#include <signal.h>

#define DEFAULT_PORT 4200

extern int TINY_VERBOSE;

class NameRecord
{
public:
  int status;
  string hostname;
  string key;
  int port;
  int is_local;
  
  NameRecord()
    {
      port = -1;
      is_local = 0;
    }

  NameRecord(const char *n_key, const char *n_hostname, int n_port, 
	     int n_is_local=0)
    {
      hostname = n_hostname;
      key = n_key;
      port = n_port;
      is_local = n_is_local;
    }

  NameRecord(const NameRecord& nr)
    {
      hostname = nr.hostname;
      key = nr.key;
      port = nr.port;
      is_local = nr.is_local;
    }

  const NameRecord& operator = (const NameRecord& nr)
    {
      hostname = nr.hostname;
      key = nr.key;
      port = nr.port;
      is_local = nr.is_local;
      return *this;
    }

  const char *GetHostName()
    {
      return hostname.c_str();
    }

  const char *GetKey()
    {
      return key.c_str();
    }

  int GetPort()
    {
      return port;
    }

  int IsLocal()
    {
      return is_local;
    }
};

struct ltstr
{
  bool operator()(const string& s1, const string& s2) const
  {
    return strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};

typedef map<string, NameRecord, ltstr> NameMap;
typedef map<string, NameRecord, ltstr> PortMap;

NameMap name_map;
PortMap port_map;

class RelayThread : public YARPThread
{
public:
  YARPInputSocket is;
  string key;

  int operator == (const RelayThread& rt) { return 1; }
  int operator < (const RelayThread& rt) { return 1; }

  virtual void Body()
    {
      YARPNameID gid = YARPNativeNameService::LocateName(key.c_str());
      BlockPrefix prefix;
      Fragments in_store, out_store;
      YARPMultipartMessage in_msg, out_msg;
#ifdef __QNX__
      signal(SIGCHLD,SIG_IGN);
#endif
      while (1)
	{
	  int id = -1;
	  printf("@@@ waiting for input for local key %s\n", key.c_str());
	  //YARPTime::DelayInSeconds(400);
	  int ct = is.ReceiveBegin((char*)(&prefix),sizeof(prefix),&id);
	  printf("@@@ receiving input for local key %s\n", key.c_str());
	  if (id!=-1)
	    {
	      int in_count = prefix.total_blocks;
	      int out_count = prefix.reply_blocks;
	      
	      in_msg.Resize(in_count);
	      out_msg.Resize(out_count);
	      in_msg.Reset();
	      out_msg.Reset();
	      printf("@@@ working with %d inputs %d replies\n", in_count,
		     out_count);
	      if (prefix.size>=0)
		{
		  in_store.Require(prefix.size);
		  out_store.Require(prefix.reply_size);
		  in_msg.Set(0,in_store.GetBuffer(),in_store.GetLength());
		  out_msg.Set(0,out_store.GetBuffer(),out_store.GetLength());
		}
	      else
		{
		  int i;
		  NetInt32 x;
		  Fragments *frag = &in_store;
		  for (i=0; i<in_count; i++)
		    {
		      ct = is.ReceiveContinue(id,(char*)(&x),sizeof(x));
		      if (ct>0)
			{
			  printf("@@@ input fragment of size %d\n", x);
			  frag->Require(x);
			  in_msg.Set(i,frag->GetBuffer(),frag->GetLength());
			  frag = frag->AddNext();
			}
		    }
		  frag = &out_store;
		  for (i=0; i<out_count; i++)
		    {
		      ct = is.ReceiveContinue(id,(char*)(&x),sizeof(x));
		      if (ct>0)
			{
			  printf("@@@ reply fragment of size %d\n", x);
			  frag->Require(x);
			  out_msg.Set(i,frag->GetBuffer(),frag->GetLength());
			  frag = frag->AddNext();
			}
		    }
		}
	      int i;
	      if (ct>0)
		{
		  for (i=0; i<in_count; i++)
		    {
		      ct = is.ReceiveContinue(id,in_msg.GetBuffer(i),
					      in_msg.GetBufferLength(i));
		    }
		  int result = YARPNativeSyncComm::Send(gid,in_msg,out_msg);
		  if (result < 0)
		    {
		      gid = YARPNativeNameService::LocateName(key.c_str());
		      result = YARPNativeSyncComm::Send(gid,in_msg,out_msg);
		      if (result<0)
			{
			  printf("@@@ continuing problem relaying to local key %s\n", key.c_str());
			  //is.Close(id);
			  char ch = -1;
			  is.ReceiveEnd(id,&ch,1);
			}
		    }
		  if (result>=0)
		    {
		      printf("@@@ beginning relay reply for %s\n", key.c_str());
		      assert(out_count>=1);	
			  char ch = 0;
			  is.ReceiveReplying(id,&ch,1);
			  for (i=0; i<out_count-1; i++)
			{
			  is.ReceiveReplying(id,out_msg.GetBuffer(i),
					     out_msg.GetBufferLength(i));
			}
		      is.ReceiveEnd(id,out_msg.GetBuffer(out_count-1),
				    out_msg.GetBufferLength(out_count-1));
		      printf("@@@ finished relay reply for %s\n", key.c_str());
		    }
		}
	    }
	  else
	    {
	      gid = YARPNameID();
	    }
	  //YARPTime::DelayInSeconds(500);
	}
    }
};

list<RelayThread *> relays;

// currently relays never get deallocated

int create_local_relay(const char *key)
{
  relays.push_back(new RelayThread);
  //printf("--- obj %ld %d\n", (long int) relays.back(),
  //relays.back()->is.GetAssignedPort());
  relays.back()->is.Register("|0");
  //printf("--- obj %ld %d\n", (long int) relays.back(),
  //relays.back()->is.GetAssignedPort());
  int port = relays.back()->is.GetAssignedPort();
  //printf("=== local relay port preliminary report is (a) %d\n", port);
  relays.back()->key = key;
  relays.back()->Begin();
  //printf("=== local relay port preliminary report is (b) %d\n", port);
  //printf("--- obj %ld %d\n", (long int) relays.back(),
  //relays.back()->is.GetAssignedPort());
  return port;
}

int consult_local_system(const char *key)
{
  int result = 0;
  if (YARPNativeNameService::IsNonTrivial())
    {
      printf("=== checking local system for %s\n", key);
      YARPNameID id = YARPNativeNameService::LocateName(key);
      if (id.IsValid())
	{
	  printf("=== Found something!\n");
	  // Okay, now we need to construct a socket relay
	  int port = create_local_relay(key);
	  if (port!=-1)
	    {
	      printf("=== Created local relay on port %d\n", port);
	      char machine[256];
	      YARPNetworkObject::GetHostName(machine,sizeof(machine));
	      name_map[key] = NameRecord(key,machine,port,1);
	      char port_key[512];
	      sprintf(port_key,"%s|%d\n", machine, port);
	      if (port_map.find(port_key)!=port_map.end())
		{
		  name_map[port_map[port_key].GetKey()] = NameRecord();
		}
	      port_map[port_key] = NameRecord(key,machine,port,1);
	      result = 1;
	    }
	  else
	    {
	      printf("=== Failed to create relay\n");
	    }
	}
      else
	{
	  printf("=== Nothing here\n");
	}
    }
  return result;
}

void run_nameloc(int port)
{
  printf("Starting up socket nameloc service on port %d\n", port);

  char buf[256];
  sprintf(buf,"|%d", port);

//#ifdef __QNX__
//      signal(SIGCHLD,SIG_IGN);
//#endif

  YARPInputSocket listener;
  listener.Register(buf);

  while (1)
    {
      NameServiceHeader hdr;
      int id = -1;
      int result = listener.ReceiveBegin((char*)(&hdr),sizeof(hdr),&id);
      if (result>=0)
	{
	  char machine[256] = "";
	  char key[256] = "";
	  if (hdr.request_type==NAME_REGISTER)
	    {
	      assert(hdr.machine_length<=sizeof(machine));
	      result = listener.ReceiveContinue(id,machine,hdr.machine_length);
	      printf("Got machine %s, port %d\n", machine, hdr.port);
	    }
	  assert(hdr.key_length<=sizeof(key));
	  result = listener.ReceiveContinue(id,key,hdr.key_length);
	  char answer[256] = "?";
	  if (result>=0)
	    {
	      if (hdr.request_type==NAME_REGISTER)
		{
		  char port_key[512];
		  sprintf(port_key,"%s|%d\n", machine, hdr.port);
		  if (port_map.find(port_key)!=port_map.end())
		    {
		      name_map[port_map[port_key].GetKey()] = NameRecord();
		    }
		  name_map[key] = NameRecord(key,machine,hdr.port);
		  port_map[port_key] = NameRecord(key,machine,hdr.port);
		  printf("### stored entry for %s (%s|%d)\n", 
			 key, machine, hdr.port);
		  sprintf(answer,"%s|%d", machine, hdr.port);
		}
	      else
		{
		  NameMap::iterator it = name_map.find(key);
		  if (it==name_map.end())
		    {
		      consult_local_system(key);
		    }
		  else
		    {
		      if (!((*it).second.IsLocal()))
			{
			  consult_local_system(key);
			}
		    }
		  it = name_map.find(key);
		  if (it!=name_map.end())
		    {
		      int p = (*it).second.GetPort();
		      if (p!=-1)
			{
			  printf("### looked up %s, found %s|%d\n",
				 key, (*it).second.GetHostName(), 
				 (*it).second.GetPort());
			  sprintf(answer,"%s|%d",
				  (*it).second.GetHostName(), 
				  (*it).second.GetPort());
			}
		      else
			{
			  printf("### looked up %s, it is superceded\n", key);
			}
		    }
		}
	      listener.ReceiveEnd(id,answer,sizeof(answer));
	      //listener.ReceiveEnd(id,answer,strlen(answer)+1);
	      printf("Got key %s\n", key);
	    }
	  printf("*** replied\n");
	}

      {
	for (NameMap::iterator it=name_map.begin(); it!=name_map.end(); it++)
	  {
	    printf("Key %s value %s | %d\n",
		   (*it).first.c_str(),
		   (*it).second.GetHostName(),
		   (*it).second.GetPort());
	  }
      }
    }
}


int main(int argc, char *argv[])
{
  TINY_VERBOSE = 0;
//#ifdef __QNX__
//      signal(SIGCHLD,SIG_IGN);
//#endif
  int port = DEFAULT_PORT;
  if (argc>=2)
    {
      port = atoi(argv[1]);
    }
  run_nameloc(port);
  return 0;
}

