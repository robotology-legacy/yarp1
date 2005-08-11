
#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif

#include <stdio.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>

#include "PotHand.h"
#include "Observe.h"
#include "IntegralImageTool.h"
#include "YARPImageLabel.h"

#define MAX_OFF 16


YARPOutputPortOf<YARPBottle> out_tracker;
YARPOutputPortOf<YARPBottle> out_learner;

void init_tracker() {
  static int init = 0;
  if (!init) {
    init = 1;
    out_tracker.Register("/hand/track/o:bot");
    out_learner.Register("/hand_detect/o:bot");
  }
}

void set_tracker(int x, int y, int w, int h) {
  YARPBottle& bot = out_tracker.Content();
  bot.writeInt(200);
  bot.writeInt(x);
  bot.writeInt(y);
  bot.writeInt(w);
  bot.writeInt(h);
  out_tracker.Write();
}

void tell_learner(int flag,int x, int y, int w, int h) {
  YARPBottle& bot = out_learner.Content();
  bot.writeInt(8);
  bot.writeInt(1);
  bot.writeFloat(((double)x)/w);
  bot.writeFloat(((double)y)/h);
  //bot.writeInt(x);
  //bot.writeInt(y);
  //bot.writeInt(w);
  //bot.writeInt(h);
  out_learner.Write();
}

class Circular {
public:

  int offx[MAX_OFF];
  int offy[MAX_OFF];
  int order[MAX_OFF];
  int pscale;

  Circular(int rad) {
    pscale = -1;
    init_offsets(rad);
  }
  
  void init_offsets(int scale) {
    if (scale!=pscale) {
      int done[MAX_OFF];
      for (int i=0; i<MAX_OFF; i++) {
	done[i] = 0;
      }
      int idx = 0;
      for (int i=MAX_OFF/2; i>=1; i/=2) {
	for (int j=0; j<MAX_OFF; j += i) {
	  if (!done[j]) {
	    done[j] = 1;
	    //printf("[%02d] %d\n", idx, j);
	    order[idx] = j;
	    idx++;
	  }
	}
      }
      float range = 2*M_PI;
      for (int i=0; i<MAX_OFF; i++) {
	float theta = i*(range/MAX_OFF);
	offx[order[i]] = (int)(cos(theta)*scale);
	offy[order[i]] = (int)(sin(theta)*scale);
      }
      pscale = scale;
    }
  }

  int Check(YARPImageOf<YarpPixelFloat>& mono, int x, int y) {
    int th = 4;
    int th2 = 30;
    float rth = 0.9;
    int mm = 100;
    float vref = mono(x,y);
    int ok = 1;
    float tot = 0;
    float mu = 0;
    for (int i=0; i<MAX_OFF; i++) {
      float valt = mono.SafePixel(x+offx[i],y+offy[i]);
      if (valt<vref+th) {
	ok = 0;
	break;
      }
      if (valt<mm) {
	ok = 0;
	break;
      }
      tot += valt;
    }
    if (ok) {
      tot /= MAX_OFF;
      mu = tot;
      int strikes = 0;
      int strikes2 = 0;
      for (int i=0; i<MAX_OFF; i++) {
	float valt = mono.SafePixel(x+offx[i],y+offy[i]);
	float valt2 = mono.SafePixel(x+offx[i]*6,y+offy[i]*6);
	if (valt>1.2*mu || valt<0.8*mu) {
	  strikes++;
	}
	if (valt2>1.2*mu || valt2<0.8*mu) {
	  strikes2++;
	}
      }
      if (strikes>MAX_OFF/4) {
	ok = 0;
      }
      if (strikes2<(MAX_OFF*3)/4) {
	ok = 0;
      }
    }
    return ok;
  }
};

float getDist(int x, int y, int xx, int yy) {
    return ((float)x-xx)*((float)x-xx)+((float)y-yy)*((float)y-yy);
}

void PotHand::Apply(YARPImageOf<YarpPixelRGB>& src, 
		    YARPImageOf<YarpPixelRGB>& dest,
		    int cx, int cy,
		    int cw, int ch) {
  if (cw==-1) {
    cw = src.GetWidth()/2;
    ch = src.GetWidth()/2;
    cx = cw;
    cy = ch;
  }
  //dest = src;
  YARPImageOf<YarpPixelFloat> mono;
  mono.CastCopy(src);

  int off = 3;
  Circular circ1(off);
  Circular circ2(off+1);


  static Observe ob;
  ob.beginObservation();

  IMGFOR(mono,x,y) {
    if (fabs(x-cx)<cw && fabs(y-cy)<ch) {
      if (x%2==0) {
	int ok = circ1.Check(mono,x,y);
	if (ok) {
	  ok = circ2.Check(mono,x,y);
	}
	if (ok) {
	  AddCircle(dest,YarpPixelRGB(255,0,0),x,y,5);
	  AddCircle(dest,YarpPixelRGB(0,255,0),x,y,3);
	  ob.addObservation(src,x,y,1);
	}
      }
    }
  }
  
  ob.endObservation();

  float quality = ob.Apply(src,dest);
  static float prev_quality = 0;
  static double last_update = -1000;
  if (quality>0.5) {
    tell_learner(1,(int)cx,(int)cy,(int)src.GetWidth(),(int)src.GetHeight());
    AddCircle(dest,YarpPixelRGB(0,255,0),cx,cy,15);
    AddCircle(dest,YarpPixelRGB(0,0,255),cx,cy,10);
    double now = YARPTime::GetTimeAsSeconds();
    if (now-last_update>0.5) {
      printf("Seeing arm (%d,%d of %dx%d)\n", 
	     cx, cy,src.GetWidth(),src.GetHeight());
      last_update = now;
      if (getDist(cx,cy,src.GetWidth()/2,src.GetHeight()/2)>src.GetHeight()/6) {
	printf("   >>> should update\n");
	set_tracker(cx,cy,src.GetWidth(),src.GetHeight());
      }
    }
  } else {
    if (prev_quality>0.5) printf("Lost arm\n");
  }
  prev_quality = quality;

#if 0
  IMGFOR(src,x,y) {
    float vref = mono(x,y);
    float ll = mono.SafePixel(x-off,y);
    float rr = mono.SafePixel(x+off,y);
    float uu = mono.SafePixel(x,y-off);
    float dd = mono.SafePixel(x,y+off);
    if (vref<ll && vref<rr && vref<uu && vref<dd) {
      if (vref<ll-th && vref<rr-th && vref<uu-th && vref<dd-th) {
	if (vref<ll*rth && vref<rr*rth && vref<uu*rth && vref<dd*rth) {
	  if (fabs(ll-rr)<th2 && fabs(uu-dd)<th2 && fabs(uu-ll)<th2 && fabs(uu-rr)<th2 && fabs(dd-ll)<th2 && fabs(dd-rr)<th2) {
	    if (ll>mm && rr>mm && uu>mm && dd>mm) {
	      AddCircle(dest,YarpPixelRGB(255,0,0),x,y,2);
	      AddCircle(dest,YarpPixelRGB(0,255,0),x,y,1);
	    }
	  }
	}
      }
    }
  }
#endif
}


float differ(float x, float y) {
  float r = x-y;
  float mx = 40;
  if (r<0) r = -r;
  if (r>mx) r = mx;
  return r;
}

void PotHand::ApplyFreq(YARPImageOf<YarpPixelRGB>& src, 
			YARPImageOf<YarpPixelRGB>& dest) {
  init_tracker();
  SatisfySize(src,dest);
  static int ct = 0;
  static YARPImageOf<YarpPixelRGB> pdest;
  ct = 0;
  if (ct%5!=0) {
    dest = pdest;
    ct++;
    return;
  }
  ct++;
  if (ct>=15) ct = 0;
  static YARPImageOf<YarpPixelFloat> mono, diff;
  static YARPImageOf<YarpPixelInt> mask, label;
  SatisfySize(src,diff);
  SatisfySize(src,mono);
  SatisfySize(src,mask);
  SatisfySize(src,label);
  mono.CastCopy(src);
  int scale = 2;
  int dd = 8; // good for half-scale
  dd /= scale;
  IMGFOR(src,x,y) {
    float c = mono(x,y);
    float v = 0;
    v += differ(c,mono.SafePixel(x+dd,y));
    v += differ(c,mono.SafePixel(x-dd,y));
    v += differ(c,mono.SafePixel(x,y+dd));
    v += differ(c,mono.SafePixel(x,y-dd));
    diff(x,y) = (v/4)/(c+10);
  }
  static IntegralImageTool ii;
  ii.GetMean(diff,diff,25/scale);
  IMGFOR(diff,x,y) {
    mask(x,y) = (diff(x,y)>=0.35);
    //mask(x,y) = (diff(x,y)>=19);
  }
  static YARPImageLabel labeller;
  int choice = labeller.Apply(mask,label);

  dest = src;
  int tx = 0;
  int ty = 0;
  int tval = 1000000;
  int tset = 0;
  IMGFOR(diff,x,y) {
    if (label(x,y)==choice) {
      int val = tx+ty;
      if (val<tval) { ty = y;  tx = x;  tval = val;  tset = 1; }
      dest(x,y).r = 255;
      //dest(x,y).g = 0;
    }
  }
  if (tset) {
    rset = 1;
    rox = tx;
    roy = ty;
    row = 25;
    roh = 25;
    //printf(">> %d %d %d\n", rset, rox, roy);
    //Apply(src,dest,tx,ty,50,50);
  } else {
    //printf("nothing\n");
  }
  //pdest = dest;
}
