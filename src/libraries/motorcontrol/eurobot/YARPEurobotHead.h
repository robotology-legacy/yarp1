#ifndef __YARPEURBOTHEAD__
#define __YARPEUROBOTHEAD__

// $Id: YARPEurobotHead.h,v 1.3 2003-09-08 16:59:00 beltran Exp $

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPGALILOnEurobotHeadAdapter.h"

typedef YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters> YARPEurobotHead;

//typedef YARPGALILOnEurobotHeadAdapter YARPEurobotHead;

#endif
