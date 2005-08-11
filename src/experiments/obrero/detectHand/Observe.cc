#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif

#include <yarp/YARPTime.h>
#include <yarp/YARPImageDraw.h>

#include "Observe.h"


#define MAX_TRACK 256

class Track {
public:
  int x, y, active;
  double last_support;
  int votes;
  int cruising;

  double tx, ty;
  int ct;

  Track() {
    x = 0; y = 0;
    last_support = -100000;
    active = 0;
    votes = 0;
    cruising = 0;
  }

  int isActive() {
    return active;
  }

  int isCruising() {
    return cruising;
  }

  int getFavor() {
    return votes;
  }

  void activate() {
    active = 1;
    votes = 0;
    begin();
  }

  void begin() { ct = 0; tx = ty = 0; }
  void add(int x, int y) { tx += x;  ty += y;  ct++; }
  void end(double stamp) {
    if (ct>0) {
      x = (int)(0.5+tx/ct);
      y = (int)(0.5+ty/ct);
      if (stamp-last_support<0.1) {
	if (votes<64) { votes++; }
      }
      if (stamp-last_support>0.2) {
	if (votes>0) { votes--; }
      }
      last_support = stamp;
      active = 1;
      cruising = 0;
    } else {
      cruising = 1;
      if (stamp-last_support>0.5) {
	active = 0;
      }
    }
  }

  int isConsistent(int xx, int yy) {
    float dist2 = ((float)x-xx)*((float)x-xx)+((float)y-yy)*((float)y-yy);
    int th = 20;
    if (dist2<th*th) {
      return 1;
    }
    return 0;
  }

  int getX() { return x; }
  int getY() { return y; }
};


class TrackMan {
public:
  Track tracker[MAX_TRACK];
  float quality;

  TrackMan() { quality = 0; }

  void begin() {
    for (int i=0; i<MAX_TRACK; i++) {
      if (tracker[i].isActive()) {
	tracker[i].begin();
      }
    }
  }
  
  void add(int x, int y) {
    int ok = 0;
    int lib = -1;
    for (int i=0; i<MAX_TRACK; i++) {
      if (tracker[i].isActive()) {
	if (tracker[i].isConsistent(x,y)) {
	  tracker[i].add(x,y);
	  ok = 1;
	  break;
	}
      } else {
	if (lib==-1) {
	  lib = i;
	}
      }
    }
    if (!ok && lib!=-1) {
      tracker[lib].activate();
      tracker[lib].add(x,y);
    }
  }
  
  void end() {
    double now = YARPTime::GetTimeAsSeconds();
    for (int i=0; i<MAX_TRACK; i++) {
      if (tracker[i].isActive()) {
	tracker[i].end(now);
      }
    }
  }
  
  float Apply(YARPImageOf<YarpPixelRGB>& src, 
	     YARPImageOf<YarpPixelRGB>& dest) {
    quality = 0;
    for (int i=0; i<MAX_TRACK; i++) {
      if (tracker[i].isActive()) {
	int favor = tracker[i].getFavor();
	if (favor>8) {
	  AddCrossHair(dest,YarpPixelRGB(0,255,255),
		       tracker[i].getX(),
		       tracker[i].getY(),
		       5+favor);
	  if (favor>=15) {
	    quality = 1;
	  }
	}
      }
    }
    return quality;
  }
};

  
static TrackMan man;

void Observe::beginObservation() {
  man.begin();
}

void Observe::addObservation(YARPImageOf<YarpPixelRGB>& src,
			     int x, int y, float q) {
  man.add(x,y);
}

void Observe::endObservation() {
  man.end();
}

float Observe::Apply(YARPImageOf<YarpPixelRGB>& src, 
		     YARPImageOf<YarpPixelRGB>& dest) {
  return man.Apply(src,dest);
  //AddCircle(dest,YarpPixelRGB(0,255,255),50,50,30);
}


