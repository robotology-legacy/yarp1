/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

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


#include "mycommands.h"

//YAPOC message structure
YAPOCMessage newcommand;

//global variables
short current_axis; //indicate currently selected axis
short axis_selected = 0; //flag for indicating an axis has been selected
float newvelocity[N_AXIS]; //array for desired velocity values
short bits[N_AXIS];       //array for status bits




/* This function is called when users select an axis in the AxisSelector
 */
int
axischange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	  //if (connected == 1){
	    PtArg_t args[1];
	    unsigned short *newaxis;
	    
	    // index is Pt_ARG_CBOX_SEL_ITEM
	    /* eliminate 'unreferenced' warnings */
	    widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	    
	    PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 );
	    PtGetResources(ABW_AxisSelector, 1, args);
	    current_axis = (short)(*newaxis - 1);
	    axis_selected = 1;
	    return( Pt_CONTINUE );

	}

/* This function is called when users press the quit button */
int
quit( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  	
	  exit (0);
	return( Pt_CONTINUE );

	}

/* this function is called when users press the zero encoder button.
if an axis has been selected, this command means user would like to zero 
encoder value for the selected axis.
if not, this command is ignored */
int
zero_encoder( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  if (axis_selected == 1){
	    
	    //CreateMessage(ZERO_ENCODER);
	    newcommand.CreateZeroEncoderMsg(current_axis);
	    SendMessage(newcommand);
	     printf("Sending request to zero encoder for axis %d\n",current_axis);
	    
	  }

	return( Pt_CONTINUE );

	}

/* this function is called when users press the set velocity button.
similarly, if an axis is selected, this function sends a request to YAPOC 
to change velocity for the selected axis to current value displayed in new
velocity widget
if not, this command is ignored */
int
SetVelocity( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  	    
	if (axis_selected == 1){
	  double newvel = read_double_from_widget(ABW_VelocityValue);
	  
	  for (int i = 0; i < N_AXIS; i++){
	    if (i == current_axis){
	      newvelocity[i] = newvel;
		bits[i] = ON;
	      }
	      else {
		newvelocity[i] = 0;
		bits[i] = OFF;
	      }
	  }
	  newcommand.CreateSetVelocityMsg(newvelocity,bits);
	  SendMessage(newcommand);
	  printf("Sending request to change velocity for axis %d to %f\n",current_axis, newvel);
	} 
	return( Pt_CONTINUE );

	}

/* this function is called when users press the calibrate all button */	
int
CalibrateAll( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  
	  newcommand.CreateCalibrateAllMsg();
	  SendMessage(newcommand);
	  printf("Sending request to calibrate all axes\n");
	  return( Pt_CONTINUE );

	}









