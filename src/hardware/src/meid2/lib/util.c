/*
 * util.c  - Cog's QNX/MEI API, utility/glue functions
 *
 * maddog 19980223
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "meid.h"



void meid_print_pid_filter(int meidx, int axis)
{
  int16 pid[COEFFICIENTS];
  
  meid_get_filter(meidx, axis, pid);  
  printf("PID Filter Coefficients for axis %d\n", axis);
  printf("   P                     = %6d\n", pid[DF_P]);
  printf("   I                     = %6d\n", pid[DF_I]);
  printf("   D                     = %6d\n", pid[DF_D]);
  printf("   accel. feed forward   = %6d\n", pid[DF_ACCEL_FF]);
  printf("   veloc. feed forward   = %6d\n", pid[DF_VEL_FF]);
  printf("   I limit               = %6d\n", pid[DF_I_LIMIT]);
  printf("   voltage offset        = %6d\n", pid[DF_OFFSET]);
  printf("   voltage output limit  = %6d\n", pid[DF_DAC_LIMIT]);
  printf("   2^(n) divisor         = %6d\n", pid[DF_SHIFT]);
  printf("   friction feed forward = %6d\n", pid[DF_FRICT_FF]);
}



void meid_print_axis(int meidx, int axis)
{
  int as;
  double vel, acc, jerk, poscom, posact, posrange, poserror;
  
  as = meid_axis_status(meidx, axis);
  printf("Axis %d : ", axis);
  if (IN_SEQUENCE & as) printf("in_sequence ");
  if (IN_POSITION & as) printf("in_position ");
  if (IN_MOTION & as) printf("in_motion ");
  if ((as & DIRECTION) == 0) printf("+direction ");
  else printf("-direction ");
  printf("frames_left=%d\n", meid_frames_to_execute(meidx, axis));

  as = meid_axis_state(meidx, axis);
  printf("       : ");
  switch(as) {
  case NO_EVENT:
    printf("no_event(0)\n"); break;
  case 1:
    printf("no_event(1)\n"); break;
  case NEW_FRAME:
    printf("new_frame\n"); break;
  case STOP_EVENT:
    printf("stop_event\n"); break;
  case E_STOP_EVENT:
    printf("e_stop_event\n"); break;
  case ABORT_EVENT:
    printf("abort_event\n"); break;
  default:
    printf("Unknown axis state %d  source %d\n", as,
	   meid_axis_source(meidx, axis));
    break;
  }
  
  meid_error_check(meid_get_velocity(meidx, axis, &vel));
  meid_error_check(meid_get_accel(meidx, axis, &acc));
  meid_error_check(meid_get_jerk(meidx, axis, &jerk));
  meid_error_check(meid_get_command(meidx, axis, &poscom));
  meid_error_check(meid_get_position(meidx, axis, &posact));
  meid_error_check(meid_get_in_position(meidx, axis, &posrange));
  meid_error_check(meid_get_error(meidx, axis, &poserror));
  printf("       : command-position=%.0lf actual-position=%.0lf error=%.0lf\n",
	 poscom, posact, poserror);
  printf("       : velocity=%.0lf accel=%.0lf jerk=%.0lf in_position_range=%.01lf\n",
	 vel, acc, jerk, posrange);
}


void meid_error_check(int error_code)
{   
//  char buffer[MAX_ERROR_LEN];
  
  switch (error_code) {
  case DSP_OK:
    /* No error, so we can ignore it. */
    break;    
/*  case MEID_ERR:
    fprintf(stderr, "MEID ERROR: ill communication!\n");
    exit(1);
    break;*/
  default:
/*    error_msg(error_code, buffer) ;*/
/*    fprintf(stderr, "MEID ERROR: %s (%d).\n", buffer, error_code);*/
    fprintf(stderr, "MEID ERROR: (%d).\n", error_code);
    exit(1);
    break;
  }
}
