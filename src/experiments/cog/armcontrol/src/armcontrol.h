/* Header "armcontrol.h" for armcontrol Application */

#ifndef __armcontrolh__
#define __armcontrolh__

#include <stdio.h>
#include <sys/sched.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

#include <VisDMatrix.h>
#include <MotorCtrl.h>
#include <YARPRateThread.h>
#include <YARPThread.h>
#include <YARPTime.h>

#include "YARPPort.h"
#include "classes.h"

// globals
extern YARPArmControl   *arm;
extern CCommandReceiver *thread;
extern CEncoderThread   *ethread;
extern CTorsoController *tthread;
extern double           *actual_pos;
extern double           *zero_pos;

// proto
int startall (void);


#define LEFTMINAXIS 2
#define LEFTMAXAXIS 8

#define CONNECTBODYSCRIPTNAME "connect-body"

#endif