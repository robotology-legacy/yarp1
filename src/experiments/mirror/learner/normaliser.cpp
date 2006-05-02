// normaliser.cpp : behaviour of normalisers
//

#include "normaliser.h"

// -------------------------------------------------------
// null normaliser
// -------------------------------------------------------

nullNormaliser::nullNormaliser(dataSet& source, dataSet& dest) : Normaliser(source, dest)
{
}

nullNormaliser::~nullNormaliser()
{
}

real nullNormaliser::normalise( real value, unsigned int index )
{

    return value;

}

real nullNormaliser::unNormalise( real value, unsigned int index )
{

    return value;

}

void nullNormaliser::normaliseAll( void )
{

    // first reset the destination data set
    _dest.reset();

    // then evaluate statistics
	evalStatistics();

    // then add normalised vectors
    real* tmpVector;
    lmAlloc(tmpVector, _source.getSize());

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) {
            tmpVector[j] = normalise( _source(i,j), 0 );
        } }
        _dest.add(tmpVector);
    } }

    delete[] tmpVector;

}

void nullNormaliser::unNormaliseAll( void )
{

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) { 
            _dest(i,j) = unNormalise( _source(i,j), 0 );
        } }
    } }

}

void nullNormaliser::evalStatistics( void )
{
}

// -------------------------------------------------------
// mean/stdv normaliser
// -------------------------------------------------------

msNormaliser::msNormaliser(dataSet& source, dataSet& dest) : Normaliser(source, dest)
{

    lmAlloc(_mean, _source.getSize());
    lmAlloc(_stdv, _source.getSize());

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

void msNormaliser::normaliseAll( void )
{

    // first reset the destination data set
    _dest.reset();

    // then evaluate statistics
	evalStatistics();

    // then add normalised vectors
    real* tmpVector;
    lmAlloc(tmpVector, _source.getSize());

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) {
            tmpVector[j] = normalise( _source(i,j), j );
        } }
        _dest.add(tmpVector);
    } }

    delete[] tmpVector;

}

void msNormaliser::unNormaliseAll( void )
{

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) { 
            _dest(i,j) = unNormalise( _source(i,j), j );
        } }
    } }

}

void msNormaliser::evalStatistics( void )
{

    // initialise means and stdvs to zero
    { foreach(_source.getSize(),i) {
        _mean[i] = 0.0;
        _stdv[i] = 0.0;
    } }

    // now evaluate them using data in _source
	// evaluate means
	{ foreach(_source.getCount(),i)
		{ foreach(_source.getSize(),j)
            _mean[j] += _source(i,j);
    } }
	{ foreach(_source.getSize(),i) _mean[i] /= _source.getCount(); }
	// evaluate standard deviations
	{ foreach(_source.getCount(),i)
		{ foreach(_source.getSize(),j) 
            _stdv[j] += (_source(i,j)-_mean[j])*(_source(i,j)-_mean[j]);
    } }
	{ foreach(_source.getSize(),i) _stdv[i] = sqrt(_stdv[i]/(_source.getCount()-1)); }

}

// -------------------------------------------------------
// max/min normaliser
// -------------------------------------------------------

mmNormaliser::mmNormaliser(dataSet& source, dataSet& dest) : Normaliser(source, dest)
{

    lmAlloc(_max, _source.getSize());
    lmAlloc(_min, _source.getSize());

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

void mmNormaliser::normaliseAll( void )
{

    // first reset the destination data set
    _dest.reset();

    // then evaluate statistics
	evalStatistics();

    // then add normalised vectors
    real* tmpVector;
    lmAlloc(tmpVector, _source.getSize());

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) {
            tmpVector[j] = normalise( _source(i,j), j );
        } }
        _dest.add(tmpVector);
    } }

    delete[] tmpVector;

}

void mmNormaliser::unNormaliseAll( void )
{

    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) {
            _dest(i,j) = unNormalise( _source(i,j), j );
        } }
    } }

}

void mmNormaliser::evalStatistics( void )
{

    // initialise max's and min's to DBL_MAX and zero
    { foreach(_source.getSize(),i) {
        _max[i] = -DBL_MAX;
        _min[i] =  DBL_MAX;
    } }

    // now evaluate them using data in _source
	// evaluate max's and min's
    { foreach(_source.getCount(),i) {
        { foreach(_source.getSize(),j) {
            if (_max[j] < _source(i,j) ) {
				_max[j] = _source(i,j);
            }
            if (_min[j] > _source(i,j) ) {
				_min[j] = _source(i,j);
            }
        } }
    } }

}
