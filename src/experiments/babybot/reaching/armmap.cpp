#include "armmap.h"

ArmMap::ArmMap(const char *nnetFile):
_headKinematics(YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
_outPortRemoteLearn(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP)
{
	char *root = GetYarpRoot();
	char filename1[256];
	
	// load nnets
	ACE_OS::sprintf (filename1, "%s/conf/babybot/%s", root, nnetFile);
	if (_nnet.load(filename1)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename1);
		exit(-1);
	}
	ACE_OS::printf("Reading neural network from %s\n", filename1);

	// register input
	Register("/reaching/nnet/i");
	_outPortRemoteLearn.Register("/reaching/nnet/o");

	_noise.resize(__nJointsArm, __maxRnd, __minRnd);
	_command.Resize(__nJointsArm);
	_prepare.Resize(__nJointsArm);

	_prepare = 0;
	_prepare(1) = 8*degToRad;
	_prepare(4) = -70*degToRad;
	
	_nUpdated = 0;
	_mode = reaching;
}

ArmMap::~ArmMap()
{

}

void ArmMap::query(const YVector &head)
{
	_headKinematics.update(head);
	const Y3DVector &cart = _headKinematics.fixation();
	
	if (_mode == atnr)
		_command = _atr.query(head) + _noise.query();
	else if (_mode == learning)
	{
		_nnet.sim(cart.data(), _command.data());
		_command = _command + _noise.query();
	}
	else
	{
		_nnet.sim(cart.data(), _command.data());
		_command(1) = _command(1) + 10*degToRad;
		_command(4) = -90*degToRad;
		_command(5) = 0;
		_command(6) = -90*degToRad;
	}
}

void ArmMap::learn(const YVector &head, const YVector &arm)
{
	// implement here learning
	// convert head to x,y,z
	_bottle.reset();
		
	_headKinematics.update(head);
	const Y3DVector &cart = _headKinematics.fixation();

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
