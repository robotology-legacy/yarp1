#include "armmap.h"
#include <yarp/YARPDebugUtils.h>

ArmMap::ArmMap(const char *reachingNNet, const char *handlocNNet):
_headKinematics(YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
YARPInputPortOf<YARPBabyBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
_outPortRemoteLearn(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP)
{
	char *root = GetYarpRoot();
	char filename[256];
	
	// load nnets
	ACE_OS::sprintf (filename, "%s/conf/babybot/%s", root, reachingNNet);
	
	if (_nnet.load(filename)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename);
		exit(-1);
	}
	/*	
	if (_rfnet.LoadNet("reachingRFWR.ini","y:/conf/babybot/")!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read rfwr ini file %s", "reachingRFWR.ini");
		exit(-1);
	}*/

	ACE_OS::printf("Read neural network from %s\n", filename);

	ACE_OS::sprintf (filename, "%s/conf/babybot/%s", root, handlocNNet);
	if (_fkinematics.load(filename) != YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename);
		exit(-1);
	}
	ACE_OS::printf("Read neural network from %s\n", filename);

	// register input
	Register("/reaching/nnet/i");
	_outPortRemoteLearn.Register("/reaching/nnet/o");

	_outPortArmTrajectory.Register("/reaching/armtrajectory/o");

	_noise.resize(__nJointsArm, __maxRnd, __minRnd);
	_command.Resize(__nJointsArm);
	_prepare.Resize(__nJointsArm);

	_prepare = YVector(__nJointsArm, __preparePosition);

	_trajectoryLength = __trajectoryLength;
	_trajectory = new YVector[_trajectoryLength];
	for(int i = 0; i<_trajectoryLength; i++)
	{
		_trajectory[i].Resize(__nJointsArm);
		_trajectory[i] = 0.0;
	}
	
	_nUpdated = 0;
	_mode = reaching;
}

ArmMap::~ArmMap()
{

}

bool ArmMap::query(const YVector &arm, const YVector &head)
{
	_headKinematics.update(head);
	// _fkinematics.update(arm, head);

	const Y3DVector &cart = _headKinematics.fixationPolar();
	Y3DVector tmp;
	tmp = cart;

	if (!_checkReachability(cart))
		return false;

//	tmp(1) = tmp(1) + __azimuthOffset;
//	tmp(2) = tmp(2) + __elevationOffset;
//	tmp(3) = tmp(3) + __distanceOffset;

	int x, y;	//retinal position (for closed loop)
	x = 114;
	y = 134;

	if (_mode == atnr)
		_command = _atr.query(head) + _noise.query();
	else if (_mode == learning)
	{
//		_nnet.sim(tmp.data(), _command.data());
		_command = _command + _noise.query();
	}
	else
	{
		_nnet.sim(tmp.data(), _command.data());
		YVector tmpY(3);
	//	_rfnet.Simulate(tmp, 0.001, tmpY);
	//	_command(1) = tmpY(1);
	//	_command(2) = tmpY(2);
	//	_command(3) = tmpY(3);

		_fkinematics.update(_command, head);
		_fkinematics.computeJacobian(x,y);		// compute from center
		YVector tmpArm(6);
		tmpArm(1) = _command(1);
		tmpArm(2) = _command(2);
		tmpArm(3) = _command(3);
	//	tmpArm(1) = _command(1);	 	//copy 1 joint from map
	//	tmpArm(2) = arm(2);
	//	tmpArm(3) = arm(3);
	//	tmpArm(4) = arm(4);
	//	tmpArm(5) = arm(5);
	//	tmpArm(6) = arm(6);

	//	_command = _fkinematics.computeCommandThreshold(tmpArm , x, y);	// to center
	//	_sendTrajectory();
		
		_formTrajectory(_command);
	}

	return true;
}

void ArmMap::learn(const YVector &head, const YVector &arm)
{
	// implement here learning
	// convert head to x,y,z
	_bottle.reset();
		
	_headKinematics.update(head);
	const Y3DVector &cart = _headKinematics.fixationPolar();

	/////////// prepare data
	// first three elements are cart position
	int i;
	for(i = 1; i <= 3; i++)
		_bottle.writeFloat(cart(i));
	
	// prepate data -- then there comes the arm position  
	for(i = 1; i <= 3; i++)
		_bottle.writeFloat(arm(i));
	
	_outPortRemoteLearn.Content() = _bottle;
	_outPortRemoteLearn.Write(1);
}

// callback
void ArmMap::OnRead(void)
{
	ACE_OS::printf("ArmMap: loading new nnet");
	Read();
	YARPBPNNetState tmp;
	extract(Content(), tmp);
	load(tmp);
	_nUpdated++;
	if (_mode == atnr)
		_mode = learning;
	ACE_OS::printf("..done!\n");
}


void ArmMap::_formTrajectory(const YVector &cmd)
{
	// first move
	_trajectory[0] = _command;
	_trajectory[0](1) = _command(1)+__shoulderOffset1;
	_trajectory[0](2) = _command(2)+__armOffset1;
	_trajectory[0](3) = _command(3)+__foreArmOffset1;
	_trajectory[0](4) = __wrist1;
	_trajectory[0](5) = __wrist2;
	_trajectory[0](6) = __wrist3;

	// actual reaching
	_trajectory[1] = _command;
	_trajectory[1](1) = _command(1)+__shoulderOffset2;
	_trajectory[1](2) = _command(2)+__armOffset2;
	_trajectory[1](3) = _command(3)+__foreArmOffset2;
	_trajectory[1](4) = __wrist1;
	_trajectory[1](5) = __wrist2;
	_trajectory[1](6) = __wrist3;

	// go back
	_trajectory[2] = _trajectory[0];
}

const YVector &ArmMap::getCommand(int index)
{
	if (index < 0)
		index = 0;
	else if (index>_trajectoryLength)
		index = _trajectoryLength;
	
	return _trajectory[index];
}

void ArmMap::_sendTrajectory()
{
	YARPBabyBottle &content = _outPortArmTrajectory.Content();

	content.reset();
	const Trajectories &tr = _fkinematics.getTrajectory();

	content.writeInt(tr.size);
	content.writeInt(tr.length);
	for(int i = 0; i < tr.length; i++)
	{
		content.writeInt(tr.pixels[i][0]);	//x
		content.writeInt(tr.pixels[i][1]);	//y
		content.writeYVector(tr.arm[i]);		//arm command
	}

	printf("Sending trajectory...");
	_outPortArmTrajectory.Write(1);
	printf("done !\n");
}

bool ArmMap::_checkReachability(const Y3DVector &cart)
{
	bool ret = true;

	YARPDebugUtils::print(cart);

	ret = ret && (cart(1) < 0.4) && (cart(1) > -0.4);	// az
	ret = ret && (cart(2) < 0.4) && (cart(2) > -0.6);	// el
	ret = ret && (cart(3) < 660) && (cart(3) > -280);	// dist

	return ret;
}