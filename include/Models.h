//
// Models.h
// 
// main include di support lib.
//

#ifndef __Modelsh__
#define __Modelsh__


#ifdef WIN32

#include <PDFilter.h>
#include <TwoDKalmanFilter.h>
#include <RecursiveLS.h>
#include <StaticLut.h>
#include <LookupTable.h>

#endif // WIN32

#ifdef __QNX__

#include <YARPSafeNew.h>

#include <PDFilter.h>
#include <TwoDKalmanFilter.h>
#include <RecursiveLS.h>
#include <StaticLut.h>
#include <LookupTable.h>
#include <LowPassFilter.h>

#endif	// __QNX__

#endif
