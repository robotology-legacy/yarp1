#include "CogGaze.h"

#include "VisMatrix.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector

#ifndef M_PI
#define M_PI 3.1415927
#endif

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
  zz = xdash[2];
  roll = asin(zz);
}

void CogGaze::Apply(JointPos& joints)
{
  HeadState state;
  JointPos& datum = joints;
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
  
  double lean_ref = -10000;
  double tilt_ref = -14000;
  
#if 0
  printf("gyro %g %g %g || eye %g %g %g || neck %g %g %g %g || ",
	 state.yaw/M_PI*180, state.pitch/M_PI*180, state.roll/M_PI*180,
	 state.tilt, state.left_pan, state.right_pan,
	 state.neck_lean, state.neck_pan, state.neck_tilt,
	 state.neck_roll);
  //#else
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
  
  
  /*
   // A rotation about the y axis, for neck_pan
   double neck_pan_angle = -state.neck_pan/neck_pan_ticks_per_rad;
   c = cos(neck_pan_angle);
   s = sin(neck_pan_angle);
   m[0][0] = c;  m[0][1] = 0;  m[0][2] = s;  m[0][3] = 0;
   m[1][0] = 0;  m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
   m[2][0] =-s;  m[2][1] = 0;  m[2][2] = c;  m[2][3] = 0;
   m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
   Matrix m_neck_pan = m;
   */
  
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
  
  /*
  // Rotation corresponding to yaw, pitch, and roll
  double y = state.yaw, p = state.pitch, r = state.roll;
  double cosp = cos(p), sinp = sin(p);
  double cosy = cos(y), siny = sin(y);
  double cosr = cos(r), sinr = sin(r);
  m[0][0] = cosp*cosy;  m[0][1] = sinr*sinp*cosy-cosr*siny; 
  m[0][2] = cosr*sinp*cosy+sinr*siny;  m[0][3] = 0;
  m[1][0] = cosp*siny;  m[1][1] = sinr*sinp*siny+cosr*cosy;  
  m[1][2] = cosr*sinp*siny-sinr*cosy;  m[1][3] = 0;
  m[2][0] = -sinp;  m[2][1] = cosp*sinr;  
  m[2][2] = cosp*cosr;  m[2][3] = 0;
  m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
  Matrix m_euler = m;
  
  Matrix m_left = m_euler * m_tilt * m_left_pan;
  Matrix m_right = m_euler * m_tilt * m_right_pan;
   */
  
  // Consider a vector pointing out from camera
  Matrix m_common = m_neck_lean * m_neck_pan * m_neck_tilt * m_neck_roll * m_tilt;
  Matrix m_left_all = m_common * m_left_pan;
  Matrix m_right_all = m_common * m_right_pan;
  
  CogGaze& dir = *this;
  for (int i=0; i<3; i++)
    {
      dir.x_left[i] = m_left_all[i][0];
      dir.y_left[i] = m_left_all[i][1];
      dir.z_left[i] = m_left_all[i][2];
      dir.x_right[i] = m_right_all[i][0];
      dir.y_right[i] = m_right_all[i][1];
      dir.z_right[i] = m_right_all[i][2];
    }
  

  Digest(m_left_all,dir.theta_left,dir.phi_left,dir.roll_left);
  Digest(m_right_all,dir.theta_right,dir.phi_right,dir.roll_right);
	  
  printf("LEFT %+8.2f %+8.2f %+8.2f      RIGHT %+8.2f %+8.2f %+8.2f\n", 
	 dir.theta_left*180.0/M_PI, 
	 dir.phi_left*180.0/M_PI,
	 dir.roll_left*180.0/M_PI,
	 dir.theta_right*180.0/M_PI, 
	 dir.phi_right*180.0/M_PI,
	 dir.roll_right*180.0/M_PI);
}

