#ifndef COGGAZE_INC
#define COGGAZE_INC

#include "conf/tx_data.h"

class CogGaze
{
public:
  float theta_left, phi_left, roll_left;
  float theta_right, phi_right, roll_right;
  float x_left[3], y_left[3], z_left[3];
  float x_right[3], y_right[3], z_right[3];
  
  void Apply(JointPos& joints);
};

#endif
