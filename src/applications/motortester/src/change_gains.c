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

void draw_gains()
{
  PtArg_t args[1];
  int val;
  
  meid_get_filter(meidx, current_axis, &gains);

  val =gains[DF_P];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_p_gain, 1, args);

  val =gains[DF_I];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_i_gain, 1, args);

  val =gains[DF_D];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_d_gain, 1, args);

  val =gains[DF_ACCEL_FF];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_accel_ff, 1, args);

  val =gains[DF_VEL_FF];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_vel_ff, 1, args);

  val =gains[DF_I_LIMIT];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_i_limit, 1, args);
  
  val =gains[DF_DAC_LIMIT];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_dac_limit, 1, args);
  
  val =gains[DF_OFFSET];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_offset, 1, args);
  
  val =gains[DF_FRICT_FF];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_friction_ff, 1, args);

  val =gains[DF_SHIFT];
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(ABW_shift, 1, args);

}

int
change_gains( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int val, gain_index;
  PtNumericIntegerCallback_t *cb;
  int nameindex;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  cb = cbinfo->cbdata;
  val = cb->numeric_value;

  meid_get_filter(meidx, current_axis, &gains);
  
  nameindex = ApName(widget);
  if (nameindex == ABN_p_gain)
    gain_index = DF_P;
  else if (nameindex == ABN_i_gain)
    gain_index = DF_I;
  else if (nameindex == ABN_d_gain)
    gain_index = DF_D;
  else if (nameindex == ABN_accel_ff)
    gain_index = DF_ACCEL_FF;
  else if (nameindex == ABN_vel_ff)
    gain_index = DF_VEL_FF;
  else if (nameindex == ABN_offset)
    gain_index = DF_OFFSET;
  else if (nameindex == ABN_i_limit)
    gain_index = DF_I_LIMIT;
  else if (nameindex == ABN_dac_limit)
    gain_index = DF_DAC_LIMIT;
  else if (nameindex == ABN_friction_ff)
    gain_index = DF_FRICT_FF;
  else if (nameindex == ABN_shift)
    gain_index = DF_SHIFT;
  else
    gain_index = -1;
    
  if (gain_index != -1)
    {
      gains[gain_index] = val;
      meid_set_filter(meidx, current_axis, &gains);
      // printf("new value = %d\n", val);
    }
  
  return( Pt_CONTINUE );
  
}

