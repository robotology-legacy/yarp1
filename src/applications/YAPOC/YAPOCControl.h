
#ifndef _YAPOC_CONTROL_H_
#define _YAPOC_CONTROL_H_

#include "YARPImageServices.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPImagePortContent.h"
#include "temporary/YARPConverter.h"
#include <YARPRateThread.h>

#include "YAPOCmsg.h"
#include "K4JointPos.h"
#include "K4MotorCtrl.h"
#include "ClientPIC.h"
#include "conf/K4_data.h"

//axismap array element index
#define AXISTYPE 0
#define AXISNUMBER 1

//axis type
#define MEITYPE 0
#define PICTYPE 1

//move type for JRKERR motors, we're only using ABSOLUTE MOVE
#define ABSOLUTE_MOVE 0
#define RELATIVE_MOVE 1

//input params: mei process name, pic process name, config file etc.
#define MEI_INI_FILENAME "/ai/ai/mb/cdp/src/YARP/conf/K4head.ini"
#define MEI_DEVICENAME "//22/dev/mei0x320"
#define AXISMAPFILENAME "/ai/ai/mb/cdp/src/YARP/conf/K4axismap.ini"
#define PIC_INI_FILENAME "/ai/ai/mb/cdp/src/YARP/conf/PICini"
#define PIC_PROCNAME_BASE "/PIC"

//output position port name
#define POSITIONCHANNELNAME             "/YAPOC/out/position"

//used to create static array of CClientPIC
//thus, has to be >= actual number of PIC Daemons
#define MAX_PIC_DAEMONS 4

//status bits for position command
#define TOBEMOVED 1
#define STAY 0

typedef unsigned char byte;

#endif /* _PIC_MSG_H_ */

