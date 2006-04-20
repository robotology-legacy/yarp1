// SVMLearningMachine.h : header file for SVM learning machines
//

#ifndef __SVMLearningMachineh__
#define __SVMLearningMachineh__

// -------------------------------------------------------
// a plain support vector machine, based upon libsvm.

#include <sstream>
#include "learningMachine.h"
#include "libsvm.h"

class SVMLearningMachine : public LearningMachine {
public:

	SVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string& );
	SVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string&, svm_parameter );
	~SVMLearningMachine( void );

	void changeC( const double );
	
	void reset( void );
	void save( void );
	const bool load( void );

	const bool addExample( const double[], const double[] );
	void train( void );
	void predictValue( const double[], double[] );

	void manageEpoch( void );

private:
	
	virtual const bool isExampleWorthAdding ( const double[], const double[] );

	// Classification?
	bool _classification;
	// one SVM problem for each dimension of the codomain
	svm_problem* _problem;
	// svm parameters
	svm_parameter _param;
	// one model pointer for each problem (each model is then created by svm_train())
	svm_model** _model;
	// epoch management
	bool _epoch;

};

// -------------------------------------------------------
// a (hopefully) better learning machine based upon libsvm.
// this one tries to sample the domain in a more sensible (uniform) way.

class UniformSVMLearningMachine : public SVMLearningMachine {
public:

	// constructor and destructor
	UniformSVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string&, double[] );
	UniformSVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string&, double[], svm_parameter );
	~UniformSVMLearningMachine( void );

private:

	const bool isExampleWorthAdding ( const double[], const double[] );

	// for each dimension of the codomain, we set a tolerance:
	// examples within tolerance of any already-stored example will be rejected
	double* _tolerance;

};

// -------------------------------------------------------
// another (hopefully) better learning machine based upon libsvm.
// this one tries to sample the domain where the prediction error is high;
// in other words, it gets some feedback about how good it is so far.

class FBSVMLearningMachine : public SVMLearningMachine {
public:

	// constructor and destructor
	FBSVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string& );
	FBSVMLearningMachine( bool, bool, unsigned int, unsigned int, unsigned int, string&, svm_parameter );
	~FBSVMLearningMachine( void );

private:

	const bool isExampleWorthAdding ( const double[], const double[] );

};

#endif
