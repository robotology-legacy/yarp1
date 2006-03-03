// learningMachine.cpp : behaviour of learning machines
//

#include "learningMachine.h"

// ---------------------- learning machine constructor

LearningMachine::LearningMachine(
  unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfExamples, string& MachineFileName
) :
  _domainSize(DomainSize), _codomainSize(CodomainSize), _numOfExamples(NumOfExamples),
  _machineFileName(MachineFileName), _exampleCount(0), _normalExampleCount(0)
{

	// allocate and clean memory for the examples: samples,

// esempio. rimpiazzare TUTTI i new con questo.
//	_sample = new sample*[_numOfExamples];
	lMAlloc(_sample, _numOfExamples);

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
