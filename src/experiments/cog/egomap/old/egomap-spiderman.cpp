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

#include "CogGaze.h"

#include "VisMatrix.h"
#include "RobotMath.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector
#define Vector3 C3dVector

#ifndef M_PI
#define M_PI 3.1415927
#endif

#define for if(1) for

YARPSemaphore wake_up_call(0);

YARPSemaphore state_mutex(1);
JointPos state_joint;
double global_theta = 0, global_phi = 0, global_roll = 0;

class InHead : public YARPInputPortOf<JointPos> 
{
  virtual void OnRead()
    {
      assert(Read(0));
      state_mutex.Wait();
      state_joint = Content();
      state_mutex.Post();
    }
} in_head;

YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;
YARPOutputPortOf<HeadMessage> out_cmd;


class HisMastersVoice : public YARPInputPortOf<NetInt32>
{
public:
  void Execute(int cmd, int param = 0);
  
    virtual void OnRead()
    {
      assert(Read(0));
      int x = Content();
      printf("Received command %d\n",x);
      //mode_cmd_mutex.Wait();
      //mode_cmd = x;
      //mode_cmd_time = YARPTime::GetTimeAsSeconds();
      int c = x;
      if (x>=100) c = x/100;
      Execute(c,x%100);
      //mode_cmd_mutex.Post();
    }
} in_voice;


class TargetLocation
{
public:
  double theta, phi;
};

#define MAX_TARGETS 100

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
  
  void Set(int index, double theta, double phi)
    {
      assert(index<MAX_TARGETS);
      flag[index] = 1;
      location[index].theta = theta;
      location[index].phi = phi;
    }
  
  int Exists(int index)
    { return flag[index]; }
  
  TargetLocation& Get(int index)
    { if (Exists(index)) return location[index]; else return null_location; }
  
} target_manager;

void ManageMarker(YARPImageOf<PixelBGR>& src, YARPImageOf<PixelBGR>& dest,
		  CogGaze& gaze, TargetLocation& location)
{
  /*
  TargetLocation& loc = target_manager.Get(i);
  float target_theta = loc.theta;
  float target_phi = loc.phi;
  float z_y = loc.phi/(M_PI/2);
  float z_x = loc.theta/(M_PI/2);
  //printf("Drawing circle for %g %g\n", loc.theta, loc.phi);
  float x = z_x*s;
  float y = z_y*s;
  // YarpPixelBGR pix0(0,128,255);
  // AddCircle(img,pix0,(int)x+width/2,(int)y+height/2,4);
  // We now try to map back
  // onto approximate retinotopic coordinates.
  
  // current x, y, z available in gaze::x_right,y_right,z_right
  double x_vis, y_vis;
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
      AddCircle(img,pix1,(int)(delta_theta+0.5),
		(int)(delta_phi+0.5),4);
    }
  else
    {
      //printf("Object occluded\n");
    }
  */
}

int main_alt()
{
  in_head.Register("/egomap/i:head");
  in_img.Register("/egomap/i:img");
  out_img.Register("/egomap/o:img");
  out_cmd.Register("/egomap/o:cmd");
  in_voice.Register("/egomap/i:cmd");

  while (1)
    {
      JointPos joints;
      in_img.Read();
      state_mutex.Wait();
      joints = state_joint;
      CogGaze gaze;
      gaze.Apply(joints);
      double roll = gaze.roll_right;
      double theta = gaze.theta_right;
      double phi = gaze.phi_right;
      //printf("DIR %g %g %g\n", theta, phi, roll);
      global_theta = theta;
      global_phi = phi;
      global_roll = roll;
      state_mutex.Post();
      double z_x = gaze.z_right[0];
      double z_y = gaze.z_right[1];
      YARPImageOf<YarpPixelBGR> img;
      img.Refer(in_img.Content());
      int width = img.GetWidth();
      int height = img.GetHeight();
      float s = 50;
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
	  YarpPixelBGR pix(255,0,0);
	  float x = cos(theta)*s;
	  float y = sin(theta)*s;
	  //printf("%g %g %g\n", theta, x, y);
	  img.SafePixel(x+width/2,y+width/2) = pix;	
	}
      for (int i=0; i<MAX_TARGETS; i++)
	{
	  if (target_manager.Exists(i))
	    {
	      TargetLocation& loc = target_manager.Get(i);
	      float target_theta = loc.theta;
	      float target_phi = loc.phi;
	      float z_y = loc.phi/(M_PI/2);
	      float z_x = loc.theta/(M_PI/2);
	      //printf("Drawing circle for %g %g\n", loc.theta, loc.phi);
	      float x = z_x*s;
	      float y = z_y*s;
	      // YarpPixelBGR pix0(0,128,255);
	      // AddCircle(img,pix0,(int)x+width/2,(int)y+height/2,4);
	      // We now try to map back 
	      // onto approximate retinotopic coordinates.
	      
	      // current x, y, z available in gaze::x_right,y_right,z_right
	      double x_vis, y_vis;
	      int visible;
	      visible = gaze.Intersect(target_theta,target_phi,x_vis,y_vis,
				       CAMERA_SOURCE_RIGHT_WIDE);
	      
	      /*
	      float zt[3];
	      zt[0] = sin(target_theta);
	      zt[1] = -cos(target_phi)*cos(target_theta);
	      zt[2] = sin(target_phi)*cos(target_theta);
	      
	      float delta_theta = zt[0]*gaze.x_right[0] +
		 zt[1]*gaze.x_right[1] + zt[2]*gaze.x_right[2];
	      float delta_phi = zt[0]*gaze.y_right[0] +
		 zt[1]*gaze.y_right[1] + zt[2]*gaze.y_right[2];
	      float sanity = zt[0]*gaze.z_right[0] +
		 zt[1]*gaze.z_right[1] + zt[2]*gaze.z_right[2];
	      //float delta_theta = zt[0];  //target_theta - global_theta;
	      //float delta_phi = zt[1];    //target_phi - global_phi;
	      float factor_theta = 67;  // just guessed these numbers
	      float factor_phi = 67;    // so far, not linear in reality
	      float nx = delta_theta;
	      float ny = delta_phi;
	      float r = global_roll;
	      float sinr = sin(r);
	      float cosr = cos(r);
	      float fx = factor_theta;
	      float fy = factor_phi;
	      //delta_theta = nx*cosr - ny*sinr;  // just guessed the signs here
	      //delta_phi   = nx*sinr + ny*cosr;  // so far
	      //delta_theta = nx*cosr - ny*sinr;  // just guessed the signs here
	      //delta_phi   = nx*sinr + ny*cosr;  // so far
	      delta_theta *= factor_theta;
	      delta_phi *= factor_phi;
	      delta_phi *= 4.0/3.0;
	      float len = sqrt(delta_theta*delta_theta+delta_phi*delta_phi);
	      delta_theta += img.GetWidth()/2;
	      delta_phi += img.GetHeight()/2;
	       */
	      
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
		  AddCircle(img,pix1,(int)(delta_theta+0.5),
			    (int)(delta_phi+0.5),4);
		}
	      else
		{
		  //printf("Object occluded\n");
		}
	    }
	}
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
      out_img.Content().PeerCopy(in_img.Content());
      out_img.Write();
    }

  return 0;
}


static YARPSemaphore mutex(1);

void RequestOrientation(double theta, double phi)
{
  mutex.Wait();
  HeadMessage& msg = out_cmd.Content();
  msg.type = HeadMsgMoveToOrientation;
  msg.j1 = theta;
  msg.j2 = phi;
  msg.j3 = 0;
  msg.j4 = theta;
  msg.j5 = phi;
  msg.j6 = 0;
  msg.j7 = 0;
  out_cmd.Write();
  mutex.Post();
}

void ResetControl()
{
  mutex.Wait();
  HeadMessage& msg = out_cmd.Content();
  msg.type = HeadMsgNonValid;
  out_cmd.Write();
  mutex.Post();
}

void MarkTarget(int id)
{
  printf("Marking current region as target %d\n", id);
  state_mutex.Wait();
  target_manager.Set(id,global_theta,global_phi);
  state_mutex.Post();
}

void RevertTarget(int id)
{
  printf("Returning to target %d\n", id);
  state_mutex.Wait();
  TargetLocation location;
  int exists = 0;
  location = target_manager.Get(id);
  exists = target_manager.Exists(id);
  state_mutex.Post();
  if (!exists)
    {
      printf("--> No such target\n");
    }
  else
    {
      double theta = location.theta;
      double phi = location.phi;
      printf("--> Should go to %g %g\n", theta, phi);
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
      ResetControl();
      break;
    }
}

class MainThread : public YARPThread
{
  virtual void Body()
    {
      main_alt();
    }
} main_thread;

void main()
{
  main_thread.Begin();
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
