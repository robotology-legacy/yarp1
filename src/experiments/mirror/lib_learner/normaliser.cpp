// normaliser.cpp : behaviour of normalisers
//

#include "normaliser.h"

// -------------------------------------------------------
// base normaliser. only provides (un)normalisation of all data.
// -------------------------------------------------------

void normaliser::normaliseAll( )
{

	// flag avoidColumn0, is set, does not normalise column 0 of the
	// source dataset. useful if column 0 represents labels in a
	// classification task.

    // first reset the destination data set
    _dest->reset();

    // then evaluate statistics
	evalStatistics();

    // then add normalised vectors
    real* tmpVector;
    lmAlloc(tmpVector, _source->getSize());

    { foreach(_source->getCount(),i) {
        { foreach(_source->getSize(),j) {
			if ( _avoidColumn0 && j==0 ) {
				tmpVector[j] = (*_source)(i,j);
			} else {
				tmpVector[j] = normalise( (*_source)(i,j), j );
			}
        } }
        _dest->add(tmpVector);
    } }

    delete[] tmpVector;

}

void normaliser::unNormaliseAll( )
{

    { foreach(_source->getCount(),i) {
        { foreach(_source->getSize(),j) { 
			if ( _avoidColumn0 && j==0 ) {
				(*_dest)(i,j) = (*_source)(i,j);
            } else {
				(*_dest)(i,j) = unNormalise( (*_source)(i,j), j );
			}
        } }
    } }

}

// -------------------------------------------------------
// fixed mean/stdv normaliser
// -------------------------------------------------------

real fixMsNormaliser::normalise( real value, unsigned int index )
{

    // for each dimension, subtract mean and divide by standard deviation
    value -= _mean;
	if ( _stdv != 0 ) {
		value /= _stdv;
	}

    return value;

}

real fixMsNormaliser::unNormalise( real value, unsigned int index )
{

    // for each dimension, multiply by standard deviation and add mean
    if ( _stdv != 0 ) {
		value *= _stdv;
	}
	value += _mean;

    return value;

}

void fixMsNormaliser::save( string fileName )
{

	ofstream dataOfstream(fileName.c_str());
	if ( dataOfstream.is_open() == 0 ) {
		cout << "ERROR: could not save statistics." << endl;
		return;
	}

    dataOfstream << _mean << " " << _stdv << endl;

}

bool fixMsNormaliser::load( string fileName )
{

	ifstream dataIfstream(fileName.c_str());
	if ( dataIfstream.is_open() == 0 ) {
		cout << "ERROR: could not load statistics." << endl;
		return false;
	}

    dataIfstream >> _mean >> _stdv;

	cout << "loaded fixed mean (" << _mean << ") and stdv (" << _stdv << ") values." << endl;

	return true;

}

// -------------------------------------------------------
// mean/stdv normaliser
// -------------------------------------------------------

msNormaliser::msNormaliser(unsigned int size)
{

    lmAlloc(_mean, size);
    lmAlloc(_stdv, size);

}

msNormaliser::~msNormaliser()
{

    delete[] _mean;
    delete[] _stdv;

}

real msNormaliser::normalise( real value, unsigned int index )
{

    // for each dimension, subtract mean and divide by standard deviation
    value -= _mean[index];
	if ( _stdv[index] != 0 ) {
		value /= _stdv[index];
	}

    return value;

}

real msNormaliser::unNormalise( real value, unsigned int index )
{

    // for each dimension, multiply by standard deviation and add mean
    if ( _stdv[index] != 0 ) {
		value *= _stdv[index];
	}
	value += _mean[index];

    return value;

}

void msNormaliser::evalStatistics( void )
{

    // initialise means and stdvs to zero
    { foreach(_source->getSize(),i) {
        _mean[i] = 0.0;
        _stdv[i] = 0.0;
    } }

    // now evaluate them using data in _source
	// evaluate means
	{ foreach(_source->getCount(),i)
		{ foreach(_source->getSize(),j)
            _mean[j] += (*_source)(i,j);
    } }
	{ foreach(_source->getSize(),i) _mean[i] /= _source->getCount(); }
	// evaluate standard deviations
	{ foreach(_source->getCount(),i)
		{ foreach(_source->getSize(),j) 
            _stdv[j] += ((*_source)(i,j)-_mean[j])*((*_source)(i,j)-_mean[j]);
    } }
	{ foreach(_source->getSize(),i) _stdv[i] = sqrt(_stdv[i]/(_source->getCount()-1)); }

}

void msNormaliser::save( string fileName )
{

	ofstream dataOfstream(fileName.c_str());
	if ( dataOfstream.is_open() == 0 ) {
		cout << "ERROR: could not save statistics." << endl;
		return;
	}

	{ foreach(_source->getSize(),j)
        dataOfstream << _mean[j] << " " << _stdv[j] << endl;
	}

}

bool msNormaliser::load( string fileName )
{

	ifstream dataIfstream(fileName.c_str());
	if ( dataIfstream.is_open() == 0 ) {
		cout << "ERROR: could not load statistics." << endl;
		return false;
	}

	{ foreach(_source->getSize(),j)
        dataIfstream >> _mean[j] >> _stdv[j];
	}

	cout << "loaded " << _source->getSize() << " mean values and standard deviations." << endl;

	return true;

}

// -------------------------------------------------------
// max/min normaliser
// -------------------------------------------------------

mmNormaliser::mmNormaliser(unsigned int size)
{

    lmAlloc(_max, size);
    lmAlloc(_min, size);

}

mmNormaliser::~mmNormaliser()
{

    delete[] _max;
    delete[] _min;

}

real mmNormaliser::normalise( real value, unsigned int index )
{

	// subtract min and divide by the range unless it is zero
	value -= _min[index];
	if ( _max[index] != _min[index] ) {
		value /= (_max[index]-_min[index])*2 - 1;
	}

    return value;

}

real mmNormaliser::unNormalise( real value, unsigned int index )
{

	// multiply by range unless it is zero and add min
    value = (value+1)/2;
	if ( _max[index] != _min[index] ) {
		value *= (_max[index]-_min[index]);
	}
	value += _min[index];

    return value;

}

void mmNormaliser::evalStatistics( void )
{

    // initialise max's and min's to DBL_MAX and zero
    { foreach(_source->getSize(),i) {
        _max[i] = -DBL_MAX;
        _min[i] =  DBL_MAX;
    } }

    // now evaluate them using data in _source
	// evaluate max's and min's
    { foreach(_source->getCount(),i) {
        { foreach(_source->getSize(),j) {
            if (_max[j] < (*_source)(i,j) ) {
				_max[j] = (*_source)(i,j);
            }
            if (_min[j] > (*_source)(i,j) ) {
				_min[j] = (*_source)(i,j);
            }
        } }
    } }

}

void mmNormaliser::save( string fileName )
{

	ofstream dataOfstream(fileName.c_str());
	if ( dataOfstream.is_open() == 0 ) {
		cout << "ERROR: could not save statistics." << endl;
		return;
	}

	{ foreach(_source->getSize(),j)
        dataOfstream << _max[j] << " " << _min[j] << endl;
	}

}

bool mmNormaliser::load( string fileName )
{

	ifstream dataIfstream(fileName.c_str());
	if ( dataIfstream.is_open() == 0 ) {
		cout << "ERROR: could not load statistics." << endl;
		return false;
	}

	{ foreach(_source->getSize(),j)
        dataIfstream >> _max[j] >> _min[j];
	}

	cout << "loaded " << _source->getSize() << " max and min values." << endl;

	return true;

}
