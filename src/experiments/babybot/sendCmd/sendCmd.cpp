// sendCmd.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPBottleContent.h>
#include <YARPBottle.h>

YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

int main(int argc, char* argv[])
{
	_outPort.Register("/motorcmd/o");
	YARPBottle tmp;
	YARPString c;
	bool quit = false;
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
		else if (c == "exit")
		{
			quit = true;
			send = false;
		}
		else if (c == "alive")
		{
			tmp.writeVocab(YBVIsAlive);
			send = true;
		}
		else if (c == "hand")
		{
			tmp.writeVocab(YBVHandNewCmd);
			YVector cmd;
			cmd.Resize(6);
			for (int i = 0; i < 6; i++)
				cin >> cmd(i+1);

			tmp.writeYVector(cmd);
			send = true;
		}
		else if (c == "arm")
		{
			tmp.writeVocab(YBVArmNewCmd);
			YVector cmd;
			cmd.Resize(6);
			for (int i = 0; i < 6; i++)
			{
				cin >> cmd(i+1);
				cmd(i+1) = cmd(i+1)*degToRad;
			}

			tmp.writeYVector(cmd);
			send = true;
		}
		else if (c == "shakehand")
		{
			tmp.writeVocab(YBVHandShake);
			send = true;
		}
		else if (c == "shakearm")
		{
			tmp.writeVocab(YBVArmShake);
			send = true;
		}
		else if (c == "resethand")
		{
			tmp.writeVocab(YBVHandResetEncoders);
			send = true;
		}
		else if (c == "armrest")
		{
			tmp.writeVocab(YBVArmForceResting);
			send = true;
		}
		else if (c == "arminhibit")
		{
			tmp.writeVocab(YBVArmInhibitResting);
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
			printf("Sent:\n");
			tmp.display();
			printf("\n");
		}

		if (quit)
			return 0;
	};	
	
	return 0;
}
