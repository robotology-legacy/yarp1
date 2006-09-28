// ----------------------------------------------------------------------
// TeleCtrl.cpp - a c++ application which controls a slave setup
//                using commands gathered from a master setup.
// ----------------------------------------------------------------------

#define MainRoutine

#include <conio.h>

#include "TeleCtrl.h"
#include "GazeControlThread.h"
#include "ArmPosControlThread.h"
#include "ArmVelControlThread.h"
#include "HandControlThread.h"

#undef MainRoutine

// -----------------------------------
// acquisition thread
// -----------------------------------

// the one and only task of this thread is that of gathering data/images from
// the collector's, grabber's and arm's streams, and saving it all in our global structures

class acquisitionThread : public YARPThread {
public:
	virtual void Body (void) {
		while ( ! IsTerminated() ) {
			// numerical data from the collector
			ReadCollectorData();
			// image from the grabber
			_imageSema.Wait();
			_img_in.Read();
			_image.Refer(_img_in.Content());
			_imageSema.Post();
			// position data from the arm
			_arm_pos_in.Read();
		    _armPos = _arm_pos_in.Content();
		}
	}
};

// -----------------------------------
// communication functions
// -----------------------------------

void RegisterPort(YARPPort& port, char* name, char* net = "default")
{

	if ( port.Register(name, net) != YARP_OK ) {
		cout << "FATAL ERROR: could not register port " << name << endl;
		exit(YARP_FAIL);
	}

}

// register ports

void registerPorts()
{

	// this procedure just registers ports; they must be properly
	// connected via the external connection script
	
	// mirrorCollector (numerical data, commands)
	RegisterPort(_coll_data_in, "/TCCollector/i:str");
	RegisterPort(_coll_cmd_in, "/TCCollector/i:int");
	RegisterPort(_coll_cmd_out, "/TCCollector/o:int");

	// Repeater (position commands)
	RegisterPort(_rep_out, "/TCRepeater/o:str");
	
	// grabber (image grabbed by babybot)
	RegisterPort(_img_in, "/TCImg/i:img", "Net1");
	// camview (image shown to the user)
	RegisterPort(_img_out, "/TCImg/o:img", "Net1");

	// headSaccades (coordinates fixated by the user)
	RegisterPort(_hs_out, "/TCHeadSaccades/o:bot");

	// arm position
	RegisterPort(_arm_pos_in, "/TCArmPos/i:vec");
}

// unregister ports

void unregisterPorts()
{

	_hs_out.Unregister();
	_img_out.Unregister();
	_img_in.Unregister();
	_rep_out.Unregister();
	_coll_cmd_out.Unregister();
	_coll_cmd_in.Unregister();
	_coll_data_in.Unregister();
	_arm_pos_in.Unregister();

}

// send a command to the collector

void SendCommandToCollector(CollectorCommand cmd)
{

	_coll_cmd_out.Content() = cmd;
	_coll_cmd_out.Write();

	_coll_cmd_in.Read();
	if ( _coll_cmd_in.Content() == CCmdFailed ) {
		cout << "failed." << endl;
		unregisterPorts();
		exit(0);
	}

}

// read data off collector

void ReadCollectorData(void)
{

	_coll_data_in.Read();
	_data = _coll_data_in.Content();

}

// check whether a key represents a command
bool isACmd(char c)
{

	for (int i=0; i<numOfCmds; ++i) {
		if ( c == keyboardCmd[i] ) {
			return true;
		}
	}

	return false;

}

// -----------------------------------
// main
// -----------------------------------

int main()
{

    libsvmLearningMachine::params params;
    params._capacity = 1000;
    params._domainSize = 6;

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

    params._filter = false;

	int machineType = typeOfMachine::plain;
	double* tolerance;

	libsvmLearner learner(typeOfMachine::plain,5,params,tolerance,0.5);

	// preliminaries
	cout.precision(2); cout.setf(ios::fixed);
	YARPScheduler::setHighResScheduling();

	// register communication ports
	registerPorts();

	// launch batch file which yarp-connects ports
	cout << endl << "Please connect ports then press enter." << endl;
	cin.get();

	// acquisition thread
	acquisitionThread at;
	// arm position control thread
	ControlThread* act = new ArmPosControlThread(0.04,_rep_out,_repeaterSema, false);
	// arm velocity control thread
//	ControlThread* act = new ArmVelControlThread(0.04,_rep_out,_repeaterSema, false);
	// hand control thread
	ControlThread* hct = new HandControlThread(1./10.,_rep_out,_repeaterSema, false);
	// gaze control thread
	ControlThread* gct = new GazeControlThread(1./10.,_hs_out,_img_out,_imageSema, false);

	// tell collector to activate the sensors
	cout << endl << "Initialising collector... "; cout.flush();
	SendCommandToCollector(CCmdConnect);
	cout << "done." << endl;

	// gather which sensors we use
	_coll_cmd_in.Read();
//	if ( _coll_cmd_in.Content() != (HardwareUseDataGlove|HardwareUseTracker0|HardwareUseGT) ) {
	if ( _coll_cmd_in.Content() != (HardwareUseTracker0|HardwareUseDataGlove) ) {
		cout << "must use tracker 0, gaze tracker and dataglove only." << endl;
		unregisterPorts();
		return 0;
	}

	// calibrate and initialise hand
	hct->calibrate();
	hct->initialise();

	// calibrate and initialise arm
	act->calibrate();
	act->initialise();

	// calibrate and initialise gaze thread
	gct->calibrate();
	gct->initialise();

	// ------------- main loop

	// tell collector to start streaming
	cout << "Teleoperation starting... "; cout.flush();
	SendCommandToCollector(CCmdStartStreaming);
	cout << "STARTED!" << endl;
	// activate acquisition thread
	at.Begin();
	// activate control threads
	act->Begin();
	hct->Begin();
	gct->Begin();

	// wait for a keyboard command
	bool goOn = true;
	while ( goOn ) {
		if ( _kbhit() ) {
			char c = _getch();
			switch ( c ) {
			case 'q': // QUIT
				cout << "MAIN THREAD: Got QUIT command. Bailing out." << endl;
				goOn = false;
				break;
			default: // unrecognised keyboard command. if it is for someone else, put it back.
				if ( isACmd(c) ) {
					_ungetch(c);
				}
				break;
			}
		}
		YARPTime::DelayInSeconds(1.0/25.0);
	} // while ( goOn )
	
	cout << endl << "Stopping teleoperation... "; cout.flush();
	// stop control threads
	gct->End();
	hct->End();
	act->End();
	// stop acquisition stream
	at.End();
	// tell collector to stop streaming
	SendCommandToCollector(CCmdStopStreaming);
	cout << "done." << endl;

	// ------------- main loop ends

	// shut down
	gct->shutdown();
	hct->shutdown();
	act->shutdown();

	// delete threads
	delete gct;
	delete hct;
	delete act;

	// release collector
	cout << "Now releasing collector. "; cout.flush();
	SendCommandToCollector(CCmdDisconnect);
	cout << "done." << endl;

	// unregister ports and bail out
	unregisterPorts();
	return 0;
  
}
