// learning.h : header file for a generic learning machine
//

//
// TODO
//
// . potrer scrivere stat della distanza su file
// . uniform sampling !!
//

#include "libsvm.h"

// little macro for easier (?) handling of for over arrays
#define foreach(limit,index) unsigned int index; for ( index=0; index<limit; ++index )

// a generic learning machine. all learning machines inherit from it.
// to be refined later on: what methods must actually be covered in a
// child of this class?
class LearningMachine {
public:
	LearningMachine() { };
	~LearningMachine() { };

	virtual const unsigned int getSampleCount() const = 0;
	virtual const unsigned int getNumOfSamples() const = 0;
	virtual const bool addSample( const double[], const double[] ) = 0;
	virtual void train() = 0;
	virtual void predict ( const double[], double[] ) const = 0;

};

// a plain learning machine based upon LIBSVM.
// construct it with the dimension of the domain, the dimension
// of the codomain and the number of samples stored.

class SVMLearningMachine : public LearningMachine {
public:

	// constructor and destructor
	SVMLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples);
	~SVMLearningMachine();

	// methods
	const unsigned int getSampleCount() const { return _sampleCount; }
	const unsigned int getNumOfSamples() const { return _numOfSamples; }
	const bool addSample( const double[], const double[] );
	const bool isSampleWorthAdding ( const double[] ) const;
	void train();
	void predict ( const double[], double[] ) const;

protected:

	// number of samples, and of currently stored samples
	unsigned int _numOfSamples;
	unsigned int _sampleCount;
	// sizes of the domain and codomain
	unsigned int _domainSize;
	unsigned int _codomainSize;

	// the samples, their values, and the means and stdvs for each dimension.
	// NOTICE: this stuff is dynamically allocated...
	svm_node** _sample;
	double** _value;
	double* _domainMean, * _codomainMean;
	double* _domainStdv, * _codomainStdv;

	// one SVM problem for each dimension of the codomain
	svm_problem* _problem;

private:

	// svm parameters: for now, common to all problems
	svm_parameter _param;
	// one model pointer for each problem (each model is then created by svm_train())
	svm_model** _model;
	// the sample to be predicted
	svm_node* _newSample;

};

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
