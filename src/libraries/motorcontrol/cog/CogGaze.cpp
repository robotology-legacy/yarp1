#ifndef __QNX__
using namespace std;
#endif
#include "VisMatrix.h"
#include "RobotMath.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector
#define Vector3 C3dVector

#include "CogGaze.h"


#ifndef M_PI
#define M_PI 3.1415927
#endif

#define for if(1) for

#define ANGLE_TO_PIXEL 67
#define X_CENTER_PIXEL 64
#define Y_CENTER_PIXEL 64

enum
{
    JOINT_TORSO_ROLL = 0,
    JOINT_TORSO_PITCH = 1,
    JOINT_TORSO_YAW = 2,
};

class HeadState
{
public:
  double yaw, pitch, roll;
  double tilt, left_pan, right_pan;
  double neck_tilt, neck_roll, neck_lean, neck_pan;

  void SetEyes(double n_tilt, double n_left_pan, double n_right_pan)
    { 
      tilt = n_tilt;  left_pan = n_left_pan;
      right_pan = n_right_pan;
    }

  void SetNeck(double n_tilt, double n_roll,
	       double n_lean, double n_pan)
    {
      neck_tilt = n_tilt;
      neck_roll = n_roll;
      neck_lean = n_lean;
      neck_pan = n_pan;
    }
};

static void Digest(const Matrix& m, float& theta, float& phi, float& roll)
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
  //zz = xdash[2];
  //roll = asin(zz);
  
  // To compute roll: find vector perpendicular to gravity and
  // to the transformed z axis
  Vector3 x3;
  for (int i=0; i<3; i++) { x3[i] = xdash[i]; }
  Vector3 y3;
  y3[0] = 0;  y3[1] = 1;  y3[2] = 0;
  Vector3 z3_org;
  z3_org[0] = 0;  z3_org[1] = 0;  z3_org[2] = 1;
  Vector3 z3;
  for (int i=0; i<3; i++) { z3[i] = zdash[i]; }
  Vector3 target = z3_org.Cross(z3);
  double norm = target.norm2();
//  printf("%g\n", norm);
  if (fabs(norm)<0.00001) norm = 1;
  double cosroll = target * x3 / norm;
  if (cosroll<0) 
    {
      target = target * (-1);
      cosroll = -cosroll;
    }
  Vector3 ref_dir = x3.Cross(target);
  // x3 and target should be rotations around z3
  // so can use z3 to "referee" which direction ref_dir is heading in
  double eval = ref_dir * z3;
  //double norm2 = target2.norm2();
  roll = acos(cosroll);
  if (eval<0) roll = -roll;
//  if (roll>M_PI/2) roll = roll;
//  if (roll<-M_PI/2) roll = -(M_PI+roll);
  // what about the sign of the roll?
  
}

void CogGaze::Apply(JointPos& joints)
{
  ArmJoints null_body;
  for (int i=0; i<3; i++)
    {
      null_body.j[i] = 0;
    }
  Apply(joints,null_body);
}

void CogGaze::Apply(JointPos& head_joints, ArmJoints& body_joints)
{
  HeadState state;
  JointPos& datum = head_joints;
  state.SetEyes(datum(1), datum(2), datum(3));
  state.SetNeck(datum(4), datum(5), datum(7), datum(6));

  // gyro reported orientation applies to origin point p_origin
  // right between the eyes, fixed wrt head.
  
  double tilt_ticks_per_right_angle = 24371;
  double left_pan_ticks_per_right_angle = 42791;
  double right_pan_ticks_per_right_angle = 44238;
  double neck_lean_ticks_per_right_angle = 25657;
  double neck_pan_ticks_per_right_angle = 49549;
  double neck_tilt_ticks_per_right_angle = 36335;
  double neck_roll_ticks_per_right_angle = 38025;
  
  double rads_per_right_angle = M_PI/2;
  double tilt_ticks_per_rad = tilt_ticks_per_right_angle/rads_per_right_angle;
  double left_pan_ticks_per_rad = left_pan_ticks_per_right_angle/rads_per_right_angle;
  double right_pan_ticks_per_rad = right_pan_ticks_per_right_angle/rads_per_right_angle;
  double neck_lean_ticks_per_rad = neck_lean_ticks_per_right_angle/rads_per_right_angle;
  double neck_pan_ticks_per_rad = neck_pan_ticks_per_right_angle/rads_per_right_angle;
  double neck_roll_ticks_per_rad = neck_roll_ticks_per_right_angle/rads_per_right_angle;
  double neck_tilt_ticks_per_rad = neck_tilt_ticks_per_right_angle/rads_per_right_angle;
  
  double deg = 180.0/M_PI;
  
  double lean_ref = -10000 + 13582;
  double tilt_ref = -14000 + 17361;
  
#if 0
  printf("gyro %g %g %g || eye %g %g %g || neck %g %g %g %g || ",
	 state.yaw/M_PI*180, state.pitch/M_PI*180, state.roll/M_PI*180,
	 state.tilt, state.left_pan, state.right_pan,
	 state.neck_lean, state.neck_pan, state.neck_tilt,
	 state.neck_roll);
#endif
  
#if 0
  printf("gyro %.1f %.1f %.1f || eye %.1f %.1f %.1f || neck %.1f %.1f %.1f %.1f || ",
	 state.yaw/M_PI*180, state.pitch/M_PI*180, state.roll/M_PI*180,
	 state.tilt*deg/tilt_ticks_per_rad, 
	 state.left_pan*deg/left_pan_ticks_per_rad, 
	 state.right_pan*deg/right_pan_ticks_per_rad,
	 (state.neck_lean-lean_ref)*deg/neck_lean_ticks_per_rad, 
	 state.neck_pan*deg/neck_pan_ticks_per_rad, 
	 (state.neck_tilt-tilt_ref)*deg/neck_tilt_ticks_per_rad,
	 state.neck_roll*deg/neck_roll_ticks_per_rad);
#endif	  

  double torso_roll_ticks = body_joints.j[JOINT_TORSO_ROLL];
  double torso_pitch_ticks = body_joints.j[JOINT_TORSO_PITCH];
  double torso_yaw_ticks = body_joints.j[JOINT_TORSO_YAW];
  
  double torso_roll_ticks_per_right_angle = 10044;
  double torso_pitch_ticks_per_right_angle = 1;
  double torso_yaw_ticks_per_right_angle = 10044;
  
  double torso_roll_ticks_per_rad = 
    torso_roll_ticks_per_right_angle/rads_per_right_angle;
  double torso_pitch_ticks_per_rad = 
    torso_pitch_ticks_per_right_angle/rads_per_right_angle;
  double torso_yaw_ticks_per_rad = 
    torso_yaw_ticks_per_right_angle/rads_per_right_angle;

  double torso_roll_angle = torso_roll_ticks/torso_roll_ticks_per_rad;
  double torso_pitch_angle = torso_pitch_ticks/torso_pitch_ticks_per_rad;
  double torso_yaw_angle = torso_yaw_ticks/torso_yaw_ticks_per_rad;

  Matrix m(4,4);
  // using homogeneous matrices; don't actually need to
  // since we just care about rotations in this case
  
  // A rotation about the y axis, for panning (left)
  double left_pan_angle = state.left_pan / left_pan_ticks_per_rad;
  double c = cos(left_pan_angle);
  double s = sin(left_pan_angle);
  m[0][0] = c;  m[0][1] = 0;  m[0][2] = s;  m[0][3] = 0; 
  m[1][0] = 0;  m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] =-s;  m[2][1] = 0;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_left_pan = m;
  
  // A rotation about the y axis, for panning (right)
  double right_pan_angle = state.right_pan/right_pan_ticks_per_rad;
  c = cos(right_pan_angle);
  s = sin(right_pan_angle);
  m[0][0] = c;  m[0][1] = 0;  m[0][2] = s;  m[0][3] = 0;
  m[1][0] = 0;  m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] =-s;  m[2][1] = 0;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_right_pan = m;
  
  // A rotation about the y axis, for panning (intermediate)
  double middle_pan_angle = (right_pan_angle+left_pan_angle)/2;
  c = cos(middle_pan_angle);
  s = sin(middle_pan_angle);
  m[0][0] = c;  m[0][1] = 0;  m[0][2] = s;  m[0][3] = 0;
  m[1][0] = 0;  m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] =-s;  m[2][1] = 0;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_middle_pan = m;
  
  // A rotation about the x axis, for tilting
  double tilt_angle = state.tilt/tilt_ticks_per_rad;
  c = cos(tilt_angle);
  s = sin(tilt_angle);
  m[0][0] = 1;  m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = 0;  m[1][1] = c;  m[1][2] =-s;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = s;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_tilt = m;
  
  // A rotation about the z axis, for neck_roll
  double roll_angle = -state.neck_roll/neck_roll_ticks_per_rad;
  c = cos(roll_angle);
  s = sin(roll_angle);
  m[0][0] = c;  m[0][1] =-s;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = s;  m[1][1] = c;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_neck_roll = m;
  
  // A rotation about the x axis, for neck_tilt
  // This axis is the only one whose "resting" position is very 
  // far from neutral -- need to check with pasa where calibration
  // puts the zero point, assuming for now that it is orthogonal
  // to gravity vector
  double neck_tilt_angle_offset = M_PI/2;
  double neck_tilt_angle = -(state.neck_tilt-tilt_ref)/neck_tilt_ticks_per_rad;
  neck_tilt_angle += neck_tilt_angle_offset;
  c = cos(neck_tilt_angle);
  s = sin(neck_tilt_angle);
  m[0][0] = 1;  m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = 0;  m[1][1] = c;  m[1][2] =-s;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = s;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_neck_tilt = m;
  
  
  // A rotation about the z axis, for neck_pan (actually a roll)
  double neck_pan_angle = -state.neck_pan/neck_pan_ticks_per_rad;
  c = cos(neck_pan_angle);
  s = sin(neck_pan_angle);
  m[0][0] = c;  m[0][1] =-s;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = s;  m[1][1] = c;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_neck_pan = m;
  
  // A rotation about the x axis, for neck_lean
  // neck_lean zero point -10000
  double neck_lean_angle = (state.neck_lean-lean_ref)/neck_lean_ticks_per_rad;
  c = cos(neck_lean_angle);
  s = sin(neck_lean_angle);
  m[0][0] = 1;  m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = 0;  m[1][1] = c;  m[1][2] =-s;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = s;  m[2][2] = c;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_neck_lean = m;

  // A rotation about the z axis, for torso yaw (a roll in our coords)
  c = cos(torso_yaw_angle);
  s = sin(torso_yaw_angle);
  m[0][0] = c;  m[0][1] =-s;  m[0][2] = 0;  m[0][3] = 0;
  m[1][0] = s;  m[1][1] = c;  m[1][2] = 0;  m[1][3] = 0;
  m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_torso_yaw = m;

  // Consider a vector pointing out from camera
  Matrix m_common = m_torso_yaw * m_neck_lean * m_neck_pan * m_neck_tilt * m_neck_roll * m_tilt;
  Matrix m_left_all = m_common * m_left_pan;
  Matrix m_right_all = m_common * m_right_pan;
  Matrix m_middle_all = m_common * m_middle_pan;
  
  CogGaze& dir = *this;
  for (int i=0; i<3; i++)
    {
      dir.x_left[i] = m_left_all[i][0];
      dir.y_left[i] = m_left_all[i][1];
      dir.z_left[i] = m_left_all[i][2];
      dir.x_right[i] = m_right_all[i][0];
      dir.y_right[i] = m_right_all[i][1];
      dir.z_right[i] = m_right_all[i][2];
      dir.x_middle[i] = m_middle_all[i][0];
      dir.y_middle[i] = m_middle_all[i][1];
      dir.z_middle[i] = m_middle_all[i][2];
    }
  
  Digest(m_left_all,dir.theta_left,dir.phi_left,dir.roll_left);
  Digest(m_right_all,dir.theta_right,dir.phi_right,dir.roll_right);
  Digest(m_middle_all,dir.theta_middle,dir.phi_middle,dir.roll_middle);
}

int CogGaze::GetRay(int pov, float& theta, float& phi, float& roll)
{
  int result = 0;
  switch (pov)
    {
    case POV_LEFT:
      theta = theta_left;
      phi = phi_left;
      roll = roll_left;
      result = 1;
      break;
    case POV_RIGHT:
      theta = theta_right;
      phi = phi_right;
      roll = roll_right;
      result = 1;
      break;
    case POV_MIDDLE:
      theta = theta_middle;
      phi = phi_middle;
      roll = roll_middle;
      result = 1;
      break;
    }
  return result;
}

int CogGaze::GetAxes(int pov, float *x, float *y, float *z)
{
  int result = 0;
  int i;
  switch (pov)
    {
    case POV_LEFT:
      for (i=0; i<3; i++)
	{
	  x[i] = x_left[i];
	  y[i] = y_left[i];
	  z[i] = z_left[i];
	}
      result = 1;
      break;
    case POV_RIGHT:
      for (i=0; i<3; i++)
	{
	  x[i] = x_right[i];
	  y[i] = y_right[i];
	  z[i] = z_right[i];
	}
      result = 1;
      break;
    case POV_MIDDLE:
      for (i=0; i<3; i++)
	{
	  x[i] = x_middle[i];
	  y[i] = y_middle[i];
	  z[i] = z_middle[i];
	}
      result = 1;
      break;
    }
  return result;
}

int CogGaze::ConvertToNormal(float theta, float phi, float& x, float& y, float& z)
{
  x = sin(theta);
  y = -cos(phi)*cos(theta);
  z = sin(phi)*cos(theta);
  return 0;
}


int CogGaze::ConvertToEuler(float x, float y, float z, float& theta, float& phi)
{
  double zz = x, yy = y, xx = z;
  theta = asin(zz);
  double costheta = cos(theta);
  phi = 0;
  if (fabs(costheta>0.00001))
    {
      phi = atan2(xx/costheta,-yy/costheta);
    }
  return 0;
}

int CogGaze::Intersect(float theta, float phi, float& x, float& y,
		       int source, int mode)
{
  float zt[3];
  //zt[0] = sin(theta);
  //zt[1] = -cos(phi)*cos(theta);
  //zt[2] = sin(phi)*cos(theta);
  ConvertToNormal(theta,phi,zt[0],zt[1],zt[2]);
  
  float theta_ref, phi_ref, roll_ref;
  float x_ref[3], y_ref[3], z_ref[3];
  
  int pov = POV_RIGHT;
  if (source==CAMERA_SOURCE_LEFT_WIDE)
    {
      pov = POV_LEFT;
    }
  else if (source==CAMERA_SOURCE_MIDDLE)
    {
      pov = POV_MIDDLE;
    }
  GetRay(pov,theta_ref,phi_ref,roll_ref);
  GetAxes(pov,x_ref,y_ref,z_ref);
  
  float delta_theta = zt[0]*x_ref[0] +
    zt[1]*x_ref[1] + zt[2]*x_ref[2];
  float delta_phi = zt[0]*y_ref[0] +
    zt[1]*y_ref[1] + zt[2]*y_ref[2];
  float sanity = zt[0]*z_ref[0] +
    zt[1]*z_ref[1] + zt[2]*z_ref[2];
    
  delta_theta *= ANGLE_TO_PIXEL;
  delta_phi *= ANGLE_TO_PIXEL;
  if (mode == CAMERA_MODE_SQUASHED_128)
    {
      delta_phi *= 4.0/3.0;
      delta_theta += X_CENTER_PIXEL;
      delta_phi += Y_CENTER_PIXEL;
    }
  else if (mode == CAMERA_MODE_240)
    {
//      delta_theta *= FAT_PIXEL_BOOST;
//      delta_phi *= FAT_PIXEL_BOOST;
      delta_theta += 320;
      delta_phi += 120;
    }

  int valid = 0;
  if (sanity>0)
    {
      valid = 1;
    }
  else
    {
      //printf("Object occluded for sure\n");
      valid = 0;
    }
  x = delta_theta;
  y = delta_phi;
  return valid;
}


void CogGaze::DeIntersect(float x, float y, float& theta, float& phi,
			  int source, int mode)
{
  float xx, yy, zz;
  DeIntersect(x,y,xx,yy,zz);
  ConvertToEuler(xx,yy,zz,theta,phi);
}


void CogGaze::DeIntersect(float x, float y, float& xx, float& yy, float& zz,
			  int source, int mode)
{
  float theta_ref, phi_ref, roll_ref;
  float x_ref[3], y_ref[3], z_ref[3];
  
  int pov = POV_RIGHT;
  if (source==CAMERA_SOURCE_LEFT_WIDE)
    {
      pov = POV_LEFT;
    }
  else if (source==CAMERA_SOURCE_MIDDLE)
    {
      pov = POV_MIDDLE;
    }
  GetRay(pov,theta_ref,phi_ref,roll_ref);
  GetAxes(pov,x_ref,y_ref,z_ref);
  
//  xx = x;
//  yy = y;
  if (mode == CAMERA_MODE_SQUASHED_128)
    {
      x -= X_CENTER_PIXEL;
      y -= Y_CENTER_PIXEL;
      y /= 4.0/3.0;
    }
  else if (mode == CAMERA_MODE_240)
    {
      x -= 320;
      y -= 120;
    }
  x /= ANGLE_TO_PIXEL;
  y /= ANGLE_TO_PIXEL;

  float z = 1-x*x-y*y;
  if (z<0) z = 0;
  z = sqrt(z);
  
  xx = x*x_ref[0] + y*y_ref[0] + z*z_ref[0];
  yy = x*x_ref[1] + y*y_ref[1] + z*z_ref[1];
  zz = x*x_ref[2] + y*y_ref[2] + z*z_ref[2];
}
