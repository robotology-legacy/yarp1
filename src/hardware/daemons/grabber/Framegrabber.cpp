#include <sys/kernel.h>
#include <sys/proxy.h>
#include <stdlib.h>
#include <signal.h>

//#include "debug-new.h"
//#define __YARPSafeNewh__

//#include "Image.h"

#include "Framegrabber.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"

#include "YARPRefCount.h"

#define Sema YARPSemaphore

extern "C" {
#include "frame.h"
#ifdef PXC_VERSION_pxc2plus
#include "pxc2plus.h"
#else
#include "pxc200.h"
#endif
}

#include <function.h>
#include <map.h>

PXC200 pxc200;
FRAMELIB flib;

#define BUFFER_COUNT 6


typedef map<int, const char *, less<int> > NameMap;

NameMap image_type_name;

void InitNames()
{
  image_type_name[YARP_PIXEL_MONO] = "mono";
  image_type_name[YARP_PIXEL_BGR] = "bgr";
  //  image_type_name[IMAGE_TYPE_BGRA] = "bgra";
};

static Sema terminating(1);
int terminated = 0;

PXCFramegrabber *running_grabber = NULL;

static void terminate(int sig)
{
  YARPThread::PrepareForDeath();
  if (running_grabber!=NULL)
    {
      int need_terminate;
      terminating.Wait();
      need_terminate = !terminated;
      terminated = 1;
      terminating.Post();
      if (need_terminate)
	{
	  running_grabber->EmergencyStop();
	}
    }
  exit(1);
}

static void terminate_atexit(void)
{
  if (running_grabber!=NULL)
    {
      int need_terminate;
      terminating.Wait();
      need_terminate = !terminated;
      terminated = 1;
      terminating.Post();
      if (need_terminate)
	{
	  running_grabber->EmergencyStop();
	}
    }
}

class SemaFrame : public Frame
{
public:
};


class FramegrabberBuffer
{
public:
  FRAME __PX_FAR *frame;
  void *buffer;
  YARPRefCountedBuffer ref_buffer;
  int grabbing;
  int grabbed;
  int acquired;
  long int ticket;
  int id;

  FramegrabberBuffer()
    { frame = NULL;  buffer = NULL;  acquired = 0;  grabbing = 0;  
      grabbed = 0; id = 0; }

  ~FramegrabberBuffer()
    { Reset(); }

  int Allocate(int nheight, int nwidth, int nformat);

  void Reset();

  void Grab(unsigned long fgh,int flags)
    { 
      assert(!grabbing&&!acquired&&frame!=NULL);
      grabbing = 1;  grabbed = 0;
      ticket = pxc200.Grab(fgh, frame, flags);
    }

  void ForceBufferLength(int len)
    {
      ref_buffer.ForceLength(len);
    }
    

  void Wait(unsigned long fgh)
    {
      if (grabbing)
	{
	  //printf("Waiting on %ld\n", ticket);
	  //pxc200.WaitFinished(fgh, ticket);
	  //printf("Done waiting on %ld\n", ticket);
#ifdef PXC_VERSION_pxc2plus
	  pxc200.ArmProxy(fgh, ticket);
	  Receive(0, NULL, 0);
#else
	  pxc200.WaitFinished(fgh, ticket);
#endif
	  grabbing = 0;
	  grabbed = 1;
	}
    }
};


int FramegrabberBuffer::Allocate(int nheight, int nwidth, int nformat)
{
  int tag;
  int nbytes = 0;
  Reset();
  switch (nformat)
    {
    case Framegrabber::PIXEL_Y8:
      tag = PBITS_Y8;
      nbytes = 1;
      break;
    case Framegrabber::PIXEL_BGR:
      tag = PBITS_RGB24;
      nbytes = 3;
      break;
      /*
    case Framegrabber::PIXEL_BGRA:
      tag = PBITS_RGB32;
      nbytes = 4;
      break;
      */
    default:
      fprintf(stderr,"Unsupported image format requested");
      exit(1);
      break;
    }
  frame = pxc200.AllocateBuffer(nwidth, nheight, tag);
  assert(frame!=NULL);
  buffer = flib.FrameBuffer(frame);
  assert(buffer!=NULL);
  ref_buffer.Set((char*)buffer,nwidth*nheight*nbytes);
  ref_buffer.AddRef();
  return nbytes;
}

void FramegrabberBuffer::Reset()
{
  if (frame!=NULL)
    {
      pxc200.FreeFrame(frame);
    }
  frame = NULL;
  buffer = NULL;
  grabbing = grabbed = 0;
  acquired = 0;
}


class BufferManager
{
public:
  FramegrabberBuffer buffer[BUFFER_COUNT];
  int h, w;
  int request_index;
  int read_index;
  int valid_index;
  int acquired_index;
  long int fgh;
  int flags;
  int low;
  int id;
  int depth;
  int format;
  Sema mutex;
  int want_wakeup;
  Sema wakeup;

  BufferManager() : mutex(1), wakeup(0)
    {
      want_wakeup = 0;
      low = 0;
      id = 1;
      Reset();
      depth = 1;
      format = 0;
    }

  virtual ~BufferManager()
    {
      Wipe();
    }

  void Set(long int nfgh, int nflags)
    {
      fgh = nfgh;
      flags = nflags;
    }

  int Next(int x)
    {
      return x = (x+1)%BUFFER_COUNT;
    }

  void Reset()
    {
      request_index = 0;
      read_index = 0;
      valid_index = -1;
      flags = 0;
      fgh = 0;
    }

  void Allocate(int nheight, int nwidth, int nformat)
    {
      h = nheight; 
      w = nwidth;
      for (int i=0; i<BUFFER_COUNT; i++)
	{
	  depth = buffer[i].Allocate(nheight,nwidth,nformat);
	}
      format = nformat;
    }

  void Wipe()
    {
      Reset();
      for (int i=0; i<BUFFER_COUNT; i++)
	{
	  buffer[i].Reset();
	}
    }

  void RequestGrabs(int requests=9999,int override_flag=-1)
    {//
      //      printf("requestgrabs\n");
      int ct = BUFFER_COUNT;
      int flag = (override_flag!=-1)?override_flag:flags;
      while (/*!buffer[request_index].grabbing &&*/ ct>0 && requests>0)
	{
	  if (!buffer[request_index].grabbing)
	    {
	      //printf("requesting\n");
	      mutex.Wait();
	      if (!buffer[request_index].acquired)
		{
		  buffer[request_index].Grab(fgh,flag);
		  requests--;
		  /*
		  cout << "Set id at " << request_index 
		    << " to " << id << endl;
		   * */
		  buffer[request_index].id = id;
		  id++;
		}
	      mutex.Post();
	      //printf("requested\n");
	    }
	  request_index = Next(request_index);
	  ct--;
	}
      /*
      if (ct==0)
	{
	  printf("Hitting rock bottom\n");
	}
      */
    }

  int GetOldest()
    {
      int nlow = BUFFER_COUNT*100;
      int glow = BUFFER_COUNT*100;
      int nhigh = -BUFFER_COUNT;
      int index = 0;
      int lid;
      mutex.Wait();
      for (int i=0; i<BUFFER_COUNT; i++)
	{
	  buffer[i].id -= low;
	  lid = buffer[i].id;
//	  if (buffer[i].id<0)
	  /*
	    {
	      cout << "DROP at " 
		<< i
		<< " to " << buffer[i].id << " by " << low << endl;
	    }
	  assert(buffer[i].id>=0);
	   */
	  if (lid<nlow) 
	    {
	      nlow = lid;
	    }
	  if (buffer[i].grabbing && !buffer[i].acquired)
	    {
	      if (lid<glow)
		{
		  glow = lid;
		  index = i;
		}
	    }
	  if (lid>nhigh)
	    {
	      nhigh = lid;
	    }
	}
      low = nlow;
      id = nhigh+1;
      
      /*
      cout << "*** high was " << nhigh 
	<< ", low was " << nlow
	<< ", glow was " << glow
	<< " (at " << index << ")"
	<< endl;
       */
      
      mutex.Post();
      return index;
    }

  int WaitGrab()
    {
      //      printf("waitgrab\n");
      read_index = GetOldest();
      if (buffer[read_index].grabbing)
	{
	  buffer[read_index].Wait(fgh);
	  mutex.Wait();
	  if (want_wakeup)
	    {
	      wakeup.Post();
	      want_wakeup = 0;
	    }
	  valid_index = read_index;
	  mutex.Post();
	  read_index = Next(read_index);
	  return 1;
	}
      else
	{
	  //read_index = Next(read_index);
	  return 0;
	}
    }

  void Acquire(Frame& f, int wait, int fh=-1, int fw=-1)
    {
      int acquire = -1;
      if (fh<0) fh = h;
      if (fw<0) fw = w;
      assert(fh<=h&&fw<=w);
      //f.Reset();  
      f.Clear();

      //f.SetID(format);
      //f.SetPixelSize(depth);
      mutex.Wait();
      if (valid_index<0 && wait)
	{
	  want_wakeup = 1;
	  mutex.Post();
	  wakeup.Wait();
	  mutex.Wait();
	}
      if (valid_index>=0)
	{
	  acquire = valid_index;
	  buffer[valid_index].acquired = 1;
	  valid_index = -1;
	}
      mutex.Post();
      if (acquire>=0)
	{
	  //cout << "Trying to acquire" << endl;
	  //	  f.Refer((char*)buffer[acquire].buffer,128,128);
	  buffer[acquire].ForceBufferLength(fh*fw*depth);
	  f.UncountedRefer(buffer[acquire].ref_buffer.GetRawBuffer(),fw,fh,format);
	  //f.GetTimestamp().Set();
	}
      f.id = acquire;
    }

  void Relinquish(Frame& f)
    {
      if (f.id>=0)
	{
	  mutex.Wait(); // not actually necessary
	  buffer[f.id].acquired = 0;
	  buffer[f.id].grabbing = 0;
	  mutex.Post();
	}
    }
};



// There can only be one instance of the PXCFramegrabber class.
// So it is okay to keep some static information here, and
// hide it from the public class.

static long int pxc_handle = -1;
static int pxc_active = 0;
static int FRAME_lib_open = 0;
static int PXC200_lib_open = 0;


class FramegrabberThread :  public BufferManager
{
public:
  //virtual int Body();
  //void Deinit() { exit(1); }
};




PXCFramegrabber::PXCFramegrabber()
{
  height = GetMaxHeight();
  width = GetMaxWidth();
  format = PIXEL_MONO;
  server = new FramegrabberThread;
  assert(server!=NULL);
  //Init();
}

PXCFramegrabber::~PXCFramegrabber()
{
  Deinit();
  if (server!=NULL)
    {
      delete server;
    }
}

PXCFramegrabber::ErrorCode PXCFramegrabber::Init()
{
  int fault = 0;

  pid_t my_proxy;
  long int fgh;

  if (!pxc_active)
    {
      if(!FRAMELIB_OpenLibrary(&flib,sizeof(FRAMELIB)))
	{
	  fprintf(stderr,"ERROR: Pixie: FrameLib Allocation failed.\n"); 
	  Deinit();
	  fault = 1;
	}
      else
	{
	  FRAME_lib_open = 1;      
	}

      if (!fault)
	{
	  if(!PXC200_OpenLibrary(&pxc200,sizeof(PXC200)))
	    { 
	      fprintf(stderr,"ERROR: Pixie: PXC200 OpenLibrary Failed.\n");
	      Deinit();
	      fault = 1;
	    }
	  else
	    {
	      PXC200_lib_open = 1;
	    }
	}
      
      if (!fault)
	{
	  if ((my_proxy = qnx_proxy_attach(0, NULL, 0, -1)) == -1) 
	    {
	      fprintf(stderr,"ERROR: Pixie: Failed to attach proxy.\n");
	      exit(1);
	    }
#ifdef PXC_VERSION_pxc2plus
	  pxc200.SetProxy(my_proxy);
#endif
	}
      
      if (!fault)
	{
	  fgh = pxc200.AllocateFG(-1);
	  if(!fgh)
	    {
	      fprintf(stderr,"ERROR: Pixie: Unable to AllocateFG\n");
	      Deinit();
	      fault = 1;
	    }
	}
      
      if (!fault)
	{
	  //int width = 128;
	  //int height = 128;
	  InitNames();
	  cout << "Grabbing at " << height << "x" << width 
	       << ", " << image_type_name[format] << " format, "
	       << "with"
	       << " [" << left << "<=x<=" << right << "] and"
	       << " [" << top << "<=y<=" << bottom << "]"
	       << endl;
	  double xscale = (right-left);
	  double yscale = (bottom-top);
	  pxc200.SetXResolution(fgh, (int)(width/xscale + 0.5));
	  pxc200.SetYResolution(fgh, (int)(height/yscale*2 + 0.5));
	  pxc200.SetLeft(fgh, (int)(left*(width/xscale) + 0.5));
	  pxc200.SetTop(fgh, (int)(top*(2*height/yscale) + 0.5));
	  pxc200.SetWidth(fgh, width);
	  pxc200.SetHeight(fgh, 2*height);
      
	  server->Set(fgh,QUEUED|FIELD0|SINGLE_FLD);
	  server->Allocate(height,width,format);

	  //  server->Begin();

	  pxc_handle = fgh;
	  pxc_active = 1;
	}
    }

  //  server->Begin(99999);

  return pxc_active?OK:BAD;
}


PXCFramegrabber::ErrorCode PXCFramegrabber::Deinit()
{
  //  server->End();
  server->Wipe();
  if(FRAME_lib_open)
    {
      FRAMELIB_CloseLibrary(&flib);
      FRAME_lib_open = 0;
    }
  if(PXC200_lib_open)
    {
      PXC200_CloseLibrary(&pxc200);
      PXC200_lib_open = 0;
    }
  pxc_active = 0;
  return OK;
}

void PXCFramegrabber::EmergencyStop()
{
  if(FRAME_lib_open)
    {
      FRAMELIB_CloseLibrary(&flib);
      FRAME_lib_open = 0;
    }
  if(PXC200_lib_open)
    {
      PXC200_CloseLibrary(&pxc200);
      PXC200_lib_open = 0;
    }
  printf("Framegrabber shut down\n");
}

PXCFramegrabber::ErrorCode PXCFramegrabber::CopyFrameToImage(YARPGenericImage& 
							     image)
{
  Frame f;
  AcquireFrame(f,1);
  image.PeerCopy(f);
  RelinquishFrame(f);
  return OK;
}
  

virtual PXCFramegrabber::ErrorCode PXCFramegrabber::AcquireFrame(Frame& frame, bool wait)
  //const Frame& PXCFramegrabber::AcquireFrame()
{
  static int my_proxy = -1;
  if (active)
    {
      server->Acquire(frame,wait);
    }
  else
    {
      if (my_proxy == -1)
	{
	  if ((my_proxy = qnx_proxy_attach(0, NULL, 0, -1)) == -1) 
	    {
	      fprintf(stderr,"ERROR: Pixie: Failed to attach proxy.\n");
	      exit(1);
	    }
	}
#ifdef PXC_VERSION_pxc2plus
      pxc200.SetProxy(my_proxy);
#endif

      //cout << "Changing size 2..." << endl;
      double xscale = (right-left);
      double yscale = (bottom-top);
      long int fgh = pxc_handle;
      assert(fgh!=-1);
      pxc200.SetXResolution(fgh, (int)(width/xscale + 0.5));
      pxc200.SetYResolution(fgh, (int)(height/yscale + 0.5));
      pxc200.SetLeft(fgh, (int)(left*(width/xscale) + 0.5));
      pxc200.SetTop(fgh, (int)(top*(height/yscale) + 0.5));
      pxc200.SetWidth(fgh, width);
      pxc200.SetHeight(fgh, height);
      // should check that is without bounds of allocated memory
      //cout << "Sending single request" << endl;
      server->RequestGrabs(1,FIELD0|QUEUED);
      server->WaitGrab();
      server->Acquire(frame,wait,height,width);
    }
  return OK;
}


PXCFramegrabber::ErrorCode PXCFramegrabber::RelinquishFrame(Frame& frame)
{
  server->Relinquish(frame);
  return OK;
}


void PXCFramegrabber::Main()
{
  int grabbing, grabbed;
  while (1)
    {
      if (pxc_active)
	{
	  active = 1;
	  server->RequestGrabs();
	  server->WaitGrab();
	  grabbing = 0;
	  grabbed = 0;
	  for (int i=0; i<BUFFER_COUNT; i++)
	    {
	      FramegrabberBuffer& fgb = server->buffer[i];
	      grabbing += fgb.grabbing;
	      grabbed += fgb.grabbed;
	      /*
	      cout << i << " --> " << fgb.grabbing << " "
		   << fgb.grabbed << " "
		   << fgb.acquired
		   << endl;
	      */
	      //	      delay(100);
	    }
	  if (grabbing<2) printf(" ---> [%d] [%d]\n", grabbing, grabbed);
	}
      Yield();
    }
}


int PXCFramegrabber::GetMaxHeight()
{
  return 480;
}


int PXCFramegrabber::GetMaxWidth()
{
  return 640;
}

PXCFramegrabber::ErrorCode PXCFramegrabber::ChangeSize(int n_height, 
						       int n_width)
{ 
  long int fgh = pxc_handle;
  assert(fgh!=-1);
  PXCFramegrabber::ErrorCode result = NOT_IMPLEMENTED;
  cout << "Changing size 1..." << endl;
  if (active)
    {
      cout << "Changing size 2..." << endl;
      double xscale = (right-left);
      double yscale = (bottom-top);
      pxc200.SetXResolution(fgh, (int)(width/xscale + 0.5));
      pxc200.SetYResolution(fgh, (int)(height/yscale + 0.5));
      pxc200.SetLeft(fgh, (int)(left*(width/xscale) + 0.5));
      pxc200.SetTop(fgh, (int)(top*(height/yscale) + 0.5));
      pxc200.SetWidth(fgh, width);
      pxc200.SetHeight(fgh, height);
      result = OK;
    }
  return result; 
}

PXCFramegrabber::ErrorCode PXCFramegrabber::AddSignalHandler()
{
  // Set up signal handlers for framegrabber process.
  ::running_grabber = this;
  signal(SIGTERM, &terminate);
  signal(SIGINT, &terminate);
  atexit(terminate_atexit);
  return OK;
}



