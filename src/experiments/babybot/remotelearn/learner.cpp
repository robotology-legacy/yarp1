#include "learner.h"

//////////////////////////////////////////////
// BatchDataSample
//

BatchDataSample::BatchDataSample()
{
	_inData = NULL;
	_outData = NULL;
}

BatchDataSample::BatchDataSample(const double *in, const double *out, int inS, int outS)
{
	_inData = new double [inS];
	_outData = new double [outS];

	memcpy(_inData, in, sizeof(double)*inS);
	memcpy(_outData, out, sizeof(double)*inS);
}
	
BatchDataSample::~BatchDataSample()
{
	CHECKANDDESTROY(_inData);
	CHECKANDDESTROY(_outData);
}

//////////////////////////////////////////////
// BatchData
//
BatchData::BatchData()
{
	_nSamples = 0;
	_inSize = 0;
	_outSize = 0;
}

BatchData::~BatchData()
{
	//
}

void BatchData::resize(int inS, int outS)
{
	lock();
	
	_destroy();

	_inSize = inS;
	_outSize = outS;
	
	unlock();
}

void BatchData::_destroy()
{
	YARPList<BatchDataSample *>::iterator it(*this);

	// delete objects (if any)
	while (!it.done())
	{
		delete (*it);
		it++;
	}

	clear();
}

void BatchData::add(double *input, double *output)
{
	lock();
	BatchDataSample *tmp = new BatchDataSample(input, output, _inSize, _outSize);

	push_back(tmp);
	_nSamples++;
	unlock();
}

void BatchData::add(YARPBottle &n)
{
	lock();
	// temp data
	double *tmpIn;
	double *tmpOut;
	tmpIn = new double [_inSize];
	tmpOut = new double [_outSize];

	int i;
	for(i = 0; i < _inSize; i++)
	{
		// extract input data sample
		n.readFloat(tmpIn+i);
	}
	for(i = 0; i < _outSize; i++)
	{
		// extract output (target) data samples
		n.readFloat(tmpOut+i);
	}

	BatchDataSample *tmp = new BatchDataSample(tmpIn, tmpOut, _inSize, _outSize);

	push_back(tmp);
	_nSamples++;

	delete [] tmpIn;
	delete [] tmpOut;

	unlock();
}

void BatchData::get(double **input, double **output, int *ns)
{
	lock();
	// alloca input and output
	double *tmpIn = new double [_inSize*_nSamples];
	double *tmpOut = new double [_outSize*_nSamples];
	*input = tmpIn;
	*output = tmpOut;

	YARPList<BatchDataSample *>::iterator it(*this);

	int counter = 0;
	while (!it.done())
	{
		double *tmpVectIn = (*it)->_inData;
		double *tmpVectOut = (*it)->_outData;
		memcpy(tmpIn, (*it)->_inData, sizeof(double)*_inSize);
		memcpy(tmpOut, (*it)->_outData, sizeof(double)*_outSize);
		
		tmpIn+=_inSize;
		tmpOut+=_outSize;
		it++;
		counter++;
	}

	*ns = _nSamples;
	unlock();
}
//////////////////////////////////////////////
// Learner
//
Learner::Learner()
{
	_input = NULL;
	_target = NULL;
	_outputBottle = NULL;;
	_outPort = NULL;
	_busyTraining = false;
	_saveTrainSet = false;
	_saveConfigFile = false;
}

Learner::Learner(const YARPString &file):
YARPBPNNet(file.c_str())
{
	_input = NULL;
	_target = NULL;
	_outputBottle = NULL;;
	_outPort = NULL;
	_busyTraining = false;

	_saveTrainSet = false;
	_saveConfigFile = false;

	_samples.resize(getInputSize(), getOutputSize());
}

// destructor
Learner::~Learner()
{
	CHECKANDDESTROY(_input);
	CHECKANDDESTROY(_target);

	_trainSetFile.close();
}

int Learner::train(int nIt, bool init)
{
	int nSamples;
	MBPOptions net_options;

	lock();
	if (_busyTraining)
	{
		unlock();
		return YARP_FAIL;
	}
	_busyTraining = true;
	unlock();
		
	_samples.get(&_input, &_target, &nSamples);
	
	net_options.IterTh = nIt;
	net_options.DigTh = -1;
	net_options.GradTh = -1;

	if (init)
		YARPBPNNet::init( (unsigned) time(NULL) );

	YARPBPNNet::trainBatchInit(nSamples, net_options);

	// be really sure the thread quit
	Join();
	// clean thread state
	CleanState();
	// start another thread
	Begin();

	return YARP_OK;
}

int Learner::train(const YARPString &filename, int nIt, bool init)
{
	lock();
	YARPBPNNet::load(filename.c_str());
	unlock();

	_samples.resize(getInputSize(), getOutputSize());

	return train(nIt, init);
}

void Learner::Body()
{
	YARPBPNNet::trainBatch(_input, _target);

	CHECKANDDESTROY(_input);
	CHECKANDDESTROY(_target);

	send();

	if (_saveConfigFile)
		save(_outConfigFile.c_str());

	_busyTraining = false;
}

void Learner::add(YARPBottle &n)
{
	_samples.add(n);
	n.rewind();
	_dumpTrainSet(n);
}

int Learner::send()
{
	if ( (_outputBottle == NULL) || (_outPort == NULL) )
		return YARP_FAIL;

	_outputBottle->reset();
	_outputBottle->setID("NNET");
	
	YARPBPNNetState state;
	save(state);

	ACE_OS::printf("Sending result of training:");
	extract(state, *_outputBottle);
	_outPort->Content() = *_outputBottle;
	_outPort->Write(1);

	ACE_OS::printf("...done\n");

	return YARP_OK;
}