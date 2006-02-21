// learning.cpp : behaviour of learning machines
//

#include <iostream>
#include <cmath>
#include "learning.h"

// ---------------------- SVM learning machine, plain version

SVMLearningMachine::SVMLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples) :
	_domainSize(DomainSize), _codomainSize(CodomainSize), _numOfSamples(NumOfSamples), _sampleCount(0)
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

	// allocate clean memory for private objects
	_sample = new svm_node*[_numOfSamples];
	{ foreach(_numOfSamples,i) _sample[i] = new svm_node[_domainSize+1]; }
	_value = new double*[_codomainSize];
	{
		foreach(_codomainSize,i) {
			_value[i] = new double[_numOfSamples];
			foreach(_numOfSamples,j) _value[i][j] = 0.0;
		}
	}
	_domainMean = new double[_domainSize];
	_codomainMean = new double[_codomainSize];
	_domainStdv = new double[_domainSize];
	_codomainStdv = new double[_codomainSize];

	_problem = new svm_problem[_codomainSize];
	_model = new svm_model*[_codomainSize];
	_newSample = new svm_node[_codomainSize];

	{ foreach(_codomainSize,i) _model[i] = NULL; }

	// initialise _problems:
	// (1) tell them how many _samples we use
	{ foreach(_codomainSize,i) _problem[i].l = _numOfSamples; }
	// (2) link their "y" field to values
	{ foreach(_codomainSize,i) _problem[i].y = &_value[i][0]; }
	// (3) create their "x"'s and link the "x" field to _samples
	{ 
		foreach(_codomainSize,i) {
			_problem[i].x = new svm_node*[_numOfSamples+1];
			foreach(_numOfSamples,j) {
				_problem[i].x[j] = &_sample[j][0];
				foreach(_domainSize,k) {
					_sample[j][k].index = k+1;
					_sample[j][k].value = 0;
				}
				_sample[j][_domainSize].index = -1;
			}
		}
	}

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
	delete[] _codomainStdv;
	delete[] _domainStdv;
	delete[] _codomainMean;
	delete[] _domainMean;
	{ foreach(_codomainSize,i) delete[] _value[i]; }
	delete [] _value;
	{ foreach(_numOfSamples,i) delete[] _sample[i]; }
	delete[] _sample;

    svm_destroy_param( &_param );

}

const bool SVMLearningMachine::addSample( const double x[], const double y[] )
{

	// add a new _sample to our training set.

	// if the buffer is full, stop and return failure, but reset
	// counter, so that next time it will be ok.
	if ( _sampleCount == _numOfSamples ) {
		_sampleCount = 0;
		return false;
	} else {
		// otherwise, add it and then bail out.
		foreach(_domainSize,i) {
			_sample[_sampleCount][i].index = i+1;
			_sample[_sampleCount][i].value = x[i];
		}
		_sample[_sampleCount][_domainSize].index = -1;
		{ foreach(_codomainSize,i) _problem[i].y[_sampleCount] = y[i]; }
		_sampleCount++;
	}
	return true;

}

const bool SVMLearningMachine::isSampleWorthAdding ( const double x[] ) const
{

	return true;

}

void SVMLearningMachine::train()
{

	// destroy old models
	{ foreach(_codomainSize,i) if ( _model[i] != NULL ) svm_destroy_model( _model[i] ); }

	// ----------- samples and values normalisation

	// clean means and standard deviations
	{ foreach(_domainSize,i) _domainMean[i] = 0.0; }
	{ foreach(_domainSize,i) _domainStdv[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainMean[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainStdv[i] = 0.0; }

	// evaluate means
	{  	foreach(_numOfSamples,i) {
			{ foreach(_domainSize,j) _domainMean[j] += _sample[i][j].value; }
			{ foreach(_codomainSize,j) _codomainMean[j] += _problem[j].y[i]; }
	} }
	{ foreach(_domainSize,j) _domainMean[j] /= _numOfSamples; }
	{ foreach(_codomainSize,j) _codomainMean[j] /= _numOfSamples; }

	// evaluate standard deviations
	{ foreach(_numOfSamples,i) {
			{ foreach(_domainSize,j) _domainStdv[j] += (_sample[i][j].value-_domainMean[j])*(_sample[i][j].value-_domainMean[j]); }
			{ foreach(_codomainSize,j) _codomainStdv[j] += (_problem[j].y[i]-_codomainMean[j])*(_problem[j].y[i]-_codomainMean[j]); }
	} }
	{ foreach(_domainSize,j) _domainStdv[j] = sqrt(_domainStdv[j]/(_numOfSamples-1)); }
	{ foreach(_codomainSize,j) _codomainStdv[j] = sqrt(_codomainStdv[j]/(_numOfSamples-1)); }

	// normalise samples
	{ foreach(_numOfSamples,i) {
		{ foreach(_domainSize,j) { _sample[i][j].value -= _domainMean[j]; _sample[i][j].value /= _domainStdv[j]; } }
		{ foreach(_codomainSize,j) { _problem[j].y[i] -= _codomainMean[j]; _problem[j].y[i] /= _codomainStdv[j]; } }
	} }

	// ----------- predict!
	{ foreach(_codomainSize,i) _model[i] = svm_train( &_problem[i], &_param ); }

	FILE* out = fopen("x0_data.txt", "w");
	for ( int i=0; i<_numOfSamples; ++i ) {
		fprintf( out, "%lf ", _problem[3].y[i]);
		for ( int j=0; j<3; ++j ) {
			fprintf( out, "%d:%lf ", _problem[3].x[i][j].index, _problem[3].x[i][j].value);
		}
		fprintf( out, "\n");
	}
	fclose(out);
	svm_save_model( "x0_model.txt", _model[3] );

}
	
void SVMLearningMachine::predict ( const double x[], double y[] ) const
{

	// fill new _sample whose value to predict
	{ 
		foreach(_domainSize,i) {
			_newSample[i].index = i+1;
			_newSample[i].value = (x[i]-_domainMean[i])/_domainStdv[i];
		}
		_newSample[_domainSize].index = -1;
	}

	// predict !!
	{ foreach(_codomainSize,i) y[i] = svm_predict(_model[i], _newSample)*_codomainStdv[i]+_codomainMean[i]; }

}

// ---------------------- SVM uniform learning machine
// this machine decides whether to add a sample or not according to a
// uniformity criterion.

UniformLearningMachine::UniformLearningMachine(unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfSamples) :
  SVMLearningMachine(DomainSize,CodomainSize,NumOfSamples)
{

	foreach(21,x) { foreach(21,y) { foreach(21,z) _spaceGrid[x][y][z] = false; } }

}

const bool UniformLearningMachine::addSample( const double x[], const double y[] )
{

	// add a new _sample to our training set.

	// just in case this is the VERY FIRST sample we add, it is a good time
	// to get the center of the world.
	if ( _sampleCount == 0 ) {
		_centerOfTheWorld[0] = x[0];
		_centerOfTheWorld[1] = x[1];
		_centerOfTheWorld[2] = x[2];
	}

	// if the buffer is full, stop and return failure, but reset
	// counter, so that next time it will be ok.
	if ( _sampleCount == _numOfSamples ) {
		_sampleCount = 0;
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
			{ foreach(_codomainSize,i) _problem[i].y[_sampleCount] = y[i]; }
			_sampleCount++;
		}
		return true;
	}

}

const bool UniformLearningMachine::isSampleWorthAdding ( const double x[] )
{

	// 2.0 inches, so far the resolution

	// evaluate coordinates, in terms of the space grid
	int coordX = (int) ((x[0]-_centerOfTheWorld[0]) / 2.0 + 10);
	int coordY = (int) ((x[1]-_centerOfTheWorld[1]) / 2.0 + 10);
	int coordZ = (int) ((x[2]-_centerOfTheWorld[2]) / 2.0 + 10);

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
