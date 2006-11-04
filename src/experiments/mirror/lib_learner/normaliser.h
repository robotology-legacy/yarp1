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

class normaliser {
public:
	normaliser() : _avoidColumn0(false) {}
    // set up the data sets
	void setDataSets(dataSet* source, dataSet* dest) { _source = source, _dest = dest; }
	void setAvoidColumn0() { _avoidColumn0 = true; }
    // (un)normalise a single value
    virtual real normalise( real, unsigned int ) = 0;
	virtual real unNormalise( real, unsigned int ) = 0;
    // (un)normalise all
    virtual void normaliseAll( void );
    virtual void unNormaliseAll( void );
	// evaluate statistics
    virtual void evalStatistics( void ) {}
	// save/load statistics
	virtual void save( string ) {};
	virtual bool load( string ) { return true; };
protected:
	bool _avoidColumn0;
	dataSet* _source;
    dataSet* _dest;
};

// null normaliser (does nothing)
class nullNormaliser : public normaliser {
public:
	real normalise( real value, unsigned int ) { return value; }
	real unNormalise( real value, unsigned int ) { return value; }
};

// fixed mean/stdv normaliser (pass them to the constructor)
class fixMsNormaliser : public normaliser {
public:
	fixMsNormaliser(real mean, real stdv)	: _mean(mean), _stdv(stdv) {}
    ~fixMsNormaliser() {}
	real normalise( real, unsigned int );
	real unNormalise( real, unsigned int );
	void save( string );
	bool load( string );
private:
	real _mean, _stdv;
};

// mean/standard deviation normaliser (evaluates them on-the-fly)
class msNormaliser : public normaliser {
public:
	msNormaliser(unsigned int);
    ~msNormaliser();
	real normalise( real, unsigned int );
	real unNormalise( real, unsigned int );
    void evalStatistics( void );
	void save( string );
	bool load( string );
private:
	real* _mean, * _stdv;
};

// max/min normaliser
class mmNormaliser : public normaliser {
public:
	mmNormaliser(unsigned int);
    ~mmNormaliser();
	real normalise( real, unsigned int );
	real unNormalise( real, unsigned int );
    void evalStatistics( void );
	void save( string );
	bool load( string );
private:
	real* _max, * _min;
};

#endif
