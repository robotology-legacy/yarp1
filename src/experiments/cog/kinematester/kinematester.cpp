#include <stdio.h>

#include "conf/tx_data.h"

#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPPort.h"
#include "YARPTime.h"

#include "VisMatrix.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector

#include "CogGaze.h"

#ifndef M_PI
#define M_PI 3.1415927
#endif

YARPSemaphore wake_up_call(0);

//YARPOutputPortOf<YARPGazeDirection> out_dir;


class HeadState
{
public:
  double yaw, pitch, roll;
  double tilt, left_pan, right_pan;
  double neck_tilt, neck_roll, neck_lean, neck_pan;
  JointPos joints;
};

class HeadStateManager
{
public:
  HeadState state;
  double last_gyro_time, last_eyes_time, last_neck_time;
  YARPSemaphore mutex;
  
  HeadStateManager() : mutex(1)
    { last_gyro_time = last_eyes_time = last_neck_time = -1000.0; }
  
  void SetGyro(double n_yaw, double n_pitch, double n_roll)
    { 
      mutex.Wait();
      state.yaw = n_yaw;  state.pitch = n_pitch;  state.roll = n_roll; 
      last_gyro_time = YARPTime::GetTimeAsSeconds();
      mutex.Post();
    }
  
  void SetEyes(double n_tilt, double n_left_pan, double n_right_pan)
    { 
      mutex.Wait();
      state.tilt = n_tilt;  state.left_pan = n_left_pan;  
      state.right_pan = n_right_pan;
      last_eyes_time = YARPTime::GetTimeAsSeconds(); 
      mutex.Post();
    }

  void SetNeck(double n_tilt, double n_roll,
	       double n_lean, double n_pan)
    {
      mutex.Wait();
      state.neck_tilt = n_tilt;
      state.neck_roll = n_roll;
      state.neck_lean = n_lean;
      state.neck_pan = n_pan;
      //last_neck_time = YARPTime::GetTimeAsSeconds(); 
      mutex.Post();
    }
  
  void SetJoints(JointPos& j)
    {
      mutex.Wait();
      state.joints = j;
      last_neck_time = YARPTime::GetTimeAsSeconds(); 
      mutex.Post();
    }

  int IsValid()
    {
      double now = YARPTime::GetTimeAsSeconds();
      mutex.Wait();
      int result = /*(now-last_gyro_time<1)&&*/(now-last_eyes_time<1)&&
	(now-last_neck_time<1);
      mutex.Post();
      return result;
    }
  
  int Get(HeadState& dest)
    {
      int result = IsValid();
      mutex.Wait();
      dest = state;
      mutex.Post();
      return result;
    }
  
} head_state_manager;


/*
class GyroReader : public YARPInputPortOf<GyroPos>
{
public:
  virtual void OnRead()
    {
      assert(Read(0));
      GyroPos& datum = Content();
//      printf("gyro %g %g %g\n", datum.y, datum.p, datum.r);
      head_state_manager.SetGyro(datum.y*M_PI/180.0, datum.p*M_PI/180.0,
				 datum.r*M_PI/180.0);
      //wake_up_call.Post();
    }
} in_gyro;
*/

class EyeReader : public YARPInputPortOf<JointPos>
{
public:
  virtual void OnRead()
    {
      assert(Read(0));
      JointPos& datum = Content();
//      printf("eye %g %g %g\n", datum(1), datum(2), datum(3));
      head_state_manager.SetEyes(datum(1), datum(2), datum(3));
//      head_state_manager.SetNeck(datum(6), datum(7), datum(4), datum(5));
      head_state_manager.SetNeck(datum(4), datum(5), datum(7), datum(6));
      head_state_manager.SetJoints(datum);
      wake_up_call.Post();
    }
} in_eye;


void Digest(const Matrix& m, float& theta, float& phi, float& roll)
{
  Vector x(4), y(4), z(4);

  x[0] = 1;  x[1] = 0;  x[2] = 0;  x[3] = 1;
  y[0] = 0;  y[1] = 1;  y[2] = 0;  y[3] = 1;
  z[0] = 0;  z[1] = 0;  z[2] = 1;  z[3] = 1;

  Vector zdash = m * z;
  double zz = zdash[0], yy = zdash[1], xx = zdash[2];
  theta = asin(zz);
  double costheta = cos(theta);
  phi = 0;
  roll = 0;
  if (fabs(costheta>0.00001))
    {
      phi = atan2(xx/costheta,-yy/costheta);
    }

  Vector xdash = m * x;
  zz = xdash[2];
  roll = asin(zz);
}

int main()
{
  //in_gyro.Register("/egomap/i:gyro");
  in_eye.Register("/kinematester/i:joint");
  YARPTime::DelayInSeconds(1);
  YARPPort::Connect("/yarp/closed_loop/p_out",
		    "/kinematester/i:joint");
  //out_dir.Register("/egomap/o:dir");

  while (1)
    {
      wake_up_call.Wait();

      HeadState state;
      int ok = head_state_manager.Get(state);
      if (ok)
	{
	  CogGaze dir;
	  dir.Apply(state.joints);
	  
	  printf("LEFT %+8.2f %+8.2f %+8.2f      RIGHT %+8.2f %+8.2f %+8.2f\n", 
		 dir.theta_left*180.0/M_PI, 
		 dir.phi_left*180.0/M_PI,
		 dir.roll_left*180.0/M_PI,
		 dir.theta_right*180.0/M_PI, 
		 dir.phi_right*180.0/M_PI,
		 dir.roll_right*180.0/M_PI);
	}
      //YARPTime::DelayInSeconds(0.05);
    }

  return 0;
}


