#ifndef __YARPDEBUGUTILSHH__
#define __YARPDEBUGUTILSHH__

#include <yarp/YARPConfig.h>
#include <yarp/YARPMatrix.h>

// a set of utility functions to facilitate print (mainly debug purpose)
namespace YARPDebugUtils
{
	/** print a YVector to screen
	*@param v: vector to be printed
	*@param prefix: string displayed before vector (optional)
	*/
	void print(const YVector &v, const char *prefix = NULL);
};

#endif