// main.cpp : a learner application
//

// -------------------------------------------------------
// preliminaries
// -------------------------------------------------------

#include<conio.h>

#include "libsvmLearner.h"

#include <yarp/YARPConfig.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>

//using namespace std;

// ---------- prototypes

void registerPort( void );
void registerAllPorts( void );
void unregisterAllPorts( void );
void parseCmdLine( int, char** );
int main( int, char** );

// -------------------------------------------------------
// globals
// -------------------------------------------------------

// data ports
YARPOutputPortOf<YVector> data_out (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<YVector> data_in (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
// cmd ports. must be NO_BUFFERS, otherwise sequentiality is lost
YARPOutputPortOf<int> cmd_out (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<int> cmd_in (YARPInputPort::NO_BUFFERS, YARP_TCP);

// port basename and network name
YARPString portName("learner");
YARPString netName("default");

// path where data/model files are loaded/saved
YARPString path("y:\\zgarbage\\");

// domain and codomain size and number of examples
int domainSize;
int codomainSize;
int capacity;

int machineType = typeOfMachine::plain;

// in case it is a uniform machine, need array of tolerances
real* tolerance;
// in case it is a feedback machine, need error threshold
real threshold = 0.5;
// are we doing classification or regression?
bool classification = false;
// do we want non-SVs filtering?
bool filter = false;
// do we want to load previously saved models and data, on startup?
bool load = false;

int samples = 0;

// -------------------------------------------------------
// helper procedures
// -------------------------------------------------------

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
		cout << "  [--name <learner name>] [--net <network name>]" << endl;
		cout << "  [--f] [--u <tolerance values>]" << endl;
		cout << "  [--cl] [--filter] [--load]" << endl;
		exit(YARP_OK);
	}

	// basename of the ports
	YARPParseParameters::parse(argc, argv, "-name", portName);
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
		if ( machineType != typeOfMachine::plain ) {
			// can't have both types of machine...
			cout << "FATAL ERROR: must specify one type only of machine." << endl;
			exit(YARP_FAIL);
		}
		machineType = typeOfMachine::uniform;
		unsigned int index;
		{ foreach(argc,i) {
			YARPString argument(argv[i]);
			if ( argument == "--u" ) { index = i; break; }
		} }
		// must be followed by domainSize reals...
		if ( argc-index < domainSize ) {
			cout << "FATAL ERROR: must specify " << domainSize << " tolerances after --u." << endl;
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
		if ( machineType != typeOfMachine::plain ) {
			// can't have both types of machine...
			cout << "FATAL ERROR: must specify one type only of machine." << endl;
			exit(YARP_FAIL);
		}
		machineType = typeOfMachine::feedback;
		unsigned int index;
		{ foreach(argc,i) {
			YARPString argument(argv[i]);
			if ( argument == "--f" ) { index = i; break; }
		} }
		// must be followed by a real
		if ( argc-index <= 1 ) {
			cout << "FATAL ERROR: must specify threshold after --f." << endl;
			exit(YARP_FAIL);
		}
		if ( sscanf(argv[index+1], "%lf", &threshold) != 1 ) {
			cout << "FATAL ERROR: invalid threshold value." << endl;
			exit(YARP_FAIL);
		}
	}

	// classification required?
	if ( YARPParseParameters::parse(argc, argv, "-cl") ) {
		classification = true;
	}

	// filter out non-SVs?
	if ( YARPParseParameters::parse(argc, argv, "-filter") ) {
		filter = true;
	}

	// load data and models on startup?
	if ( YARPParseParameters::parse(argc, argv, "-load") ) {
		load = true;
	}

	// show parameters
	cout << "CMDLINE: port basename is \"" << portName << "\"." << endl;
	cout << "CMDLINE: network name is \"" << netName << "\"." << endl;
	cout << "CMDLINE: domain dim. " << domainSize << ", codomain dim. " << codomainSize << ", " << capacity << " examples." << endl;
	switch ( machineType ) {
	case typeOfMachine::plain:
		cout << "CMDLINE: plain machine selected." << endl;
		break;
	case typeOfMachine::uniform:
		cout << "CMDLINE: uniform machine selected. Tolerances are ";
		{ foreach(domainSize,i) cout << tolerance[i] << " "; }
		cout << endl;
		break;
	case typeOfMachine::feedback:
		cout << "CMDLINE: feedback machine selected. Threshold is " << threshold << endl;
		break;
	}
	if ( classification ) {
		cout << "CMDLINE: doing classification."<<endl;
	} else {
		cout << "CMDLINE: doing regression."<<endl;
	}
	if ( filter ) {
		cout << "CMDLINE: non-SVs filtering: ON."<<endl;
	} else {
		cout << "CMDLINE: non-SVs filtering: OFF."<<endl;
	}
	if ( ! load ) {
		cout << "CMDLINE: this machine WON'T load/save any models/data on startup."<<endl;
	}
	cout << "CMDLINE: model/data saved in/loaded from \"" << path << "\"."<<endl;

}

// -------------------------------------------------------
// main routine
// -------------------------------------------------------

int main ( int argc, char** argv )
{

	// parse cmd line
	parseCmdLine( argc, argv );

	// register ports
	registerAllPorts();

	// create the parameters
    libsvmLearningMachine::params params;
    params._capacity = capacity;
    params._domainSize = domainSize;
	string name(portName.c_str());
    params._name = name;
    params._path = path.c_str();
    if ( classification ) {
		params._svmparams.svm_type = NU_SVC;
		params._svmparams.kernel_type = RBF;
		params._svmparams.degree = 3;
		params._svmparams.gamma = 1/(real)params._domainSize;
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
		params._svmparams.svm_type = EPSILON_SVR;
		params._svmparams.kernel_type = RBF;
		params._svmparams.degree = 3;
		params._svmparams.gamma = 1/(real)params._domainSize;
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
    params._filter = filter;

	// create learner (pool of learning machines)
	libsvmLearner learner(machineType,codomainSize,params,tolerance,threshold);

	// if required, try and load previously saved model and data
	if ( load ) {
		learner.load();
	}

	// vectors and arrays of reals to interact with the machine
	YVector example, prediction(codomainSize);
	real* x, * y;
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
				learner.reset();
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
				learner.predict(x);
				{ foreach(codomainSize,i) prediction[i] = learner._predicted[i]; } 
				data_out.Content() = prediction;
				data_out.Write();
			} else if ( example.Length() == domainSize+codomainSize ) {
				// a full example: add it
				{ foreach(domainSize,i) x[i] = example[i]; }
				{ foreach_s(domainSize,domainSize+codomainSize,i) y[i-domainSize] = example[i]; }
				if ( learner.addExample(x, y) ) {
					cout << learner.getCount() << "/" << learner.getCapacity() << "     \r";
					// every now and then, train the machines
					if ( learner.getCount() % 5 == 4 ) {
						learner.train();
						if ( filter ) {
							learner.filterSVs();
						}
						learner.save();
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
				learner.reset();
				break;
			case 'c': // CHANGE C
				double newC;
				cout << "Got CHANGE-C command." << endl;
				cout << "Insert new 'C' value";
				cin >> newC;
				learner.setC(newC);
				learner.train();
					learner.save();
				break;
			case 'q': // QUIT
				cout << "Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			default: // unrecognised keyboard command
				cout << "Unrecognised keyboard command." << endl;
				cout << "'r' reset machine" << endl;
				cout << "'c' change 'C' coefficient" << endl;
				cout << "'q' quit" << endl << endl;
			}
		}

		// wait for some time
//		YARPTime::DelayInSeconds(1.0/25.0);

	} // while ( goOn )

	cout << "Bailing out." << endl;
	unregisterAllPorts();
	if ( machineType == typeOfMachine::uniform ) {
		delete[] tolerance;
	}
	delete[] x;
	delete[] y;

	return 0;

}
