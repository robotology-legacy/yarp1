// main.cpp : a learning module
//

// ---------- headers

#include <iostream>
#include <conio.h>

#include <yarp/YARPConfig.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>

#include "SVMLearningMachine.h"
#include "lMCommands.h"

using namespace std;

// ---------- prototypes

void registerPorts( void );
void unregisterPorts( void );
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
int numOfExamples;

// whta kind of machine? (default: plain)
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

// the learning machine
SVMLearningMachine* learner;

// ---------- procedures

void RegisterPort(YARPPort& port, YARPString& name, YARPString& net)
{

	if ( port.Register(name.c_str(), net.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << name << " on network " << net << endl;
		exit(YARP_FAIL);
	}

}

void registerPorts()
{

	YARPString fullPortName;

	fullPortName = "/" + portName + "/o:vec"; RegisterPort(data_out, fullPortName, netName);
	fullPortName = "/" + portName + "/i:vec"; RegisterPort(data_in, fullPortName, netName);
	fullPortName = "/" + portName + "/o:int"; RegisterPort(cmd_out, fullPortName, netName);
	fullPortName = "/" + portName + "/i:int"; RegisterPort(cmd_in, fullPortName, netName);

}

void unregisterPorts( void )
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
	if( ! YARPParseParameters::parse(argc, argv, "-ex", &numOfExamples) ) {
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
		lMAlloc(tolerance, domainSize);
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
	cout << "CMDLINE: domain dim. " << domainSize << ", codomain dim. " << codomainSize << ", " << numOfExamples << " examples." << endl;
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
	registerPorts();

	// create the parameters
	svm_parameter param;
	if ( classification ) {
		param.svm_type = NU_SVC;
		param.kernel_type = RBF;
		param.degree = 3;
		param.gamma = 1/(double)domainSize;
		param.coef0 = 0;
		param.nu = 0.3;
		param.cache_size = 10;
		param.C = 1;
		param.eps = 1e-3;
		param.p = 0.1;
		param.shrinking = 1;
		param.probability = 0;
		param.nr_weight = 0;
		param.weight_label = 0;
		param.weight = 0;
	} else {
		param.svm_type = NU_SVR;
		param.kernel_type = RBF;
		param.degree = 3;
		param.gamma = 1/(double)domainSize;
		param.coef0 = 0;
		param.nu = 0.3;
		param.cache_size = 10;
		param.C = 30;
		param.eps = 1e-3;
		param.p = 0.1;
		param.shrinking = 1;
		param.probability = 0;
		param.nr_weight = 0;
		param.weight_label = 0;
		param.weight = 0;
	}

	// create learning machine according to cmd line
	string portNameStr(portName.c_str());
	switch ( machineType ) {
	case kindOfMachine::plain:
		learner = new SVMLearningMachine(
			classification,
			true,
			domainSize,
			codomainSize,
			numOfExamples,
			portNameStr,
			param);
		break;
	case kindOfMachine::uniform:
		learner = new UniformSVMLearningMachine(
			classification,
			true,
			domainSize,
			codomainSize,
			numOfExamples,
			portNameStr,
			tolerance,
			param);
		break;
	case kindOfMachine::feedback:
		learner = new FBSVMLearningMachine(
			classification,
			true,
			domainSize,
			codomainSize,
			numOfExamples,
			portNameStr,
			param);
		break;
	}
	if ( learner == 0 ) {
		cout << "FATAL ERROR: no memory for the learning machine." << endl;
		exit(YARP_FAIL);
	}

	// try and load previously saved model and data
	learner->load();

	// vectors and arrays of double to interact with the machine
	YVector example, prediction(codomainSize);
	double* x, * y;
	lMAlloc(x,domainSize);
	lMAlloc(y,codomainSize);

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
				learner->reset();
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
				learner->predictValue(x, y);
				{ foreach(codomainSize,i) prediction[i] = y[i]; } 
				data_out.Content() = prediction;
				data_out.Write();
			} else if ( example.Length() == domainSize+codomainSize ) {
				// a full example: add it
				{ foreach(domainSize,i) x[i] = example[i]; }
				{ foreach_s(domainSize,domainSize+codomainSize,i) y[i-domainSize] = example[i]; }
				if ( learner->addExample(x, y) ) {
					cout << learner->getExampleCount() << "/" << learner->getNumOfExamples() << "     \r";
					// every now and then, train and save the model
					if ( learner->getExampleCount() % 20 == 5 ) {
						learner->train();
						learner->save();
					}
				}
			} else {
				cout << "ERROR: got sample/example of the wrong size." << endl;
			}
		}

		// ----- any keyboard commands?
		if ( _kbhit() ) {
			switch ( _getch() ) {
			case 'c': // CHANGE C
				double newC;
				cout << "Got CHANGE C command." << endl;
				cout << "Insert new 'C' value";
				cin>>newC;
				learner->changeC(newC);
				learner->train();
				learner->save();
				break;
			case 'r': // RESET
				cout << "Got RESET command. Resetting machine." << endl;
				learner->reset();
				break;
			case 'q': // QUIT
				cout << "Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			default: // unrecognised keyboard command
				cout << "Unrecognised keyboard command." << endl;
				cout << "'r' reset machine" << endl;
				cout << "'c' change 'C'" << endl;
				cout << "'q' quit" << endl << endl;
			}
		}

		// wait for some time
		YARPTime::DelayInSeconds(1.0/25.0);

	} // while ( goOn )

	cout << "Bailing out." << endl;
	unregisterPorts();
	if ( machineType == kindOfMachine::uniform ) {
		delete[] tolerance;
	}
	delete[] x;
	delete[] y;
	delete learner;

	return 0;

}
