#ifndef __YARPBABYBOTHEAD__
#define __YARPBABYBOTHEAD__

// $Id: YARPBabybotHead.h,v 1.5 2003-10-17 16:34:40 babybot Exp $

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPMEIOnBabybotHeadAdapter.h"

class YARPBabybotHead: public YARPGenericControlBoard<YARPMEIOnBabybotHeadAdapter, YARPBabybotHeadParameters>
{
public:

};

#endif