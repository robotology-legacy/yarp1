/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "abimport.h"
#include "proto.h"
#include "motortester.h"

void print_pid_filter(short *pid)
{
  printf("PID Filter Coefficients\n");
  printf("   P                     = %6d\n", pid[DF_P]);
  printf("   I                     = %6d\n", pid[DF_I]);
  printf("   D                     = %6d\n", pid[DF_D]);
  printf("   accel. feed forward   = %6d\n", pid[DF_ACCEL_FF]);
  printf("   veloc. feed forward   = %6d\n", pid[DF_VEL_FF]);
  printf("   voltage output limit  = %6d\n", pid[DF_DAC_LIMIT]);
  printf("   I limit               = %6d\n", pid[DF_I_LIMIT]);
  printf("   friction feed forward = %6d\n", pid[DF_FRICT_FF]);
  printf("   voltage offset        = %6d\n", pid[DF_OFFSET]);
  printf("   2^(n) divisor         = %6d\n", pid[DF_SHIFT]);
}

int
save_filter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int stat;
  // int16 bootgains[COEFFICIENTS];
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  printf("Writing new boot gains...\n");
  print_pid_filter(gains);  
  
  stat = meid_set_boot_filter(meidx, current_axis, gains);
  printf("set_boot_filter returns %d ... calling checksum\n", stat);
  stat = meid_mei_checksum(meidx);
  printf("checksum returned %d\n", stat);
  
  
  return( Pt_CONTINUE );
  
}

