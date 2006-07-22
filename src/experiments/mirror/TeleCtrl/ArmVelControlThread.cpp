#include "ArmVelControlThread.h"

YVector Xdot(3), Qdot(3), starting_Qdot(3);

// -----------------------------------
// thread body
// -----------------------------------

void ArmVelControlThread::Body (void)
{

	double frX, frY, frZ, oldFrX = 0.0, oldFrY = 0.0, oldFrZ = 0.0;
	double frRo, dx, dy, dz;

	// evaluate rotation matrix off calibration angles
	double rot[4][4];
	rot[1][1] =  cos(_ref.el)*cos(_ref.az); rot[1][2] = -sin(_ref.az)*cos(_ref.el); rot[1][3] =  -sin(_ref.el);
	rot[2][1] =  sin(_ref.az);              rot[2][2] =  cos(_ref.az);              rot[2][3] =  0;
	rot[3][1] =  sin(_ref.el)*cos(_ref.az); rot[3][2] = -sin(_ref.el)*sin(_ref.az); rot[3][3] =  cos(_ref.el);

	// start from zero
	starting_Qdot = 0.0;

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
	    frRo = _data.tracker0Data.roll - _ref.ro;

		// compute tracker velocity (cms/sec)
		Xdot(1) =  (frX-oldFrX)*InchCm/_streamingFrequency;
		Xdot(2) = -(frY-oldFrY)*InchCm/_streamingFrequency;
		Xdot(3) =  (frZ-oldFrZ)*InchCm/_streamingFrequency;
		// assume zero if too small
		if ( Xdot.norm2() < 3.0 ) {
			Xdot = 0.0;
		}

		// compute required joints velocity (rads/sec)
		inverse_velocity();
		// next time, start from here
		starting_Qdot = Qdot;

		cout 
			<< "Xdot " << Xdot(1) << " " << Xdot(2)  << " " << Xdot(3)
			<< " Q " << _armPos(1)/DegRad  << " " << _armPos(2)/DegRad << " " << _armPos(3)/DegRad
			<< " Qdot " << Qdot(1)  << " " << Qdot(2)  << " " << Qdot(3)
			<< "       \r";
		cout.flush();

		// send velocity commands to the arm
		sendVelCmd(Qdot(1), Qdot(2), Qdot(3), 0.0, 0.0, 0.0);

		// update old frame coordinates
		oldFrX = frX; oldFrY = frY; oldFrZ = frZ;
	}
    
	return;

}

// -----------------------------------
// thread mandatory methods
// -----------------------------------

void ArmVelControlThread::calibrate()
{

	// gather reference coordinates
	cout << "(arm calibration) Place hand at origin and press enter. "; cout.flush(); cin.get();
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
	_ref.x  = _data.tracker0Data.x;
	_ref.y  = _data.tracker0Data.y;
	_ref.z  = _data.tracker0Data.z;
	_ref.ro = _data.tracker0Data.roll;

	/* wrist pitch calibration is totally absolute: the one based upon
	   the subject bending his wrist cannot work well.
	   --------------
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
	   -------------- */
	_wristPitch.eval(120,210);

    // calibrate wrist roll (invariably goes from 0 to 90)
	_wristRoll.eval(0,90);

}

void ArmVelControlThread::initialise()
{

	// have arm and hand positioned in the starting position
	sendPosCmd( _armInit0,_armInit1,_armInit2,_armInit3,_armInit4,_armInit5 );

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

void ArmVelControlThread::sendVelCmd(const double dof1, const double dof2, const double dof3,
									 const double dof4, const double dof5, const double dof6)
{

	YVector armCmd(6);
	YARPBabyBottle tmpBottle;
	tmpBottle.setID(YBVMotorLabel);

	// send a command to the arm
	armCmd(1) = dof1*DegRad;
	armCmd(2) = dof2*DegRad;
	armCmd(3) = dof3*DegRad;
	armCmd(4) = dof4*DegRad;
	armCmd(5) = dof5*DegRad;
	armCmd(6) = dof6*DegRad;

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

void ArmVelControlThread::sendPosCmd(const double dof1, const double dof2, const double dof3,
									 const double dof4, const double dof5, const double dof6)
{

	YVector armCmd(6);
	YARPBabyBottle tmpBottle;
	tmpBottle.setID(YBVMotorLabel);

	// send a command to the arm
	armCmd(1) = dof1*DegRad;
	armCmd(2) = dof2*DegRad;
	armCmd(3) = dof3*DegRad;
	armCmd(4) = dof4*DegRad;
	armCmd(5) = dof5*DegRad;
	armCmd(6) = dof6*DegRad;

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

// -----------------------------------
// inverse velocity computation
// -----------------------------------

void inverse_velocity(void)
{

	// given the desired Xdot, I'll get you the appropriate Qdot

	// four points for the simplex, three are the arguments,
	// tolerance is 1e-6, starting simplex is much bigger
	const int MP = 4,NP = 3;
	const DP FTOL = 1.0e-6;
	const DP simplex_dim = 1;

	// declare x, y, p
	Vec_DP x(NP),y(MP);
	Mat_DP p(MP,NP);
  
	// initialise y and p. p's rows contain the initial simplex coordinates;
	// given the the point init, we build a simplex around it. y is the evaluated
	// in the vertices of the simplex
	int i, nfunc, j;
	for ( i=0; i<MP; i++ ) {
		for ( j=0; j<NP; j++ ) {
			p[i][j] = ( i==(j+1) ? starting_Qdot[j]+simplex_dim : starting_Qdot[j] );
			x[j] = p[i][j];
	    }
		y[i] = evaluate_error_velocity(x);
	}
	// go for the gold!
	amoeba(p,y,FTOL,evaluate_error_velocity,nfunc);

	// ok, show me what you got!
	Qdot(1) = p[0][0];
	Qdot(2) = p[0][1];
	Qdot(3) = p[0][2];

}

DP evaluate_error_velocity (Vec_I_DP& Q_NR)
{

	YVector tmpXdot(3), tmpQdot(3);
	tmpQdot(1) = Q_NR[0];
	tmpQdot(2) = Q_NR[1];
	tmpQdot(3) = Q_NR[2];

	forward_velocity(tmpQdot,tmpXdot);

	return (tmpXdot - Xdot).norm2();

}

void forward_velocity(YVector& tmpQdot, YVector& tmpXdot)
{

	// arm physical data
	const double L1 = 13.0*InchCm, L2 = 7.8*InchCm, L2prime = -0.75*InchCm, L3 = 8.0*InchCm,
				 H1 = 0.0, H2 = 5.9*InchCm-2.5;

	double Q1, Q2, Q3;
	Q1 = _armPos(1)/DegRad;
	Q2 = _armPos(2)/DegRad;
	Q3 = _armPos(3)/DegRad;

	// evaluate Jocobian matrix (done via matlab)
	double J11 = -sin(Q1)*(cos(Q2)*(cos(Q3)*L3+L2)-sin(Q2)*(sin(Q3)*L3+L2prime))+cos(Q1)*(H2+H1);
	double J12 = cos(Q1)*(-sin(Q2)*(cos(Q3)*L3+L2)-cos(Q2)*(sin(Q3)*L3+L2prime));
	double J13 = cos(Q1)*(-cos(Q2)*sin(Q3)*L3-sin(Q2)*cos(Q3)*L3);
	double J21 = 0;
	double J22 = cos(Q2)*(cos(Q3)*L3+L2)-sin(Q2)*(sin(Q3)*L3+L2prime);
	double J23 = -sin(Q2)*sin(Q3)*L3+cos(Q2)*cos(Q3)*L3;
	double J31 = -cos(Q1)*(cos(Q2)*(cos(Q3)*L3+L2)-sin(Q2)*(sin(Q3)*L3+L2prime))-sin(Q1)*(H2+H1);
	double J32 = -sin(Q1)*(-sin(Q2)*(cos(Q3)*L3+L2)-cos(Q2)*(sin(Q3)*L3+L2prime));
	double J33 = -sin(Q1)*(-cos(Q2)*sin(Q3)*L3-sin(Q2)*cos(Q3)*L3);

	// now, Xdot = J * Qdot
	tmpXdot(1) = J11*tmpQdot(1) + J12*tmpQdot(2) + J13*tmpQdot(3);
	tmpXdot(2) = J21*tmpQdot(1) + J22*tmpQdot(2) + J23*tmpQdot(3);
	tmpXdot(3) = J31*tmpQdot(1) + J32*tmpQdot(2) + J33*tmpQdot(3);

}

/*
function jacobian

syms Q1 Q2 Q3 L1 L2 L2prime L3 H1 H2 real

% ---------- evaluate forward kinematics

% first joint (rot Q1 about Y)
e_csi_1 = [ [  cos(Q1)        0  sin(Q1)        0 ];
	        [        0        1        0        0 ];
		    [ -sin(Q1)        0  cos(Q1)        0 ];
			[        0        0        0        1 ] ];

% second joint (rot Q2 about Z, trans L1 on Y, trans H1 on Z)
e_csi_2 = [ [  cos(Q2) -sin(Q2)        0        0  ];
	        [  sin(Q2)  cos(Q2)        0        L1 ];
		    [        0        0        1        H1 ];
			[        0        0        0        1  ] ];

% third joint (rot Q3 about Z, trans L2 on X, trans L2prime on Y, trans H2 on Z)
e_csi_3 = [ [  cos(Q3) -sin(Q3)        0        L2      ];
	        [  sin(Q3)  cos(Q3)        0        L2prime ];
		    [        0        0        1        H2      ];
			[        0        0        0        1       ] ];

% end effector (trans L3 on X)
g_st_0 =  [ [        1        0        0        L3 ];
	        [        0        1        0        0  ];
		    [        0        0        1        0  ];
			[        0        0        0        1  ] ];

% end-effector's position, translated back to its frame
T = e_csi_1 * (e_csi_2 * (e_csi_3 * g_st_0));
zero = [0 0 0 1]';
X = (T * zero) - [L2+L3 L1+L2prime H1+H2 1]';
X = X(1:3);

% ---------- now evaluate jacobian matrix: J_{ij} = dX(i)/DQj
J = [diff(X,'Q1') diff(X,'Q2')  diff(X,'Q3')]
*/
