//$id$

#include "YARPEurobotArm.h"
#include <YARPDeviceDriver.h>
#include <iostream>

#define DEBUG(string) YARP_DEBUG("GEN_COMPONENT_DEBUG :", string)


void testvelocity(int, double);
YARPEurobotArm _arm;

int main(int argc, char * argv[])
{
	
	int error;
	
	error = _arm.initialize();
	
	std::cout << "Ok, setting PID mode\n";
	
	_arm.activatePID();

	//_arm.calibrate();
	
	
	testvelocity(5, 0.1);
	testvelocity(4, 0.1);
	testvelocity(3, 0.1);
	testvelocity(2, 0.1);
	//double pos2[4] = {0.0,0.0,0.0,0.0};

	//while (1)
	//{

	 //_arm.getPositions(pos2);
     //   printf(" Position: %f %f %f %f \n",pos2[0],pos2[1],pos2[2],pos2[3]);
	 //			delay(2000);
	//}
	
	
	_arm.uninitialize();
	
}

void testvelocity(int index, double speed)
{
	double pos[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
	double pos2[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
	
	pos[index] = speed;
	
	for (int i = 0; i < 2; i ++)
	{
		
	_arm.velocityMove(pos);
	delay(2000);
	 _arm.getPositions(pos2);
        printf(" Position: %f %f %f %f \n",pos2[0],pos2[1],pos2[2],pos[3]);
	pos[index] = -0.1;
	_arm.velocityMove(pos);
	delay (2000);
	_arm.getPositions(pos2);
        printf(" Position: %f %f %f %f \n",pos2[0],pos2[1],pos2[2],pos[3]);
	pos[index] = 0.1;
	}
	
	pos[index] = 0.0;
	_arm.velocityMove(pos);
}
