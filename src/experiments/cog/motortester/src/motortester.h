#include <stdio.h>
#include <stdlib.h>

#include "meid.h"
#include "mei/pcdsp.h"

#define FALSE 0
#define TRUE  1

extern int meidx;
extern int current_axis;
extern int16 gains[COEFFICIENTS];
extern void draw_gains();
extern char errmsg[100];

/*
 * globals for reading from widgets (in initialize.c)
 */
extern int read_integer_from_widget(PtWidget_t *widget);
extern double read_double_from_widget(PtWidget_t *widget);
extern char *read_text_from_widget(PtWidget_t *widget); 
extern int read_selected_item_from_widget(PtWidget_t *widget);
extern void enable_widget(PtWidget_t *widget_name);
extern void disable_widget(PtWidget_t *widget_name);

/*
 * globals for triggering 
 */
extern int trigger_enabled;
extern int trigger_time;
extern int trigger_fired;
extern void remove_trigger(int update_button_state);
extern void set_trigger(int update_button_state);

/*
 * globals for repeat movements
 */
extern int repeat_enabled;
extern int repeat_next_goal;
extern int repeat_goal1;
extern int repeat_goal2;
extern double repeat_vel;
extern double repeat_accel;
extern void remove_repeat(int update_button_state);
extern void set_repeat(int update_button_state);
#define REPEAT_GOAL_NONE 0
#define REPEAT_GOAL_1    1
#define REPEAT_GOAL_2    2


/*
 * need to know the device name to connect
 * (this should be a command line parameter later).
 */
#define DEFAULTDEVICENAME "//4/dev/mei0x320"
extern char DEVICENAME[256];
