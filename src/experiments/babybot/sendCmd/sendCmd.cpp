// sendCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <YARPPort.h>
#include <YARPBottleContent.h>
#include <YARPBottle.h>

YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

int main(int argc, char* argv[])
{
	_outPort.Register("/motorcmd/o");
	YARPBottle tmp;
	string c;
	while(cin>> c)
	{
		tmp.reset();
		bool send = false;
		tmp.setID(YBLabelMotor);
		if (c == "e")
			break;
		if (c == "start")
		{
			tmp.writeVocab(YBVArmRndStart);
			send = true;
		}
		else if (c == "stop")
		{
			tmp.writeVocab(YBVArmRndStop);
			send = true;
		}
		else if (c == "quit")
		{
			tmp.writeVocab(YBVExit);
			send = true;
		}
		else if (c == "alive")
		{
			tmp.writeVocab(YBVIsAlive);
			send = true;
		}
		else 
		{
			send = false;

		}

		if (send)
		{
			_outPort.Content() = tmp;
			_outPort.Write();
		}
	};	
	
	return 0;
}
