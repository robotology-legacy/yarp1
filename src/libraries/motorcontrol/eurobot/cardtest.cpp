//$id$

#include "YARPEurobotHead.h"
#include <YARPDeviceDriver.h>
#include <iostream>

#define DEBUG(string) YARP_DEBUG("GEN_COMPONENT_DEBUG :", string)


void testvelocity(int, double);
YARPEurobotHead _head;

int main(int argc, char * argv[])
{
	
	int error;
	
	error = _head.initialize();
	
	// calibration 
	_head.idleMode();
	// idle mode

	std::cout << "--> Head is idle. Calibrate it manually then press any key + return\n";
	
	char c;
	std::cin >> c;

	std::cout << "Ok, setting PID mode\n";
	
	_head.activatePID();
	
	testvelocity(0, 0.1);
	testvelocity(1, 0.1);
	testvelocity(2, 0.1);
	testvelocity(3, 0.1);
	
	
	//_head.setPositions(pos);
	
	
	_head.uninitialize();
	//SingleAxisParameters cmd;
	//cmd.axis = 0;
	//int value = 2000;
	//cmd.parameters = &value;
	//_head.IOCtl(CMDSetPosition, &cmd);
	
}

void testvelocity(int index, double speed)
{
	double pos[4] = {0.0,0.0,0.0,0.0};
	double pos2[4] = {0.0,0.0,0.0,0.0};
	
	pos[index] = speed;
	
	for (int i = 0; i < 2; i ++)
	{
		
	_head.velocityMove(pos);
	delay(2000);
	 _head.getPositions(pos2);
        printf(" Position: %f %f %f %f \n",pos2[0],pos2[1],pos2[2],pos[3]);
	pos[index] = -0.1;
	_head.velocityMove(pos);
	delay (2000);
	_head.getPositions(pos2);
        printf(" Position: %f %f %f %f \n",pos2[0],pos2[1],pos2[2],pos[3]);
	pos[index] = 0.1;
	}
	
	pos[index] = 0.0;
	_head.velocityMove(pos);
}
