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


extern YAPOCMessage newcommand;

//arrays for storing newpositions and statusbits
float newpositions[N_AXIS];
short statusbits[N_AXIS]; //is either ON or OFF 
char *ON_TEXT = "On";
char *OFF_TEXT = "Off";


/* initialization function for making sure that all onoff buttons are off and
   reset all statusbits to OFF */
int initialize_onoff(){
  //check that all are initially off
  int stat = 0;
  stat += read_state_from_onoff(ABW_Axis0OnOff);
  stat += read_state_from_onoff(ABW_Axis1OnOff);
  stat += read_state_from_onoff(ABW_Axis2OnOff);
  stat += read_state_from_onoff(ABW_Axis3OnOff);
  stat += read_state_from_onoff(ABW_Axis4OnOff);
  stat += read_state_from_onoff(ABW_Axis5OnOff);
  stat += read_state_from_onoff(ABW_Axis6OnOff);
  stat += read_state_from_onoff(ABW_Axis7OnOff); 
  stat += read_state_from_onoff(ABW_Axis8OnOff);
   stat += read_state_from_onoff(ABW_Axis9OnOff);
   stat += read_state_from_onoff(ABW_Axis10OnOff);
   stat += read_state_from_onoff(ABW_Axis11OnOff);
   stat += read_state_from_onoff(ABW_Axis12OnOff);
   stat += read_state_from_onoff(ABW_Axis13OnOff);
   stat += read_state_from_onoff(ABW_Axis14OnOff);
   stat += read_state_from_onoff(ABW_Axis15OnOff);
  stat += read_state_from_onoff(ABW_Axis16OnOff);
  stat += read_state_from_onoff(ABW_Axis17OnOff);
  stat += read_state_from_onoff(ABW_Axis18OnOff);
  stat += read_state_from_onoff(ABW_Axis19OnOff);
  stat += read_state_from_onoff(ABW_Axis20OnOff);
  stat += read_state_from_onoff(ABW_Axis21OnOff);
  stat += read_state_from_onoff(ABW_Axis22OnOff);
  stat += read_state_from_onoff(ABW_Axis23OnOff);
  stat += read_state_from_onoff(ABW_Axis24OnOff);
  stat += read_state_from_onoff(ABW_Axis25OnOff);
  stat += read_state_from_onoff(ABW_Axis26OnOff);
  stat += read_state_from_onoff(ABW_Axis27OnOff);
  stat += read_state_from_onoff(ABW_Axis28OnOff);
   stat += read_state_from_onoff(ABW_Axis29OnOff);
  if (stat > 0){
    printf("At least one axis is not initially off\n"); 
    return -1;
  }
  else {
    printf("All axes are initially zero\n");

  }
  //initialize statusbits
  for (int s = 0; s < N_AXIS; s++)
      statusbits[s] = OFF;
  return 0;
  
}

/* initialization function to read slider's initial values into newpositions */
int initialize_slider(){
  printf("Initialize newpositions to: \n");
  //initialize newpositions
  
  newpositions[0] = read_value_from_slider(ABW_Axis0Slider);
  printf("%f ",newpositions[0]); fflush(stdout);
  
  newpositions[1] = read_value_from_slider(ABW_Axis1Slider);
  printf("%f ",newpositions[1]);
  newpositions[2] = read_value_from_slider(ABW_Axis2Slider);
  printf("%f ",newpositions[2]);
  newpositions[3] = read_value_from_slider(ABW_Axis3Slider);
  printf("%f ",newpositions[3]);
  newpositions[4] = read_value_from_slider(ABW_Axis4Slider);
  printf("%f ",newpositions[4]);
  newpositions[5] = read_value_from_slider(ABW_Axis5Slider);
  printf("%f ",newpositions[5]);
  newpositions[6] = read_value_from_slider(ABW_Axis6Slider);
  printf("%f ",newpositions[6]);
  newpositions[7] = read_value_from_slider(ABW_Axis7Slider);
  printf("%f ",newpositions[7]);
  newpositions[8] = read_value_from_slider(ABW_Axis8Slider);
  printf("%f ",newpositions[8]);
  newpositions[9] = read_value_from_slider(ABW_Axis9Slider);
  printf("%f ",newpositions[9]);
  newpositions[10] = read_value_from_slider(ABW_Axis10Slider);
  printf("%f ",newpositions[10]);
  newpositions[11] = read_value_from_slider(ABW_Axis11Slider);
  printf("%f ",newpositions[11]);
  newpositions[12] = read_value_from_slider(ABW_Axis12Slider);
  printf("%f ",newpositions[12]);
  newpositions[13] = read_value_from_slider(ABW_Axis13Slider);
  printf("%f ",newpositions[13]);
  newpositions[14] = read_value_from_slider(ABW_Axis14Slider);
  printf("%f ",newpositions[14]);
  newpositions[15] = read_value_from_slider(ABW_Axis15Slider);
  printf("%f ",newpositions[15]);
  newpositions[16] = read_value_from_slider(ABW_Axis16Slider);
  printf("%f ",newpositions[16]);
  newpositions[17] = read_value_from_slider(ABW_Axis17Slider);
  printf("%f ",newpositions[17]);
  newpositions[18] = read_value_from_slider(ABW_Axis18Slider);
  printf("%f ",newpositions[18]);
  newpositions[19] = read_value_from_slider(ABW_Axis19Slider);
  printf("%f ",newpositions[19]);
  newpositions[20] = read_value_from_slider(ABW_Axis20Slider);
  printf("%f ",newpositions[20]);
  newpositions[21] = read_value_from_slider(ABW_Axis21Slider);
  printf("%f ",newpositions[21]);
  newpositions[22] = read_value_from_slider(ABW_Axis22Slider);
  printf("%f ",newpositions[22]);
  newpositions[23] = read_value_from_slider(ABW_Axis23Slider);
  printf("%f ",newpositions[23]);
  newpositions[24] = read_value_from_slider(ABW_Axis24Slider);
  printf("%f ",newpositions[24]);
  newpositions[25] = read_value_from_slider(ABW_Axis25Slider);
  printf("%f ",newpositions[25]);
  newpositions[26] = read_value_from_slider(ABW_Axis26Slider);
  printf("%f ",newpositions[26]);
  newpositions[27] = read_value_from_slider(ABW_Axis27Slider);
  printf("%f ",newpositions[27]);
  newpositions[28] = read_value_from_slider(ABW_Axis28Slider);
  printf("%f ",newpositions[28]);
  newpositions[29] = read_value_from_slider(ABW_Axis29Slider);
  printf("%f ",newpositions[29]);

  return 1;
  
  
}

/* this function is called when users move slider for axis 0 */
int
axis0slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	  
	  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[0] = (float)cb->position;
	  printf("Slider 0 has been moved to %f\n",newpositions[0]);
	  return( Pt_CONTINUE );
	  

	}

/* this function is called when users move slider for axis 1 */
int
axis1slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
          PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[1] = (float)cb->position;
	  printf("Slider 1 has been moved to %f\n",newpositions[1]);
	  return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 2 */
int
axis2slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

          PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[2] = (float)cb->position;
	  printf("Slider 2 has been moved to %f\n",newpositions[2]);
	  return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 3 */
int
axis3slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[3] = (float)cb->position;
	  printf("Slider 3 has been moved to %f\n",newpositions[3]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 4 */
int
axis4slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[4] = (float)cb->position;
	  printf("Slider 4 has been moved to %f\n",newpositions[4]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 5 */
int
axis5slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[5] = (float)cb->position;
	  printf("Slider 5 has been moved to %f\n",newpositions[5]);
	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 6 */
int
axis6slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[6] = (float)cb->position;
	  printf("Slider 6 has been moved to %f\n",newpositions[6]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 7 */
int
axis7slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[7] = (float)cb->position;
	  printf("Slider 7 has been moved to %f\n",newpositions[7]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 8 */
int
axis8slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[8] = (float)cb->position;
	  printf("Slider 8 has been moved to %f\n",newpositions[8]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 9 */
int
axis9slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[9] = (float)cb->position;
	  printf("Slider 9 has been moved to %f\n",newpositions[9]);
	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 10 */
int
axis10slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[10] = (float)cb->position;
	  printf("Slider 10 has been moved to %f\n",newpositions[10]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 11 */
int
axis11slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[11] = (float)cb->position;
	  printf("Slider 11 has been moved to %f\n",newpositions[11]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 12 */
int
axis12slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[12] = (float)cb->position;
	  printf("Slider 12 has been moved to %f\n",newpositions[12]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 13 */
int
axis13slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[13] = (float)cb->position;
	  printf("Slider 13 has been moved to %f\n",newpositions[13]);
	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 14 */
int
axis14slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[14] = (float)cb->position;
	  printf("Slider 14 has been moved to %f\n",newpositions[14]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 29 */
int
axis29slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[29] = (float)cb->position;
	  printf("Slider 29 has been moved to %f\n",newpositions[29]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 28 */
int
axis28slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[28] = (float)cb->position;
	  printf("Slider 28 has been moved to %f\n",newpositions[28]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 27 */
int
axis27slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[27] = (float)cb->position;
	  printf("Slider 27 has been moved to %f\n",newpositions[27]);
	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 26 */
int
axis26slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[26] = (float)cb->position;
	  printf("Slider 26 has been moved to %f\n",newpositions[26]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 25 */
int
axis25slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[25] = (float)cb->position;
	  printf("Slider 25 has been moved to %f\n",newpositions[25]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 24 */
int
axis24slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[24] = (float)cb->position;
	  printf("Slider 24 has been moved to %f\n",newpositions[24]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 23 */
int
axis23slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[23] = (float)cb->position;
	  printf("Slider 23 has been moved to %f\n",newpositions[23]);
	  

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 22 */
int
axis22slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[22] = (float)cb->position;
	  printf("Slider 22 has been moved to %f\n",newpositions[22]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 21 */
int
axis21slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[21] = (float)cb->position;
	  printf("Slider 21 has been moved to %f\n",newpositions[21]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 20 */
int
axis20slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[20] = (float)cb->position;
	  printf("Slider 20 has been moved to %f\n",newpositions[20]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 19 */
int
axis19slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[19] = (float)cb->position;
	  printf("Slider 19 has been moved to %f\n",newpositions[19]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 18 */
int
axis18slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[18] = (float)cb->position;
	  printf("Slider 18 has been moved to %f\n",newpositions[18]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 17 */
int
axis17slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[17] = (float)cb->position;
	  printf("Slider 17 has been moved to %f\n",newpositions[17]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 16 */
int
axis16slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[16] = (float)cb->position;
	  printf("Slider 16 has been moved to %f\n",newpositions[16]);

	return( Pt_CONTINUE );

	}

/* this function is called when users move slider for axis 15 */
int
axis15slider_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

PtSliderCallback_t *cb;
	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  cb = (PtSliderCallback_t *)cbinfo->cbdata;
	  newpositions[15] = (float)cb->position;
	  printf("Slider 15 has been moved to %f\n",newpositions[15]);

	return( Pt_CONTINUE );

	}

/* this function is called when users clicks on the move button, which is
a request to move all 30 motors to their current slider position values */
int
MoveToPos( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{


	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  short uselimit_flag = 1;
	  newcommand.CreatePositionCommandMsg(newpositions,statusbits,uselimit_flag);
	  SendMessage(newcommand);
	  
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 0 */
int
Axis0Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	 int naxis = 0; 
	 int stat = read_state_from_onoff(widget);
	  if (stat == 0){ 
	    if (statusbits[naxis] != OFF){
	      //off
	      statusbits[naxis] = OFF;
	      write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	    }
	  }
	  else {
	    if (statusbits[naxis] == OFF){
	    //on
	      statusbits[naxis] = ON;
	      write_text_to_widget(widget,ON_TEXT,Pg_RED);
	    }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 1 */
int
Axis1Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 1;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	    if (statusbits[naxis] != OFF){
	      //off
	      statusbits[naxis] = OFF;
	      write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	    }
	  }
	  else {
	    if (statusbits[naxis] == OFF){
	      //on
	      statusbits[naxis] = ON;
	      write_text_to_widget(widget,ON_TEXT,Pg_RED);
	    }
	  }
	  return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 2 */
int
Axis2Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  /* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 2;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	    if (statusbits[naxis] != OFF){
	      //off
	      statusbits[naxis] = OFF;
	      write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	    }
	  }
	  else {
	    if (statusbits[naxis] == OFF){
	      //on
	      statusbits[naxis] = ON;
	      write_text_to_widget(widget,ON_TEXT,Pg_RED);
	    }
	  }
	  return( Pt_CONTINUE );
	  
	}

/* this function is called when users toggles onoff button for axis 3 */
int
Axis3Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	int naxis = 3;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 4 */
int
Axis4Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 4;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 5 */
int
Axis5Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 5;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 6 */
int
Axis6Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 6;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 7 */
int
Axis7Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 7;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 8 */
int
Axis8Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 8;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 9 */
int
Axis9Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 9;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 10 */
int
Axis10Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 10;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 11 */
int
Axis11Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 11;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 12 */
int
Axis12Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 12;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 13 */
int
Axis13Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 13;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 14 */
int
Axis14Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 14;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 15 */
int
Axis15Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 15;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 16 */
int
Axis16Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 16;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 17 */
int
Axis17Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 17;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 18 */
int
Axis18Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 18;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 19 */
int
Axis19Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 19;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 20 */
int
Axis20Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 20;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 21 */
int
Axis21Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 21;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 22 */
int
Axis22Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 22;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 23 */
int
Axis23Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 23;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 24 */
int
Axis24Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 24;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 25 */
int
Axis25Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 25;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 26 */
int
Axis26Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 26;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 27 */
int
Axis27Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 27;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 28 */
int
Axis28Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 28;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users toggles onoff button for axis 29 */
int
Axis29Toggle( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  int naxis = 29;
	  int stat = read_state_from_onoff(widget);
	  if (stat == 0){
	      if (statusbits[naxis] != OFF){
		    //off
		    statusbits[naxis] = OFF;
		    write_text_to_widget(widget,OFF_TEXT, Pg_BLUE);
	      }
	  }
	  else {
	      if (statusbits[naxis] == OFF){
		    //on
		    statusbits[naxis] = ON;
		    write_text_to_widget(widget,ON_TEXT,Pg_RED);
	      }
	  }
	return( Pt_CONTINUE );

	}

/* this function is called when users click on the turn on all button, which
means 'turn on all onoff buttons ' */
int TurnOnAll( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  short one = 1;
  for (int i = 0; i < N_AXIS; i++){
    if (statusbits[i] == OFF){
      statusbits[i] = ON;
      if (i == 0)
	{
	  write_state_to_onoff(ABW_Axis0OnOff, one);
	  write_text_to_widget(ABW_Axis0OnOff, ON_TEXT, Pg_RED);}
      if (i == 1)
	{ 
	  write_state_to_onoff(ABW_Axis1OnOff, one); 
	  write_text_to_widget(ABW_Axis1OnOff, ON_TEXT, Pg_RED); 
	}
      if (i == 2)
	{ 
	  write_state_to_onoff(ABW_Axis2OnOff, one); 
	  write_text_to_widget(ABW_Axis2OnOff, ON_TEXT, Pg_RED); 
	}
      if (i == 3)
	{ write_state_to_onoff(ABW_Axis3OnOff, one); 
	  write_text_to_widget(ABW_Axis3OnOff, ON_TEXT, Pg_RED); }
      if (i == 4)
	{ write_state_to_onoff(ABW_Axis4OnOff, one); 
	  write_text_to_widget(ABW_Axis4OnOff, ON_TEXT, Pg_RED); }
      if (i == 5)
	{ write_state_to_onoff(ABW_Axis5OnOff, one); 
	  write_text_to_widget(ABW_Axis5OnOff, ON_TEXT, Pg_RED); }
      if (i == 6)
	{ write_state_to_onoff(ABW_Axis6OnOff, one); 
	  write_text_to_widget(ABW_Axis6OnOff, ON_TEXT, Pg_RED); }
      if (i == 7)
	{ write_state_to_onoff(ABW_Axis7OnOff, one); 
	  write_text_to_widget(ABW_Axis7OnOff, ON_TEXT, Pg_RED); }
      if (i == 8)
	{ write_state_to_onoff(ABW_Axis8OnOff, one); 
	  write_text_to_widget(ABW_Axis8OnOff, ON_TEXT, Pg_RED); }
      if (i == 9)
	{ write_state_to_onoff(ABW_Axis9OnOff, one); 
	  write_text_to_widget(ABW_Axis9OnOff, ON_TEXT, Pg_RED); }
      if (i == 10)
	{ write_state_to_onoff(ABW_Axis10OnOff, one); 
	  write_text_to_widget(ABW_Axis10OnOff, ON_TEXT, Pg_RED); }
      if (i == 11)
	{ write_state_to_onoff(ABW_Axis11OnOff, one); 
	  write_text_to_widget(ABW_Axis11OnOff, ON_TEXT, Pg_RED); }
      if (i == 12)
	{ write_state_to_onoff(ABW_Axis12OnOff, one); 
	  write_text_to_widget(ABW_Axis12OnOff, ON_TEXT, Pg_RED); }
      if (i == 13)
	{ write_state_to_onoff(ABW_Axis13OnOff, one); 
	  write_text_to_widget(ABW_Axis13OnOff, ON_TEXT, Pg_RED); }
      if (i == 14)
	{ write_state_to_onoff(ABW_Axis14OnOff, one); 
	  write_text_to_widget(ABW_Axis14OnOff, ON_TEXT, Pg_RED); }
      if (i == 15)
	{ write_state_to_onoff(ABW_Axis15OnOff, one); 
	  write_text_to_widget(ABW_Axis15OnOff, ON_TEXT, Pg_RED); }
       if (i == 16)
	{ write_state_to_onoff(ABW_Axis16OnOff, one); 
	  write_text_to_widget(ABW_Axis16OnOff, ON_TEXT, Pg_RED); }
      if (i == 17)
	{ write_state_to_onoff(ABW_Axis17OnOff, one); 
	  write_text_to_widget(ABW_Axis17OnOff, ON_TEXT, Pg_RED); }
      if (i == 18)
	{ write_state_to_onoff(ABW_Axis18OnOff, one); 
	  write_text_to_widget(ABW_Axis18OnOff, ON_TEXT, Pg_RED); }
      if (i == 19)
	{ write_state_to_onoff(ABW_Axis19OnOff, one); 
	  write_text_to_widget(ABW_Axis19OnOff, ON_TEXT, Pg_RED); }
      if (i == 20)
	{ write_state_to_onoff(ABW_Axis20OnOff, one); 
	  write_text_to_widget(ABW_Axis20OnOff, ON_TEXT, Pg_RED); }
      if (i == 21)
	{ write_state_to_onoff(ABW_Axis21OnOff, one); 
	  write_text_to_widget(ABW_Axis21OnOff, ON_TEXT, Pg_RED); }
      if (i == 22)
	{ write_state_to_onoff(ABW_Axis22OnOff, one); 
	  write_text_to_widget(ABW_Axis22OnOff, ON_TEXT, Pg_RED); }
      if (i == 23)
	{ write_state_to_onoff(ABW_Axis23OnOff, one); 
	  write_text_to_widget(ABW_Axis23OnOff, ON_TEXT, Pg_RED); }
      if (i == 24)
	{ write_state_to_onoff(ABW_Axis24OnOff, one); 
	  write_text_to_widget(ABW_Axis24OnOff, ON_TEXT, Pg_RED); }
      if (i == 25)
	{ write_state_to_onoff(ABW_Axis25OnOff, one); 
	  write_text_to_widget(ABW_Axis25OnOff, ON_TEXT, Pg_RED); }
      if (i == 26)
	{ write_state_to_onoff(ABW_Axis26OnOff, one); 
	  write_text_to_widget(ABW_Axis26OnOff, ON_TEXT, Pg_RED); }
      if (i == 27)
	{ write_state_to_onoff(ABW_Axis27OnOff, one); 
	  write_text_to_widget(ABW_Axis27OnOff, ON_TEXT, Pg_RED); }
      if (i == 28)
	{ write_state_to_onoff(ABW_Axis28OnOff, one); 
	  write_text_to_widget(ABW_Axis28OnOff, ON_TEXT, Pg_RED); }
      if (i == 29)
	{ write_state_to_onoff(ABW_Axis29OnOff, one); 
	  write_text_to_widget(ABW_Axis29OnOff, ON_TEXT, Pg_RED); }
    }
  }
   return( Pt_CONTINUE ); 
}

/* this function is called when users click on the turn off all button, which
means 'turn off all onoff buttons ' */
int
TurnOffAll( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
  
    /* eliminate 'unreferenced' warnings */
    widget = widget, apinfo = apinfo, cbinfo = cbinfo;
    short zero = 0;
    for (int i = 0; i < N_AXIS; i++){
          if (statusbits[i] == ON){
	    statusbits[i] = OFF;
	    if (i == 0)
	      {
		write_state_to_onoff(ABW_Axis0OnOff, zero);
		write_text_to_widget(ABW_Axis0OnOff, OFF_TEXT, Pg_BLUE);
	      }
	    if (i == 1)
	      { write_state_to_onoff(ABW_Axis1OnOff, zero);
		write_text_to_widget(ABW_Axis1OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 2)
	      { write_state_to_onoff(ABW_Axis2OnOff, zero);
		write_text_to_widget(ABW_Axis2OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 3)
	      { write_state_to_onoff(ABW_Axis3OnOff, zero);
		write_text_to_widget(ABW_Axis3OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 4)
	      { write_state_to_onoff(ABW_Axis4OnOff, zero);
		write_text_to_widget(ABW_Axis4OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 5)
	      { write_state_to_onoff(ABW_Axis5OnOff, zero);
		write_text_to_widget(ABW_Axis5OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 6)
	      { write_state_to_onoff(ABW_Axis6OnOff, zero);
		write_text_to_widget(ABW_Axis6OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 7)
	      { write_state_to_onoff(ABW_Axis7OnOff, zero);
		write_text_to_widget(ABW_Axis7OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 8)
	      { write_state_to_onoff(ABW_Axis8OnOff, zero);
		write_text_to_widget(ABW_Axis8OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 9)
	      { write_state_to_onoff(ABW_Axis9OnOff, zero);
		write_text_to_widget(ABW_Axis9OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 10)
	      { write_state_to_onoff(ABW_Axis10OnOff, zero);
		write_text_to_widget(ABW_Axis10OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 11)
	      { write_state_to_onoff(ABW_Axis11OnOff, zero);
		write_text_to_widget(ABW_Axis11OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 12)
	      { write_state_to_onoff(ABW_Axis12OnOff, zero);
		write_text_to_widget(ABW_Axis12OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 13)
	      { write_state_to_onoff(ABW_Axis13OnOff, zero);
		write_text_to_widget(ABW_Axis13OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 14)
	      { write_state_to_onoff(ABW_Axis14OnOff, zero);
		write_text_to_widget(ABW_Axis14OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 15)
	      { write_state_to_onoff(ABW_Axis15OnOff, zero);
		write_text_to_widget(ABW_Axis15OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 16)
	      { write_state_to_onoff(ABW_Axis16OnOff, zero);
		write_text_to_widget(ABW_Axis16OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 17)
	      { write_state_to_onoff(ABW_Axis17OnOff, zero);
		write_text_to_widget(ABW_Axis17OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 18)
	      { write_state_to_onoff(ABW_Axis18OnOff, zero);
		write_text_to_widget(ABW_Axis18OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 19)
	      { write_state_to_onoff(ABW_Axis19OnOff, zero);
		write_text_to_widget(ABW_Axis19OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 20)
	      { write_state_to_onoff(ABW_Axis20OnOff, zero);
		write_text_to_widget(ABW_Axis20OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 21)
	      { write_state_to_onoff(ABW_Axis21OnOff, zero);
		write_text_to_widget(ABW_Axis21OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 22)
	      { write_state_to_onoff(ABW_Axis22OnOff, zero);
		         write_text_to_widget(ABW_Axis22OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 23)
	      { write_state_to_onoff(ABW_Axis23OnOff, zero);
		write_text_to_widget(ABW_Axis23OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	         if (i == 24)
	      { write_state_to_onoff(ABW_Axis24OnOff, zero);
		write_text_to_widget(ABW_Axis24OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 25)
	      { write_state_to_onoff(ABW_Axis25OnOff, zero);
		write_text_to_widget(ABW_Axis25OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 26)
	      { write_state_to_onoff(ABW_Axis26OnOff, zero);
		write_text_to_widget(ABW_Axis26OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 27)
	      { write_state_to_onoff(ABW_Axis27OnOff, zero);
		    write_text_to_widget(ABW_Axis27OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 28)
	      { write_state_to_onoff(ABW_Axis28OnOff, zero);
		    write_text_to_widget(ABW_Axis28OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	    if (i == 29)
	      { write_state_to_onoff(ABW_Axis29OnOff, zero);
		    write_text_to_widget(ABW_Axis29OnOff, OFF_TEXT, Pg_BLUE); 
	      }
	  }
    }
     return( Pt_CONTINUE );
}
