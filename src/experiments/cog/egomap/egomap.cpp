#ifndef __QNX__
using namespace std;
#endif

#include "VisMatrix.h"
#include "RobotMath.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector
#define Vector3 C3dVector


#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "conf/tx_data.h"

#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPPort.h"
#include "YARPTime.h"
#include "YARPImage.h"
#include "YARPImagePortContent.h"
#include "YARPImageDraw.h"

#include "YARPVisualSearch.h"
#include "YARPBottle.h"

#include "CogGaze.h"
#include "ImgTrack.h"

#ifndef M_PI
#define M_PI 3.1415927
#endif

#define for if(1) for

//#define USE_RECT
#define USE_FAT

enum
{
  JOINT_TORSO_ROLL = 0,
  JOINT_TORSO_PITCH = 1,
  JOINT_TORSO_YAW = 2,
};


void MarkTarget(int id);
void AppearTarget(int id);
void RevertTarget(int id);
void ResetControl();

YARPSemaphore wake_up_call(0);

YARPSemaphore state_mutex(1);
JointPos state_joint;
ArmJoints state_arm;
double global_theta = 0, global_phi = 0, global_roll = 0;

static YARPSemaphore mutex(1);


class InHead : public YARPInputPortOf<JointPos> 
{
public:
  virtual void OnRead()
    {
      assert(Read(0));
      state_mutex.Wait();
      state_joint = Content();
      state_mutex.Post();
    }
} in_head;

class InArm : public YARPInputPortOf<ArmJoints>
{
public:
  InArm()
    {
      for (int i=0; i<3; i++)
	{
	  state_arm.j[i] = 0;
	}
    }
  
  virtual void OnRead()
    {
      assert(Read(0));
      state_mutex.Wait();
      state_arm = Content();
      printf("*** torso input roll:%g pitch:%g yaw:%g\n", 
	     state_arm.j[JOINT_TORSO_ROLL], 
	     state_arm.j[JOINT_TORSO_PITCH],
	     state_arm.j[JOINT_TORSO_YAW]);
      state_mutex.Post();
    }
} in_arm;

YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;
YARPOutputPortOf<HeadMessage> out_cmd;


void RequestOrientation(double theta, double phi, double timeout=3)
{
  mutex.Wait();
  HeadMessage& msg = out_cmd.Content();
  msg.type = HeadMsgMoveToOrientation;
  msg.j1 = theta;
  msg.j2 = phi;
  msg.j3 = timeout;
  msg.j4 = theta;
  msg.j5 = phi;
  msg.j6 = 0;
  msg.j7 = 0;
  out_cmd.Write();
  mutex.Post();
}


class HisMastersVoice : public YARPInputPortOf<YARPBottle>
{
public:
  int index;
  int set_index;
  int max_index;
  int count;
  
  HisMastersVoice() : YARPInputPortOf<YARPBottle>(DOUBLE_BUFFERS)
    { index = 0; set_index=0;  max_index=1; count = 0; }
  
  void Execute(int cmd, int param = 0);
  
  virtual void OnRead()
    {
      assert(Read(0));
      YARPBottleIterator it(Content());
      int x = it.ReadVocab();
      if (x==YBC_ASSESSMENT)
	{
	  int x2 = it.ReadVocab();
	  if (x2 == YBC_POSITIVE)
	    {
	      printf("*** Worthy location %d\n", index);
	      Execute(2,index);
	      index = (index+1)%2;
	      if (index>max_index)
		{
		  max_index = index;
		}
	    }
	}
      if (x==YBC_GAZE)
	{
	  int x2 = it.ReadVocab();
	  switch (x2)
	    {
	    case YBC_GAZE_FIXATE:
		{
		  if (it.ReadInt()==0)
		    {
		      printf("*** Moving on %d\n", set_index);
		      Execute(3,set_index);
		      set_index = (set_index+1)%max_index;
		      count = 3;
		    }
		}
	      break;
	    case YBC_GAZE_FREEZE:
	      Execute(4,0);
	      break;
	    case YBC_GAZE_UNFREEZE:
	      Execute(5,0);
	      break;
	    }
	}
      if (count == 1)
	{
	  printf("*** Resetting controller\n");
	  Execute(4,0);
	}
      if (x==YBC_TICK)
	{
	  if (count>0) count--;
	}
      if (x==YBC_EGOMAP)
	{
	  int x2 = it.ReadVocab();
	  switch (x2)
	    {
	    case YBC_TARGET_SET:
		{
		  MarkTarget(it.ReadInt());
		}
	      break;
	    case YBC_TARGET_REVERT:
		{
		  RevertTarget(it.ReadInt());
		}
	      break;
	    case YBC_TARGET_APPEAR:
		{
		  AppearTarget(it.ReadInt());
		}
	      break;
	    case YBC_TARGET_RESET:
		{
		  ResetControl();
		}
	      break;
	    case YBC_TARGET_ORIENT:
		{
		  double p1 = it.ReadFloat();
		  double p2 = it.ReadFloat();
		  p1 *= M_PI*1.1;
		  p2 *= M_PI*1.1;
		  printf("DIRECT (scaled) request for %g %g\n", p1, p2);
		  RequestOrientation(p1,p2);
		}
	      break;
	    }
	}
      /*
      int x = Content();
      printf("Received command %d\n",x);
      //mode_cmd_mutex.Wait();
      //mode_cmd = x;
      //mode_cmd_time = YARPTime::GetTimeAsSeconds();
      int c = x;
      if (x>=100) c = x/100;
      Execute(c,x%100);
      //mode_cmd_mutex.Post();
       */
    }
} in_voice;


class TargetLocation
{
public:
  double theta, phi;
  
  //YARPImageOf<YarpPixelMono> proto_image;
  int px, py;
  int proto_state;
  int request;
  float proto_quality;
  YARPVisualSearch seeker;
  
  TargetLocation()
    {
      Reset();
    }
  
  void Reset()
    {
      proto_state = 0;
      proto_quality = 0;
      request = 0;
      seeker.Reset();
    }
  
  void Request(int req)
    {
      request = req;
    }
};

void RotateTemplate(YARPImageOf<YarpPixelBGR>& src, 
		    YARPImageOf<YarpPixelBGR>& dest,
		    float angle,
		    int x, int y, int scale)
{
  float cosa = cos(angle);
  float sina = sin(angle);
  float SQUEEZE = 4.0/3.0;
  float threshold = scale*scale;
  for (int xx=x-scale; xx<=x+scale; xx++)
    {
      for (int yy=y-scale; yy<=y+scale; yy++)
	{
	  float delta = (xx-x)*(xx-x)+(yy-y)*(yy-y);
	  if (delta<=threshold)
	    {
	      int xx2 = (int)(0.5+x+((SQUEEZE*(xx-x)*cosa-(yy-y)*sina)/SQUEEZE));
	      int yy2 = (int)(0.5+(y+SQUEEZE*(xx-x)*sina+(yy-y)*cosa));
	      dest.SafePixel(xx,yy) = src.SafePixel(xx2,yy2);
	    }
	}
    }
}

#define MAX_TARGETS 10

class TargetManager
{
public:
  int flag[MAX_TARGETS];
  TargetLocation location[MAX_TARGETS], null_location;
  
  TargetManager()
    {
      null_location.theta = 0;
      null_location.phi = 0;
      for (int i=0; i<MAX_TARGETS; i++)
	{
	  flag[i] = 0;
	}
    }
  
  void Set(int index, double theta, double phi, int reset = 1)
    {
      assert(index<MAX_TARGETS);
      flag[index] = 1;
      if (reset)
	{
	  location[index].Reset();
	}
      location[index].theta = theta;
      location[index].phi = phi;
    }
  
  void Appear(int index)
    {
      assert(index<MAX_TARGETS);
      location[index].Request(1);
    }
  
  int Exists(int index)
    { assert(index>=0);
      assert(index<MAX_TARGETS);
      return flag[index]; }
  
  TargetLocation& Get(int index)
    { if (Exists(index)) return location[index]; else return null_location; }
  
} target_manager;

YARPImageOf<YarpPixelMono> prev_image, current_image;

void ManageMarkerBegins(YARPImageOf<YarpPixelBGR>& src,
			YARPImageOf<YarpPixelBGR>& dest)
{
  static int first = 1;
#ifdef USE_RECT
  current_image.CastCopy(src);
  if (first)
    {
      prev_image.PeerCopy(current_image);
    }
#endif
  first = 0;
}

void ManageMarkerEnds(YARPImageOf<YarpPixelBGR>& src,
		      YARPImageOf<YarpPixelBGR>& dest)
{
#ifdef USE_RECT
  prev_image.PeerCopy(current_image);
#endif
}

void ManageMarker(YARPImageOf<YarpPixelBGR>& src, 
		  YARPImageOf<YarpPixelBGR>& dest,
		  CogGaze& gaze, TargetLocation& loc)
{
  float target_theta = loc.theta;
  float target_phi = loc.phi;
  float z_y = loc.phi/(M_PI/2);
  float z_x = loc.theta/(M_PI/2);
  float s = 50;
  float x = z_x*s;
  float y = z_y*s;

  // current x, y, z available in gaze::x_right,y_right,z_right
  float x_vis, y_vis;
  int visible;
  visible = gaze.Intersect(target_theta,target_phi,x_vis,y_vis,
			   CAMERA_SOURCE_RIGHT_WIDE);
  int sanity = visible;
  double delta_theta = x_vis;
  double delta_phi = y_vis;
  delta_theta -= 64;
  delta_phi -= 64;
  float len = sqrt(delta_theta*delta_theta+delta_phi*delta_phi);
  delta_theta += 64;
  delta_phi += 64;
  
  if (sanity>0)
    {
      YarpPixelBGR pix1((len<50)?255:0,128,0);
      AddCircle(dest,pix1,(int)(delta_theta+0.5),
		(int)(delta_phi+0.5),4);
      float xtarget = delta_theta;
      float ytarget = delta_phi;
      float xfat = xtarget/128.0*640;
      float yfat = ytarget/128.0*240;
      if (xtarget>=0 && xtarget<src.GetWidth() &&
	  ytarget>=0 && ytarget<src.GetHeight())
	{
	  if (loc.proto_state == 1)
	    {
	      loc.proto_state = 2;
#ifdef USE_RECT
	      ImgTrackTool verifier;
	      int tx = (int)(xtarget+0.5);
	      int ty = (int)(ytarget+0.5);
	      verifier.Apply(loc.proto_image,current_image,tx,ty);
	      loc.proto_quality = verifier.GetQuality();
	      printf("Capturing quality... %g\n", loc.proto_quality);
#endif
	    }
	  if (loc.proto_state == 0)
	    {
	      loc.proto_state = 1;
#ifdef USE_RECT
	      loc.proto_image.PeerCopy(current_image);
	      printf("Capturing template...\n"); 
	      // really should make sure we are comfortably within
	      // trackable zone
#endif

	      loc.seeker.Add(src,xfat,yfat,35,15);
	      float power = loc.seeker.Apply(src,dest,xfat,yfat);
	      printf("Capturing color hist sample... power %g\n", power); 
	    }
	  
	  if (loc.request == 1)
	    {
	      float power = loc.seeker.Apply(src,dest,xfat,yfat);
	      printf("Power is %g\n", power); 
	    }

#ifdef USE_RECT
	    {
	  ImgTrackTool tracker;
	  int tx = (int)(xtarget+0.5);
	  int ty = (int)(ytarget+0.5);
	  tracker.Apply(prev_image,current_image,tx,ty);
	  int nx = tracker.GetX();
	  int ny = tracker.GetY();
	  if (tx!=nx || ty!=ny)
	    {
	      AddCircleOutline(dest,pix1,(int)(nx+0.5),
			       (int)(ny+0.5),10);
	    }
	    }
#endif

	  int tx = (int)(xtarget+0.5);
	  int ty = (int)(ytarget+0.5);
	  if (loc.proto_state)
	    {
#ifdef USE_RECT
	      ImgTrackTool verifier;
	      verifier.Apply(loc.proto_image,current_image,tx,ty);
	      int nx = verifier.GetX();
	      int ny = verifier.GetY();
	      float quality = verifier.GetQuality();
	      float desired_quality = loc.proto_quality;
	      //printf("Quality is %g; want about %g\n", quality,
	      //desired_quality);
	      int ok = (quality<10000);
#else
	      int nx = tx;
	      int ny = ty;
	      int ok = 1;
	      float power = loc.seeker.Apply(src,dest,xfat,yfat);
	      if (power<0.7)
		{
		  ok = 0;
		}
#endif
	      YarpPixelBGR pix2(0,128,0);
	      YarpPixelBGR pix3(255,255,255);
	      AddCircleOutline(dest,pix3,(int)(nx+0.5),
			   (int)(ny+0.5),4+7*ok);
	      AddCrossHair(dest,pix2,(int)(nx+0.5),
			   (int)(ny+0.5),3+7*ok);
	      AddCircleOutline(dest,pix2,(int)(nx+0.5),
			   (int)(ny+0.5),3+7*ok);
	    }
	}
    }
  else
    {
      //printf("Object occluded\n");
    }
  loc.request = 0;
}

int main_alt()
{
  in_head.Register("/egomap/i:head");
  in_arm.Register("/egomap/i:arm");
  in_img.Register("/egomap/i:img");
  out_img.Register("/egomap/o:img");
  out_cmd.Register("/egomap/o:cmd");
  in_voice.Register("/egomap/i:bot");

  while (1)
    {
      JointPos joints;
      ArmJoints body_joints;
      in_img.Read();
      state_mutex.Wait();
      joints = state_joint;
      body_joints = state_arm;
      CogGaze gaze;
      gaze.Apply(joints,body_joints);
      double roll = gaze.roll_right;
      double theta = gaze.theta_right;
      double phi = gaze.phi_right;
//      printf("DIR %g %g %g\n", theta, phi, roll);

      static int is_out = 0;
      static double out_time = -10000;
      double now = YARPTime::GetTimeAsSeconds();
      if (theta>1.0 || theta<-1.0 || phi<-0.4*M_PI/2)
	{
	  if (!is_out)
	    {
	      out_time = now;
	      is_out = 1;
	    }
	  else if (now-out_time>2)
	    {
	      double target_theta = theta/2;
	      double target_phi = phi/2;
	      RequestOrientation(target_theta,target_phi,2);
	      is_out = 0;
	    }
	  //printf("OUT of bounds of decency %g\n", now-out_time);
	}
      else
	{
	  is_out = 0;
	}
      
      static int is_out2 = 0;
      static double out_time2 = -10000;
      if (theta>0.5 || theta<-0.5 || phi<-0.05*M_PI/2)
	{
	  if (!is_out2)
	    {
	      out_time2 = now;
	      is_out2 = 1;
	    }
	  else if (now-out_time2>10)
	    {
	      double target_theta = theta/4;
	      double target_phi = phi/4;
	      if (target_phi<0) target_phi = -target_phi;
	      RequestOrientation(target_theta,target_phi,2);
	      is_out2 = 0;
	    }
	  //printf("OUT of bounds of decency %g\n", now-out_time);
	}
      else
	{
	  is_out2 = 0;
	}
      
      
      
      global_theta = theta;
      global_phi = phi;
      global_roll = roll;
      state_mutex.Post();
      double z_x = gaze.z_right[0];
      double z_y = gaze.z_right[1];
      YARPImageOf<YarpPixelBGR> img, img_src;
      
#ifndef USE_FAT
      out_img.Content().PeerCopy(in_img.Content());
#else
      out_img.Content().ScaledCopy(in_img.Content(),128,128);
#endif
      
      img.Refer(out_img.Content());
      img_src.Refer(in_img.Content());
      int width = img.GetWidth();
      int height = img.GetHeight();
      float s = 50;
      int rotated = 0;
#ifndef USE_FAT
      if (phi<M_PI*0.35)
	{
	  RotateTemplate(img_src,img,roll,64,64,s);
	  rotated = 1;
	}
#endif
      for (int i=0; i<width; i++)
	{
	  YarpPixelBGR pix0(0,255,0);
	  img(i,width/2) = pix0;
	}
      for (int i=0; i<width; i++)
	{
	  float s2 = (i-width/2.0);
	  float x = cos(roll)*s2;
	  float y = -sin(roll)*s2;
	  YarpPixelBGR pix(255,0,0);
	  img.SafePixel((int)(0.5+x+(width+1)/2.0),(int)(0.5+y+(width+1)/2.0)) = pix;	
	}
      int step = 500;
      for (int i=0; i<step; i++)
	{
	  float theta = i*M_PI*2.0/step;
	  YarpPixelBGR pix(rotated?255:0,0,0);
	  float x = cos(theta)*s;
	  float y = sin(theta)*s;
	  //printf("%g %g %g\n", theta, x, y);
	  img.SafePixel(x+width/2,y+width/2) = pix;	
	}
      ManageMarkerBegins(img_src,img);
      for (int i=0; i<MAX_TARGETS; i++)
	{
	  if (target_manager.Exists(i))
	    {
	      TargetLocation& loc = target_manager.Get(i);
	      ManageMarker(img_src,img,gaze,loc);
	    }
	}
      ManageMarkerEnds(img_src,img);
      z_y = phi/(M_PI/2);
      z_x = theta/(M_PI/2);
      if (0)
      for (int i=0; i<5; i++)
	{
	  float x = z_x*s;
	  float y = z_y*s;
	  YarpPixelBGR pix(255,0,0);
	  YarpPixelBGR pix2(0,0,255);
	  img.SafePixel(i+x+width/2,y+width/2) = pix;	
	  img.SafePixel(-i+x+width/2,y+width/2) = pix;	
	  img.SafePixel(i+x+width/2,y+width/2-1) = pix2;	
	  img.SafePixel(-i+x+width/2,y+width/2+1) = pix2;	
	  img.SafePixel(x+width/2,i+y+width/2) = pix;	
	  img.SafePixel(x+width/2,-i+y+width/2) = pix;
	  img.SafePixel(x+width/2+1,i+y+width/2) = pix2;	
	  img.SafePixel(x+width/2-1,-i+y+width/2) = pix2;	
	}
//      gaze.DeIntersect(float x, float y, float& theta, float& phi,//
//		       int source = CAMERA_SOURCE_RIGHT_WIDE,
//		       int mode = CAMERA_MODE_SQUASHED_128);
      out_img.Write();
    }

  return 0;
}


void ResetControl()
{
  mutex.Wait();
  HeadMessage& msg = out_cmd.Content();
  msg.type = HeadMsgNonValid;
  out_cmd.Write();
  mutex.Post();
}

void FreezeControl()
{
  RequestOrientation(global_theta,global_phi);
}

void MarkTarget(int id)
{
  printf("Marking current region as target %d\n", id);
  state_mutex.Wait();
  target_manager.Set(id,global_theta,global_phi);
  state_mutex.Post();
}

void AppearTarget(int id)
{
    printf("Requesting appearance-based operation for target %d\n", id);
    state_mutex.Wait();
    target_manager.Appear(id);
    state_mutex.Post();
}

void RevertTarget(int id)
{
  printf("Returning to target %d\n", id);
  state_mutex.Wait();
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  int exists = 0;
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  TargetLocation& location = target_manager.Get(id);
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  exists = target_manager.Exists(id);
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  state_mutex.Post();
  //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
  if (!exists)
    {
      printf("--> No such target\n");
    }
  else
    {
      double theta = location.theta;
      double phi = location.phi;
      printf("--> Should go to %g %g\n", theta, phi);
      //printf("__LINE__ %d\n", __LINE__);  fflush(stdout);
      RequestOrientation(theta,phi);
    }
}

void HisMastersVoice::Execute(int cmd, int param)
{
  switch(cmd)
    {
    case 2:
      MarkTarget(param);
      break;
    case 3:
      RevertTarget(param);
      break;
    case 4:
      FreezeControl();
      break;
    case 5:
      ResetControl();
      break;
    }
}

class MainThread : public YARPThread
{
public:
  virtual void Body()
    {
      main_alt();
    }
} main_thread;

void main()
{
  main_thread.Body();
  
  // LOOP NO LONGER EXECUTES
  while (1)
    {
      char chs[200] = "";
      float p1 = 0, p2 = 0;
      int x = 0;
      scanf("%s", &chs);
      char ch = chs[0];
      scanf("%f", &p1);
      x = (int)(p1+0.5);
      if (ch!='\0')
	{
	  if (ch == 't')
	    {
	      MarkTarget(x);
	    }
	  else if (ch == 'r')
	    {
	      RevertTarget(x);
	    }
	  else if (ch == 'a')
	    {
	      AppearTarget(x);
	    }
	  else if (ch == 'z')
	    {
	      ResetControl();
	    }
	  else if (ch == 'd')
	    {
	      scanf("%f", &p2);
	      p1 *= M_PI*1.1;
	      p2 *= M_PI*1.1;
	      printf("DIRECT (scaled) request for %g %g\n", p1, p2);
	      RequestOrientation(p1,p2);
	    }
	}
      else
	{
	  printf("Tick...\n");
	  YARPTime::DelayInSeconds(1.0);
	}
    }
}
