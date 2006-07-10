// ----------------------------------------------------------------------
// TeleCtrl.cpp - a c++ application which controls a slave setup
//                using commands gathered from a master setup.
// ----------------------------------------------------------------------

#define MainRoutine

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
// the collector's and grabber's streams, and saving it all in our global structures

class acquisitionThread : public YARPThread {
public:
	virtual void Body (void) {
		while ( ! IsTerminated() ) {
			// numerical data from the collector
			_coll_data_in.Read();
		    _data = _coll_data_in.Content();
			// image from the grabber
			_imageSema.Wait();
			_img_in.Read();
			_image.Refer(_img_in.Content());
			_imageSema.Post();
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

// -----------------------------------
// main
// -----------------------------------

int main()
{

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
	ControlThread* apct = new ArmPosControlThread(1./10.,_rep_out,_repeaterSema, false);
	// arm velocity control thread
	ControlThread* avct = new ArmVelControlThread(1./10.,_rep_out,_repeaterSema, false);
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
	if ( _coll_cmd_in.Content() != (HardwareUseTracker0) ) {
		cout << "must use tracker 0, gaze tracker and dataglove only." << endl;
		unregisterPorts();
		return 0;
	}

	// calibrate and initialise hand
	hct->calibrate();
	hct->initialise();

	// calibrate and initialise arm
	apct->calibrate();
	apct->initialise();

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
	apct->Begin();
	hct->Begin();
	gct->Begin();

	// wait for a key to stop
	cin.get();
	
	cout << endl << "Stopping teleoperation... "; cout.flush();
	// stop control threads
	gct->End();
	hct->End();
	apct->End();
	// stop acquisition stream
	at.End();
	// tell collector to stop streaming
	SendCommandToCollector(CCmdStopStreaming);
	cout << "done." << endl;

	// ------------- main loop ends

	// shut down
	gct->shutdown();
	hct->shutdown();
	apct->shutdown();

	// delete threads
	delete gct;
	delete hct;
	delete apct;

	// release collector
	cout << "Now releasing collector. "; cout.flush();
	SendCommandToCollector(CCmdDisconnect);
	cout << "done." << endl;

	// unregister ports and bail out
	unregisterPorts();
	return 0;
  
}
