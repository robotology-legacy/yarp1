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

	// register input
	Register("/reaching/nnet/i");
	_outPortRemoteLearn.Register("/reaching/nnet/o");

	_noise.resize(__nJointsArm, __maxRnd, __minRnd);
	_command.Resize(__nJointsArm);

	_nUpdated = 0;
}

ArmMap::~ArmMap()
{

}

const YVector &ArmMap::query(const YVector &head)
{
	// read head position
	if (_nUpdated == 0)
		_command = _atr.query(head) + _noise.query();
	else
	{
		_nnet.sim(head.data(), _command.data());
		_command = _command + _noise.query();
	}
	
	return _command;
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
	ACE_OS::printf("..done!\n");
}
