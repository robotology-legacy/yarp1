//
// Common data packets.
//

#ifndef __tx_datah__
#define __tx_datah__

#include "YARPBool.h"


// constants.
const int NumHeadJoints = 7;
const int NumArmJoints = 19;
const int COGHeadJoints = 7;
const int COGArmJoints = 19;


//
//
//
typedef struct _single_p_data
{
	double x, y;
	int valid;
}
SinglePosData;

typedef struct _stereo_p_data
{
	double xl, yl;
	double xr, yr;
	int valid;
}
StereoPosData;

// all this stuff is not exactly elegant C++... anyway!
class CBox2Send 
{
public:
	bool valid;

	// logpolar
	int cmax, rmax;
	int cmin, rmin;

	// cartesian
	int xmax, ymax;
	int xmin, ymin;

	// all in cartesian.
	int total_sal;
	int total_pixels;
	int xsum, ysum;
	int centroid_x;
	int centroid_y;
};

// this is used to send the most significant 
// boxes from one node to another.
typedef class _5_boxes
{
public:
	CBox2Send box1;
	CBox2Send box2;
	CBox2Send box3;
	CBox2Send box4;
	CBox2Send box5;
  
  CBox2Send& operator() (int index)
    {
      switch(index)
	{
	case 0: return box1; break;
	case 1: return box2; break;
	case 2: return box3; break;
	case 3: return box4; break;
	case 4: return box5; break;
	};
      return box1;
    }
  
  static int GetMaxBoxes()
    { return 5; }
}
FiveBoxesInARow;

// something more might be needed later.
typedef struct _disparity_data
{
	double disparity;
}
DisparityData;

// joint speed.
typedef struct _joint_speed
{
	double j1;
	double j2;
	double j3;
	double j4;
	double j5;
	double j6;
	double j7;

	inline double& operator() (int i)
	{
		switch(i)
		{
		case 1: return j1;
		case 2: return j2;
		case 3: return j3;
		case 4: return j4;
		case 5: return j5;
		case 6: return j6;
		case 7: return j7;
		}

		return j1;
	}
}
JointSpeed;
typedef JointSpeed YARPHeadJointSpeed;

typedef struct _gyro_pos
{
	double y,p,r;
}
GyroPos;

// same struct for joint position.
typedef struct _joint_speed JointPos;
typedef JointPos YARPHeadJoints;

typedef struct _simple_point
{
	int x, y;
} IntPosData;

// head message type.
enum 
{
	HeadMsgNonValid = 0,
	HeadMsgOffset = 1,
	HeadMsgStopNeckMovement = 2,
	HeadMsgStopAll = 3,
	HeadMsgStopAllVisual = 4,
	HeadMsgMoveToPosition = 5,
    HeadMsgMoveToOrientation = 6,
    HeadMsgMoveToOrientationRight = 7,
};

typedef struct _head_msg_type
{
	int type;
	double j1;
	double j2;
	double j3;
	double j4;
	double j5;
	double j6;
	double j7;

	inline double& operator() (int i)
	{
		switch(i)
		{
		case 1: return j1;
		case 2: return j2;
		case 3: return j3;
		case 4: return j4;
		case 5: return j5;
		case 6: return j6;
		case 7: return j7;
		}

		return j1;
	}
}
HeadMessage;

enum
{
	ArmMsgNonValid = 0,
	ArmMsgMoveTo = 1,
	ArmMsgWaveHand = 2,
	ArmMsgMoveToOld = 3,
	ArmMsgPush = 4,
	ArmMsgTorsoVMove = 5,
	ArmMsgTorsoPosition = 6,
	ArmMsgTorsoStop = 7,
	ArmMsgTorsoStart = 8,
	ArmMsgPushOne = 9,
	ArmMsgTorsoLink2Head = 10,
};

typedef struct _arm_msg_type
{
	int type;
	double j[COGArmJoints];
}
ArmMessage;

typedef struct _arm_joint_array
{
	double j[COGArmJoints];
}
ArmJoints;
typedef ArmJoints YARPArmJoints;

typedef struct _trackermesg
{
	bool valid;
	double aj[COGArmJoints];
	double hj[COGHeadJoints];
	int img[2];
}
TrackerMsg;

/* TO BE REMOVED: it uses YARPBottle now.
enum 
{
	ShlcNone = 0,
	ShlcStartReaching = 1,
	ShlcStopReaching = 2,
	ShlcStartPushing = 3,
	ShlcStopPushing = 4,
};

typedef int ShlcMessage;
*/

#endif
