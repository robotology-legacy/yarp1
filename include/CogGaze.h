#ifndef COGGAZE_INC
#define COGGAZE_INC

#include "conf/tx_data.h"

enum
{
  CAMERA_MODE_NULL,
  CAMERA_MODE_UNIT,
  CAMERA_MODE_SQUASHED_128,
  CAMERA_MODE_240, // this mode is not yet completely supported
};

enum
{
  CAMERA_SOURCE_NULL = 0,
  CAMERA_SOURCE_LEFT_WIDE = 1,
  CAMERA_SOURCE_RIGHT_WIDE = 2,
  CAMERA_SOURCE_MIDDLE = 3,
};

enum
{
  POV_NULL = 0,
  POV_LEFT = 1,
  POV_RIGHT = 2,
  POV_MIDDLE = 3,
};


class CogGaze
{
public:
  // theta is pan-like, phi is tilt-like, roll is (surprisingly) roll-like
  float theta_left, phi_left, roll_left;
  float theta_right, phi_right, roll_right;
  float theta_middle, phi_middle, roll_middle;
  float x_left[3], y_left[3], z_left[3];
  float x_right[3], y_right[3], z_right[3];
  float x_middle[3], y_middle[3], z_middle[3];

  // compute all the above variables from the current joint positions
  void Apply(JointPos& joints);
  void Apply(JointPos& head_joints, ArmJoints& body_joints);
  
  int GetRay(int pov, float& theta, float& phi, float& roll);
  
  int GetAxes(int pov, float *x, float *y, float *z);
  
  int ConvertToNormal(float theta, float phi, float& x, float& y, float& z);
  int ConvertToEuler(float x, float y, float z, float& theta, float& phi);

  // find where a ray in a given direction will intersect with a given
  // image plane (return value is true iff intersection is actually
  // possible)
  int Intersect(float theta, float phi, float& x, float& y,
		int source = CAMERA_SOURCE_RIGHT_WIDE,
		int mode = CAMERA_MODE_SQUASHED_128);
  
  void DeIntersect(float x, float y, float& theta, float& phi,
		   int source = CAMERA_SOURCE_RIGHT_WIDE,
		   int mode = CAMERA_MODE_SQUASHED_128);

  void DeIntersect(float x, float y, float& xx, float& yy, float& zz,
		   int source = CAMERA_SOURCE_RIGHT_WIDE,
		   int mode = CAMERA_MODE_SQUASHED_128);
 
};

#endif
