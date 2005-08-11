
#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif


//#define DV_OUT
#define SDL_OUT

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

#include "sidetap.h"
#include "PotHand.h"
#include "Observe.h"
#include "Grapher.h"

#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageDraw.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPTime.h>

#include "EdHeadRemote.h"

/*
  /armMap/head/o:bot
  bottle 
  int id = 9;
  int x, y;
  int conf = 100;
 */


double pred_x, pred_y;
int pred_cx, pred_cy;
double pred_time = -10000;

void ShowPrediction(double x, double y, int cx, int cy) {
  pred_x = x;
  pred_y = y;
  pred_cx = cx;
  pred_cy = cy;
  pred_time = YARPTime::GetTimeAsSeconds();
}

class HeadTrackerInput : public YARPInputPortOf<YARPBottle> {
public:
  /*
  EdHeadRemote head;

  HeadTrackerInput() {
    int x, y;
    head.get(x,y);
  }
  */

  virtual void OnRead() {
    if (Read()) {
      //printf("GOT SOMETHING!\n");
      YARPBottle& bot = Content();
      int id = -1;
      id = bot.readInt();
      if (id==9) {
	double x = 0;
        double y = 0;
	int conf = 0;
	bot.readFloat(&x);
	bot.readFloat(&y);
	//x = bot.readFloat();
	//y = bot.readFloat();
	//x = bot.readInt();
	//y = bot.readInt();
	conf = bot.readInt();
	int cx = 0, cy = 0;
	//head.get(cx,cy);
	static int ct = 0;
	ct++;
	if(ct>=30) {
	  ct = 0;
	  printf("*** Got prediction %g %g\n", x, y);
	}
	ShowPrediction(x,y,cx,cy);
      }
    }
  }
};

class MySideTap : public SideTap {
public:
  virtual bool ShouldSupplyImage() { return true; }
  virtual bool ShouldRecoverImage() { 
#ifdef DV_OUT
    return true; 
#else
    return false; 
#endif
  }
  virtual bool ShouldSave() { return ShouldRecoverImage(); }
  virtual void Apply(SideTapImage& image, SideTapSound& sound);
};


HeadTrackerInput head_input;
static int init = 0;

void MySideTap::Apply(SideTapImage& image, SideTapSound& sound) {
  if (!init) {
    init = 1;
    head_input.Register("/hand/track/i:bot");
  }
  if (image.IsValid()) {
    static int ct = 0;
    //fprintf(stderr,"MySideTap::Apply() in sidetap.cc received an image! %dx%d pixels\n", image.GetWidth(), image.GetHeight());
    /*
    for (int i=0; i<image.GetWidth(); i++)
      {
	image(i,ct).r = 255;
      }
    ct = (ct+1)%image.GetHeight();
    */

    /*
    YARPImageOf<YarpPixelRGB> img;
    img.Resize(image.GetWidth(),image.GetHeight()/2);
    IMGFOR(img,x,y) {
      img(x,y).r = image(x,y*2).r;
      img(x,y).g = image(x,y*2).g;
      img(x,y).b = image(x,y*2).b;
    }
    char buf[256];
    static int idx = 0;
    sprintf(buf,"/scratch/frames/frame_%06d.ppm",idx);
    idx++;
    YARPImageFile::Write(buf,img);
    */

    static PotHand ph;
    static YARPImageOf<YarpPixelRGB> dest, dest2, image_small, image_smaller;
    static int rct = 0;
    if (rct%1==0) {
      image_small.Resize(image.GetWidth()/2,image.GetHeight()/2);
      image_smaller.Resize(image.GetWidth()/4,image.GetHeight()/4);
      IMGFOR(image_small,x,y) {
	image_small(x,y) = image(x*2,y*2);
      }
      IMGFOR(image_smaller,x,y) {
	image_smaller(x,y) = image(x*4,y*4);
      }
      //image_small.ScaledCopy(image,image.GetWidth()/scale,image.GetHeight()/scale);
      //dest = image_small;
      ph.ApplyFreq(image_smaller,dest2);
      SatisfySize(image_small,dest);
      IMGFOR(dest,x,y) {
	dest(x,y) = dest2(x/2,y/2);
      }
      if (ph.rset) {
	ph.Apply(image_small,dest,ph.rox*2,ph.roy*2,ph.row*2,ph.roh*2);
      }
      double now = YARPTime::GetTimeAsSeconds();
      if (now-pred_time<2) {
	/*
	for (int i=0; i<dest.GetWidth(); i++) {
	  dest(i,20).g = 255;
	}
	*/
	/*
	int ww = dest.GetWidth();
	int hh = dest.GetHeight();
	float bx = ww/2;
	float by = hh/2;
	static float theta = 0;
	theta += 0.05;
	float tx = cos(theta);
	float ty = sin(theta);
	tx = pred_x - pred_cx;
	ty = pred_y - pred_cy;
	float mag = sqrt(tx*tx+ty*ty);
	tx /= mag;
	ty /= mag;
	float vx = 0;
	float vy = 0;
	float hx = 0;
	float hy = 0;
	if (fabs(tx)>0.001) {
	  vx = (ww/2)*((tx>0)?1:-1);
	  vy = vx*(ty/tx);
	}
	if (fabs(ty)>0.001) {
	  hy = (hh/2)*((ty>0)?1:-1);
	  hx = hy*(tx/ty);
	}
	float gx = hx;
	float gy = hy;
	if (fabs(vy)<=hh/2) {
	  gx = vx;
	  gy = vy;
	}
	float mag_th = 10;
	if (mag>mag_th) mag = mag_th;
	gx *= mag/mag_th;
	gy *= mag/mag_th;
	gx += bx;
	gy += by;
	*/
	float gx = pred_x*dest.GetWidth();
	float gy = pred_y*dest.GetHeight();

	AddCircle(dest,YarpPixelRGB(255,0,255),gx,gy,20);
	AddCircle(dest,YarpPixelRGB(128,0,255),gx,gy,10);
      }
      //image.ScaledCopy(image_small,image.GetWidth(),image.GetHeight());
    }
    rct++;
#ifdef DV_OUT
    IMGFOR(image,x,y) {
      image(x,y) = dest(x/2,y/2);
    }
#endif
#ifdef SDL_OUT
    static Grapher grapher;
    grapher.Apply(dest);
#endif
  }
}

SideTap *CreateSideTap() {
  SideTap *tap = new MySideTap;
  assert(tap!=NULL);
  return tap;
}
