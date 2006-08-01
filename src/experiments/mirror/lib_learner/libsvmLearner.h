// -------------------------------------------------------
// preliminaries
// -------------------------------------------------------

#include "libsvmLearningMachine.h"
#include "lMCommands.h"

using namespace std;

// what kind of machines? (default: plain)
namespace typeOfMachine {
	enum {
		plain = 0,
		uniform = 1,
		feedback = 2
	};
};

// -------------------------------------------------------
// a class enforcing multiple-output learning machines
// -------------------------------------------------------

class libsvmLearner {
public:
    libsvmLearner(int typeOfMachine, unsigned int codomainSize, libsvmLearningMachine::params& params,
		real* tolerance, real threshold)
        : _machineType(typeOfMachine), _codomainSize(codomainSize), _params(params)
    {
		// create learning machines according to cmd line
        lmAlloc(_machine, _codomainSize);
        lmAlloc(_norm, _codomainSize);
		switch ( _machineType ) {
		case typeOfMachine::plain:
			// create normaliser
	        { foreach(_codomainSize,i) {
				_norm[i] = new msNormaliser(_params._domainSize+1);
				libsvmLearningMachine::params tmpParams(params);
				char tmp[32]; itoa(i,tmp,10); tmpParams._name = _params._name + "." + tmp;
				_machine[i] = new libsvmLearningMachine(_norm[i], tmpParams);
			} }
		break;
		case typeOfMachine::uniform:
	        { foreach(_codomainSize,i) {
				_norm[i] = new msNormaliser(_params._domainSize+1);
				libsvmUniLearningMachine::params tmpParams(params);
				char tmp[32]; itoa(i,tmp,10); tmpParams._name = _params._name + "." + tmp;
				_machine[i] = new libsvmUniLearningMachine(_norm[i], tmpParams, tolerance);
			} }
		break;
		case typeOfMachine::feedback:
	        { foreach(_codomainSize,i) {
				_norm[i] = new msNormaliser(_params._domainSize+1);
				libsvmFBLearningMachine::params tmpParams(params);
				char tmp[32]; itoa(i,tmp,10); tmpParams._name = _params._name + "." + tmp;
				_machine[i] = new libsvmFBLearningMachine(_norm[i], tmpParams, threshold);
			} }
		break;
		}
	
        // allocate space for the predicted vector
        lmAlloc(_predicted, _codomainSize);
    }
	~libsvmLearner() {
		delete[] _predicted;
		delete[] _norm;
		delete[] _machine;
	}

	// viewing counters
	unsigned int getDomainSize( void ) const { return _params._domainSize; }
	unsigned int getCapacity( void ) const {
		unsigned int totalCapacity = 0;
		{ foreach(_codomainSize,i) totalCapacity += _machine[i]->getCapacity(); }
		return totalCapacity;
	}
	unsigned int getCount( void ) const {
		unsigned int totalCount = 0;
		{ foreach(_codomainSize,i) totalCount += _machine[i]->getCount(); }
		return totalCount;
	}
	// resetting the machine
    void reset( void ) {
        { foreach(_codomainSize,i) { _machine[i]->reset(); } }
    }
    // loading and saving data / models
    void saveModel( void ) {
        { foreach(_codomainSize,i) { _machine[i]->saveModel(); } }
    }
    bool loadModel( void ) {
        bool res = true;
        { foreach(_codomainSize,i) { 
            res &= _machine[i]->loadModel();
        } }
        return res;
    }
    void save( void ) {
        { foreach(_codomainSize,i) { _machine[i]->save(); } }
    }
    bool load( void ) {
        bool res = true;
        { foreach(_codomainSize,i) { 
            res &= _machine[i]->load();
        } }
        return res;
    }
    void setC( const double C ) {
        { foreach(_codomainSize,i) { _machine[i]->setC(C); } }
    }
    bool addExample( const real x[], const real y[] ) {
        bool res = true;
        { foreach(_codomainSize,i) { 
            res &= _machine[i]->addExample(x,y[i]);
        } }
        return res;
    }
    void train( void ) {
        { foreach(_codomainSize,i) { _machine[i]->train(); } }
    }
    void predict( const real x[] ) {
        { foreach(_codomainSize,i) { _predicted[i] = _machine[i]->predict(x); } }
    }
	void filterSVs( void) {
        { foreach(_codomainSize,i) { _machine[i]->filterSVs(); } }
	}
    real* _predicted;

private:
	// what kind of machines do we use?
	int _machineType;
	// normalisers and machines
	Normaliser** _norm;
    libsvmLearningMachine** _machine;
	// size of the codomain
    unsigned int _codomainSize;
	// machines parameters (all the same)
    libsvmLearningMachine::params _params;
};
