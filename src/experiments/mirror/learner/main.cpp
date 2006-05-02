// main.cpp : a learner application
//

// ---------- headers

#include<conio.h>

#include "libsvmLearningMachine.h"
#include "lMCommands.h"

#include <yarp/YARPConfig.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>

using namespace std;

// ---------- a class enforcing multiple-output learning machines
template <class LM> class learner {
public:
    learner(unsigned int codomainSize, LM::paramsType params)
        : _codomainSize(codomainSize), _params(params)
    {
        // allocate machines (with correct constructor)
        lmAlloc(_machine, _codomainSize);
        { foreach(codomainSize,i) { _machine[i] = new LM(_params); } }
        // allocate space for the predicted vector
        lmAlloc(_predicted, _codomainSize);
    }

	// viewing counters
	unsigned int getDomainSize( void ) const { return _params._domainSize; }
	unsigned int getCapacity( void ) const { return _params._capacity; }
	unsigned int getCount( void ) const { return _machine[0]->getCount(); }
	// resetting the machine
    void reset( void ) {
        { foreach(codomainSize,i) { _machine[i]->reset(); } }
    }
    // loading and saving status
    void save( void ) {
        { foreach(codomainSize,i) { _machine[i]->save(); } }
    }
    bool load( void ) {
        bool res = false;
        { foreach(codomainSize,i) { 
            res &= _machine[i]->load();
        } }
        return res;
    }

    bool addExample( const real x[], const real y[] ) {
        bool res = false;
        { foreach(codomainSize,i) { 
            res &= _machine[i]->addExample(x,y[i]);
        } }
        return res;
    }
    void train( void ) {
        { foreach(codomainSize,i) { _machine[i]->train(); } }
    }
    void predict( const real x[] ) {
        { foreach(codomainSize,i) { _predicted[i] = _machine[i]->predict(x); } }
    }

    real* _predicted;

private:
    LM** _machine;
    unsigned int _codomainSize;
    LM::paramsType _params;
};

// ---------- prototypes

void registerPort( void );
void registerAllPorts( void );
void unregisterAllPorts( void );
void parseCmdLine( int, char** );
int main( int, char** );

// ---------- global variables

// data ports
YARPOutputPortOf<YVector> data_out (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<YVector> data_in (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
// cmd ports. must be NO_BUFFERS, otherwise sequentiality is lost
YARPOutputPortOf<int> cmd_out (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<int> cmd_in (YARPInputPort::NO_BUFFERS, YARP_TCP);

// port basename and network name
YARPString portName("learner");
YARPString netName("default");

// domain and codomain size and number of examples
int domainSize;
int codomainSize;
int capacity;

// what kind of machine? (default: plain)
namespace kindOfMachine {
	enum {
		plain = 0,
		uniform = 1,
		feedback = 2
	};
};
int machineType = kindOfMachine::plain;
// in case it is a uniform machine, need array of tolerances
double* tolerance;
// are we doing classification or regression?
bool classification;

// ---------- procedures

void registerPort(YARPPort& port, YARPString& name, YARPString& net)
{

	if ( port.Register(name.c_str(), net.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << name << " on network " << net << endl;
		exit(YARP_FAIL);
	}

}

void registerAllPorts()
{

	YARPString fullPortName;

	fullPortName = "/" + portName + "/o:vec"; registerPort(data_out, fullPortName, netName);
	fullPortName = "/" + portName + "/i:vec"; registerPort(data_in, fullPortName, netName);
	fullPortName = "/" + portName + "/o:int"; registerPort(cmd_out, fullPortName, netName);
	fullPortName = "/" + portName + "/i:int"; registerPort(cmd_in, fullPortName, netName);

}

void unregisterAllPorts( void )
{

	cmd_in.Unregister();
	cmd_out.Unregister();
	data_in.Unregister();
	data_out.Unregister();

}

void parseCmdLine( int argc, char** argv )
{

	// help requested?
	if ( YARPParseParameters::parse(argc, argv, "-help") ) {
		cout << "USAGE: " << argv[0] << endl;
		cout << "  --dom <domain size> --cod <codomain size>" << endl;
		cout << "  --ex <number of examples>" << endl;
		cout << "  [--port <port basename>] [--net <network name>]" << endl;
		cout << "  [--f] [--u <tolerance values>]" << endl;
		cout << "  [--cla]" << endl;
		exit(YARP_OK);
	}

	// basename of the ports
	YARPParseParameters::parse(argc, argv, "-port", portName);
	// network name
	YARPParseParameters::parse(argc, argv, "-net", netName);
	// domain size
	if( ! YARPParseParameters::parse(argc, argv, "-dom", &domainSize) ) {
		cout << "FATAL ERROR: must specify domain size." << endl;
		exit(YARP_FAIL);
	}
	// codomain size
	if( ! YARPParseParameters::parse(argc, argv, "-cod", &codomainSize) ) {
		cout << "FATAL ERROR: must specify codomain size." << endl;
		exit(YARP_FAIL);
	}
	// number of examples
	if( ! YARPParseParameters::parse(argc, argv, "-ex", &capacity) ) {
		cout << "FATAL ERROR: must specify number of examples." << endl;
		exit(YARP_FAIL);
	}

	// uniform machine required?
	if ( YARPParseParameters::parse(argc, argv, "-u") ) {
		if ( machineType != kindOfMachine::plain ) {
			// can't have both types of machine...
			cout << "FATAL ERROR: must specify one type only of machine." << endl;
			exit(YARP_FAIL);
		}
		machineType = kindOfMachine::uniform;
		unsigned int index;
		{ foreach(argc,i) {
			YARPString argument(argv[i]);
			if ( argument == "--u" ) { index = i; break; }
		} }
		// must be followed by domainSize doubles...
		if ( argc-index < domainSize ) {
			cout << "FATAL ERROR: must specify " << domainSize << " tolerance arguments." << endl;
			exit(YARP_FAIL);
		}
		lmAlloc(tolerance, domainSize);
		{ foreach_s(index+1,index+1+domainSize,i) 
			if ( sscanf(argv[i], "%lf", &tolerance[i-index-1]) != 1 ) {
				cout << "FATAL ERROR: invalid tolerance value." << endl;
				exit(YARP_FAIL);
			}
		}
	}

	// feedback machine required?
	if ( YARPParseParameters::parse(argc, argv, "-f") ) {
		if ( machineType != kindOfMachine::plain ) {
			// can't have both types of machine...
			cout << "FATAL ERROR: must specify one type only of machine." << endl;
			exit(YARP_FAIL);
		}
		machineType = kindOfMachine::feedback;
	}

	// classification required?
	if ( YARPParseParameters::parse(argc, argv, "-cla") ) {
		classification = true;
	}

	// show parameters
	cout << "CMDLINE: port basename is " << portName << "." << endl;
	cout << "CMDLINE: network name is " << netName << "." << endl;
	cout << "CMDLINE: domain dim. " << domainSize << ", codomain dim. " << codomainSize << ", " << capacity << " examples." << endl;
	switch ( machineType ) {
	case kindOfMachine::plain:
		cout << "CMDLINE: plain machine selected." << endl;
		break;
	case kindOfMachine::uniform:
		cout << "CMDLINE: uniform machine selected. Tolerances are: ";
		{ foreach(domainSize,i) cout << tolerance[i] << " "; }
		cout << endl;
		break;
	case kindOfMachine::feedback:
		cout << "CMDLINE: feedback machine selected." << endl;
		break;
	}
	if ( classification ) {
		cout << "CMDLINE: doing classification."<<endl;
	} else {
		cout << "CMDLINE: doing regression."<<endl;
	}


}

// ---------- main

int main ( int argc, char** argv )
{

	// parse cmd line
	parseCmdLine( argc, argv );

	// register ports
	registerAllPorts();

	// create the parameters
    libsvmLearningMachine<msNormaliser>::paramsType params;
    params._capacity = capacity;
    params._domainSize = domainSize;
	string name(portName.c_str());
    params._name = name;
    if ( classification ) {
		params._svmparams.svm_type = NU_SVC;
		params._svmparams.kernel_type = RBF;
		params._svmparams.degree = 3;
		params._svmparams.gamma = 1/(double)params._domainSize;
		params._svmparams.coef0 = 0;
		params._svmparams.nu = 0.3;
		params._svmparams.cache_size = 10;
		params._svmparams.C = 1;
		params._svmparams.eps = 1e-3;
		params._svmparams.p = 0.1;
		params._svmparams.shrinking = 1;
		params._svmparams.probability = 0;
		params._svmparams.nr_weight = 0;
		params._svmparams.weight_label = 0;
		params._svmparams.weight = 0;
	} else {
		params._svmparams.svm_type = NU_SVR;
		params._svmparams.kernel_type = RBF;
		params._svmparams.degree = 3;
		params._svmparams.gamma = 1/(double)params._domainSize;
		params._svmparams.coef0 = 0;
		params._svmparams.nu = 0.3;
		params._svmparams.cache_size = 10;
		params._svmparams.C = 30;
		params._svmparams.eps = 1e-3;
		params._svmparams.p = 0.1;
		params._svmparams.shrinking = 1;
		params._svmparams.probability = 0;
		params._svmparams.nr_weight = 0;
		params._svmparams.weight_label = 0;
		params._svmparams.weight = 0;
	}

	// create learning machine according to cmd line
    learner<libsvmLearningMachine<msNormaliser> > myLearner(4,params);

//	switch ( machineType ) {
//	case kindOfMachine::plain:
  //      { foreach(codomainSize,i) {
//            learner[i] = new libsvmLearningMachine<msNormaliser>(param);
//        	if ( learner[i] == 0 ) {
//		        cout << "FATAL ERROR: no memory for the learning machine." << endl;
//		        exit(YARP_FAIL);
//	        }
  //      } }
//		break;
//	case kindOfMachine::uniform:
//		learner = new UniformSVMLearningMachine(param);
//		break;
//	case kindOfMachine::feedback:
//		learner = new FBSVMLearningMachine(param);
//		break;
//	}

	// try and load previously saved model and data
	myLearner.load();

	// vectors and arrays of double to interact with the machine
	YVector example, prediction(codomainSize);
	double* x, * y;
	lmAlloc(x,domainSize);
	lmAlloc(y,codomainSize);

	// ------------ main command parsing loop

	bool goOn = true;

	while ( goOn ) {

		// ----- command thru the network
		if ( cmd_in.Read(false) ) {
			switch ( cmd_in.Content() ) {
			case lMCommand::Quit: // QUIT
				cmd_out.Content() = lMCommand::Ok;
				cmd_out.Write(true);
				cout << "Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			case lMCommand::Reset: // RESET
				cout << "Got RESET command. Resetting machine." << endl;
				myLearner.reset();
				cmd_out.Content() = lMCommand::Ok;
				cmd_out.Write(true);
				break;
			default: // unrecognised port command
				cout << "Unrecognised command from the network " << cmd_in.Content() << "." << endl;
				cmd_out.Content() = lMCommand::Failed;
				cmd_out.Write(true);
			}
		}

		// ----- read an example or a sample to be predicted
		if ( data_in.Read(false) ) {
			example = data_in.Content();
			if ( example.Length() == domainSize ) {
				// this is just a sample: then predict and send
				{ foreach(domainSize,i) x[i] = example[i]; }
				myLearner.predict(x);
				{ foreach(codomainSize,i) prediction[i] = myLearner._predicted[i]; } 
				data_out.Content() = prediction;
				data_out.Write();
			} else if ( example.Length() == domainSize+codomainSize ) {
				// a full example: add it
				{ foreach(domainSize,i) x[i] = example[i]; }
				{ foreach_s(domainSize,domainSize+codomainSize,i) y[i-domainSize] = example[i]; }
				if ( myLearner.addExample(x, y) ) {
					cout << myLearner.getCount() << "/" << myLearner.getCapacity() << "     \r";
					// every now and then, train and save the model
					if ( myLearner.getCount() % 20 == 5 ) {
						myLearner.train();
						myLearner.save();
					}
				}
			} else {
				cout << "ERROR: got sample/example of the wrong size." << endl;
			}
		}

		// ----- any keyboard commands?
		if ( _kbhit() ) {
			switch ( _getch() ) {
			case 'r': // RESET
				cout << "Got RESET command. Resetting machine." << endl;
				myLearner.reset();
				break;
			case 'q': // QUIT
				cout << "Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			default: // unrecognised keyboard command
				cout << "Unrecognised keyboard command." << endl;
				cout << "'r' reset machine" << endl;
				cout << "'q' quit" << endl << endl;
			}
		}

		// wait for some time
		YARPTime::DelayInSeconds(1.0/25.0);

	} // while ( goOn )

	cout << "Bailing out." << endl;
	unregisterAllPorts();
	if ( machineType == kindOfMachine::uniform ) {
		delete[] tolerance;
	}
	delete[] x;
	delete[] y;

	return 0;

}
