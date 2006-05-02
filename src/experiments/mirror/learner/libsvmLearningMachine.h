// libsvmLearningMachine.h : header file for libsvm-based SVM learning machines
//

#ifndef __libsvmLearningMachineh__
#define __libsvmLearningMachineh__

#include "learningMachine.h"
#include "libsvm.h"

// -------------------------------------------------------
// libsvm learning machine
// -------------------------------------------------------

template <class NORMALISER>
class libsvmLearningMachine : public LearningMachine<NORMALISER> {
public:

    class paramsType : public LearningMachine<NORMALISER>::paramsType {
    public:
        paramsType() {
	        // default libsvm parameters
    	    _svmparams.svm_type = EPSILON_SVR;
        	_svmparams.kernel_type = RBF;
	        _svmparams.degree = 3;
	        _svmparams.gamma = 1/(double)_domainSize;
	        _svmparams.coef0 = 0;
	        _svmparams.nu = 0.5;
	        _svmparams.cache_size = 10;
	        _svmparams.C = 1;
	        _svmparams.eps = 1e-3;
	        _svmparams.p = 0.1;
	        _svmparams.shrinking = 1;
	        _svmparams.probability = 0;
	        _svmparams.nr_weight = 0;
	        _svmparams.weight_label = 0;
	        _svmparams.weight = 0;
        }
        // parameters of the libsvm SVM machine
        svm_parameter _svmparams;
    };

//  libsvmLearningMachine( void ) {}
//	libsvmLearningMachine( unsigned int capacity, unsigned int domainSize, string name )
//        : LearningMachine<class NORMALISER>(capacity, domainSize, name) {}
	libsvmLearningMachine( paramsType& );
	~libsvmLearningMachine( void );

	void reset( void );
	void save( void );
	bool load( void );

	bool addExample( const real[], const real );
	void train( void );
	real predict( const real[] );

	virtual bool isExampleWorthAdding ( const real[], const real );

private:

    // libsvm parameters
	paramsType _params;
	// libsvm "problem"
	svm_problem _problem;
	// libsvm model pointer
	svm_model* _model;

};

#include "libsvmLearningMachine.cpp"

#endif
