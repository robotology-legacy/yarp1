#include "functionList.h"
#include <yarp/YARPVocab.h>

const int __armJoints = 6;
const int __headJoints = 5;
const int __handJoints = 6;

// just send a vocab
bool sendSimpleVocab(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	return true;
}

// send a vocab command followed by a YVector (arm)
bool sendArmJointCommand(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	cout << "Arm command requires now " << __armJoints << " joint position (grad)\n";
	YVector cmd;
	cmd.Resize(__armJoints);
	for (int i = 0; i < __armJoints; i++)
	{
		cin >> cmd(i+1);
		cmd(i+1) = cmd(i+1)*degToRad;
	}

	bottle.writeYVector(cmd);
	return true;
}

// send a vocab command followed by a YVector (head)
bool sendHeadJointCommand(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	cout << "Head command requires now " << __headJoints << " joint position (grad)\n";
	YVector cmd;
	cmd.Resize(__headJoints);
	for (int i = 0; i < __headJoints; i++)
	{
		cin >> cmd(i+1);
		cmd(i+1) = cmd(i+1)*degToRad;
	}

	bottle.writeYVector(cmd);
	return true;
}

// send a vocab command followed by a YVector (hand)
bool sendHandJointCommand(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	cout << "Hand command requires now " << __handJoints << " joint position (grad)\n";
	YVector cmd;
	cmd.Resize(__handJoints);
	for (int i = 0; i < __handJoints; i++)
	{
		cin >> cmd(i+1);
		cmd(i+1) = cmd(i+1)*degToRad;
	}

	bottle.writeYVector(cmd);
	return true;
}

// send a vocab, ask for two double
bool sendHandKinematicsView(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	cout << "Command requires now two double (azimuth, elevation)\n";
	YVector cmd;
	cmd.Resize(2);
	cin >> cmd(1);
	cin >> cmd(2);
		
	bottle.writeYVector(cmd);
	return true;
}

bool sendString(const YBVocab &vocab, YARPBottle &bottle)
{
	bottle.writeVocab(vocab);
	cout << "Command now requires a string\n";
	YARPString tmp;
	cin >> tmp;

	bottle.writeText(tmp.c_str());
	return true;
}
