/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "motortester.h"
#include "abimport.h"
#include "proto.h"


/* 
 * single axis standard calibration.
 */
#define CALIBRATEERROR      500
#define CalibrationSpeed    5000
#define CalibrationAcc      50000
#define FAST_ACCEL          1000000.0

double 
VelocityMove(int axis, double vel, double acc, double errlimit)
{
        double err, pos;
        meid_error_check(meid_clear_status (meidx, axis));
        meid_error_check(meid_v_move(meidx, axis, vel, acc));

        delay (100);
        do
        {
            meid_get_error (meidx, axis, &err);
        }
        while (fabs(err) < errlimit);

        meid_error_check(meid_v_move (meidx, axis, 0.0, FAST_ACCEL));

        delay(100);

        meid_error_check(meid_get_position (meidx, axis, &pos));
        meid_error_check(meid_set_command (meidx, axis, pos));

        return pos;
}

int 
CalibrateAxis (int axis)
{
        
        double p1,p2,maxrange;
        p1 = VelocityMove (axis, CalibrationSpeed, CalibrationAcc, CALIBRATEERROR);
        p2 = VelocityMove (axis, -CalibrationSpeed, CalibrationAcc, CALIBRATEERROR);

        meid_start_move (meidx, axis, (p1+p2)/2.0, CalibrationSpeed, CalibrationAcc);
        printf ("p1: %f p2: %f\n", p1, p2);
        printf ("Moving to %f\n", (p1+p2)/2.0);
        while (!meid_axis_done(meidx, axis)) delay(20);

        meid_set_position (meidx, axis, 0.0);
       
        maxrange = (p1-p2)/2.0; 
        printf ("Max range: %f\n", maxrange);
        printf ("Limits are %f %f\n", -maxrange, maxrange);

        return 0;
}


int
calibrate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  CalibrateAxis (current_axis);

#if 0  
  int stat;
  int emin, emax;

  printf("starting calibrate\n");

  stat = meid_calibrate_axis(meidx, current_axis);

  printf("getting range\n");
  
  meid_get_axis_range(meidx, current_axis, &emin, &emax);
  
  printf("min=%d max=%d\n", emin, emax);
#endif
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
}


