//
// Simulates an iplHeader structure.
//

#ifndef __FakeIplh__
#define __FakeIplh__

#ifdef __QNX__

#include <YARPSafeNew.h>
#include <YARPBool.h>
#include <ipl.h>

// to fool a few Windows declarations.
typedef int HDC;
typedef char _TCHAR;

// my personal prototypes
int _iplCalcPadding (int lineSize, int align);

#endif

#ifdef WIN32
#include <ipl.h>
//#include <iplutil.h>
#endif

#ifdef __LINUX__
#include <ipl.h>
//#include <iplutil.h>
#endif


#endif
