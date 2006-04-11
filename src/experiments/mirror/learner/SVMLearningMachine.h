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

	SVMLearningMachine( bool, unsigned int, unsigned int, unsigned int, string& );
	SVMLearningMachine( bool, unsigned int, unsigned int, unsigned int, string&, svm_parameter );
	~SVMLearningMachine( void );

	void changeC( const double );
	
	void reset( void );
	void save( void );
	const bool load( void );

	const bool addExample( const double[], const double[] );
	void train( void );
	void predictValue( const double[], double[] );

private:
	
	// Classification?
	bool _classification;
	// one SVM problem for each dimension of the codomain
	svm_problem* _problem;
	// svm parameters
	svm_parameter _param;
	// one model pointer for each problem (each model is then created by svm_train())
	svm_model** _model;

};

// -------------------------------------------------------
// a (hopefully) better learning machine based upon libsvm.
// this one tries to sample the domain in a more sensible (uniform) way.

class UniformSVMLearningMachine : public SVMLearningMachine {
public:

	// constructor and destructor
	UniformSVMLearningMachine( bool, unsigned int, unsigned int, unsigned int, string&, double[] );
	UniformSVMLearningMachine( bool, unsigned int, unsigned int, unsigned int, string&, double[], svm_parameter );
	~UniformSVMLearningMachine( void );

	const bool addExample( const double[], const double[] );
	const bool isExampleWorthAdding ( const double[] );

private:

	// for each dimension of the codomain, we set a tolerance:
	// examples within tolerance of any already-stored example will be rejected
	double* _tolerance;

};

#endif
