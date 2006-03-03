// SVMLearningMachine.cpp : behaviour of SVM learning machines
//

#include "SVMLearningMachine.h"

// ---------------------- SVM learning machine, plain version

SVMLearningMachine::SVMLearningMachine(
  unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples, string& MachineFileName
 ) : LearningMachine(DomainSize,CodomainSize,NumOfSamples,MachineFileName)
{

	// initialise SVM parameters
	_param.svm_type = EPSILON_SVR;
	_param.kernel_type = RBF;
	_param.degree = 3;
	_param.gamma = 1/(double)_domainSize;
	_param.coef0 = 0;
	_param.nu = 0.5;
	_param.cache_size = 10;
	_param.C = 1;
	_param.eps = 1e-3;
	_param.p = 0.1;
	_param.shrinking = 1;
	_param.probability = 0;
	_param.nr_weight = 0;
	_param.weight_label = 0;
	_param.weight = 0;

	// check them. the problem is not required unless we use NU-SVC,
	// which is never the case for us, so a NULL thing will be OK.
	const char* error_msg;
	error_msg = svm_check_parameter( 0, &_param );
	if ( error_msg != 0 ) {
		exit(0);
	}

	// allocate SVM problems
	_problem = new svm_problem[_codomainSize];
	// allocate and clean svm models pointers
	_model = new svm_model*[_codomainSize];
	{ foreach(_codomainSize,i) _model[i] = 0; }

	// initialise SVM problems:
	// (1) tell them how many _samples we use
	{ foreach(_codomainSize,i) _problem[i].l = _numOfExamples; }
	// (2) link their "y" field to values of the LearningMachine
	{ foreach(_codomainSize,i) _problem[i].y = &_value[i][0]; }
	// (3) create their "x"'s and link the "x" field to _samples
	{ foreach(_codomainSize,i) {
		_problem[i].x = new svm_node*[_numOfExamples+1];
		foreach(_numOfExamples,j) _problem[i].x[j] = (svm_node*) &_sample[j][0];
	} }
	// there you are. from now on you alter _samples[] and _value[], and
	// really you are touching the SVM problems.

}

SVMLearningMachine::~SVMLearningMachine()
{

	// delete models
	{ foreach(_codomainSize,i) if ( _model[i] != 0 ) svm_destroy_model( _model[i] ); }
	delete[] _model;
	// delete problems
	{ foreach(_codomainSize,i) delete[] _problem[i].x; }
	delete[] _problem;
	// delete sample to be predicted
	delete[] _newSample;
	// destroy machine parameters
    svm_destroy_param( &_param );

}

const bool SVMLearningMachine::addExample( const double x[], const double y[] )
{

	// unless the buffer is full,
	if ( _exampleCount == _numOfExamples ) {
		return false;
	} else {
		// add the new example, update counter and bail out.
		foreach(_domainSize,i) {
			_sample[_exampleCount][i].index = i+1;
			_sample[_exampleCount][i].value = x[i];
		}
		_sample[_exampleCount][_domainSize].index = -1;
		{ foreach(_codomainSize,i) _value[i][_exampleCount] = y[i]; }
		_exampleCount++;
		return true;
	}

}

void SVMLearningMachine::train()
{

	// destroy old models
	{ foreach(_codomainSize,i) if ( _model[i] != 0 ) svm_destroy_model( _model[i] ); }

	// normalise the whole pool of examples
	normaliseExamples();

	// train new models
	{ foreach(_codomainSize,i) _model[i] = svm_train( &_problem[i], &_param ); }

}
	
void SVMLearningMachine::predictValue( const double x[], double y[] )
{

	// if models are void, do not even try to predict, but return zeroes
	if ( _model[0] == 0 ) {
		foreach(_codomainSize,i) y[i] = 0.0;
		return;
	}

	// otherwise, build sample to predict (normalise it first)
	{ foreach(_domainSize,i) {
		_newSample[i].index = i+1;
		_newSample[i].value = x[i];
		normalise( &_newSample[i].value, _domainMean[i], _domainStdv[i] );
		}
		_newSample[_domainSize].index = -1;
	}

	// and then predict (un-normalise before bailing out)
	{ foreach(_codomainSize,i) {
		y[i] = svm_predict(_model[i], (svm_node*)_newSample);
		unNormalise( &y[i], _codomainMean[i], _codomainStdv[i] );
	} }

}

void SVMLearningMachine::reset( void )
{

	// destroy models and reset them to 0
	{ foreach(_codomainSize,i) {
		if ( _model[i] != 0 ) svm_destroy_model( _model[i] );
		_model[i] = 0;
	} }

	LearningMachine::reset();

}

void SVMLearningMachine::save( void )
{

	// save models
	{ foreach(_codomainSize,i) {
		char modelFileName[255];
		sprintf(modelFileName, "%s.%d.model", _machineFileName.c_str(), i);
		svm_save_model(modelFileName, _model[i]);
	} }

	LearningMachine::save();

}

const bool SVMLearningMachine::load( void )
{

	// load a (set of) previously saved model. if one of the models
	// or the stats fail to be found, set all models to 0

	{ foreach(_codomainSize,i) {
		char modelFileName[255];
		sprintf(modelFileName, "%s.%d.model", _machineFileName.c_str(), i);
		_model[i] = svm_load_model(modelFileName);
		if ( _model[i] == 0 ) {
			foreach(_codomainSize,j) _model[j] = 0;
			return false;
		}
	} }

	if ( LearningMachine::load() == false ) {
		foreach(_codomainSize,j) _model[j] = 0;
		return false;
	}

	return true;

}

// ---------------------- SVM learning machine, uniform version

UniformSVMLearningMachine::UniformSVMLearningMachine(
  unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples,
  string& MachineFileName, double Tolerance[] ) :
  SVMLearningMachine(DomainSize,CodomainSize,NumOfSamples,MachineFileName)
{

	// allocate and initialise tolerances array
	_tolerance = new double[_domainSize];
	foreach(_domainSize,i) _tolerance[i] = Tolerance[i];

}

UniformSVMLearningMachine::~UniformSVMLearningMachine( void )
{

	// delete tolerances
	delete[] _tolerance;

}

const bool UniformSVMLearningMachine::addExample( const double x[], const double y[] )
{

	// if the buffer is full, stop.
	if ( _exampleCount == _numOfExamples ) {
		return false;
	} else {
		// otherwise, is this example worth adding to the current pool?
		if ( isExampleWorthAdding( x ) ) {
			// yes: then add it and then bail out.
			foreach(_domainSize,i) {
				_sample[_exampleCount][i].index = i+1;
				_sample[_exampleCount][i].value = x[i];
			}
			_sample[_exampleCount][_domainSize].index = -1;
			{ foreach(_codomainSize,i) _value[i][_exampleCount] = y[i]; }
			_exampleCount++;
		}
		return true;
	}

}

const bool UniformSVMLearningMachine::isExampleWorthAdding( const double x[] )
{

	// so: if the example to be added is "close" to any previous one, don't add it
	bool closeToMe;

	// compare with non-normalised samples
	{ foreach_s(_normalExampleCount,_exampleCount,i) {
		closeToMe = true;
		foreach(_domainSize,j) {
			if ( abs(x[j]-_sample[i][j].value) > _tolerance[j] ) {
				closeToMe = false;
				break;
			}
		}
		if ( closeToMe ) {
			return false;
		}
	} }

	// normalise and compare with normalised samples
	double* normal_x = new double[_domainSize];
	{ foreach(_domainSize,i) {
		normal_x[i] = x[i];
		normalise( &normal_x[i], _domainMean[i], _domainStdv[i] );
	} }
	{ foreach(_normalExampleCount,i) {
		closeToMe = true;
		foreach(_domainSize,j) {
			if ( abs(normal_x[j]-_sample[i][j].value) > _tolerance[j] ) {
				closeToMe = false;
				break;
			}
		}
		if ( closeToMe ) {
			delete[] normal_x;
			return false;
		}
	} }
	delete[] normal_x;

	return true;

}
