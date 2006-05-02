// learningMachine.cpp : behaviour of learning machines
//

// #include "learningMachine.h"

// -------------------------------------------------------
// plain learning machine
// -------------------------------------------------------

template <class NORMALISER> 
LearningMachine<NORMALISER>::LearningMachine( paramsType& params )
 : _params(params), _count(0),
   _rawData(_params._capacity,_params._domainSize+1),
   _normalData(_params._capacity,_params._domainSize+1)
{

    _norm = new NORMALISER(_rawData,_normalData);
    
}

template <class NORMALISER> 
LearningMachine<NORMALISER>::LearningMachine( void )
 : _count(0),
   _rawData(_params._capacity,_params._domainSize+1),
   _normalData(_params._capacity,_params._domainSize+1)
{

    _norm = new NORMALISER(_rawData,_normalData);

}

template <class NORMALISER> 
LearningMachine<NORMALISER>::~LearningMachine( void )
{
    
    delete _norm;

}

template <class NORMALISER> 
void LearningMachine<NORMALISER>::save( void )
{

    // save non-normalised data
    string dataFileName = _params._name + ".raw.data";
    _rawData.save(dataFileName);
    cout << "saved raw data to " << dataFileName << "." << endl;
    // save normalised data
    dataFileName = _params._name + ".norm.data";
    _normalData.save(dataFileName);
    cout << "saved normalised data to " << dataFileName << "." << endl;

}

template <class NORMALISER> 
bool LearningMachine<NORMALISER>::load( void )
{

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
