// learningMachine.cpp : behaviour of learning machines
//

#include "learningMachine.h"

// ---------------------- learning machine constructor

LearningMachine::LearningMachine(
  unsigned int DomainSize, unsigned int CodomainSize, unsigned int NumOfExamples,
  string& MachineFileName, bool normalise=true
) :
  _domainSize(DomainSize), _codomainSize(CodomainSize), _numOfExamples(NumOfExamples),
  _machineFileName(MachineFileName), _normalise(normalise),
  _exampleCount(0), _normalExampleCount(0)
{

	// allocate and clean memory for the examples: samples,
	lMAlloc(_sample, _numOfExamples);
	{ foreach(_numOfExamples,i) {
		lMAlloc(_sample[i], _domainSize+1);
		foreach(_domainSize,k) {
			_sample[i][k].index = k+1;
			_sample[i][k].value = 0;
		}
		_sample[i][_domainSize].index = -1;
	} }
	// and values
	lMAlloc(_value, _codomainSize);
	{ foreach(_codomainSize,i) {
		lMAlloc(_value[i], _numOfExamples);
		foreach(_numOfExamples,j) _value[i][j] = 0.0;
	} }

	// allocate the new sample, whose value to predict
	lMAlloc(_newSample, _codomainSize);

	// the means and stdvs
	lMAlloc(_domainMean, _domainSize);
	lMAlloc(_codomainMean, _codomainSize);
	lMAlloc(_domainStdv, _domainSize);
	lMAlloc(_codomainStdv, _codomainSize);
	lMAlloc(_domainMin, _domainSize);
	lMAlloc(_codomainMin, _codomainSize);
	lMAlloc(_domainMax, _domainSize);
	lMAlloc(_codomainMax, _codomainSize);

	// evaluate statistics first time, that is, fill them with a row of 0s
	evalStats();

}

LearningMachine::~LearningMachine()
{

	delete[] _codomainStdv;
	delete[] _domainStdv;
	delete[] _codomainMean;
	delete[] _domainMean;
	delete[] _codomainMin;
	delete[] _domainMin;
	delete[] _codomainMax;
	delete[] _domainMax;

	delete[] _newSample;

	{ foreach(_codomainSize,i) delete[] _value[i]; }
	delete[] _value;
	{ foreach(_numOfExamples,i) delete[] _sample[i]; }
	delete[] _sample;

}

void LearningMachine::reset( void )
{

	// just reset counters
	_normalExampleCount = 0;
	_exampleCount = 0;

}

void LearningMachine::saveData( void )
{

	string dataFileName = _machineFileName + ".data";
	ofstream dataOfstream(dataFileName.c_str());
	if ( dataOfstream.is_open() == 0 ) {
		cout << "ERROR: could not save data." << endl;
		return;
	}

	// file header: domain and codomain size, and HOW MANY data you're going to save
	dataOfstream << _domainSize << " " << _codomainSize << " " << _exampleCount << endl;

	// saved data must be NON normalised
	// un-normalise normal ones
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) unNormaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) unNormaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }
	// save them all
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) dataOfstream << _sample[i][j].value << " "; }
		{ foreach(_codomainSize,j) dataOfstream << _value[j][i] << " "; }
		dataOfstream << endl;
	} }
	// re-normalise those who have been un-normalised
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) normaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) normaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	cout << "saved data to " << dataFileName << "." << endl;

}

const bool LearningMachine::loadData( void )
{

	string dataFileName = _machineFileName + ".data";
	ifstream dataIfstream(dataFileName.c_str());
	if ( dataIfstream.is_open() == false ) {
		cout << "no previously saved data found." << endl;
		return false;
	}

	// load file header and check consistency of previously saved data...
	unsigned int domainSize, codomainSize, exampleCount;
	dataIfstream >> domainSize >> codomainSize >> exampleCount;
	// domain and codomain size must match
	if ( domainSize != _domainSize || codomainSize != _codomainSize ) {
		cout << "ERROR: previously saved data file does not match. Starting from scratch." << endl;
		return false;
	}
	// number of stored examples can't be bigger than what this machine can hold
	if ( exampleCount > _numOfExamples ) {
		cout << "ERROR: more saved data than allowed for this machine. Starting from scratch." << endl;
		return false;
	}

	// set new number of examples
	_exampleCount = exampleCount;

	// load them
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) { dataIfstream >> _sample[i][j].value; _sample[i][j].index = j+1; } }
		_sample[i][_domainSize].index = -1;
		{ foreach(_codomainSize,j) dataIfstream >> _value[j][i]; }
	} }
	
	// loaded data are non normalised; then evaluate statistics and normalise
	normaliseExamplesMeanStd();

	cout << "loaded " << _exampleCount << " data from " << dataFileName << "." << endl;
	
	return true;

}

void LearningMachine::evalStats()
{

	// if this machine has no normalisation,
	// just set means to zero and stdvs to one
	if ( _normalise == false ) {
		{ foreach(_domainSize,i) _domainMean[i] = 0.0; }
		{ foreach(_domainSize,i) _domainStdv[i] = 1.0; }
		{ foreach(_codomainSize,i) _codomainMean[i] = 0.0; }
		{ foreach(_codomainSize,i) _codomainStdv[i] = 1.0; }
		return;
	}

	// otherwise, evaluate proper statistics
	// clean means and standard deviations
	{ foreach(_domainSize,i) _domainMean[i] = 0.0; }
	{ foreach(_domainSize,i) _domainStdv[i] = 0.0; }
	{ foreach(_domainSize,i) _domainMax[i] = -DBL_MAX; }
	{ foreach(_domainSize,i) _domainMin[i] = DBL_MAX; }
	{ foreach(_codomainSize,i) _codomainMean[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainStdv[i] = 0.0; }
	{ foreach(_codomainSize,i) _codomainMax[i] = -DBL_MAX; }
	{ foreach(_codomainSize,i) _codomainMin[i] = DBL_MAX; }

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

	// evaluate maxs and mins
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) {
			if (_domainMax[j]<_sample[i][j].value)
				_domainMax[j]=_sample[i][j].value;
			if (_domainMin[j]>_sample[i][j].value)
				_domainMin[j]=_sample[i][j].value;
		} }
		{ foreach(_codomainSize,j) {
			if (_codomainMax[j]<_value[j][i])
				_codomainMax[j]=_value[j][i];
			if (_codomainMin[j]>_value[j][i])
				_codomainMin[j]=_value[j][i];
		} }
	} }
}

void LearningMachine::normaliseMeanStd( double* value, const double mean, const double stdv )
{

	// normalisation of a value: subtract the mean and divide by the
	// standard deviation unless it is zero
	*value -= mean;
	if ( stdv != 0 ) {
		*value /= stdv;
	}

}

void LearningMachine::unNormaliseMeanStd( double* value, const double mean, const double stdv )
{

	// un-normalisation of a value: multiply by standard deviation
	// unless it is zero and add the mean
	if ( stdv != 0 ) {
		*value *= stdv;
	}
	*value += mean;

}

void LearningMachine::normaliseMaxMin( double* value, const double max, const double min )
{

	// normalisation of a value: subtract the min and divide by the
	// range unless it is zero
	*value -= min;
	if ( max != min ) {
		*value = *value/(max-min)*2-1;
	}

}

void LearningMachine::unNormaliseMaxMin( double* value, const double max, const double min )
{

	// un-normalisation of a value: multiply by range
	// unless it is zero and add the min
	*value = (*value+1)/2;
	if ( max != min ) {
		*value *= (max-min);
	}
	*value += min;

}

void LearningMachine::normaliseExamplesMeanStd()
{

	// un-normalise already normalised examples
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) unNormaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) unNormaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// evaluate new means and stdvs
	evalStats();

	// re-normalise all examples
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) normaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		{ foreach(_codomainSize,j) normaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// now they are all normalised!
	_normalExampleCount = _exampleCount;

}

void LearningMachine::normaliseExamplesMeanStdDomain()
{

	// un-normalise already normalised examples
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) unNormaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		//{ foreach(_codomainSize,j) unNormaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// evaluate new means and stdvs
	evalStats();

	// re-normalise all examples
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) normaliseMeanStd( &_sample[i][j].value, _domainMean[j], _domainStdv[j] ); }
		//{ foreach(_codomainSize,j) normaliseMeanStd( &_value[j][i], _codomainMean[j], _codomainStdv[j] ); }
	} }

	// now they are all normalised!
	_normalExampleCount = _exampleCount;

}

void LearningMachine::normaliseExamplesMaxMin()
{

	// un-normalise already normalised examples
	{ foreach(_normalExampleCount,i) {
		{ foreach(_domainSize,j) unNormaliseMaxMin( &_sample[i][j].value, _domainMax[j], _domainMin[j] ); }
		{ foreach(_codomainSize,j) unNormaliseMaxMin( &_value[j][i], _codomainMax[j], _codomainMin[j] ); }
	} }

	// evaluate new maxs and mins
	evalStats();

	// re-normalise all examples
	{ foreach(_exampleCount,i) {
		{ foreach(_domainSize,j) normaliseMaxMin( &_sample[i][j].value, _domainMax[j], _domainMin[j] ); }
		{ foreach(_codomainSize,j) normaliseMaxMin( &_value[j][i], _codomainMax[j], _codomainMin[j] ); }
	} }

	// now they are all normalised!
	_normalExampleCount = _exampleCount;

}
