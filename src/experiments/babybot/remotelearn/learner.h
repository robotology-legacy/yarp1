#ifndef __LEARNER__
#define __LEARNER__

#define CHECKANDDESTROY(pointer) if(pointer!=NULL) { delete [] pointer;\
														pointer = NULL;}

#include <yarp/YARPBPNNet.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPList.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPLogFile.h>

class BatchDataSample
{
public:
	BatchDataSample();
	BatchDataSample(const double *in, const double *out, int inS, int outS);
		
	~BatchDataSample();

	double *_inData;
	double *_outData;
};

class BatchData : public YARPList<BatchDataSample *>
{
public:
	BatchData();
	~BatchData();

	void resize(int inS, int outSize);
	void add(double *input, double *target);
	void add(YARPBottle &n);
	void get(double **input, double **output, int *ns);

	int howMany()
	{ return _nSamples; }

	inline void lock()
	{ _mutex.Wait(); }

	inline void unlock()
	{ _mutex.Post(); }
	
private:
	void _destroy();
	int _nSamples;
	int _inSize;
	int _outSize;
	YARPSemaphore _mutex;
};

class Learner: private YARPBPNNet, private YARPThread
{
public:
	// default constructor
	Learner();
	// construct nnet from file
	Learner(const YARPString &file);
	// destructor
	~Learner();

	void add(YARPBottle &n);

	virtual void Body(void);
	
	// start a train sequence
	// if a previous train is still to be finished returns
	// YARP_FAIL, returns YARP_OK otherwise
	int train(int nIt, bool init);
	// before train, load nnet file from disk
	// internally uses train(nIt, init)
	int train(const YARPString &filename, int nIt, bool init);

	int howMany()
	{ return _samples.howMany(); }

	inline void lock()
	{ _mutex.Wait(); }

	inline void unlock()
	{ _mutex.Post(); }

	void setOutputPort(YARPOutputPortOf<YARPBottle> *port, YARPBottle *b)
	{
		_outputBottle = b;
		_outPort = port;
	}

	// set file which will be used to append received train samples 
	void setTrainSetFile(const YARPString &file)
	{
		_trainSetFile.append(file);
		_saveTrainSet = true;
	}

	// set filename name to save output config file
	void setOutConfigFile(const YARPString &file)
	{
		_outConfigFile = file;
		_saveConfigFile = true;
	}

	int send();

private:

	void _dumpTrainSet(YARPBottle &b)
	{
		if (!_saveTrainSet)
			return;

		double tmp;

		int i,j;
		for(i = 0; i < n_input; i++)
		{
			b.readFloat(&tmp);
			b.moveOn();
			_trainSetFile.dump(tmp);
		}

		for(j = 0; j < n_output; j++)
		{
			b.readFloat(&tmp);
			b.moveOn();
			_trainSetFile.dump(tmp);
		}

		_trainSetFile.newLine();
		_trainSetFile.flush();
	}

	BatchData _samples;
	
	// local storage for training
	double *_input;
	double *_target;
	bool _busyTraining;
	YARPSemaphore _mutex;

	YARPBottle *_outputBottle;
	YARPOutputPortOf<YARPBottle> *_outPort;

	YARPString _initFile;
	YARPLogFile _trainSetFile;
	bool _saveTrainSet;
	YARPString _outConfigFile;
	bool _saveConfigFile;
};

#endif