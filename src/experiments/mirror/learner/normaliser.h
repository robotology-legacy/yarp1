// normaliser.h : header file for normalisers
//

#ifndef __normaliserh__
#define __nornaliserh__

#include "dataSet.h"

// -------------------------------------------------------
// normalisers
// -------------------------------------------------------
// a normaliser holds references to a source data set (non normalised)
// and a destination data set (normalised).

class Normaliser {
public:
	Normaliser(dataSet& source, dataSet& dest) : _source(source), _dest(dest) {}
    // (un)normalise a single value
    virtual real normalise( real, const real, const real ) = 0;
	virtual real unNormalise( real, const real, const real ) = 0;
    // (un)normalise all
    virtual void normaliseAll( void ) = 0;
    virtual void unNormaliseAll( void ) = 0;
    virtual void evalStatistics( void ) = 0;
protected:
	dataSet& _source;
    dataSet& _dest;
};

// null normaliser (does nothing)
class nullNormaliser : public Normaliser {
public:
	nullNormaliser(dataSet&, dataSet&);
    ~nullNormaliser();
	real normalise( real, const real, const real );
	real unNormalise( real, const real, const real );
    void normaliseAll( void );
    void unNormaliseAll( void );
    void evalStatistics( void );
};

// mean/standard deviation normaliser
class msNormaliser : public Normaliser {
public:
	msNormaliser(dataSet&, dataSet&);
    ~msNormaliser();
	real normalise( real, const real, const real );
	real unNormalise( real, const real, const real );
    void normaliseAll( void );
    void unNormaliseAll( void );
    void evalStatistics( void );
private:
	real* _mean, * _stdv;
};

// max/min normaliser
class mmNormaliser : public Normaliser {
public:
	mmNormaliser(dataSet&, dataSet&);
    ~mmNormaliser();
	real normalise( real, const real, const real );
	real unNormalise( real, const real, const real );
    void normaliseAll( void );
    void unNormaliseAll( void );
    void evalStatistics( void );
private:
	real* _max, * _min;
};

#endif
