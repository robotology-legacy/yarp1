// learningMachine.h : header file for learning machines
//

#ifndef __learningMachineh__
#define __learningMachineh__

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;

// -------------------------------------------------------

// little macros for easier (?) handling of for over arrays

#define foreach(limit,index) unsigned int index; for ( index=0; index<limit; ++index )
#define foreach_s(start,limit,index) unsigned int index; for ( index=start; index<limit; ++index )

// allocating memory

template <class T> void lMAlloc ( T*& base, unsigned int num_elem )
{

	base = new T[num_elem];

	if ( base == 0 ) {
		cout << "FATAL ERROR (learningMachine): no memory." << endl;
		exit(-1);
	}

}

// -------------------------------------------------------
// a sample (borrowed from libsvm). The use of an index for each sample value
// eases handling of sparse matrices.

struct sample {
	int index;
	double value;
};

// -------------------------------------------------------
// definition: an "example" is a pair (sample,value) where sample is in R^n and value is in R^m.
// seems excessive to define a new struct/class for an example.

// -------------------------------------------------------
// a generic (regression) learning machine: a map (R^m -> R^n).
// it stores examples and can normalise/un-normalise them.

class LearningMachine {
public:

	// initialise with the domain size, the codomain size,
	// the maximum number of examples it can hold and the file name
	LearningMachine( unsigned int, unsigned int, unsigned int, string& );
	~LearningMachine( void );

	// viewing counters
	const unsigned int getExampleCount( void ) const { return _exampleCount; }
	const unsigned int getNumOfExamples( void ) const { return _numOfExamples; }
	// resetting the machine, loading and saving models and stats
	virtual void reset( void );
	virtual void save( void );
	virtual const bool load( void );
	// normalising
	void evalStats( void );
	void normalise( double*, const double, const double );
	void unNormalise( double*, const double, const double );
	void normaliseExamples( void );

	// abstract methods. any concrete learning machine must be able at least
	// to add an example, train its models and predict a new value given a sample
	virtual const bool addExample( const double[], const double[] ) = 0;
	virtual void train( void ) = 0;
	virtual void predictValue( const double[], double[] ) = 0;

protected:

	// sizes of the domain and codomain
	unsigned int _domainSize;
	unsigned int _codomainSize;
	// maximum number of samples
	unsigned int _numOfExamples;
	// currently stored normalised samples
	unsigned int _normalExampleCount;
	// currently stored samples
	unsigned int _exampleCount;

	// the examples
	sample** _sample;
	double** _value;
	// the sample whose value to predict
	sample* _newSample;

	// the means and stdvs
	double* _domainMean, * _codomainMean;
	double* _domainStdv, * _codomainStdv;

	// file name for saving and loading the machine state
	string _machineFileName;

};

#endif
