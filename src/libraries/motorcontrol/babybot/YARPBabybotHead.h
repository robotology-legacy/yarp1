#ifndef __YARPBABYBOTHEAD__
#define __YARPBABYBOTHEAD__

// $Id: YARPBabybotHead.h,v 1.3 2003-05-21 13:29:10 natta Exp $

#include <conf/YARPConfig.h>
#include <YarpGenericComponent.h>
#include "YARPMEIOnBabybotHeadAdapter.h"

typedef YARPGenericComponent<YARPMEIOnBabybotHeadAdapter, YARPBabybotHeadParameters> YARPBabybotHead;

#endif