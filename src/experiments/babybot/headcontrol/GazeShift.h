// GazeShift.h: interface for the GazeShift class.
//
//////////////////////////////////////////////////////////////////////

#define __gazeshifth__

#ifndef __gazeshifth__
#define __gazeshifth__

#include "PIDFilter.h"
#include "HeadStatus.h"
#include "NetworkData.h"
#include "VisDMatrix.h"

class GazeShift  
{
public:
	GazeShift(HeadStatus *head, PIDFilter &pan, PIDFilter &tilt, PIDFilter &neck_p, PIDFilter &neck_t){
		p_head = head;

		eye_pan_pid = pan;
		eye_tilt_pid = tilt;

		neck_pan_pid = neck_p;
		neck_tilt_pid = neck_t;

		deltaQ.Resize(_head::_nj);
		cl_deltaQ.Resize(_head::_nj);
	}

	virtual ~GazeShift(){};

	CVisDVector & handle(const TargetPosition &target){
		deltaQ = 0.0;
		double x = (target.right(1) + target.left(1)) * 0.5;
		double y = (target.right(2) + target.left(2)) * 0.5;
		
		if (target.valid)
			deltaQ = closed_loop(x,y);

		return deltaQ;
	}

private:
	inline CVisDVector &closed_loop(double x, double y){
		cl_deltaQ(right_eye) = eye_pan_pid.pid(x);
		cl_deltaQ(left_eye)	 = eye_pan_pid.pid(-x);
		cl_deltaQ(eye_tilt)	 = eye_tilt_pid.pid(y);

		// form prediction.
		double pred_r = p_head->current_position(right_eye) + (deltaQ(right_eye) + p_head->current_command(right_eye)) * p_head->deltaT;
		double pred_l = p_head->current_position(left_eye) + (deltaQ(left_eye) + p_head->current_command(left_eye)) * p_head->deltaT;
		double pred_tilt = p_head->current_position(eye_tilt) + (deltaQ(eye_tilt) + p_head->current_command(eye_tilt)) * p_head->deltaT;

		cl_deltaQ(neck_pan) = neck_pan_pid.pid(pred_r - pred_l);
		cl_deltaQ(neck_tilt) = neck_tilt_pid.pid(pred_tilt - p_head->current_position(neck_tilt));

		return cl_deltaQ;
	}

	CVisDVector deltaQ;			//resulting command
	CVisDVector cl_deltaQ;		//closed loop command
	PIDFilter   eye_pan_pid;	//eye pan
	PIDFilter	eye_tilt_pid;	//eye tilt
	PIDFilter   neck_pan_pid;	//neck pan
	PIDFilter	neck_tilt_pid;	//neck tilt
	HeadStatus *p_head;
};

#endif // !defined(AFX_GAZESHIFT_H__999B858D_2EED_4E78_94FE_92DCEEEA5CC3__INCLUDED_)
