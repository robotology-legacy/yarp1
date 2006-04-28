// learningMachine.h : header file for learning machines
//

#ifndef __learningMachineh__
#define __learningMachineh__

#include "dataSet.h"
#include "normaliser.h"

// -------------------------------------------------------
// definitions
// -------------------------------------------------------
// - LEARNING MACHINE: a map R^m -> R^n, with the notable special case of classification,
//     in which R^n really consists of labels
// - INPUT SPACE: the space to map from (e.g., R^m)
// - OUTPUT SPACE: the space to map to (e.g., R^n)
// - SAMPLE: element of the input space
// - VALUE: element of the output space
// - EXAMPLE: a pair <SAMPLE,VALUE>. examples are used for training, whereas a sample
//     maps to a value through prediction
// all numeric values are stored as "real".

// -------------------------------------------------------
// plain learning machine
// -------------------------------------------------------
// only does data bookkeeping. has two data sets (normalised and
// non normalised) and a template normaliser. can save and load its own status

template <class NORMALISER> class LearningMachine {
public:
	class paramsType {
	  public:
		paramsType(unsigned int capacity, unsigned int domainSize, string name)
            : _capacity(capacity), _domainSize(domainSize), _name(name) {}
		paramsType() : _capacity(100), _domainSize(1), _name("learner") {}
		unsigned int _capacity;
		unsigned int _domainSize;
		string _name;
	};

	// initialise with a parameter set
    LearningMachine( paramsType& params ) : _params(params), _count(0),
      _rawData(_params._capacity,_params._domainSize),
      _normalData(_params._capacity,_params._domainSize)
    {
        _norm = new NORMALISER(_rawData,_normalData);
    }
    // otherwise, use standard params constructor
    LearningMachine( void ) : _count(0),
      _rawData(_params._capacity,_params._domainSize),
      _normalData(_params._capacity,_params._domainSize)
    {
        _norm = new NORMALISER(_rawData,_normalData);
    }
    ~LearningMachine( void ) { delete _norm; }

	// viewing counters
	unsigned int getDomainSize( void ) const { return _params._domainSize; }
	unsigned int getCapacity( void ) const { return _params._capacity; }
	unsigned int getCount( void ) const { return _count; }
	// resetting the machine
    virtual void reset( void ) { _count = 0; }
    // loading and saving status
    void save( void ) {
        // save non-normalised data
	    string dataFileName = _params._name + ".raw.data";
        _rawData.save(dataFileName);
	    cout << "saved raw data to " << dataFileName << "." << endl;
        // save normalised data
	    dataFileName = _params._name + ".norm.data";
        _normalData.save(dataFileName);
	    cout << "saved raw data to " << dataFileName << "." << endl;
    }
    bool load( void ) {
        // load non-normalised data
        string dataFileName = _params._name + ".raw.data";
        if ( _rawData.load(dataFileName) ==  false ) {
            return false;
        }
    	// set new number of examples
	    _count = _rawData.getCount();
    	// loaded data are non normalised: normalise
	    _norm->evalStatistics();
	    _norm->normaliseAll();
    	cout << "loaded " << _count << " data from " << dataFileName << "." << endl;
    	return true;
    }

	// abstract methods. any concrete learning machine must be able at least
	// to add an example, train its models and predict a new value given a sample
//	virtual bool addExample( const real[], const real[] ) = 0;
//	virtual void train( void ) = 0;
//	virtual void predictValue( const real[], real[] ) = 0;

//protected:
    // must be protected rather than private, since children
    // classes must be able to access them.
	paramsType _params;
    // how many samples considered so far?
    unsigned int _count;
    // raw and normalised data
	dataSet _rawData;
    dataSet _normalData;
    // the normaliser
	Normaliser* _norm;

};

#endif
