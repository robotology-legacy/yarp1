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
YARPInputPortOf<YVector> data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
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
double* tolerance;

// the learning machine
LearningMachine* learner;

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
		cout << "  [--u <tolerance values>]" << endl;
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
		tolerance = new double[domainSize];
		if ( tolerance == 0 ) {
			cout << "FATAL ERROR: no memory for the tolerances." << endl;
			exit(YARP_FAIL);
		}
		{ foreach_s(index+1,index+1+domainSize,i) 
			if ( sscanf(argv[i], "%lf", &tolerance[i-index-1]) != 1 ) {
				cout << "FATAL ERROR: invalid tolerance value." << endl;
				exit(YARP_FAIL);
			}
		}
	}

	// show parameters
	cout << "CMDLINE: port basename is " << portName << "." << endl;
	cout << "CMDLINE: network name is " << netName << "." << endl;
	cout << "CMDLINE: mapping R^" << domainSize << " to R^" << codomainSize << ", " << numOfExamples << " examples." << endl;
	if ( uniformMachine ) {
		cout << "CMDLINE: uniform machine selected. Tolerances are: ";
		{ foreach(domainSize,i) cout << tolerance[i] << " "; }
		cout << endl;
	}

}

// ---------- main

int main ( int argc, char** argv )
{

	// parse cmd line
	parseCmdLine( argc, argv );

	// register ports
	registerPorts();

	// create learning machine according to cmd line
	if ( uniformMachine ) {
		string portNameString(portName.c_str());
		learner = new UniformSVMLearningMachine(domainSize,codomainSize,numOfExamples,portNameString,tolerance);
	} else {
		string portNameString(portName.c_str());
		learner = new SVMLearningMachine(domainSize,codomainSize,numOfExamples,portNameString);
	}
	if ( learner == 0 ) {
		cout << "FATAL ERROR: no memory for the learning machine." << endl;
		exit(YARP_FAIL);
	}

	// ------------ main command parsing loop

	bool goOn = true;

	while ( goOn ) {

		// ----- we've received a command!
		if ( cmd_inport.Read(false) ) {
			switch ( cmd_inport.Content() ) {
			case lMCommand::Quit: // QUIT
				cmd_outport.Content() = lMCommand::Ok;
				cmd_outport.Write(true);
				goOn = false;
				break;
			case lMCommand::Reset: // RESET
				learner->reset();
				cmd_outport.Content() = lMCommand::Ok;
				cmd_outport.Write(true);
				break;
			default: // unrecognised port command
				cout << "Unrecognised command from the network." << endl;
				cmd_outport.Content() = lMCommand::Failed;
				cmd_outport.Write(true);
			}
		}

		// ----- we've read an example!
		if ( data_inport.Read(false) ) {
		}

		// ----- any keyboard commands?
		if ( _kbhit() ) {
			switch ( _getch() ) {
			case 'r': // RESET
				learner->reset();
				cout << "Machine has been reset." << endl;
				break;
			case 'q': // QUIT
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
	unregisterPorts();
	if ( uniformMachine ) delete[] tolerance;
	delete learner;

	return 0;

}
