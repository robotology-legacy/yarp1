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
#include "YARPTime.h"

#define N_AXIS 30

/* input and output port */
YARPOutputPortOf<YAPOCMessage> outCommandPort;
YARPInputPortOf<K4JointPos> inPositionPort;


/* utility function for reading integer from numeric widget */
int read_integer_from_widget(PtWidget_t *widget)
{
      PtArg_t args[1];
      int *intval;
  
      PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &intval, 0);
      PtGetResources(widget, 1, args);
      return(*intval);
}

/* utility function for reading current state of the onoff button */
int read_state_from_onoff(PtWidget_t *widget)
{
  PtArg_t args[1];
  int *intval;
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, &intval,0);
  PtGetResources(widget,1,args);
  return(*intval);
  
}

/* utility function for changing current state of the onoff button */
void write_state_to_onoff(PtWidget_t *widget, short newstate)
{
  PtArg_t args[1];
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, newstate,0);
  PtSetResources(widget,1,args);
}

/* utility function for reading value from a slider */
float read_value_from_slider(PtWidget_t *widget)
{
    PtArg_t args[1];
    int *intval;
    float dval;
    PtSetArg(&args[0], Pt_ARG_GAUGE_VALUE, &intval, 0);
  
    PtGetResources(widget, 1, args);
  dval = (float)(*intval); 
  
    return(dval);
}

/* utility function for reading double from a numeric widget */
double read_double_from_widget(PtWidget_t *widget)
{
      PtArg_t args[1];
      double *dval;
  
      PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &dval, 0);
      PtGetResources(widget, 1, args);
      return(*dval);
}

/* utility function for reading from a text widget */
char *read_text_from_widget(PtWidget_t *widget)
{
      PtArg_t args[1];
      char *str;
  
      PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &str, 0);
      PtGetResources(widget, 1, args);
      return(str);
}

/* utility function for writing into a text widget */
void write_text_to_widget(PtWidget_t *widget, char* text, PgColor_t textcolor){
 
  PtArg_t args[2];
  PtSetArg(&args[0],Pt_ARG_COLOR, textcolor, 0);
  PtSetArg(&args[1],Pt_ARG_TEXT_STRING,text,1);
  PtSetResources(widget,2,args);
 
}

/* utility function for writing an integer into a numeric widget */
void  write_integer_to_widget(PtWidget_t *widget, int val){
  PtArg_t args[1];
  PtSetArg(&args[0],Pt_ARG_NUMERIC_VALUE, val, 0);
  PtSetResources(widget,1,args);
}

/* utility function for reading currently selected item from a selector widget */
int read_selected_item_from_widget(PtWidget_t *widget)
{
      PtArg_t args[1];
      short int *index;
  
      PtSetArg(&args[0], Pt_ARG_CBOX_SEL_ITEM, &index, 0);
      PtGetResources(widget, 1, args);
      return(*index);
}

/* utility function for sending command to the output port */
int SendMessage(YAPOCMessage out_msg)
{
    outCommandPort.Content() = out_msg;
    outCommandPort.Write();
    
    printf("Sending command type = %d\n ",out_msg.type);
  

  return 1;
  }

/* utility function for reading from input port */
K4JointPos ReadInput(){
  inPositionPort.Read(1);
  return inPositionPort.Content();
}

/* utility function for registering input and output ports */
int
Connect2Port()

	{

	outCommandPort.Register("/TestGUI/out/command");
	inPositionPort.Register("/TestGUI/in/position");
	return 1;

	}

/* initialization function called automatically by abmain */
int Initialize( int argc, char *argv[]){
  Connect2Port();
  
  //wait until at least one client is connected to output port
  while (outCommandPort.IsReceiving() <= 0){
    YARPTime::DelayInSeconds(1);
  }
return 1;

}
