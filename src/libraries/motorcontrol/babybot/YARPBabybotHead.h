#ifndef __YARPBABYBOTHEAD__
#define __YARPBABYBOTHEAD__

// $Id: YARPBabybotHead.h,v 1.4 2003-09-04 09:32:58 babybot Exp $

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPMEIOnBabybotHeadAdapter.h"

typedef YARPGenericControlBoard<YARPMEIOnBabybotHeadAdapter, YARPBabybotHeadParameters> YARPBabybotHead;

#endif