#include "armmap.h"

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

void ArmMap::query(const YVector &arm, const YVector &head)
{
	_headKinematics.update(head);
	_fkinematics.update(arm, head);

	const Y3DVector &cart = _headKinematics.fixationPolar();
	Y3DVector tmp;
	tmp = cart;
//	tmp(1) = tmp(1) + __azimuthOffset;
//	tmp(2) = tmp(2) + __elevationOffset;
//	tmp(3) = tmp(3) + __distanceOffset;

	int x, y;	//retinal position (for closed loo)
	x = 128;
	y = 128;


	if (_mode == atnr)
		_command = _atr.query(head) + _noise.query();
	else if (_mode == learning)
	{
		_nnet.sim(tmp.data(), _command.data());
		_command = _command + _noise.query();
	}
	else
	{
		_nnet.sim(tmp.data(), _command.data());
		_fkinematics.computeJacobian(x,y);		// compute from center
		YVector tmpArm(6);
		tmpArm(1) = _command(1);	 	//copy 1 joint from map
		tmpArm(2) = arm(2);
		tmpArm(3) = arm(3);
		tmpArm(4) = arm(4);
		tmpArm(5) = arm(5);
		tmpArm(6) = arm(6);

		// overwrite command
		printf("Query with arm:\n");
		for(int i = 1; i <= 3; i++)
			printf("%lf\t", tmpArm(i));
			
		printf("\nx: %d\ty: %d\n", x, y);

		_command = _fkinematics.computeCommandThreshold(tmpArm , x, y);	// to center

		// DEBUG, dump current command
		_formTrajectory(_command);
	}
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
	_trajectory[0](4) = __wrist1;
	_trajectory[0](5) = __wrist2;
	_trajectory[0](6) = __wrist3;

	// actual reaching
	_trajectory[1] = _command;
	_trajectory[1](1) = _command(1) +__shoulderOffset2;
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