#ifndef __YARPBABYBOTHEAD__
#define __YARPBABYBOTHEAD__

// $Id: YARPBabybotHead.h,v 1.2 2003-04-26 11:10:26 natta Exp $

#include <YarpGenericComponent.h>
#include "YARPMEIOnBabybotHeadAdapter.h"

typedef YARPGenericComponent<YARPMEIOnBabybotHeadAdapter, YARPBabybotHeadParameters> YARPBabybotHead;

#endif