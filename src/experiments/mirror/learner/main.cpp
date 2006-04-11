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

// YARP communication ports: data ports
YARPOutputPortOf<YVector> data_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
//YARPInputPortOf<YVector> data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<YVector> data_inport (YARPInputPort::NO_BUFFERS, YARP_TCP);
// YARP communication ports: cmd ports.
// must be NO_BUFFERS, otherwise sequentiality is lost -
// to improve synchronous communication, then, we force .write(true) on this port
YARPOutputPortOf<int> cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<int> cmd_inport (YARPInputPort::NO_BUFFERS, YARP_TCP);

// port basename and network name
YARPString portName("learner");
YARPString netName("default");

// domain and codomain size and number of examples
int domainSize;
int codomainSize;
int numOfExamples;

// if it is a uniform machine
bool uniformMachine;
bool classification;
double* tolerance;

// the learning machine
//LearningMachine* learner;
SVMLearningMachine* learner;

// ---------- procedures

void registerPorts( void )
{

	YARPString fullPortName;

	// register data output port
	cout << "Registering data output port..." << endl;
	fullPortName = "/" + portName + "/o:vec";
	if ( data_outport.Register(fullPortName.c_str(),netName.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << fullPortName << endl;
		exit(YARP_FAIL);
	} else {
		cout << "done." << endl;
	}
	
	// register data input port
	cout << "Registering data input port..." << endl;
	fullPortName = "/" + portName + "/i:vec";
	if ( data_inport.Register(fullPortName.c_str(),netName.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << fullPortName << endl;
		exit(YARP_FAIL);
	} else {
		cout << "done." << endl;
	}
	
	// register cmd output port
	cout << "Registering command output port..." << endl;
	fullPortName = "/" + portName + "/o:int";
	if ( cmd_outport.Register(fullPortName.c_str(),netName.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << fullPortName << endl;
		exit(YARP_FAIL);
	} else {
		cout << "done." << endl;
	}
	
	// register cmd input port
	cout << "Registering command input port..." << endl;
	fullPortName = "/" + portName + "/i:int";
	if ( cmd_inport.Register(fullPortName.c_str(),netName.c_str()) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << fullPortName << endl;
		exit(YARP_FAIL);
	} else {
		cout << "done." << endl;
	}
	
}

void unregisterPorts( void )
{

	cmd_inport.Unregister();
	cmd_outport.Unregister();
	data_inport.Unregister();
	data_outport.Unregister();

}

void parseCmdLine( int argc, char** argv )
{

	// help requested?
	if ( YARPParseParameters::parse(argc, argv, "-help") ) {
		cout << "USAGE: " << argv[0] << endl;
		cout << "  --dom <domain size> --cod <codomain size>" << endl;
		cout << "  --ex <number of examples>" << endl;
		cout << "  [--port <port basename>] [--net <network name>]" << endl;
		cout << "  [--u <tolerance values>] [--cla]" << endl;
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
		uniformMachine = true;
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

	if ( YARPParseParameters::parse(argc, argv, "-cla") )
		classification=true;

	// show parameters
	cout << "CMDLINE: port basename is " << portName << "." << endl;
	cout << "CMDLINE: network name is " << netName << "." << endl;
	cout << "CMDLINE: mapping R^" << domainSize << " to R^" << codomainSize << ", " << numOfExamples << " examples." << endl;
	if ( uniformMachine ) {
		cout << "CMDLINE: uniform machine selected. Tolerances are: ";
		{ foreach(domainSize,i) cout << tolerance[i] << " "; }
		cout << endl;
	}
	if ( classification )
		cout << "CMDLINE: classification machine selected."<<endl;


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
	if ( uniformMachine ) {
		string portNameString(portName.c_str());
		learner = new UniformSVMLearningMachine(classification,domainSize,codomainSize,numOfExamples,portNameString,tolerance,param);
	} else {
		string portNameString(portName.c_str());
		learner = new SVMLearningMachine(classification,domainSize,codomainSize,numOfExamples,portNameString,param);
	}
	if ( learner == 0 ) {
		cout << "FATAL ERROR: no memory for the learning machine." << endl;
		exit(YARP_FAIL);
	}

	// try and load a previously saved model
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
		if ( cmd_inport.Read(false) ) {
			switch ( cmd_inport.Content() ) {
			case lMCommand::Quit: // QUIT
				cmd_outport.Content() = lMCommand::Ok;
				cmd_outport.Write(true);
				cout << "Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			case lMCommand::Reset: // RESET
				cout << "Got RESET command. Resetting machine." << endl;
				learner->reset();
				cmd_outport.Content() = lMCommand::Ok;
				cmd_outport.Write(true);
				break;
			default: // unrecognised port command
				cout << "Unrecognised command from the network " << cmd_inport.Content() << "." << endl;
				cmd_outport.Content() = lMCommand::Failed;
				cmd_outport.Write(true);
			}
		}

		// ----- read an example or a sample to be predicted
		if ( data_inport.Read(false) ) {
			example = data_inport.Content();
			if ( example.Length() == domainSize ) {
				// this is just a sample: then predict and send
				{ foreach(domainSize,i) x[i] = example[i]; }
				learner->predictValue(x, y);
				{ foreach(codomainSize,i) prediction[i] = y[i]; } 
				data_outport.Content() = prediction;
				data_outport.Write();
			} else if ( example.Length() == domainSize+codomainSize ) {
				// a full example: add it
				// but first try to predict it
				{ foreach(domainSize,i) x[i] = example[i]; }
				learner->predictValue(x, y);
				double errMax=0;
				{ foreach(codomainSize,i) {
					double tmp=abs(example[domainSize+i] - y[i]);
					if (errMax<tmp) errMax=tmp;
				} }
				
				{ foreach_s(domainSize,domainSize+codomainSize,i) y[i-domainSize] = example[i]; }
				learner->addExample(x, y);

				cout << learner->getExampleCount() << "/" << learner->getNumOfExamples() << " (errMax="<<errMax<<")     \r";
				// every now and then, train and save the model
				if ( learner->getExampleCount() % 20 == 5 ) {
					learner->train();
					learner->save();
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
	if ( uniformMachine ) delete[] tolerance;
	delete[] x;
	delete[] y;
	delete learner;

	return 0;

}
