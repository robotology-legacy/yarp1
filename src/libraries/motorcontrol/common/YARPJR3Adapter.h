// by nat June 2003

#ifndef __YARPJR3Adapterh__
#define __YARPJR3Adapterh__


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPJR3DeviceDriver.h>

typedef JR3OpenParameters YARPJR3Params;


class YARPJR3Adapter: public YARPJR3DeviceDriver
{
public:
	int initialize (YARPJR3Params& params)
	{
		/// need additional initialization, put it here.
		open ((void *)&params);

		IOCtl(CMDJR3ResetOffsets, NULL);
		IOCtl(CMDJR3SetMinFullScales, NULL);

		return YARP_OK;
	}

	int uninitialize (void)
	{
		/// need additional termination stuff, here's the place for it.
		return close ();
	}
};

#endif