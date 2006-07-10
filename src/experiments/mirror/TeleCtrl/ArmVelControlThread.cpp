#include "ArmVelControlThread.h"

// -----------------------------------
// thread body
// -----------------------------------

void ArmVelControlThread::Body (void)
{

	// for the inverse kinematics
	YVector desired_X(3), required_Q(3), starting_Q(3), current_X(3);
	starting_Q[0] = 0; starting_Q[1] = 0; starting_Q[2] = 0;

	double frX, frY, frZ, frAz, frEl, frRo;
	double dx, dy, dz;
	double prevDx = 0.0, prevDy = 0.0, prevDz = 0.0, prevRo = 0.0, prevWristPitch = 0.0;
	double rot[4][4];

	rot[1][1] =  cos(_ref.el)*cos(_ref.az); rot[1][2] = -sin(_ref.az)*cos(_ref.el); rot[1][3] =  -sin(_ref.el);
	rot[2][1] =  sin(_ref.az);              rot[2][2] =  cos(_ref.az);              rot[2][3] =  0;
	rot[3][1] =  sin(_ref.el)*cos(_ref.az); rot[3][2] = -sin(_ref.el)*sin(_ref.az); rot[3][3] =  cos(_ref.el);

	// loop until the thread is terminated
	while ( !IsTerminated() ) {

		// set operating frequency
		YARPTime::DelayInSeconds(_streamingFrequency);
	    
		// evaluate data wrt reference frame
	    dx = _data.tracker0Data.x - _ref.x;
	    dy = _data.tracker0Data.y - _ref.y;
	    dz = _data.tracker0Data.z - _ref.z;
		frX  = rot[1][1]*dx +rot[1][2]*dy +rot[1][3]*dz;
	    frY  = rot[2][1]*dx +rot[2][2]*dy +rot[2][3]*dz;
	    frZ  = rot[3][1]*dx +rot[3][2]*dy +rot[3][3]*dz;
	    frAz = DegRad * _data.tracker0Data.azimuth - _ref.az;
	    frEl = DegRad * _data.tracker0Data.elevation - _ref.el;
	    frRo = DegRad * _data.tracker0Data.roll - _ref.ro;

		// compute arm joint angles: set the desired_X (in cms)
		desired_X[0] = frX*InchCm; desired_X[1] = -frY*InchCm; desired_X[2] = frZ*InchCm;
		// evaluate inverse kinematics according to tracker position
		inverse_kinematics(desired_X, required_Q, starting_Q);
		// let next starting point be the one we've just found
		starting_Q = required_Q; // all Qs are in DEGREES

	    cout 
			<< "X " << desired_X[0]  << "\t" << desired_X[1]  << "\t" << desired_X[2] << "\t"
			<< "Q " << required_Q[0] << "\t" << required_Q[1] << "\t" << required_Q[2] << "\t"
//			<< "roll " << frRo/DegRad << "\t"
//			<< "wrist pitch " << _armInit4+30-_wristPitch(_data.gloveData.wristPitch)/DegRad << "\t"
			<< "       \r";
		cout.flush();

		// send commands to the arm, after correction (_armInits and signs)
		sendPosCmd(
			(_armInit0-required_Q[0])*DegRad,
			(_armInit1-required_Q[1])*DegRad,
			(_armInit2-required_Q[2])*DegRad,
			_armInit3*DegRad+frRo,
			(_armInit4+20)*DegRad-_wristPitch(_data.gloveData.wristPitch),
			_armInit5*DegRad
		);
	}
    
	return;

}

// -----------------------------------
// thread mandatory methods
// -----------------------------------

void ArmVelControlThread::calibrate()
{

	// gather reference coordinates
	cout << "(arm calibration 1/3) Place hand at origin and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
	_ref.x  = _data.tracker0Data.x;
	_ref.y  = _data.tracker0Data.y;
	_ref.z  = _data.tracker0Data.z;
	_ref.az = DegRad * _data.tracker0Data.azimuth;
	_ref.el = DegRad * _data.tracker0Data.elevation;
	_ref.ro = DegRad * _data.tracker0Data.roll;

	// calibrate wrist pitch
	// pitch down
	int glovePitchDown;
	cout << "(arm calibration 2/3) Bend your wrist down and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    glovePitchDown = _data.gloveData.wristPitch;
    // pitch up
	int glovePitchUp;
	cout << "(arm calibration 3/3) Bend your wrist up and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
    glovePitchUp = _data.gloveData.wristPitch;
    cout << "wrist pitch " << glovePitchDown << " - " << glovePitchUp  << endl;
    // calibrate wrist pitch
	_wristPitch.eval(glovePitchUp,glovePitchDown);

}

void ArmVelControlThread::initialise()
{

	// have arm and hand positioned in the starting position
	sendPosCmd(_armInit0*DegRad,_armInit1*DegRad,_armInit2*DegRad,
			   _armInit3*DegRad,_armInit4*DegRad,_armInit5*DegRad );

    cout << endl << "Positioning. Press any key when finished. "; cout.flush(); cin.get();
		
}

void ArmVelControlThread::shutdown()
{

	// let the arm go to rest

	YARPBabyBottle tmpBottle;
	
	tmpBottle.setID(YBVMotorLabel);
	tmpBottle.reset();
	tmpBottle.writeVocab(YBVocab(YBVArmForceResting));

	if ( _enabled ) {
		_sema.Wait();
		_outPort.Content() = tmpBottle;
		_outPort.Write();
		_sema.Post();
	}

}

void ArmVelControlThread::sendPosCmd(const double dof1, const double dof2, const double dof3,
								  const double dof4, const double dof5, const double dof6)
{

	YVector armCmd(6);
	YARPBabyBottle tmpBottle;
	tmpBottle.setID(YBVMotorLabel);

	// send a command to the arm
	armCmd(1) = dof1;
	armCmd(2) = dof2;
	armCmd(3) = dof3;
	armCmd(4) = dof4;
	armCmd(5) = dof5;
	armCmd(6) = dof6;

	tmpBottle.reset();
	tmpBottle.writeVocab(YBVocab(YBVArmForceNewCmd));
	tmpBottle.writeYVector(armCmd);
	
	if ( _enabled ) {
		_sema.Wait();
		_outPort.Content() = tmpBottle;
		_outPort.Write(true);
		_sema.Post();
	}
  
}
