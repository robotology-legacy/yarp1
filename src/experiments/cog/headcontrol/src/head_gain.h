//
// head_gain.h
//

#ifndef __head_gainh__
#define __head_gainh__

const double GAIN_vor_yaw = -600.0;
const double GAIN_vor_pitch = -500.0;
const double GAIN_stabilize_roll = 1700.0;
//const double GAIN_vor_yaw = -1200.0;
//const double GAIN_vor_pitch = -1000.0;
//const double GAIN_stabilize_roll = 1700.0;

const double GAIN_vergence = 200.0;
const double GAIN_eyes_pan = 1500.0;
const double GAIN_eyes_tilt = 400.0;

const double GAIN_neck_pan = 0.8;
const double GAIN_neck_tilt = 2.0;
const double GAIN_head_tilt = 0.3;

const double GAIN_saccade_pan = 2500.0;
const double GAIN_saccade_tilt = 200.0;

#endif
