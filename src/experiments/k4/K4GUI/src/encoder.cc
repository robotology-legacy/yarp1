
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
#include "K4JointPos.h"

#define SCALE_FACTOR 3 //scale factor for scaling encoder value to fit in 
                       //trend viewers

int current_axis[N_VIEWERS]; //array for storing currently selected axes for
// each of the 4 viewers

/* this function initializes current_axis such that viewers initially display
   axis 0,1,2,3 */
void initialize_trends(){

  current_axis[0] = 0;
  current_axis[1] = 1;
  current_axis[2] = 2;
  current_axis[3] = 3;
  
}

/* this function is called periodically (10 ms). it essentially reads encoder
   values for all motors in the system and displays them in trend viewers */
int
TimerActivate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  PtArg_t args[1];
	  int scaled_position_array[N_VIEWERS];
	  int actual_position_array[N_VIEWERS];
	  int position_value[1];
	  int val;
	  
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;


	  float temp;
	  
	  K4JointPos pos = ReadInput();
	    for (int i = 0; i < N_VIEWERS; i++){
	      //printf("set pos array [%d] to pos(%d)\n",i,current_axis[i]+1);
	      actual_position_array[i] = pos.positions[current_axis[i]];
	      temp = pos.positions[current_axis[i]] / SCALE_FACTOR;
	      scaled_position_array[i] = temp;
	    }
	    
	    //trend 1
	    position_value[0] = scaled_position_array[0];
	    
	    PtSetArg(&args[0], Rt_ARG_TREND_DATA, position_value, 1);
	    PtSetResources(ABW_Trend1, 1, args);
	    
	    
	    val = actual_position_array[0];
	    PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
	    PtSetResources(ABW_TrendValue1, 1, args);
	    
	    //trend2
	  position_value[0] = scaled_position_array[1];
	  
	  PtSetArg(&args[0], Rt_ARG_TREND_DATA, position_value, 1);
	  PtSetResources(ABW_Trend2, 1, args);
	  
	  
	  val = actual_position_array[1];
	  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
	  PtSetResources(ABW_TrendValue2, 1, args);

	  
	    //trend 3
	    position_value[0] = scaled_position_array[2];
	  
	    PtSetArg(&args[0], Rt_ARG_TREND_DATA, position_value, 1);
	    PtSetResources(ABW_Trend3, 1, args);
	  
	  
	    //val = position_value[0];
	    val = actual_position_array[2];
	  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
	    PtSetResources(ABW_TrendValue3, 1, args);
	  
	    //trend4
	  position_value[0] = scaled_position_array[3];
	  
	  PtSetArg(&args[0], Rt_ARG_TREND_DATA, position_value, 1);
	  PtSetResources(ABW_Trend4, 1, args);
	  
	  
	  val = actual_position_array[3];
	  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, val, 0);
	  PtSetResources(ABW_TrendValue4, 1, args);

	  
	  
	  //trend5 (all)
	  
	  PtSetArg(&args[0], Rt_ARG_TREND_DATA, scaled_position_array, 4);
	  PtSetResources(ABW_Trend5, 1, args);
	  
       
	  
	  
	  return( Pt_CONTINUE );

	}


/* this function is called when users select a different axis to be displayed
   in trend viewer 1 */
int
Trend1AxisChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	  PtArg_t args[1];
	  unsigned short *newaxis;
	  
	  // index is Pt_ARG_CBOX_SEL_ITEM
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  
	  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 );
	  PtGetResources(ABW_AxisSelector1, 1, args);
	  current_axis[0] = *newaxis - 1;
	  printf("Trend 1 current axis = %d\n",current_axis[0]);
	  
	return( Pt_CONTINUE );

	}

/* this function is called when users select a different axis to be displayed
   in trend viewer 2 */
int
Trend2AxisChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  PtArg_t args[1];
	  unsigned short *newaxis;
	  
	  // index is Pt_ARG_CBOX_SEL_ITEM
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  
	  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 );
	  PtGetResources(ABW_AxisSelector2, 1, args);
	  current_axis[1] = *newaxis - 1;
	  printf("Trend 2 current axis = %d\n",current_axis[1]);
	return( Pt_CONTINUE );

	}

/* this function is called when users select a different axis to be displayed
   in trend viewer 3 */
int
Trend3AxisChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
PtArg_t args[1];
	  unsigned short *newaxis;
	  
	  // index is Pt_ARG_CBOX_SEL_ITEM
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  
	  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 );
	  PtGetResources(ABW_AxisSelector3, 1, args);
	  current_axis[2] = *newaxis - 1;
	  printf("Trend 3 current axis = %d\n",current_axis[2]);
	return( Pt_CONTINUE );

	}

/* this function is called when users select a different axis to be displayed
   in trend viewer 4 */
int
Trend4AxisChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	  PtArg_t args[1];
	  unsigned short *newaxis;
	  
	  // index is Pt_ARG_CBOX_SEL_ITEM
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  
	  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 );
	  PtGetResources(ABW_AxisSelector4, 1, args);
	  current_axis[3] = *newaxis - 1;
	  printf("Trend 4 current axis = %d\n",current_axis[3]);
	return( Pt_CONTINUE );

	}


/* this function realizes trend the last viewer which displays viewer 1,2,3,4 
   concurrently */
int
Realized( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	  int trend_color_array[N_VIEWERS] = {   Pg_RED,
	    Pg_BLUE,
	    Pg_YELLOW,
	            Pg_MAGENTA
	      
	   };
	  
	  
	   RtTrendAttr_t one_attr, several_attr[N_VIEWERS];
	   PtArg_t args[2];
	  
	   /* Set up the color list. */
	   PtSetArg (&args[0], Rt_ARG_TREND_COLOR_LIST, trend_color_array, N_VIEWERS);
	   PtSetResources (widget, 1, args);
	  
	   /* Map the trends to colors. */
	   for (int i = 0; i < N_VIEWERS; i++)
	      several_attr[i].map = i+1;
	      
     	   PtSetArg (&args[0], Rt_ARG_TREND_ATTRIBUTES,
		                          several_attr, 0);
	   PtSetResources (widget, 1, args);
	  
	  
	  
	  return( Pt_CONTINUE );

	}

