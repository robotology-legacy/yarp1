
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




#include <time.h>
#include <sys/time.h>

double getTOD() {
  timeval tv;
  gettimeofday(&tv,NULL);

  double s = tv.tv_usec/(1000*1000.0) + tv.tv_sec;
  return s;
}


double smoothTime() {
  double now = getTOD();
  static double first = now;
  double t = now-first;
  static int ct = 0;
  if (ct>0) {
    static double dt = t;
    static double pt = 0;
    double factor = 0.01;
    dt = dt*(1-factor)+(t/ct)*factor;
    ct++;
    pt = pt+dt;
    return pt;
  }
  ct++;
  return 0;
}

double getTime() { 
  //double now = YARPTime::GetTimeAsSeconds();
  double now = smoothTime();
  static double first = now;
  return now-first;
}


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
      fprintf(stderr,"Waiting for input\n");
      char buf[1000];
      cin >> buf;
      if (buf[0] == 'g') {
	int n1 = 0, n2 = 0;
	double v = 0;
	cin >> n1 >> n2 >> v;
	if (n1<MAX_PARAM&&n2<MAX_CHANNEL) {
	  mutex.Wait();
	  params[n1][n2] = v;
	  printf("Got %d %d %g\n", n1, n2, v);
	  mutex.Post();
	}
      }
    }
  }


  double iparams(int x, int y) {
    float factor = 0.05;
    float prev = prev_params[x][y];
    float diff = params[x][y]-prev;
    if (fabs(diff)<0.2) {
      factor = 0.4;
    }
    prev_params[x][y] += diff*factor;
    /*
    if (fabs(diff)<0.001) {
      prev_params[x][y] = params[x][y];
    }
    */

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
    double now = getTime();
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
    float b = iparams(5,0)/25.0; // breathe
    float breath_width = iparams(6,0)/4;

    mutex.Wait();
    ext_params->glotVol = iparams(0,0)/100;
    ext_params->glotPitch = iparams(0,1)/25;
    ext_params->aspVol = iparams(1,0)*3;
    ext_params->fricVol = iparams(2,0)*3;
    ext_params->fricPos = iparams(2,1);
    ext_params->fricCF = iparams(2,2)*100;
    ext_params->fricBW = 500+iparams(2,3)*100;
    for (int i=0; i<TOTAL_REGIONS; i++) {
      ext_params->radius[i] = iparams(3,i); // (100-iparams(3,i))/50;
    }
    ext_params->velum = iparams(4,0);
    if (b>0.1) {
      double t = now-first_now;
      double phase = (1+sin(b*t))/2.0;
      //double seq = sin(b*t-3.14159/2);
      double phase2 = phase;
      if (phase2>1) phase2 = 1;
      double vol = (phase-0.5)*2;
      if (vol<0) vol = 0;
      //if (seq<0) vol = 1;
      if (breath_width>0.1) {
	vol = pow(vol,breath_width);
      }
      params[5][1] = ext_params->glotVol * vol;
      params[5][2] = ext_params->glotPitch * phase2;
      params[5][3] = ext_params->aspVol * vol;
      params[5][4] = ext_params->fricVol * vol;
      ext_params->glotVol = iparams(5,1);
      ext_params->glotPitch = iparams(5,2);
      ext_params->aspVol = iparams(5,3);
      ext_params->fricVol = iparams(5,4);
      //printf("%g %g\n", t, ext_params->glotVol);
    }
    iparams(7,0);
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
  fprintf(stderr,"Time is %g\n", getTime());
  /*
  for (int i=0; i<10000; i++) {
    printf("Time is %g\n", getTime());
    YARPTime::DelayInSeconds(0.001);
  }
  exit(1);
  */
  voice_cmd.Begin();
}


