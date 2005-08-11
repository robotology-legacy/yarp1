
#include <assert.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>

#include "EdHeadRemote.h"


static YARPSemaphore mutex(1);
static double last_read = -10000;
static double last_x = 0;
static double last_y = 0;
static int inited = 0;

class EdHeadRemoteIn : public YARPInputPortOf<YVector> {
public:
  virtual void OnRead() {
    if (Read()) {
      static int was_reading = 0;
      double now = YARPTime::GetTimeAsSeconds();
      mutex.Wait();
      int reading = (now-last_read<1);
      mutex.Post();
      // completely pointless code
      if (reading!=was_reading) {
	if (reading)
	  printf("getting input\n");
	else
	  printf("lost input\n");
	was_reading = reading;
      }
      YVector& vect = Content();
      int len = vect.Length();
      /*
      printf(">>> [%d] ", len);
      for (int i=0; i<len; i++) {
	printf("%g ", vect[i]);
      }
      printf("\n");
      */
      mutex.Wait();
      last_read = now;
      if (len==2) {
	last_x = (int)vect[0];
	last_y = (int)vect[1];
      }
      mutex.Post();
    }
  }
};

static EdHeadRemoteIn in_bot;

static YARPOutputPortOf<YARPBottle> out_bot;

EdHeadRemote::EdHeadRemote() {
}

EdHeadRemote::~EdHeadRemote() {
}

void EdHeadRemote::init() {
  if (!inited) {
    inited = 1;
    in_bot.Register("/visualTracker/head/i:vect");
    out_bot.Register("/visualTracker/head/o:bot");
  }
}

void EdHeadRemote::set(int x, int y) {
  init();
  double now = YARPTime::GetTimeAsSeconds();
  mutex.Wait();
  double read = last_read;
  mutex.Post();
  if (now-read<1) {
    YARPBottle& bot = out_bot.Content();
    bot.writeInt(6);
    bot.writeInt(1);
    int vect[2] = { x, y };
    bot.writeIntVector(vect,2);
    out_bot.Write();
    printf("Commanded %d %d\n", x, y);
  } else {
    printf("IGNORING COMMANDED MOTION - not safe, no input\n");
  }
}

void EdHeadRemote::get(int& x, int& y) {
  init();
  x = last_x;
  y = last_y;
}
