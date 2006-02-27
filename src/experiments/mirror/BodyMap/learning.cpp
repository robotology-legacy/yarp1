// learning.cpp : behaviour of learning machines
//

#include <iostream>
#include <cmath>
#include "learning.h"

// ---------------------- learning machine constructor

LearningMachine::LearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples) :
  _domainSize (DomainSize), _codomainSize(CodomainSize), _numOfSamples(NumOfSamples),
  _sampleCount(0), _normalSampleCount(0)
{

	// allocate clean memory for private objects

	// the samples
	_sample = new sample*[_numOfSamples];
	{ foreach(_numOfSamples,i) {
		_sample[i] = new sample[_domainSize+1];
		foreach(_domainSize,k) {
			_sample[i][k].index = k+1;
			_sample[i][k].value = 0;
		}
		_sample[i][_domainSize].index = -1;
	} }

	// the values
	_value = new double*[_codomainSize];
	{ foreach(_codomainSize,i) {
		_value[i] = new double[_numOfSamples];
		foreach(_numOfSamples,j) _value[i][j] = 0.0;
	} }

	// allocate the new sample, the one to be predicted
	_newSample = new sample[_codomainSize];

	// the means and stdvs
	_domainMean = new double[_domainSize];
	_codomainMean = new double[_codomainSize];
	_domainStdv = new double[_domainSize];
	_codomainStdv = new double[_codomainSize];

}

LearningMachine::~LearningMachine()
{

	delete[] _codomainStdv;
	delete[] _domainStdv;
	delete[] _codomainMean;
	delete[] _domainMean;

	{ foreach(_codomainSize,i) delete[] _value[i]; }
	delete[] _value;

	{ foreach(_numOfSamples,i) delete[] _sample[i]; }
	delete[] _sample;

}

void LearningMachine::evalStats()
{

	// clean means and standard deviations
	{ foreach(_domainSize,i) _domainMean[i] = 0.0; }
	{ foreach(_domainSize,i) _domainStdv[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainMean[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainStdv[i] = 0.0; }

	// evaluate means
	{ foreach(_sampleCount,i) {
			{ foreach(_domainSize,j) _domainMean[j] += _sample[i][j].value; }
			{ foreach(_codomainSize,j) _codomainMean[j] += _value[j][i]; }
	} }
	{ foreach(_domainSize,j) _domainMean[j] /= _sampleCount; }
	{ foreach(_codomainSize,j) _codomainMean[j] /= _sampleCount; }

	// evaluate standard deviations
	{ foreach(_sampleCount,i) {
			{ foreach(_domainSize,j) _domainStdv[j] +=
				(_sample[i][j].value-_domainMean[j])*(_sample[i][j].value-_domainMean[j]); }
			{ foreach(_codomainSize,j) _codomainStdv[j] +=
				(_value[j][i]-_codomainMean[j])*(_value[j][i]-_codomainMean[j]); }
	} }
	{ foreach(_domainSize,j) _domainStdv[j] = sqrt(_domainStdv[j]/(_sampleCount-1)); }
	{ foreach(_codomainSize,j) _codomainStdv[j] = sqrt(_codomainStdv[j]/(_sampleCount-1)); }

}

void LearningMachine::normalise()
{

	// un-normalise already normalised samples
	{ foreach(_normalSampleCount,i) {
		{ foreach(_domainSize,j) {
			_sample[i][j].value *= _domainStdv[j];
			_sample[i][j].value += _domainMean[j];
		} }
		{ foreach(_codomainSize,j) {
			_value[j][i] *= _codomainStdv[j];
			_value[j][i] += _codomainMean[j];
		} }
	} }

	// evaluate new means and stdvs
	evalStats();

	// re-normalise all samples
	{ foreach(_sampleCount,i) {
		{ foreach(_domainSize,j) {
			_sample[i][j].value -= _domainMean[j];
			_sample[i][j].value /= _domainStdv[j];
		} }
		{ foreach(_codomainSize,j) {
			_value[j][i] -=	_codomainMean[j];
			_value[j][i] /= _codomainStdv[j];
		} }
	} }

	// now they are all normalised!
	_normalSampleCount = _sampleCount;

}

// ---------------------- SVM learning machine, plain version

SVMLearningMachine::SVMLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples) :
	LearningMachine(DomainSize,CodomainSize,NumOfSamples)
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
	_param.weight_label = NULL;
	_param.weight = NULL;

	// check them. the problem is not required unless we use NU-SVC,
	// which is never the case for us, so a NULL thing will be OK.
	const char* error_msg;
	error_msg = svm_check_parameter( NULL, &_param );
	if ( error_msg != NULL ) {
		exit(0);
	}

	// allocate SVM problems
	_problem = new svm_problem[_codomainSize];
	// allocate models pointers
	_model = new svm_model*[_codomainSize];
	{ foreach(_codomainSize,i) _model[i] = NULL; }

	// initialise _problems:
	// (1) tell them how many _samples we use
	{ foreach(_codomainSize,i) _problem[i].l = _numOfSamples; }
	// (2) link their "y" field to values of the LearningMachine
	{ foreach(_codomainSize,i) _problem[i].y = &_value[i][0]; }
	// (3) create their "x"'s and link the "x" field to _samples
	{ foreach(_codomainSize,i) {
		_problem[i].x = new svm_node*[_numOfSamples+1];
		foreach(_numOfSamples,j) _problem[i].x[j] = (svm_node*) &_sample[j][0];
	} }
	// there you are. from now on you alter _samples[] and _value[], and
	// really you are touching the SVM problems.

	// initial training. this really gets us a load of zeroes, but
	// is required so that the machine is immediately up'n'running
	train();

	return;

}

SVMLearningMachine::~SVMLearningMachine()
{

	{ foreach(_codomainSize,i) svm_destroy_model( _model[i] ); }
	delete[] _model;
	{ foreach(_codomainSize,i) delete[] _problem[i].x; }
	delete[] _problem;
	delete[] _newSample;

    svm_destroy_param( &_param );

}

const bool SVMLearningMachine::addSample( const double x[], const double y[] )
{

	// if the buffer is full, stop forever
	if ( _sampleCount == _numOfSamples ) {
		return false;
	} else {
		// otherwise, add it, update counter and then bail out.
		foreach(_domainSize,i) {
			_sample[_sampleCount][i].index = i+1;
			_sample[_sampleCount][i].value = x[i];
		}
		_sample[_sampleCount][_domainSize].index = -1;
		{ foreach(_codomainSize,i) _value[i][_sampleCount] = y[i]; }
		_sampleCount++;
		return true;
	}

}

void SVMLearningMachine::train()
{

	// destroy old models
	{ foreach(_codomainSize,i) if ( _model[i] != NULL ) svm_destroy_model( _model[i] ); }

	// normalise
	normalise();

	// train new models
	{ foreach(_codomainSize,i) _model[i] = svm_train( &_problem[i], &_param ); }

}
	
void SVMLearningMachine::predict ( const double x[], double y[] ) const
{

	// fill new sample whose value to predict (normalise it)
	{ foreach(_domainSize,i) {
		_newSample[i].index = i+1;
		_newSample[i].value = (x[i]-_domainMean[i])/_domainStdv[i];
		}
		_newSample[_domainSize].index = -1;
	}

	// predict (and then un-normalise)
	{ foreach(_codomainSize,i)
		y[i] = svm_predict(_model[i], (svm_node*)_newSample) * _codomainStdv[i] + _codomainMean[i];
	}

}

// ---------------------- SVM uniform learning machine
// this machine decides whether to add a sample or not according to a
// uniformity criterion.

UniformLearningMachine::UniformLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples) :
  SVMLearningMachine(DomainSize,CodomainSize,NumOfSamples)
{

	resetSpaceGrid();

}

void UniformLearningMachine::resetSpaceGrid()
{

	foreach(21,x) { foreach(21,y) { foreach(21,z) _spaceGrid[x][y][z] = false; } }

}

const bool UniformLearningMachine::addSample( const double x[], const double y[] )
{

	// just in case this is the VERY FIRST sample we add, it is a good time
	// to get the center of the world.
	if ( _sampleCount == 0 ) {
		_centerOfTheWorld[0] = x[0];
		_centerOfTheWorld[1] = x[1];
		_centerOfTheWorld[2] = x[2];
	}

	// if the buffer is full, stop.
	if ( _sampleCount == _numOfSamples ) {
		return false;
	} else {
		// otherwise, is this sample worth adding to the current pool?
		if ( isSampleWorthAdding( x ) ) {
			// yes: then add it and then bail out.
			foreach(_domainSize,i) {
				_sample[_sampleCount][i].index = i+1;
				_sample[_sampleCount][i].value = x[i];
			}
			_sample[_sampleCount][_domainSize].index = -1;
			{ foreach(_codomainSize,i) _value[i][_sampleCount] = y[i]; }
			_sampleCount++;
		}
		return true;
	}

}

const bool UniformLearningMachine::isSampleWorthAdding ( const double x[] )
{

	// 1 inch is the resolution

	// evaluate coordinates, in terms of the space grid
	int coordX = (int) ((x[0]-_centerOfTheWorld[0]) / 1.0 + 10);
	int coordY = (int) ((x[1]-_centerOfTheWorld[1]) / 1.0 + 10);
	int coordZ = (int) ((x[2]-_centerOfTheWorld[2]) / 1.0 + 10);

	// outside the grid? then leave it...
	if ( coordX<0 || coordX>20 || coordY<0 || coordY>20 || coordZ<0 || coordZ>20 ) {
		return false;
	}

	// otherwise: have we already been here?
	if ( _spaceGrid[coordX][coordY][coordZ] ) {
		return false;
	} else {
		// no: then add me !!
		_spaceGrid[coordX][coordY][coordZ] = true;
		return true;
	}

}
