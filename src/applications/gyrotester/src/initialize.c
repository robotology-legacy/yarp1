/* Init function -- establishes contact with ISD daemon */
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
#include "gyrotester.h"
#include "abimport.h"
#include "proto.h"

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */

int isdx;
char errmsg[100];

int
initialize( int argc, char *argv[] )
{
  int stat;
  
  /* eliminate 'unreferenced' warnings */
  argc = argc, argv = argv;

  stat = isd_initialize("/yarp/isd", &isdx);
  if (stat != 0)
    {
      sprintf(errmsg, "Failed to locate InterSense daemon... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "initialize", errmsg,
	      __FILE__);
      exit(-1);
    }

  // set gyro to binary mode
  stat = isd_set_output_mode(isdx, TRUE, FALSE);
  
  return( Pt_CONTINUE );
  
}

