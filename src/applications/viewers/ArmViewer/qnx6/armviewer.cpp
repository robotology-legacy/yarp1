
#include <stdio.h>
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPDIBConverter.h>
#include <YARPLogpolar.h>
#include <iostream>


#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>
#include <YARPImageFile.h>
#include <YARPControlBoardNetworkData.h>

#define rad2deg 57.32484076

int main(int argc, char argv[])
{

	YARPInputPortOf<YARPControlBoardNetworkData> _armStatusPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPControlBoardNetworkData _arm_status;

	_armStatusPort.Register("/armstatus/i","Net0");

	while(1)
	{
		_armStatusPort.Read();
		_arm_status = _armStatusPort.Content();

		//cout << _arm_status._current_position;
		
		ACE_OS::fprintf (stdout, "Current Position: %f %f %f %f %f %f \r", 
								  _arm_status._current_position(1)*rad2deg,
								  _arm_status._current_position(2)*rad2deg,
								  _arm_status._current_position(3)*rad2deg,
								  _arm_status._current_position(4)*rad2deg,
								  _arm_status._current_position(5)*rad2deg,
								  _arm_status._current_position(6)*rad2deg);
	}

}
