#include <iostream.h>
#include <math.h>

#include "ImgTrack.h"
#include "YARPImage.h"
#include "YARPImagePort.h"
#include "YARPTime.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPNetworkTypes.h"
#include "YARPBottle.h"
#include "YARPBottleCodes.h"

#include "CogGaze.h"
#include "YARPImageDraw.h"

#include <conf/tx_data.h>

//#define OSC_TEST

#ifndef for
#define for if (1) for
#endif

YARPInputPortOf<YARPGenericImage> in_img;

YARPOutputPortOf<YARPGenericImage> out_img;

YARPInputPortOf<StereoPosData> in_data;
YARPOutputPortOf<StereoPosData> out_data;

enum
{
  MODE_CMD_STAY = 0,
  MODE_CMD_NEXT = 1,
  MODE_CMD_STABILIZE = 2,
};



enum
{
    JOINT_TORSO_ROLL = 0,
    JOINT_TORSO_PITCH = 1,
    JOINT_TORSO_YAW = 2,
};

YARPSemaphore wake_up_call(0);

YARPSemaphore state_mutex(1);
JointPos state_joint;
ArmJoints state_arm;
double global_theta = 0, global_phi = 0, global_roll = 0;

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
      //printf("*** torso input roll:%g pitch:%g yaw:%g\n",
      //state_arm.j[JOINT_TORSO_ROLL],
      //state_arm.j[JOINT_TORSO_PITCH],
      //state_arm.j[JOINT_TORSO_YAW]);
      state_mutex.Post();
    }
} in_arm;




double mode_cmd_time = -10000;
int mode_cmd = -1;
double mode_cmd_timeout = -10000;
YARPSemaphore mode_cmd_mutex(1);


int at_x = 64, at_y = 64, at_new = 1;
double obj_time = -10000;
double stabilize_time = -10000;
//double stabilize_time = 1e30;
YARPSemaphore tracker_mutex(1);


class HisMastersVoice : public YARPInputPortOf<YARPBottle>
{
public:
  HisMastersVoice() : YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS)
    {}
  virtual void OnRead()
    {
      assert(Read(0));
      //printf("Received command\n");
      /*
      int x = Content();
      printf("Received command %d\n",x);
      mode_cmd_mutex.Wait();
      mode_cmd = x;
      mode_cmd_time = YARPTime::GetTimeAsSeconds();
      mode_cmd_mutex.Post();
       */
      YARPBottleIterator it(Content());
      int msg = it.ReadVocab();
      if (msg == YBC_GAZE)
	{
	  msg = it.ReadVocab();
	  if (msg == YBC_GAZE_FIXATE)
	    {
	      int flag = it.ReadInt();
	      printf(">>> Received command: %s (%d)\n",it.ReadText(), flag);
	      mode_cmd_mutex.Wait();
	      mode_cmd = flag?MODE_CMD_STAY:MODE_CMD_NEXT;
	      mode_cmd_time = YARPTime::GetTimeAsSeconds();
	      mode_cmd_mutex.Post();
	    }
	}
      else if (msg == YBC_OBJECT)
	{
	  msg = it.ReadVocab();
	  if (msg == YBC_FOUND)
	    {
	      int x = it.ReadInt();
	      int y = it.ReadInt();
	      printf(">>> found object at %d %d\n", x, y);
	      tracker_mutex.Wait();
	      at_x = x;
	      at_y = y;
	      at_new = 1;
	      obj_time = YARPTime::GetTimeAsSeconds();
	      tracker_mutex.Post();
	    }
	}
      else if (msg == YBC_GAZE)
	{
	  msg = it.ReadVocab();
	  if (msg == YBC_STABILIZE)
	    {
	      double t = it.ReadFloat();
	      printf(">>> asked to stabilize with timeout %g\n", t);
	      mode_cmd_mutex.Wait();
	      mode_cmd = MODE_CMD_STAY;
	      double now = YARPTime::GetTimeAsSeconds();
	      mode_cmd_time = now;
	      mode_cmd_timeout = now+t;
	      mode_cmd_mutex.Post();
	    }
	}
      //printf("Waiting for command (%d)\n",msg);
    }
} in_bot;



#define DEFAULT_NAME  "/attn_tracker"

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

float Distance(float x1, float y1, float x2, float y2)
{
  return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void Filter(YARPImageOf<YarpPixelBGR>& src,
	    YARPImageOf<YarpPixelBGR>& dest)
{
  static int stabilizing = 0;

  static ImgTrackTool tracker;
  static ImgTrackTool sub_tracker;
  static YARPImageOf<YarpPixelMono> prev;
  static int tx = 64, ty = 64;
  static double last_update = -100000.0;
  static double last_movement = -100000.0;
  static double last_head_movement = -100000.0;

  double now = YARPTime::GetTimeAsSeconds();
  double now2 = now;
  static double last_round = now2;
  static double last_reset = 0;
  static double diff_total = 0;
  static int diff_count = 0;
  diff_total += now2-last_round;
  diff_count++;
  if (now2 - last_reset>5)
    {
      printf("Time between frames in ms is %g\n", 1000*diff_total/diff_count);
      diff_count = 0;
      diff_total = 0;
      last_reset = now2;
    }
  last_round = now2;

  JointPos joints;
  ArmJoints body_joints;

  state_mutex.Wait();
  joints = state_joint;
  body_joints = state_arm;
  state_mutex.Post();

  CogGaze gaze;
  gaze.Apply(joints,body_joints);
  static CogGaze prev_gaze = gaze;

  float theta, phi, roll;
  gaze.GetRay(POV_RIGHT,theta,phi,roll);
  static float theta_prev = theta, phi_prev = phi;
  if (now-last_head_movement>0.5)
    {
      double val = 180*(fabs(theta-theta_prev)+fabs(phi-phi_prev))/M_PI;
      if (val>3)
	{
	  printf("Head is moving\n");
	  last_head_movement = now;
	  last_movement = now;
	}
      //printf("Moved by about %g degrees\n", val);
      theta_prev = theta;
      phi_prev = phi;
    }

  /*
  printf("theta %g, phi %g\n", theta, phi);
  if (fabs(phi)<M_PI/4)
    {
      printf("over-extended in theta\n");
    }
  if (fabs(phi)>0.9*M_PI/2)
    {
      printf("over-extended in theta\n");
    }
  */

  
  float gx, gy;
  gaze.Intersect(theta,phi,gx,gy);

  float gxp, gyp;
  prev_gaze.Intersect(theta,phi,gxp,gyp);
  
  float dgx = gx-gxp;
  float dgy = gy-gyp;
  

  //  dest.PeerCopy(src);
  //  return;
  

  YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(src);
  dest.PeerCopy(src);
  
  tracker_mutex.Wait();
  int x = at_x;
  int y = at_y;
  int n = at_new;
  tracker_mutex.Post();

  static int px = tx, py = ty;
  static int movement = 1;
  static int first = 1;

  if ((now-last_head_movement<7)||((now-last_movement<1) && (now-last_update<30)))
    {
      if (now-obj_time<3)
	{
	  tracker_mutex.Wait();
	  at_new = 0;
	  tracker_mutex.Post();
	}
    }
  if (first)
    {
      at_new = 1;
    }
  first = 0;
  
  int got_new = 0;
  if (at_new && !stabilizing)
    {
      prev.PeerCopy(mono);
      px = tx = at_x;
      py = ty = at_y;
      at_new = 0;
      printf("*** Got new target %d %d\n", at_x, at_y);
      last_update = now;
      got_new = 1;
    }

  int DX = 10; int DY = 4;
  int BX = 5;  int BY = 5;
  int act_vector = 0;
  tracker.SetBlockSize(6,6);
  tracker.SetSearchWindowSize(10,10);
  sub_tracker.SetBlockSize(6,6);
  sub_tracker.SetSearchWindowSize(10,10);
  if (!got_new)
    {
      tracker.SetSearchWindowOffset((int)(dgx+0.5),(int)(dgy+0.5));
      sub_tracker.SetSearchWindowOffset((int)(dgx+0.5),(int)(dgy+0.5));
    }
  else
    {
      tracker.SetSearchWindowOffset(0,0);
      sub_tracker.SetSearchWindowOffset(0,0);
    }
  tx = px; ty = py;

  if (tx<BX+DX) tx = BX+DX;
  if (tx>127-BX-DX) tx = 127-BX-DX;
  if (ty<BX+DX) ty = BX+DX;
  if (ty>127-BX-DX) ty = 127-BX-DX;

  int fell = 0;
  float new_tx=64, new_ty=64, new_tx2=64, new_ty2=64;
  int sub_x=64, sub_y=64, sub_x2=64, sub_y2=64;
  if (!stabilizing)
    {
      int otx = tx;
      int oty = ty;
      static int flipper = 1;
      //flipper = flipper;
      tracker.Apply(prev,mono,tx,ty);
      x = tracker.GetX();
      y = tracker.GetY();
      int sgn = 1;
      float new_dx = x-(tx+sgn*dgx);
      float new_dy = y-(ty+sgn*dgy);
      float new_mag = sqrt(new_dx*new_dx+new_dy*new_dy);
      if (new_mag<0.001) new_mag = 0.001;
      new_dx /= new_mag;
      new_dy /= new_mag;
      //new_dx *= flipper;
      //new_dy *= flipper;
      if (new_mag>0.9)
	{
	  //printf(">>> %g %g %g \n", new_dx, new_dy, new_mag);
	}
      float nscale = 7;
      new_tx = tx-new_dx*nscale;
      new_ty = ty-new_dy*nscale;
      new_tx2 = tx+new_dx*nscale;
      new_ty2 = ty+new_dy*nscale;
      sub_tracker.Apply(prev,mono,new_tx2,new_ty2);
      sub_x2 = sub_tracker.GetX();
      sub_y2 = sub_tracker.GetY();
      sub_tracker.Apply(prev,mono,new_tx,new_ty);
      sub_x = sub_tracker.GetX();
      sub_y = sub_tracker.GetY();
      float sub_dx = sub_x-(new_tx+dgx*sgn);
      float sub_dy = sub_y-(new_ty+dgy*sgn);
      float sub_mag = sqrt(sub_dx*sub_dx+sub_dy*sub_dy);
      float sub_dx2 = sub_x2-(new_tx2+dgx*sgn);
      float sub_dy2 = sub_y2-(new_ty2+dgy*sgn);
      float sub_mag2 = sqrt(sub_dx2*sub_dx2+sub_dy2*sub_dy2);
      if (new_mag>2)
	{
	  act_vector = 1;
	  //printf(">>> sub_mag %g   sub_mag2 %g\n", sub_mag, sub_mag2);
	  if (sub_mag>2 && sub_mag2<2)
	    {
	      printf("Should fall inwards\n");
	      x = (int)sub_x;
	      y = (int)sub_y;
	      fell = 1;
	    }
	  if (sub_mag2>2 && sub_mag<2)
	    {
	      printf("Should fall outwards\n");
	      x = (int)sub_x2;
	      y = (int)sub_y2;
	      fell = 1;
	    }
	}
    }

  tx = x;
  ty = y;
  float quality = tracker.GetQuality();
  int low_quality = 0;
  static int low_q_ct = 0;
  //px += dgx;
  //py += dgy;
  if (quality<2)
    {
      printf("low match quality (%g)\n", quality);
      if (low_q_ct<10)
	{
	  low_q_ct++;
	}
      if (low_q_ct>3)
	{
	  low_quality = 1;
	  x = tx = px+dgx;
	  y = ty = py+dgy;
	}
    }
  else
    {
      low_q_ct -= 3;
      if (low_q_ct<0) low_q_ct = 0;
    }
  
  int DISP = 10;
  
  
  movement = 0;
  double dist = sqrt((px-tx)*(px-tx)+(py-ty)*(py-ty));
  if (fell || (dist>2) || (sqrt(dgx*dgx+dgy*dgy)>2))
//  if (px!=tx || py!=ty)
    {
      prev.PeerCopy(mono);
      px = tx; py = ty;
      prev_gaze = gaze;
    }
  if (dist>5)
    {
      //printf("***MOVE\n");
      movement = 1;
      last_movement = now;
    }

#ifdef OSC_TEST
  static double flip_time = now;
  static int flip_sign = 1;
  if (now-flip_time>2)
    {
      flip_sign *= -1;
      flip_time = now;
    }
  x = 64+(-20+40*((now-flip_time)/2))*flip_sign;
  y = 64;
  act_vector = 0;
#endif

  if (stabilize_time>now)
    {
      if (!stabilizing)
	{
	  x = y = 64;
	  stabilizing = 1;
	}
    }
  else
    {
      stabilizing = 0;
    }
  
  double an_x = x-64, an_y = y-64;
  double val = an_x*dgx + an_y*dgy;
  //printf("*** val is %g // %g %g\n", val, an_x, an_y);
  static int violation_count = 0;
  if (val>5)
    {
      if (violation_count>20)
	{
	  printf("*** resetting tracker\n");
	  x = y = tx = ty = px = py = 64;
	}
      else
	{
	  violation_count++;
	}
    }
  else
    {
      violation_count = 0;
    }


  
  YarpPixelBGR pix(255,0,0);
  
  YarpPixelBGR pixg(0,255,0);
  YarpPixelBGR pixb(0,0,255);
  YarpPixelBGR pixr(128,64,0);
  YarpPixelBGR pixk(0,0,0);
  YarpPixelBGR pixw(255,255,255);
  //AddCircleOutline(dest,pixg,(int)gx,(int)gy,6);
  //AddCircleOutline(dest,pixg,(int)gxp,(int)gyp,4);
  AddCircleOutline(dest,pixw,(int)x,(int)y,5);
  AddCircleOutline(dest,pixk,(int)x,(int)y,6);
  AddCircle(dest,pixk,(int)x,(int)y,4);
  AddCrossHair(dest,pixw,(int)x+1,(int)y,6);
  AddCrossHair(dest,pixw,(int)x-1,(int)y,6);
  AddCrossHair(dest,pixw,(int)x,(int)y+1,6);
  AddCrossHair(dest,pixw,(int)x,(int)y-1,6);
  AddCrossHair(dest,pixr,(int)x,(int)y,6);
  AddCircle(dest,pixw,(int)x,(int)y,2);
  if (stabilizing)
    {
      AddCircleOutline(dest,pixg,(int)x,(int)y,6);
      AddCircleOutline(dest,pixg,(int)x,(int)y,7);
    }

  if (act_vector)
    {
      AddCircle(dest,pix,(int)(new_tx+dgx),(int)(new_ty+dgy),3);
      AddCircle(dest,pix,(int)(new_tx2+dgx),(int)(new_ty2+dgy),3);
      AddCircle(dest,pixb,(int)sub_x,(int)sub_y,2);
      AddCircle(dest,pixb,(int)sub_x2,(int)sub_y2,2);
    }

  out_data.Content().valid = true;
  out_data.Content().xl = x;
  out_data.Content().yl = y;
  out_data.Content().xr = x;
  out_data.Content().yr = y;
  out_data.Write();
}

YARPSemaphore pos_mutex;
Box pos_last;
int pos_last_set = 0;

class BoxThread : public YARPThread
{
public:
  double last_update, last_minor_update;
  double next_time, stay_time, stabilize_time;
  
  BoxThread()
    {
      last_minor_update = last_update = -1000;
      next_time = stay_time = -1000;
    }
  
  virtual void Body()
    {
      while (1)
	{
	  static avoid_x = 64, avoid_y = 64;
	  //printf("Reading...\n");
	  in_data.Read();
	  double now = YARPTime::GetTimeAsSeconds();
	  StereoPosData& pos = in_data.Content();
	  mode_cmd_mutex.Wait();
	  int mc = mode_cmd;
	  double timeout = mode_cmd_timeout;
	  mode_cmd = -1;
	  mode_cmd_mutex.Post();
	  if (mc != -1)
	    {
	      if (mc == MODE_CMD_STAY)
		{
		  stay_time = now;
		}
	      if (mc = MODE_CMD_NEXT)
		{
		  avoid_x = at_x;
		  avoid_y = at_y;
		  next_time = now;
		  stay_time = -10000;
		}
	      if (mc = MODE_CMD_STABILIZE)
		{
		  tracker_mutex.Wait();
		  stabilize_time = timeout;
		  tracker_mutex.Post();
		}
	    }
	  if (pos.valid)
	    {
	      static prev_x = 64, prev_y = 64;
	      if (Distance(at_x,at_y,prev_x,prev_y)>3)
		{
		  last_minor_update = now;
		  last_update = now;
		}
	      prev_x = at_x;
	      prev_y = at_y;
	      if (Distance(at_x,at_y,pos.xl,pos.yl)<10)
		{
		  last_minor_update = now;
		}
	      int natural_update = (now>last_update+5 || now>last_minor_update+1);
	      int stay_in_effect = (now<stay_time+7);
	      int forced_update = 0;
	      
	      if (now<next_time-2)
		{
		  if (Distance(avoid_x,avoid_y,pos.xl,pos.yl)>20)
		    {
		      forced_update = 1;
		      next_time = -10000;
		    }
		}
	      
	      if ((natural_update && !stay_in_effect) || forced_update)
		{
		  int up = 0;
		  tracker_mutex.Wait();
		  if (now - obj_time > 5 && now - mode_cmd_time > 5)
		    {
		      prev_x = at_x = pos.xl;
		      prev_y = at_y = pos.yl;
		      at_new = 1;
		      up = 1;
		    }
		  tracker_mutex.Post();
		  if (up)
		    {
		      last_minor_update = last_update = now;
		    }
		}
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

  sprintf(buf, "%s/i:arm", name);
  in_arm.Register(buf);
  sprintf(buf, "%s/i:head", name);
  in_head.Register(buf);
  
  sprintf(buf, "%s/i:pos", name);
  in_data.Register(buf);
  sprintf(buf, "%s/o:pos", name);
  out_data.Register(buf);
  sprintf(buf, "%s/i:bot", name);
  in_bot.Register(buf);
  
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
