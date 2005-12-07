
#include "yarpy.h"

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPThread.h>

#include <assert.h>

#include <iostream>
using namespace std;

#include <math.h>

#define MAX_CHANNEL 20
#define MAX_PARAM 20

class VoiceCmd : public YARPThread {
private:
  
  double params[MAX_CHANNEL][MAX_PARAM];
  double prev_params[MAX_CHANNEL][MAX_PARAM];
  YARPSemaphore mutex;

public:  
  VoiceCmd() : mutex(1) {
    for (int i=0; i<MAX_CHANNEL; i++) {
      for (int j=0; j<MAX_PARAM; j++) {
	params[i][j] = 0;
	prev_params[i][j] = 0;
      }
    }
    for (int j=0; j<MAX_PARAM; j++) {
      params[3][j] = 1;
    }
  }

  virtual void Body() {
    while (1) {
      printf("Waiting for input\n");
      char buf[1000];
      cin >> buf;
      if (buf[0] == 'g') {
	int n1 = 0, n2 = 0;
	double v = 0;
	cin >> n1 >> n2 >> v;
	if (n1<MAX_PARAM&&n2<MAX_CHANNEL) {
	  mutex.Wait();
	  params[n1][n2] = v;
	  mutex.Post();
	}
      }
    }
  }


  double iparams(int x, int y) {
    float factor = 0.05;
    prev_params[x][y] += (params[x][y]-prev_params[x][y])*factor;
    return prev_params[x][y];
  }

  double getParam(int x, int y) {
    return prev_params[x][y];
  }

  void setParams(TRMParameters *ext_params) {
    assert(ext_params!=NULL);
    
    /*
      0 0 double glotVol;
      0 1 double glotPitch;
      1 0 double aspVol;
      2 0 double fricVol;
      2 1 double fricPos;
      2 2 double fricCF;
      2 3 double fricBW;
      3 0-7 double radius[TOTAL_REGIONS];
      4 0 double velum;
    */
    double now = YARPTime::GetTimeAsSeconds();
    static double first_now = now;
    double mark = now - first_now;
    static double last_mark = mark;
    static double step = 0;
    static double step_ct = 0;
    static double ct = 0;
    static double ct2 = 0;
    static int upped = 0;
    ct++;
    double r = ct*0.01;
    r = 0.5;
    /*
    params[0][0] = 25*(sin(r)+1)+25; // volume
    params[0][1] = sin(r)+1.3+(sin(ct2*100)+1)*1; //pitch
    if (sin(r)<-0.7) {
      params[0][0] = 0;
      if (!upped) {
	ct2++;
	upped = 1;
      } 
    } else {
      upped = 0;
    }
    params[0][0] = 75;
    */
    /*
    if (cos(r)>0.4) {
      params[0][0] = 0;
    }
    for (int i=6; i<TOTAL_REGIONS; i++) {
      params[3][i] = max(sin(r),0.0);
    }
    */
    mutex.Wait();
    ext_params->glotVol = iparams(0,0);
    ext_params->glotPitch = iparams(0,1)/25;
    ext_params->aspVol = iparams(1,0);
    ext_params->fricVol = iparams(2,0)/10;
    ext_params->fricPos = iparams(2,1)/10;
    ext_params->fricCF = iparams(2,2)*100;
    ext_params->fricBW = 500+iparams(2,3)*100;
    for (int i=0; i<TOTAL_REGIONS; i++) {
      ext_params->radius[i] = (100-iparams(3,i))/50;
    }
    ext_params->velum = iparams(4,0)/100;
    mutex.Post();

  }
} the_voice;


VoiceCmd& getVoice() {
  /*
  static VoiceCmd *voice = NULL;
  if (voice==NULL) {
    voice = new VoiceCmd;
  }
  return *voice;
  */
  return the_voice;
}

#define voice_cmd (getVoice())


double getParam(int x, int y) {
  return voice_cmd.getParam(x,y);
}

void setParams(TRMParameters *params) {
  voice_cmd.setParams(params);
}


void yarpy() {
  printf("Time is %g\n", YARPTime::GetTimeAsSeconds());
  voice_cmd.Begin();
}


double getTime() { 
  double now = YARPTime::GetTimeAsSeconds();
  static double first = now;
  return now-first;
}
