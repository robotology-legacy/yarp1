// learning.cpp : behaviour of learning machines
//

#include <fstream>
#include <cmath>
#include "learning.h"

// little macros for easier (?) handling of for over arrays
#define foreach(limit,index) unsigned int index; for ( index=0; index<limit; ++index )
#define foreach_s(start,limit,index) unsigned int index; for ( index=start; index<limit; ++index )

// ---------------------- learning machine constructor

LearningMachine::LearningMachine(
  unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfExamples, string& MachineFileName
) :
  _domainSize(DomainSize), _codomainSize(CodomainSize), _numOfExamples(NumOfExamples),
  _machineFileName(MachineFileName), _exampleCount(0), _normalExampleCount(0)
{

	// allocate and clean memory for the examples: samples,
	_sample = new sample*[_numOfExamples];
	{ foreach(_numOfExamples,i) {
		_sample[i] = new sample[_domainSize+1];
		foreach(_domainSize,k) {
			_sample[i][k].index = k+1;
			_sample[i][k].value = 0;
		}
		_sample[i][_domainSize].index = -1;
	} }
	// and values
	_value = new double*[_codomainSize];
	{ foreach(_codomainSize,i) {
		_value[i] = new double[_numOfExamples];
		foreach(_numOfExamples,j) _value[i][j] = 0.0;
	} }

	// allocate the new sample, whose value to predict
	_newSample = new sample[_codomainSize];

	// the means and stdvs
	_domainMean = new double[_domainSize];
	_codomainMean = new double[_codomainSize];
	_domainStdv = new double[_domainSize];
	_codomainStdv = new double[_codomainSize];

	// evaluate statistics first time, that is, fill them with a row of 0s
	evalStats();

}

LearningMachine::~LearningMachine()
{

	// delete statistics
	delete[] _codomainStdv;
	delete[] _domainStdv;
	delete[] _codomainMean;
	delete[] _domainMean;

	// delete examples: values
	{ foreach(_codomainSize,i) delete[] _value[i]; }
	delete[] _value;
	// and samples
	{ foreach(_numOfExamples,i) delete[] _sample[i]; }
	delete[] _sample;

}

void LearningMachine::reset( void )
{

	// just reset counters
	_normalExampleCount = 0;
	_exampleCount = 0;

}

void LearningMachine::save( void )
{

	// save means and stdvs
	string statsFileName = _machineFileName + ".stats";
	ofstream ofOut(statsFileName.c_str());

	{ foreach(_domainSize,i) ofOut << _domainMean[i] << " " << _domainStdv[i] << endl; }
	{ foreach(_codomainSize,i) ofOut << _codomainMean[i] << " " << _codomainStdv[i] << endl; }

}

const bool LearningMachine::load( void )
{

	// load means and stdvs
	string statsFileName = _machineFileName + ".stats";
	ifstream ifIn(statsFileName.c_str());
	if ( ifIn.is_open() == false ) {
		return false;
	}

	{ foreach(_domainSize,i) ifIn >> _domainMean[i] >> _domainStdv[i]; }
	{ foreach(_codomainSize,i) ifIn >> _codomainMean[i] >> _codomainStdv[i]; }
	
	return true;

}

void LearningMachine::evalStats()
{

	// clean means and standard deviations
	{ foreach(_domainSize,i) _domainMean[i] = 0.0; }
	{ foreach(_domainSize,i) _domainStdv[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainMean[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainStdv[i] = 0.0; }

	// evaluate means
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) _domainMean[j] += _sample[i][j].value; }
		{ foreach(_codomainSize,j) _codomainMean[j] += _value[j][i]; }
	} }
	{ foreach(_domainSize,j) _domainMean[j] /= _exampleCount; }
	{ foreach(_codomainSize,j) _codomainMean[j] /= _exampleCount; }

	// evaluate standard deviations
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) _domainStdv[j] +=
			(_sample[i][j].value-_domainMean[j])*(_sample[i][j].value-_domainMean[j]); }
		{ foreach(_codomainSize,j) _codomainStdv[j] +=
			(_value[j][i]-_codomainMean[j])*(_value[j][i]-_codomainMean[j]); }
	} }
	{ foreach(_domainSize,j) _domainStdv[j] = sqrt(_domainStdv[j]/(_exampleCount-1)); }
	{ foreach(_codomainSize,j) _codomainStdv[j] = sqrt(_codomainStdv[j]/(_exampleCount-1)); }

}

void LearningMachine::normalise( double* value, const double mean, const double stdv )
{

	// normalisation of a value: subtract the mean and divide by the
	// standard deviation unless it is zero
	*value -= mean;
	if ( stdv != 0 ) {
		*value /= stdv;
	}

}

void LearningMachine::unNormalise( double* value, const double mean, const double stdv )
{

	// un-normalisation of a value: multiply by standard deviation
	// unless it is zero and add the mean
	if ( stdv != 0 ) {
		*value *= stdv;
	}
	*value += mean;

}

void LearningMachine::normaliseExamples()
{

	// un-normalise already normalised examples
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) unNormalise( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) unNormalise( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// evaluate new means and stdvs
	evalStats();

	// re-normalise all examples
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) normalise( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) normalise( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// now they are all normalised!
	_normalExampleCount = _exampleCount;

}

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
	{ foreach(_codomainSize,i) svm_destroy_model( _model[i] ); }
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
		svm_destroy_model( _model[i] );
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
