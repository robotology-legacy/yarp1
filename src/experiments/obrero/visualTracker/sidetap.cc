/*
* sidetap.h -- classes to processes frames produced by dvgrab
* Copyright (C) 2004 Paul Fitzpatrick <paulfitz@ai.mit.edu>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
*/

#include <assert.h>
#include <stdio.h>

//#define SHOW_GUI
#define REMOTE_HEAD

#include "sidetap.h"
#include "robot_gui.h"

#include <yarp/YARPImageDraw.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPSemaphore.h>

#include "ObreroVision.h"
#include "ImgTrack.h"
#include "MotionGroup.h"

#include "Motor.h"

static int at_x = 0, at_y = 0;

static YARPSemaphore set_mutex(1);
static int img_w = -1, img_h = -1;
static int set_pending = 0, set_x = 0, set_y = 0, set_w = 0, set_h = 0;

void set_location(int x, int y, int w, int h);
void abs_req_motor(int x, int y);
void update_motor();
void set_home_bias();

bool inhibited=false;

class MySideTap : public SideTap {
public:
  virtual bool ShouldSupplyImage() { return true; }
  virtual bool ShouldSave() { return false; }
  virtual void Apply(SideTapImage& image, SideTapSound& sound);
};

class Messages : public YARPInputPortOf<YARPBottle> {
public:
  virtual void OnRead() 
  {
    Read();
    int msg;
    YARPBottle &bot = Content();
    bool ret = bot.readInt(&msg);
    if (msg==200)
      {
	if (bot.readInt(&msg))
	  {
	    if (msg==1)
	      inhibited=true;
	    else
	      inhibited=false;
	  }
      }
  }
};

class TrackerPort : public YARPInputPortOf<YARPBottle> {
public:
  virtual void OnRead() {
    if (Read()) {
      printf("GotSomething!\n");
      int msg;
      YARPBottle& bot = Content();
      bool ret = bot.readInt(&msg);
      if (ret) {
	if (msg==200) {
	  int x, y, w, h;
	  ret = bot.readInt(&x);
	  if (ret) { bot.readInt(&y);    }
	  if (ret) { bot.readInt(&w); }
	  if (ret) { bot.readInt(&h); }
	  if (ret) {
	    printf("valid request to go to %d,%d (in %dx%d image)\n",
		   x, y, w, h);
	    set_location(x,y,w,h);
	  }
	} else if (msg==9) {
	  double xx = 0, yy = 0;
	  bot.readFloat(&xx);
	  bot.readFloat(&yy);
	  if (img_w!=-1) {
	    int x, y, w, h;
	    w = img_w;
	    h = img_h;
	    x = (int)(xx*w);
	    y = (int)(yy*h);
	    printf("valid request to go to %d,%d (in %dx%d image)\n",
		   x, y, w, h);
	    set_location(x,y,w,h);
	  }
	}
      }
    }
  }
};

TrackerPort inTracker;
Messages messageHandler;

double home_bias_time = -10000;

int IsHoming(double now) {
  return now-home_bias_time<3;
}

void set_location(int x, int y, int w, int h) {
  if(inhibited)
    return;
  set_mutex.Wait();
  set_pending = 1;
  set_x = x;
  set_y = y;
  set_w = w;
  set_h = h;
  set_mutex.Post();
  printf("%d pass set location %d,%d (in %dx%d image)\n",
	 set_pending, set_x, set_y, set_w, set_h);
}


void MySideTap::Apply(SideTapImage& image, SideTapSound& sound) {
  /*
  static int dropper = 0;
  dropper++;
  if (dropper>=10) {
    dropper = 0;
  } else {
    return;
  }
  */

  static ObreroVision vision;
  static ImgTrackTool track;
  static MotionGroup motion;
  if (image.IsValid()) {
    if (img_w==-1) {
      img_w = image.GetWidth();
      img_h = image.GetHeight();
    }
    static int ct = 0;
    //printf("MySideTap::Apply() in sidetap.cc received an image! %dx%d pixels\n", image.GetWidth(), image.GetHeight());
    //for (int i=0; i<image.GetWidth(); i++)
    //{
    //image(i,ct).r = 255;
    //}
    ct = (ct+1)%image.GetHeight();
    static YARPImageOf<YarpPixelRGB> work1, work2;
    work1.ScaledCopy(image,128,128);

    static int counter = 0;
    counter++;
    /*
    IMGFOR(work1,x,y) {
      if (x>=counter%128&&x<(counter+75)%128) {
	work1(x,y).r = (counter+13*x+y)*253;
	work1(x,y).g = (counter+253*x+y)*13;
	work1(x,y).b = (counter*17+5*x+y)*7;
      }
    }
    */

    //vision.Apply(work1,work2);
    work2.CastCopy(work1);
    track.Apply(work1);
    static int motion_flip = 0;
    motion_flip++;
    if (motion_flip>=5) {
      motion_flip = 0;
    }
    double now = YARPTime::GetTimeAsSeconds();
    if (motion_flip==-1 /*i.e. never*/) {
      motion.Apply(work1,work2);
      static double last_motion_update = now;
      static double pre_motion_update = now;
      if (motion.IsValid()) {
	if (motion.GetStrength()>100) {
	  last_motion_update = now+1;
	  pre_motion_update = now-1000;
	}
	if (now-last_motion_update>1) {
	  if (now-pre_motion_update>1.5) {
	    pre_motion_update = now;
	  }
	  if (now-pre_motion_update>0.5) {
	    printf("*** tracker update!\n");
	    track.SetXY(motion.GetX(),motion.GetY());
	    last_motion_update = now;
	  } 
	}
      }
    }

    static double last_update = now+2;

    // boredom

    static double last_bounds = now;


    update_motor();
    static int ctx = 0;
    ctx++;
    if (ctx%30==0) {
      printf("%d %d  // %d %d\n", track.GetX(), track.GetY(), at_x, at_y);
      ctx = 0;
    }
    if (fabs(at_x)<40 && (at_y<70&&at_y>-40)) {
      last_bounds = now;
    }

    if (now-last_bounds>3) {
      printf("*** bored with bounds\n");
      //abs_req_motor(0,0);
      set_home_bias();
      //last_update = now+5;
      last_bounds = now;
    }
    if (IsHoming(now)) {
      track.SetXY(work1.GetWidth()/2,work1.GetHeight()/2);
    }
    if (set_pending) {
      set_mutex.Wait();
      if (set_pending) {
	set_pending = 0;
	int w1 = work1.GetWidth();
	int h1 = work2.GetHeight();
	if (set_w==w1&&set_h==h1) {
	  track.SetXY(set_x,set_y);
	} else {
	  track.SetXY((int)(set_x*((float)w1/set_w)),
		      (int)(set_y*((float)h1/set_h)));
	}
	printf("*** tracker update! %d,%d\n",track.GetX(),track.GetY());
      }
      set_mutex.Post();
    }

    AddCircle(work2,YarpPixelRGB(0,255,255),track.GetX(),
	      track.GetY(),5);
    IMGFOR(work2,x,y) {
      image(x,y) = work2(x,y);
    }
    static int first = 1;
    static int xx = 0, yy = 0;
    if (first) {
      first = 0;
      req_motor(0,0);
      inTracker.Register("/tracker/i:pos");
      messageHandler.Register("/tracker/inhibition/i:bot");
      YARPTime::DelayInSeconds(1);
    }
    if (now-last_update>0.1) {
      int dx = track.GetX()-64;
      int dy = track.GetY()-64;
      //dx /= 10;
      //dy /= 10;
      //printf("DX %d %d\n", dx, dy);
      xx += dx;
      yy += dy;
      req_motor(dx,dy);
      last_update = now;
    }
#ifdef SHOW_GUI
    robot_gui(image);
#endif
  }
}

SideTap *CreateSideTap() {
  SideTap *tap = new MySideTap;
  assert(tap!=NULL);
  return tap;
}

#ifdef REMOTE_HEAD
#include "EdHeadRemote.h"
static EdHeadRemote head;
#else
#include "EdHead.h"
static EdHead head;
#endif
void abs_req_motor(int x, int y) {
  head.set(x,y);
}

void update_motor() {
  int x0 = 0, y0 = 0;
  head.get(x0,y0);
  at_x = x0;
  at_y = y0;
}


void req_motor(int x, int y) {
  int x0 = 0, y0 = 0;
  head.get(x0,y0);
  at_x = x0;
  at_y = y0;
  int tx = x0+x/3;
  int ty = y0+y/4;
  double now = YARPTime::GetTimeAsSeconds();
  if (IsHoming(now)) {
    tx = (int)(x0 * 0.8);
    ty = (int)(y0 * 0.8);
  }

  static double prevTime = now+2;
  static int offset = 0;
  //if (now-prevTime>0.25) 
    {
      if ((tx-x0)*(tx-x0)+(ty-y0)*(ty-y0)>10) {
	head.set(tx,ty);
      }
    offset += 5;
    prevTime = now;
  }
}

void set_home_bias() {
  home_bias_time = YARPTime::GetTimeAsSeconds();
}

