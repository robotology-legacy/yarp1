#include "ArmPosControlThread.h"

// -----------------------------------
// thread body
// -----------------------------------

void ArmPosControlThread::Body (void)
{

	double frX, frY, frZ, frRo, dx, dy, dz;
	YVector desired_X(3), required_Q(3), starting_Q(3);
	starting_Q(3) = 0.0;

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
	    frRo = _data.tracker0Data.roll - _ref.ro;

		// set the desired_X (in cms)
		desired_X[0] = frX*InchCm; desired_X[1] = -frY*InchCm; desired_X[2] = frZ*InchCm;
		// evaluate inverse kinematics
		inverse_kinematics(desired_X, required_Q, starting_Q);
		// let next starting point be the one we've just found
		starting_Q = required_Q; // all Qs are in DEGREES

/*	    cout 
			<< "X " << desired_X[0]  << "\t" << desired_X[1]  << "\t" << desired_X[2] << "\t"
			<< "Q " << required_Q[0] << "\t" << required_Q[1] << "\t" << required_Q[2] << "\t"
			<< "roll " << _armInit3+_wristRoll(frRo/DegRad) << "\t"
			<< "pitch " << _armInit4+_wristPitch(_data.gloveData.wristPitch) << "\t"
			<< "       \r";
		cout.flush();*/

		// send commands to the arm, after correction (_armInits and signs)
		sendPosCmd(
			_armInit0-required_Q[0],
			_armInit1-required_Q[1],
			_armInit2-required_Q[2],
			_armInit3+_wristRoll(frRo),
			_armInit4+_wristPitch(_data.gloveData.wristPitch),
			_armInit5
		);
	}
    
	return;

}

// -----------------------------------
// thread mandatory methods
// -----------------------------------

void ArmPosControlThread::calibrate()
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

void ArmPosControlThread::initialise()
{

	// have arm and hand positioned in the starting position
	sendPosCmd(_armInit0,_armInit1,_armInit2,_armInit3,_armInit4,_armInit5 );

    cout << endl << "Positioning. Press any key when finished. "; cout.flush(); cin.get();
		
}

void ArmPosControlThread::shutdown()
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

void ArmPosControlThread::sendPosCmd(const double dof1, const double dof2, const double dof3,
								  const double dof4, const double dof5, const double dof6)
{

	// this function needs DEGREES; it converts them to radiants

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
// inverse kinematics computation
// -----------------------------------

// inverse kinematics desired configuration
YVector desired_cfg(3);

void inverse_kinematics(YVector& X, YVector& Q, YVector& init)
{

	// here we are. you get me the desired Cartesian position 
	// of the e.e., X, and a starting point, init, and I am going
	// to get you the appropriate Qs (IN DEGREES)

	// set desired cfg
	desired_cfg[0] = X[0]; desired_cfg[1] = X[1]; desired_cfg[2] = X[2];

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
			p[i][j] = ( i==(j+1) ? init[j]+simplex_dim : init[j] );
			x[j] = p[i][j];
	    }
		y[i] = evaluate_error(x);
	}
	// go for the gold!
	amoeba(p,y,FTOL,evaluate_error,nfunc);

	// ok, show me what you got!
	Q[0] = p[0][0]; Q[1] = p[0][1]; Q[2] = p[0][2];

}

DP evaluate_error (Vec_I_DP& Q_NR)
{

	// error of the forward kinematics w.r.t. a given desired position

	// translate from NR's vector representation to YARP's
	YVector X(3), Q(3);
	Q[0] = Q_NR[0]; Q[1] = Q_NR[1]; Q[2] = Q_NR[2];

	// evaluate forward kinematics
	forward_kinematics(Q,X);

	// return norm of distance to desired position; add a punishment term
	// which avoids bending the elbow in a totally innatural way
	return (X - desired_cfg).norm2() + (0.0001*(Q[2]+90)*(Q[2]+90));

}
  
void forward_kinematics(YVector& Q, YVector& X)
{

	// forward kinematics: for given joint coordinates Q (deg), give end-effector position X

	// WARNING. this function is HIGHLY dependent on
	//   (1) the Babybot arm, that is, a PUMA200,
	//   (2) the initial position of the arm, which MUST be set each time during the
	//       startup phase (check the main() procedure and the _armInit globals)

	// specifications of the joints - taken from the PUMA200 manual
	// lenghts of the joints - Figure 2-1 of the manual
	const double L1 = 13.0*InchCm, L2 = 7.8*InchCm, L2prime = -0.75*InchCm, L3 = 8.0*InchCm,
				 H1 = 0.0, H2 = 5.9*InchCm-2.5;

	// turn degrees into radiants
	Q[0] *= DegRad;
	Q[1] *= DegRad;
	Q[2] *= DegRad;

	/* the forward kinematics exression has been evaluated symbolically via
	   matlab, using this simple .m file:

		function sym_for_kin

		syms Q0 Q1 Q2 L1 L2 L2prime L3 H1 H2 real

	    % first joint (rot Q0 about Y)
		e_csi_1 = [ [  cos(Q0)        0  sin(Q0)        0 ];
			        [        0        1        0        0 ];
				    [ -sin(Q0)        0  cos(Q0)        0 ];
					[        0        0        0        1 ] ];

		% second joint (rot Q1 about Z, trans L1 on Y, trans H1 on Z)
		e_csi_2 = [ [  cos(Q1) -sin(Q1)        0        0  ];
			        [  sin(Q1)  cos(Q1)        0        L1 ];
				    [        0        0        1        H1 ];
					[        0        0        0        1  ] ];

		% third joint (rot Q2 about Z, trans L2 on X, trans L2prime on Y, trans H2 on Z)
		e_csi_3 = [ [  cos(Q2) -sin(Q2)        0        L2 ];
			        [  sin(Q2)  cos(Q2)        0        L2prime ];
				    [        0        0        1        H2 ];
					[        0        0        0        1  ] ];

		% end effector (trans L3 on X)
		g_st_0 =  [ [        1        0        0        L3 ];
			        [        0        1        0        0  ];
				    [        0        0        1        0  ];
					[        0        0        0        1  ] ];

		% end-effector's position, translated back to its frame
		T = e_csi_1 * (e_csi_2 * (e_csi_3 * g_st_0));
		zero = [0 0 0 1]';
		X = (T * zero) - [L2+L3 L1+L2prime H1+H2 1]'

	*/

	X[0] = cos(Q[0])*(cos(Q[1])*(cos(Q[2])*L3+L2)-sin(Q[1])*(sin(Q[2])*L3+L2prime))+sin(Q[0])*(H2+H1)-L2-L3;
	X[1] = sin(Q[1])*(cos(Q[2])*L3+L2)+cos(Q[1])*(sin(Q[2])*L3+L2prime)-L2prime;
	X[2] = -sin(Q[0])*(cos(Q[1])*(cos(Q[2])*L3+L2)-sin(Q[1])*(sin(Q[2])*L3+L2prime))+cos(Q[0])*(H2+H1)-H2-H1;

	// revert the Q to degrees
	Q[0] /= DegRad;
	Q[1] /= DegRad;
	Q[2] /= DegRad;

}

// -----------------------------------
// from Numerical Recipes
// -----------------------------------

inline void get_psum(Mat_I_DP& p, Vec_O_DP& psum)
{
    
	int i,j;
    DP sum;
    int mpts = p.nrows();
    int ndim = p.ncols();
    
    for (j=0;j<ndim;j++) {
		for (sum=0.0,i=0;i<mpts;i++) {
			sum += p[i][j];
		}
		psum[j] = sum;
    }

}
  
void amoeba(Mat_IO_DP& p, Vec_IO_DP& y, const DP ftol, DP funk(Vec_I_DP&), int& nfunk)
{
    
    const int NMAX=5000;
    const DP TINY = 1.0e-10;
    int i, ihi, ilo, inhi, j;
    DP rtol, ysave, ytry;
    int mpts = p.nrows();
    int ndim = p.ncols();
    Vec_DP psum(ndim);

    nfunk = 0;

    get_psum( p, psum );
    
    for (;;) {

      ilo = 0;
      ihi = (y[0]>y[1]) ? (inhi=1,0) : (inhi=0,1);

      for (i=0;i<mpts;i++) {

        if (y[i] <= y[ilo]) {
          ilo = i;
        }

        if (y[i] > y[ihi]) {
          inhi = ihi;
          ihi = i;
        } else if (y[i] > y[inhi] && i != ihi) {
          inhi = i;
        }

      }
      
      rtol = 2.0*fabs(y[ihi]-y[ilo]) / (fabs(y[ihi])+fabs(y[ilo])+TINY);
      
      if (rtol < ftol) {
        SWAP( y[0], y[ilo] );
        for (i=0;i<ndim;i++) {
          SWAP( p[0][i], p[ilo][i] );
        }
        break;
      }
      
      if (nfunk >= NMAX) {
        //        nrerror("NMAX exceeded");
        return;
      }
      
      nfunk += 2;
      ytry = amotry( p, y, psum, funk, ihi, -1.0 );
      
      if (ytry <= y[ilo]) {

        ytry = amotry( p, y, psum, funk, ihi, 2.0 );

      } else if (ytry >= y[inhi]) {

        ysave = y[ihi];
        ytry = amotry( p, y, psum, funk, ihi, 0.5 );
        if (ytry >= ysave) {
          for (i=0;i<mpts;i++) {
            if (i != ilo) {
              for (j=0;j<ndim;j++) {
                p[i][j] = psum[j] = 0.5*(p[i][j]+p[ilo][j]);
              }
              y[i] = funk(psum);
            }
          }
          nfunk += ndim;
          get_psum( p, psum );
        }

      } else {

        --nfunk;

      }

    }
    
}

DP amotry(Mat_IO_DP& p, Vec_O_DP& y, Vec_IO_DP& psum, DP funk(Vec_I_DP&), const int ihi, const DP fac)
{
    
    int j;
    DP fac1, fac2, ytry;
    int ndim = p.ncols();
    Vec_DP ptry(ndim);
    
    fac1 = (1.0-fac) / ndim;
    fac2 = fac1 - fac;
    
    for (j=0;j<ndim;j++) {
      ptry[j] = psum[j]*fac1 - p[ihi][j]*fac2;
    }
    
    ytry = funk(ptry);
    
    if (ytry < y[ihi]) {

      y[ihi] = ytry;

      for (j=0;j<ndim;j++) {
        psum[j] += ptry[j] - p[ihi][j];
        p[ihi][j] = ptry[j];
      }

    }

    return ytry;
    
}
