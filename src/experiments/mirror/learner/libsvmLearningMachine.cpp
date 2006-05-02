// libsvmLearningMachine.cpp : behaviour of libsvm-based SVM learning machines
//

// #include "libsvmLearningMachine.h"

// -------------------------------------------------------
// libsvm learning machine
// -------------------------------------------------------

template <class NORMALISER> 
libsvmLearningMachine<NORMALISER>::libsvmLearningMachine( paramsType& params )
    : _params(params), _model(0)
{

    // allocate the problem's x's
    lmAlloc(_problem.x, _params._capacity);
   	{ foreach(_params._capacity,i) {
		lmAlloc(_problem.x[i], _params._domainSize+1);
        { foreach(_params._domainSize,j) {
			_problem.x[i][j].index = j+1;
			_problem.x[i][j].value = 0.0;
        } }
		_problem.x[i][_params._domainSize].index = -1;
    } }
    // allocate the problem's y's
	lmAlloc(_problem.y, _params._capacity);

}
    
template <class NORMALISER> 
libsvmLearningMachine<NORMALISER>::~libsvmLearningMachine()
{

    // destroy problem's y's
    delete[] _problem.y;
    // destroy problems's x's
	{ foreach(_params._capacity,i) delete[] _problem.x[i]; }
	delete[] _problem.x;
    // destroy model
    if ( _model != 0 ) svm_destroy_model( _model );
    // destroy paramters
    svm_destroy_param( &_params._svmparams );

}

template <class NORMALISER> 
void libsvmLearningMachine<NORMALISER>::reset()
{

	// destroy models and reset them to 0
	if ( _model != 0 ) svm_destroy_model( _model );
	_model = 0;

	LearningMachine<NORMALISER>::reset();

}

template <class NORMALISER> 
void libsvmLearningMachine<NORMALISER>::save()
{

	// save model
	string modelFileName = _params._name + ".model";
	if ( svm_save_model(modelFileName.c_str(), _model ) == -1 ) {
		cout << "ERROR: could not save model." << endl;
		return;
	}
	cout << "saved model to " << modelFileName << "." << endl;

	// save data
	LearningMachine<NORMALISER>::save();

}

template <class NORMALISER>
bool libsvmLearningMachine<NORMALISER>::load()
{

	// load a previously saved model
	string modelFileName = _params._name + ".model";
	_model = svm_load_model(modelFileName.c_str());
	if ( _model == 0 ) {
		cout << "no previously saved model found." << endl;
		reset();
		return false;
	}
	cout << "loaded model from " << modelFileName << "." << endl;

	// try and load data
	if ( LearningMachine<NORMALISER>::load() == false ) {
		reset();
		return false;
	}

	return true;

}

template <class NORMALISER>
bool libsvmLearningMachine<NORMALISER>::addExample( const real x[], const real y )
{

	// if the buffer is full, stop
	if ( _count == _params._capacity ) {
		return false;
	} else {
		// otherwise, is this example worth adding to the current pool?
		if ( isExampleWorthAdding(x, y) ) {
			// yes: then add it and then bail out.
            real* tmpVector;
            lmAlloc(tmpVector, _params._domainSize+1);
            tmpVector[0] = y;
            { foreach(_params._domainSize,i) tmpVector[i+1] = x[i]; }
            _rawData.add(tmpVector);
            delete[] tmpVector;
            _count++;
			return true;
		} else {
			// no. then return false
			return false;
		}
	}

}

template <class NORMALISER>
void libsvmLearningMachine<NORMALISER>::train()
{

	// destroy old model
	if ( _model != 0 ) svm_destroy_model( _model );

	// normalise data
    _norm->normaliseAll();

    // tell libsvm how may examples we have
	_problem.l = _count;

    // now copy the normalised data set and values into the problem's data structures
    // allocate the problem's x's
   	{ foreach(_params._capacity,i) {
		_problem.y[i] = _normalData(i,0);
        { foreach(_params._domainSize,j) {
			_problem.x[i][j].value = _normalData(i,j+1);
        } }
    } }

	// check problem consistency
	if ( svm_check_parameter( &_problem, &_params._svmparams ) != 0 ) {
		cout << "FATAL ERROR: libsvm parameters are incorrect." << endl;
		exit(-1);
	}

	// train !!
	_model = svm_train( &_problem, &_params._svmparams );

}

template <class NORMALISER>
real libsvmLearningMachine<NORMALISER>::predict( const real x[] )
{

	// if model is void, do not even try to predict, but return zeroes
	if ( _model == 0 ) {
		return 0.0;
	}

	// otherwise, build sample to predict (normalise it first)
    svm_node* tmpSample;
    lmAlloc(tmpSample, _params._domainSize+1);
    { foreach(_params._domainSize,i) {
    	_norm->normalise( x[i], i );
		tmpSample[i].index = i+1;
		tmpSample[i].value = x[i];
		}
	}
	tmpSample[_params._domainSize].index = -1;

	// and then predict (un-normalise before bailing out)
	real y = svm_predict(_model, tmpSample);
	y = _norm->unNormalise( y, 0 );
    delete[] tmpSample;

    return y;

}

template <class NORMALISER>
bool libsvmLearningMachine<NORMALISER>::isExampleWorthAdding ( const real x[], const real y )
{

    return true;

}

/*void SVMLearningMachine::train()
{

	// destroy old models
	{ foreach(_codomainSize,i) if ( _model[i] != 0 ) svm_destroy_model( _model[i] ); }

	// normalise the whole pool of examples
	if (_classification) {
		normaliseExamplesMeanStdDomain();
	} else {
		normaliseExamplesMeanStd();
	}

	{ foreach(_codomainSize,i) _problem[i].l = _exampleCount; }

	{ foreach(_codomainSize,i) {
		// check them.
		const char* error_msg;
		error_msg = svm_check_parameter( &_problem[i], &_param );
		if ( error_msg != 0 ) {
			cout << "FATAL ERROR: libsvm parameters are incorrect." << endl;
			exit(-1);
		}
	} }

	// train new models
	{ foreach(_codomainSize,i) _model[i] = svm_train( &_problem[i], &_param ); }

}
*/