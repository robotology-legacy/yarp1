
#ifndef _MYCOMMANDS_H_
#define _MYCOMMANDS_H_


#include "YARPPort.h"
//#include "YAPOC-msg.h"
#include "YAPOCmsg.h"
#include "K4JointPos.h"


#define N_AXIS 30       //total number of motors
#define N_VIEWERS 4    //number of individual viewers


int Connect2Port();
int SendMessage(YAPOCMessage msg);
K4JointPos ReadInput();
int initialize_slider();
int initialize_onoff();
void initialize_trends();
int read_integer_from_widget(PtWidget_t *widget);
float read_value_from_slider(PtWidget_t *widget);
int read_state_from_onoff(PtWidget_t *widget);
double read_double_from_widget(PtWidget_t *widget);
char *read_text_from_widget(PtWidget_t *widget);
void write_text_to_widget(PtWidget_t *widget, char* text, PgColor_t color);
void write_integer_to_widget(PtWidget_t *widget, int val);
void write_state_to_onoff(PtWidget_t *widget, short newstate);
int read_selected_item_from_widget(PtWidget_t *widget);

#endif /* _PIC_MSG_H_ */

