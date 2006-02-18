// learning.h : header file for a generic learning machine
//

#include "libsvm.h"

// little macro for easier (?) handling of for over arrays
#define foreach(limit,index) unsigned int index; for ( index=0; index<limit; ++index )

// a learning machine. construct it with the dimension of the domain, the dimension
// of the codomain and the number of samples stored.

class BodyMapLearningBlock {
public:

	// constructor and destructor
	BodyMapLearningBlock(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples);
	~BodyMapLearningBlock();

	// methods
	const unsigned int getSampleCount() const { return _sampleCount; }
	const unsigned int getNumOfSamples() const { return _numOfSamples; }
	const bool addSample( const double[], const double[] );
	const bool isSampleWorthAdding ( const double[] ) const;
	void train();
	void predict ( const double[], double[] ) const;

private:

	// sizes of the domain and codomain
	unsigned int _domainSize;
	unsigned int _codomainSize;
	// number of samples, and of currently stored samples
	unsigned int _numOfSamples;
	unsigned int _sampleCount;

	// the samples, their values, and the means and stdvs for each dimension.
	// NOTICE: this stuff is dynamically allocated...
	svm_node** _sample;
	double** _value;
	double* _domainMean, * _codomainMean;
	double* _domainStdv, * _codomainStdv;

	// svm parameters: for now, common to all problems
	svm_parameter _param;
	// one SVM problem for each dimension of the codomain
	svm_problem* _problem;
	// one model pointer for each problem (each model is then
	// created by svm_train()
	svm_model** _model;
	// the sample to be predicted
	svm_node* _newSample;

};
