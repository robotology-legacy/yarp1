#include <iostream.h>
#include <math.h>

#include "ImgTrack.h"
#include "YARPImage.h"
#include "YARPImagePort.h"
#include "YARPTime.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"

#include <conf/tx_data.h>
#include "FaceContent.h"

#ifndef for
#define for if (1) for
#endif

YARPInputPortOf<YARPGenericImage> in_img;

YARPOutputPortOf<YARPGenericImage> out_img;

YARPInputPortOf<StereoPosData> in_data;
YARPOutputPortOf<StereoPosData> out_data;

#define DEFAULT_NAME  "/attn_tracker"

#define MAX_BOX 1
#define MAX_TRACKER 1

class Box
{
public:
  int tlx, tly, brx, bry;
  int cx, cy;
  
  void Set(int n_tlx, int n_tly, int n_brx, int n_bry)
    {
      tlx = n_tlx;   tly = n_tly;
      brx = n_brx;   bry = n_bry;
      cx = (tlx+brx)/2;
      cy = (tly+bry)/2;
    }
  
  float GetDistance(const Box& b) const
    {
      return sqrt((cx-b.cx)*(cx-b.cx) + (cy-b.cy)*(cy-b.cy));
    }
};

class Trackers
{
public:
  YARPImageOf<YarpPixelMono> prev, face_prev;
  int first;
  ImgTrackTool track_tool;
  YARPSemaphore mutex;

  class Tracker
  {
  public:
    Box box;
    int is_active;
    int is_tracking;
    int is_lagged;
    double last;
  };
  Tracker tracker[MAX_TRACKER];

  int observing;

  Trackers() : mutex(1)
    {
      first = 1;
      observing = 0;
      for (int i=0; i<MAX_TRACKER; i++)
	{
	  tracker[i].is_active = 0;
	  tracker[i].is_tracking = 0;
	}
    }
  
  int GetClosest(const Box& target, float threshold = 99999999999.0)
    {
      float best = threshold;
      float besti = -1;
      for (int i=0; i<MAX_TRACKER; i++)
	{
	  if (tracker[i].is_active)
	    {
	      float d = target.GetDistance(tracker[i].box);
	      if (d<best)
		{
		  best = d;
		  besti = i;
		}
	    }
	}
      return besti;
    }

  int GetAvailableTracker()
    {
      int index = -1;
      for (int i=0; i<MAX_TRACKER; i++)
	{
	  if (!tracker[i].is_active)
	    {
	      index = i; break;
	    }
	}
      return index;
    }

  void UpdateActivity()
    {
      double now = YARPTime::GetTimeAsSeconds();
      for (int i=0; i<MAX_TRACKER; i++)
	{
	  if (tracker[i].is_active)
	    {
	      if (now - tracker[i].last >= 0.5)
		{
		  tracker[i].is_active = 0;
		  tracker[i].is_tracking = 0;
		}
	    }
	}
    }
  
  void BeginObservations()
    {
      if (!observing)
	{
	  mutex.Wait();
	  UpdateActivity();
	  observing = 1;
	}
    }
  
  
  void Observe(const Box& box)
    {
      int theta = 2;
      double now = YARPTime::GetTimeAsSeconds();
      BeginObservations();
      int index = GetClosest(box,12);
      int index2 = GetClosest(box,25);
      if (index2<0)
	{
	  index = GetAvailableTracker();
	  if (index<0)
	    {
	      //printf("*** No trackers available\n");
	    }
	}
      else
	{
	  tracker[index].last = now;
	}
      if (index>=0)
	{
	  Tracker& t = tracker[index];
	  if (!(t.is_tracking))
	    {
	      t.box = box;
	      t.is_lagged = 1;
	    }
	  else
	    {
	      t.is_lagged = 0;
	    }
	  if (t.is_active<theta)
	    {
	      t.is_active++;
	    }
	  if (t.is_active>=theta)
	    {
	      t.is_tracking = 1;
	    }
	  else
	    {
	      t.is_tracking = 0;
	    }
	  t.last = now;
	}
    }
    
  void EndObservations()
    {
      observing = 0;
      mutex.Post();
    }
  
  void Update(YARPImageOf<YarpPixelMono>& img,
	      YARPImageOf<YarpPixelBGR>& dest,
	      FiveBoxesInARow& out_boxes)
    {
      if (first)
	{
	  prev.PeerCopy(img);
	  first = 0;
	}
      int count = 0;
      int count2 = 0;
      int index = 0;
      mutex.Wait();
      UpdateActivity();
      int box_index = 0;
      for (int k=0; k<FiveBoxesInARow::GetMaxBoxes(); k++)
	{
	  out_boxes(k).valid = false;
	}
      for (int i=0; i<MAX_TRACKER; i++)
	{
	  if (tracker[i].is_active)
	    {
	      count2++;
	    }
	  if (tracker[i].is_tracking)
	    {
	      count++;
	      int ox = tracker[i].box.cx;
	      int oy = tracker[i].box.cy;
	      int x = ox;
	      int y = oy;
	      int theta = 15;
	      if (oy>theta && oy<=img.GetHeight()-theta &&
		  ox>theta && ox<=img.GetWidth()-theta)
		{
		  if (0) //tracker[i].is_lagged
		    {
		      track_tool.Apply(face_prev,img,ox,oy);
		      tracker[i].is_lagged = 0;
		    }
	          else 
		    {
			{
			  track_tool.Apply(prev,img,ox,oy);
			}
		    }
		  x = track_tool.GetX();
		  y = track_tool.GetY();
		}
	      int dx = x-ox;
	      int dy = y-oy;
	      if (dx!=0 || dy!=0)
		{
//		  printf("Delta %d %d (to %d %d)\n", dx, dy, x, y);
		}
	      tracker[i].box.brx += dx;
	      tracker[i].box.bry += dy;
	      tracker[i].box.tlx += dx;
	      tracker[i].box.tly += dy;
	      tracker[i].box.cx += dx;
	      tracker[i].box.cy += dy;
	      if (box_index<FiveBoxesInARow::GetMaxBoxes() && box_index>=0)
		{
		  CBox2Send& dest2 = out_boxes(box_index);
		  box_index++;
		  Box& src = tracker[i].box;
		  dest2.xmin = src.tlx;
		  dest2.ymin = src.tly;
		  dest2.xmax = src.brx;
		  dest2.ymax = src.bry;
		  dest2.valid = true;
		  for (int i = -3; i<= 3; i++)
		    {
		      for (int j=-3; j<=3; j++)
			{
			  if ((i+j)%2)
			    {
			      dest.SafePixel(x+j,y+i) = YarpPixelBGR(255,255,255);
			    }
			  else
			    {
			      dest.SafePixel(x+j,y+i) = YarpPixelBGR(0,0,0);
			    }
			}
		    }
		}
	    }
	}
      mutex.Post();
      //if (count>0)
	{
//	  printf("*** %d trackers tracking, %d active\n", count,
//		 count2);
	}
      prev.PeerCopy(img);
    }
  
} trackers;


void Filter(YARPImageOf<YarpPixelBGR>& src,
	    YARPImageOf<YarpPixelBGR>& dest)
{
  FiveBoxesInARow boxes;
  YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(src);
  dest.PeerCopy(src);
  //trackers.face_prev.CastCopy(in_mix.Content().GetImage());
  trackers.Update(mono,dest,boxes);
  out_data.Content().valid = true;
  if (boxes(0).valid)
    {
      out_data.Content().xl = boxes(0).xmin;
      out_data.Content().yl = boxes(0).ymin;
      out_data.Content().xr = boxes(0).xmax;
      out_data.Content().yr = boxes(0).ymax;
    }
  else
    {
      out_data.Content().xl = 64;
      out_data.Content().yl = 64;
      out_data.Content().xr = 64;
      out_data.Content().yr = 64;
    }
  out_data.Write();
}

YARPSemaphore pos_mutex;
Box pos_last;
int pos_last_set = 0;

class BoxThread : public YARPThread
{
public:
  BoxThread()
    {
    }
  
  virtual void Body()
    {
      while (1)
	{
	  //printf("Reading...\n");
	  in_data.Read();
	  StereoPosData& pos = in_data.Content();
	  if (pos.valid)
	    {
	      trackers.BeginObservations();
	      Box b;
	      b.Set(pos.xl,pos.yl,pos.xr,pos.yr);
	      trackers.Observe(b);
	      trackers.EndObservations();
	    }
	}
    }
} box_thread;



int main(int argc, const char *argv[])
{
  const char *name = DEFAULT_NAME;
  char buf[256];
  if (argc>1)
    {
      name = argv[1];
    }
  sprintf(buf, "%s/i:img", name);
  in_img.Register(buf);
  sprintf(buf, "%s/o:img", name);
  out_img.Register(buf);

  sprintf(buf, "%s/i:pos", name);
  in_data.Register(buf);
  sprintf(buf, "%s/o:pos", name);
  out_data.Register(buf);
  
  box_thread.Begin();
  
  while(1)
    {
      in_img.Read();
      YARPImageOf<YarpPixelBGR> in;
      YARPImageOf<YarpPixelBGR> out;
      in.Refer(in_img.Content());
      out_img.Content().SetID(YARP_PIXEL_BGR);
      SatisfySize(in,out_img.Content());
      out.Refer(out_img.Content());
      Filter(in,out);
      //out.PeerCopy(in);
      out_img.Write();
//      YARPTime::DelayInSeconds(1000000);
    }
  return 0;
}
