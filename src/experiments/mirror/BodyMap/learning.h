// learning.h : header file for learning machines
//

#include "libsvm.h"

// little macro for easier (?) handling of for over arrays
#define foreach(limit,index) unsigned int index; for ( index=0; index<limit; ++index )

// a sample (borrowed from libsvm)
struct sample {
	int index;
	double value;
};

// -------------------------------------------------------
// a generic learning machine: a map (R^m -> R^n)

// it stores pairs (sample,value) and can normalise/un-normalise them
// any child to this class must be able at least to accept a new pair
// (sample,value), train and predict.

class LearningMachine {
public:

	// initialise with the domain size (m), the codomain size (n)
	// and the maximum number of pairs (sample,value) it can hold (NumOfSamples)
	LearningMachine( unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples );
	~LearningMachine();

	// viewing counters
	const unsigned int getSampleCount() const { return _sampleCount; }
	const unsigned int getNumOfSamples() const { return _numOfSamples; }
	// normalising
	void LearningMachine::evalStats(void);
	void LearningMachine::normalise(void);

	// abstract methods
	virtual const bool addSample( const double[], const double[] ) = 0;
	virtual void train() = 0;
	virtual void predict ( const double[], double[] ) const = 0;

protected:

	// sizes of the domain and codomain (m,n) for a map (R^m -> R^n)
	unsigned int _domainSize;
	unsigned int _codomainSize;
	// maximum number of samples
	unsigned int _numOfSamples;
	// currently stored normalised samples
	unsigned int _normalSampleCount;
	// currently stored samples
	unsigned int _sampleCount;

	// the samples and their values
	sample** _sample;
	double** _value;
	// the sample to be predicted
	sample* _newSample;

	// the means and stdvs
	double* _domainMean, * _codomainMean;
	double* _domainStdv, * _codomainStdv;

};

// -------------------------------------------------------
// a plain learning machine based upon LIBSVM.

class SVMLearningMachine : public LearningMachine {
public:

	// constructor and destructor
	SVMLearningMachine( unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples );
	~SVMLearningMachine();

	// methods
	const bool addSample( const double[], const double[] );
	void train();
	void predict ( const double[], double[] ) const;

private:

	// one SVM problem for each dimension of the codomain
	svm_problem* _problem;
	// svm parameters: for now, common to all problems
	svm_parameter _param;
	// one model pointer for each problem (each model is then created by svm_train())
	svm_model** _model;

};

// -------------------------------------------------------
// a (hopefully) better learning machine based upon LIBSVM.

// this one tries to sample the domain in a more sensible way.

class UniformLearningMachine : public SVMLearningMachine {
public:

	// constructor and destructor
	UniformLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples);

	const bool addSample( const double[], const double[] );
	const bool isSampleWorthAdding ( const double[] );
	void resetSpaceGrid();

private:

	bool _spaceGrid[21][21][21];
	double _centerOfTheWorld[3];

};
