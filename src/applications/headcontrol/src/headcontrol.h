/* Header "headcontrol.h" for headcontrol Application */

#ifndef __headcontrolh__
#define __headcontrolh__

// sys includes.
#include <stdio.h>
#include <sys/sched.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>
#include <conf/head_gain.h>

#include <VisDMatrix.h>
#include <MotorCtrl.h>
#include <YARPRateThread.h>
#include <YARPThread.h>
#include <YARPTime.h>

#include "YARPPort.h"

#include "portnames.h"

#include "closed_loop_eyes.h"
#include "vor_control.h"
#include "saccade.h"
#include "disparity_ctrl.h"

#include "channels.h"
#include "classes.h"

extern CControlThread	*thread;
extern YARPHeadControl	*head;
extern double		*encoder;

extern char INI_FILENAME[];
extern char MEI_DEVICENAME[];
extern char ATTN_CONNECT[];
extern char CTRL_CONNECT[];
extern char GYRO_NAME[];

// external components & scripts.
#define ATTENTIONCONNECTALL			"attention-connect-all"
#define ATTENTIONCONNECTCONTROL 	"attention-connect-control"
#define CONNECTBODYSCRIPTNAME		"connect-body"
#define GYROTESTER					"gyrotester"

// prototypes.
int startall (void);

#endif
