#ifndef __YARPEURBOTHEAD__
#define __YARPEUROBOTHEAD__

// $Id: YARPEurobotHead.h,v 1.2 2003-05-28 16:04:59 beltran Exp $

#include <conf/YARPConfig.h>
#include <YARPGenericComponent.h>
#include "YARPGALILOnEurobotHeadAdapter.h"

typedef YARPGenericComponent<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters> YARPEurobotHead;

//typedef YARPGALILOnEurobotHeadAdapter YARPEurobotHead;

#endif