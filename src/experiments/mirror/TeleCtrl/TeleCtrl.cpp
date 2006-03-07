/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #emmebi,claudio,carlos#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: TeleCtrl.cpp,v 1.12 2006-03-07 17:10:35 claudio72 Exp $
///

// ----------------------------------------------------------------------
// TeleCtrl.cpp - a c++ application which controls a setup (the SLAVE)
//  using commands gathered from a sensory setup (MASTER). Basically this is
//  the frontend between MASTER and SLAVE.
// ----------------------------------------------------------------------

// -----------------------------------
// headers
// -----------------------------------

// standard
#include <iostream>
#include <cmath>
// ACE
#include <ace/config.h>
#include <ace/OS.h>
// YARP
#include <yarp/YARPConfig.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
// commands exchanged with mirrorCollector
#include "CollectorCommands.h"
// Numrical Recipes - minimisation algorithm for inverse kinematics
#include "nr.h"
// baby bottle. to be removed later on
#include "YARPBabyBottle.h"
#include "YARPVocab_babybot.h"

// -----------------------------------
// globals
// -----------------------------------

#define SIZE_BUF 128
#define NO_ITERATIONS 5
#define min(x,y) ((x)<(y)?(x):(y))

using namespace std;

// global options
struct SaverOptions {

	SaverOptions() {
		useCamera0 = useCamera1 = sizeX = sizeY =
		useTracker0 = useTracker1 =	useDataGlove = usePresSens = 0;
	};

	int useCamera0, useCamera1;
	int sizeX, sizeY;
	int useTracker0, useTracker1;
	int useDataGlove;
	int usePresSens;

} _options;

// standard name for port
const char *DEFAULT_TOMASTER_NAME = "TeleCtrlToMaster";
const char *DEFAULT_TOSLAVE_NAME  = "TeleCtrlToSlave";
// scripts (dis)connecting TeleCtrl's ports to MASTER's and SLAVE's ones
const char *CONNECT_SCRIPT    = "%YARP_ROOT%\\src\\experiments\\mirror\\TeleCtrl\\TeleCtrlConnect.bat";
const char *DISCONNECT_SCRIPT = "%YARP_ROOT%\\src\\experiments\\mirror\\TeleCtrl\\TeleCtrlDisconnect.bat";

// ports: data from master
YARPInputPortOf<CollectorNumericalData> _master_data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<YARPGenericImage> _master_img0_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<YARPGenericImage> _master_img1_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
// commands to/from master
YARPInputPortOf<int> _master_cmd_inport (YARPInputPort::NO_BUFFERS, YARP_TCP);
YARPOutputPortOf<CollectorCommand> _master_cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// commands to the slave
YARPOutputPortOf<YARPBabyBottle> _slave_outport(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);

// cameras
CollectorImage _img0;
CollectorImage _img1;
// numerical data
CollectorNumericalData _data;
// reference frame (set by calibration at the beginning)
double refX=0, refY=0, refZ=0, refAz=0, refEl=0, refRo=0;

// degrees to radiants
const double myDegToRad = 2*M_PI/360.0;
// inches to cms
const double myInchToCM = 2.45;
// movement tolerance (cms)
const double tolerance = 0.5/myInchToCM;
// wrist roll tolerance (deg)
const double tolerance_roll = 2.0*myDegToRad;
// streaming frequency
const double streamingFrequency = 1.0/10.0;
// inverse kinematics desired configuration - as a global, so far
YVector desired_cfg(3);

//---------------------------------------------------- 
// Dataglove local values
// 
double dThumb[2]  = {0.0, 0.0};
double dIndex[2]  = {0.0, 0.0};
double dMiddle[2] = {0.0, 0.0};
double dWristYaw = 0.0;

// Values for open hand 
int iThumbOpen[2]  = {0, 0};
int iIndexOpen[2]  = {0, 0};
int iMiddleOpen[2] = {0, 0};

// Values for close hand
int iThumbClose[2]  = {0, 0};
int iIndexClose[2]  = {0, 0};
int iMiddleClose[2] = {0, 0};

// Values for wrist
const double robotWristDown = -90*myDegToRad;
const double robotWristUp   =  90*myDegToRad;
double dWrist0;
double dWristF;

//Data glove transformation factors
double dThumbFactor[2]  = {0.0, 0.0};
double dIndexFactor[2]  = {0.0, 0.0};
double dMiddleFactor[2] = {0.0, 0.0};
double dWristYawFactor = 0.0;

// arm joints initial position (in degrees). the arm is initially
// stretched with the hand down (looks like the Fascist salutation, unluckily)
double offsetQ0 =   20.0;
double offsetQ1 =   25.0;
double offsetQ2 =  -40.0;
double offsetQ3 =    0.0;
double offsetQ4 =  -20.0;
double offsetQ5 = -180.0;

// -----------------------------------
// prototypes
// -----------------------------------

void forward_kinematics(YVector&, YVector&);
double evaluate_error(YVector&);
void inverse_kinematics(YVector&, YVector&, YVector&);
void NR::amoeba(Mat_IO_DP&, Vec_IO_DP&, const DP, DP funk(Vec_I_DP&), int&);
DP NR::amotry(Mat_IO_DP&, Vec_O_DP&, Vec_IO_DP&, DP funk(Vec_I_DP&),const int, const DP);

void registerPorts(void);
void unregisterPorts(void);
int SendArmPositions(double, double, double, double, double, double);
int SendHandPositions(double *, double *, double *);

int main(void);

// -----------------------------------
// streaming thread body.
// gather data from the master, process, feed commands to the slave
// -----------------------------------

class streamingThread : public YARPThread {
public:
virtual void Body (void)
{

	// for the inverse kinematics
	YVector desired_X(3), required_Q(3), starting_Q(3), current_X(3);
	starting_Q[0] = 0; starting_Q[1] = 0; starting_Q[2] = 0;

	double frX, frY, frZ, frAz, frEl, frRo;
	double dx, dy, dz;
	double prev_dx = 0.0, prev_dy = 0.0, prev_dz = 0.0, prev_roll = 0.0;
	double rot[4][4];

	rot[1][1] =  cos(refEl)*cos(refAz); rot[1][2] = -sin(refAz)*cos(refEl); rot[1][3] =  -sin(refEl);
	rot[2][1] =  sin(refAz);            rot[2][2] =  cos(refAz);            rot[2][3] =  0;
	rot[3][1] =  sin(refEl)*cos(refAz); rot[3][2] = -sin(refEl)*sin(refAz); rot[3][3] =  cos(refEl);

	// loop until the thread is terminated
	while ( !IsTerminated() ) {

		YARPTime::DelayInSeconds(streamingFrequency);

	    // get data
	    _master_data_inport.Read();
	    _data = _master_data_inport.Content();
      
	    // evaluate data wrt reference frame
	    dx = _data.tracker0Data.x - refX;
	    dy = _data.tracker0Data.y - refY;
	    dz = _data.tracker0Data.z - refZ;
		frX  = rot[1][1]*dx +rot[1][2]*dy +rot[1][3]*dz;
	    frY  = rot[2][1]*dx +rot[2][2]*dy +rot[2][3]*dz;
	    frZ  = rot[3][1]*dx +rot[3][2]*dy +rot[3][3]*dz;
	    frAz = myDegToRad * _data.tracker0Data.azimuth - refAz;
	    frEl = myDegToRad * _data.tracker0Data.elevation - refEl;
	    frRo = myDegToRad * _data.tracker0Data.roll - refRo;

		// if we didn't move so much, skip it
		double step = sqrt( (dx-prev_dx)*(dx-prev_dx) +
			                (dy-prev_dy)*(dy-prev_dy) +
							(dz-prev_dz)*(dz-prev_dz) );
		double step_roll = sqrt( (frRo-prev_roll)*(frRo-prev_roll) );
//		if ( step < tolerance && step_roll < tolerance_roll ) {
		if ( 0 ) {
			continue;
		} else {
			prev_dx = dx; prev_dy = dy; prev_dz = dz; prev_roll = frRo;
		}

	    // set the desired_X (in cms)
	    desired_X[0] = frX*myInchToCM; desired_X[1] = frY*myInchToCM; desired_X[2] = frZ*myInchToCM;
	    // evaluate inverse kinematics according to tracker position
	    inverse_kinematics(desired_X, required_Q, starting_Q);
	    // let next starting point be the one we've just found
	    starting_Q = required_Q; // all Qs are in DEGREES
    
		// compute glove wrist position
		double dWristYaw = dWrist0+_data.gloveData.wrist[0]*dWristF;

		// output to screen
	    cout.precision(2);
		cout.setf(ios::fixed);
	    cout 
			<< "roll " << frRo/myDegToRad << "\tyaw " << dWristYaw/myDegToRad << "      \r";
//			<< "X:\t" << desired_X[0]  << "\t" << desired_X[1] << "\t"  << desired_X[2] << "\t"
//		    << "Q:\t" << required_Q[0] << "\t" << required_Q[1] << "\t" << required_Q[2] << "     \r";
//	 		  << offsetQ0-required_Q[0] << "\t" << offsetQ1-required_Q[1] << "\t" << offsetQ2-required_Q[2] << "    \r";
	    cout.flush();

		// send commands to the arm, after correction (offsets and signs)
		SendArmPositions(
			(offsetQ0-required_Q[0])*myDegToRad,
			(offsetQ1-required_Q[1])*myDegToRad,
			(offsetQ2-required_Q[2])*myDegToRad,
			offsetQ3*myDegToRad-frRo,
			offsetQ4*myDegToRad-dWristYaw,
			offsetQ5*myDegToRad );

		// send commands to the hand
		dThumb[0]  = (double)abs(_data.gloveData.thumb[0]-iThumbClose[0]) * (double)dThumbFactor[0];
		dIndex[0]  = (double)abs(_data.gloveData.index[0]-iIndexClose[0]) * (double)dIndexFactor[0];
		dMiddle[0] = (double)abs(_data.gloveData.middle[0]-iMiddleClose[0]) * (double)dMiddleFactor[0];
		// output to screen
//		cout.precision(3);
//		cout << "Hand:\t" << dIndex[0] << "\t" << dMiddle[0] << "\t" << dThumb[0] << "       \r";
//		cout.flush();
//      SendHandPositions(dThumb, dIndex, dMiddle);

	}
    
	return;

	}

};

// -----------------------------------
// functions & procedures
// -----------------------------------

// functions which realise the downhill simplex method (taken from Numerical Recipes)
// they are added to the namespace NR

namespace NR {
  
  inline void get_psum(Mat_I_DP& p, Vec_O_DP& psum) {
    
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
  
} // end of namespace NR

// forward kinematics: for given joint coordinates Q (deg), give end-effector position X
//
// WARNING. this function is HIGHLY dependent on
//   (1) the Babybot arm, that is, a PUMA200,
//   (2) the initial position of the arm, which MUST be set each time during the
//       startup phase (check the main() procedure and the offsetQx globals)

void forward_kinematics(YVector& Q, YVector& X)
{

	// specifications of the joints - taken from the PUMA200 manual
	// lenghts of the joints - Figure 2-1 of the manual
	const float L1 = 13.0*myInchToCM, L2 = 7.8*myInchToCM, L2prime = -0.75*myInchToCM, L3 = 8.0*myInchToCM,
				H1 = 0.0, H2 = 5.9*myInchToCM-2.5;

	// configuration matrices and vectors
	YMatrix e_csi_1(4,4), e_csi_2(4,4), e_csi_3(4,4), g_st_0(4,4), T(4,4);
	YVector zero(4), translation(4), tmpX(4);

	// turn degrees into radiants
	Q[0] *= myDegToRad;
	Q[1] *= myDegToRad;
	Q[2] *= myDegToRad;

	// evaluate configurations:
	// first joint (rotate Q[0] radiants about Y axis)
	e_csi_1[0][0] =  cos(Q[0]); e_csi_1[0][1] = 0;          e_csi_1[0][2] =  sin(Q[0]); e_csi_1[0][3] =  0;
	e_csi_1[1][0] =          0; e_csi_1[1][1] = 1;          e_csi_1[1][2] =          0; e_csi_1[1][3] =  0;
	e_csi_1[2][0] = -sin(Q[0]); e_csi_1[2][1] = 0;          e_csi_1[2][2] =  cos(Q[0]); e_csi_1[2][3] =  0;
	e_csi_1[3][0] =          0; e_csi_1[3][1] = 0;          e_csi_1[3][2] =          0; e_csi_1[3][3] =  1;
	// second joint (rot Q[1] about Z, displaced L1 on Y, H1 on Z)
	e_csi_2[0][0] =  cos(Q[1]); e_csi_2[0][1] = -sin(Q[1]); e_csi_2[0][2] =          0; e_csi_2[0][3] =  0;
	e_csi_2[1][0] =  sin(Q[1]); e_csi_2[1][1] =  cos(Q[1]); e_csi_2[1][2] =          0; e_csi_2[1][3] =  L1;
	e_csi_2[2][0] =          0; e_csi_2[2][1] =          0; e_csi_2[2][2] =          1; e_csi_2[2][3] =  H1;
	e_csi_2[3][0] =          0; e_csi_2[3][1] =          0; e_csi_2[3][2] =          0; e_csi_2[3][3] =  1;
	// third joint (rot Q[2] about Z, displaced L2 on X, L2prime on Y, H2 on Z)
	e_csi_3[0][0] =  cos(Q[2]); e_csi_3[0][1] = -sin(Q[2]); e_csi_3[0][2] =          0; e_csi_3[0][3] =  L2;
	e_csi_3[1][0] =  sin(Q[2]); e_csi_3[1][1] =  cos(Q[2]); e_csi_3[1][2] =          0; e_csi_3[1][3] =  L2prime;
	e_csi_3[2][0] =          0; e_csi_3[2][1] =          0; e_csi_3[2][2] =          1; e_csi_3[2][3] =  H2;
	e_csi_3[3][0] =          0; e_csi_3[3][1] =          0; e_csi_3[3][2] =          0; e_csi_3[3][3] =  1;
	// end effector (displaced L3 on X)
	g_st_0[0][0]  =          1; g_st_0[0][1]  =          0; g_st_0[0][2]  =          0; g_st_0[0][3]  =  L3;
	g_st_0[1][0]  =          0; g_st_0[1][1]  =          1; g_st_0[1][2]  =          0; g_st_0[1][3]  =  0;
	g_st_0[2][0]  =          0; g_st_0[2][1]  =          0; g_st_0[2][2]  =          1; g_st_0[2][3]  =  0;
	g_st_0[3][0]  =          0; g_st_0[3][1]  =          0; g_st_0[3][2]  =          0; g_st_0[3][3]  =  1;
	// final configuration: chain-multiply all !!
	T = e_csi_1 * (e_csi_2 * (e_csi_3 * g_st_0));

	// give me the end-effector's position translated back to its frame.
	zero[0] = 0; zero[1] = 0; zero[2] = 0; zero[3] = 1;
	translation[0] = L2+L3; translation[1] = L1+L2prime; translation[2] = H1+H2; translation[3] = 1;
	tmpX = (T * zero) - translation;

	// project on first three cordinates (we are not interested in the last "1")
	X[0] = tmpX[0]; X[1] = tmpX[1]; X[2] = tmpX[2];

	// revert the Q to degrees
	Q[0] /= myDegToRad;
	Q[1] /= myDegToRad;
	Q[2] /= myDegToRad;

}

// error of the forward kinematics w.r.t. a given desired position
DP evaluate_error(Vec_I_DP& Q_NR)
{

	// translate from NR's vector representation to YARP's
	YVector X(3), Q(3);
	Q[0] = Q_NR[0]; Q[1] = Q_NR[1]; Q[2] = Q_NR[2];

	// evaluate forward kinematics
	forward_kinematics(Q,X);

	// return norm of distance to desired position; add a punishment term
	// which avoids bending the elbow in a totally innatural way
	return (X - desired_cfg).norm2() + (0.0001*(Q[2]+90)*(Q[2]+90));

}

// here we are. you get me the desired Cartesian position 
// of the e.e., X, and a starting point, init, and I am going
// to get you the appropriate Qs (IN DEGREES)
void inverse_kinematics(YVector& X, YVector& Q, YVector& init)
{

	// set desired cfg (a global so far... must work around this!)
	desired_cfg[0] = X[0]; desired_cfg[1] = X[1]; desired_cfg[2] = X[2];

	// four points for the simplex, three are the arguments,
	// tolerance is 1e-6, starting simplex is much bigger
	const int MP = 4,NP = 3;
	const DP FTOL = 1.0e-6;
	const DP simplex_dim = 1;

	// declare x, y, p
	Vec_DP x(NP),y(MP);
	Mat_DP p(MP,NP);
  
	// initialise y and p. p's rows contains the initial simplex coordinates;
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
	NR::amoeba(p,y,FTOL,evaluate_error,nfunc);

	// ok, show me what you got!
	Q[0] = p[0][0]; Q[1] = p[0][1]; Q[2] = p[0][2];

}

// register ports
void registerPorts()
{

  char portName[255];
  int ret = 0;

  // master input data port registration (glove, tracker, pressens)
  cout << "Registering master input data port..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:str", DEFAULT_TOMASTER_NAME);
  ret = _master_data_inport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL: could not register port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // master input image port registration
  cout << "Registering master input images ports..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:img0", DEFAULT_TOMASTER_NAME);
  ret = _master_img0_inport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL: could not register port " << portName << endl;
    exit(YARP_FAIL);
  }
  ACE_OS::sprintf(portName,"/%s/i:img1", DEFAULT_TOMASTER_NAME);
  ret = _master_img1_inport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL: could not register port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // master input command port registration
  cout << "Registering master input cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:int", DEFAULT_TOMASTER_NAME);
  ret = _master_cmd_inport.Register(portName, "default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
  }

  // master output command port registration
  cout << "Registering master output cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/o:int", DEFAULT_TOMASTER_NAME);
  ret = _master_cmd_outport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // slave output command port registration
  cout << "Registering slave output cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/o:str", DEFAULT_TOSLAVE_NAME);
  ret = _slave_outport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
    exit(YARP_FAIL);
  }

}

void unregisterPorts()
{

  _slave_outport.Unregister();
  _master_cmd_outport.Unregister();
  _master_cmd_inport.Unregister();
  _master_img0_inport.Unregister();
  _master_img1_inport.Unregister();
  _master_data_inport.Unregister();

}

int SendArmPositions(double dof1, double dof2, double dof3, double dof4, double dof5, double dof6)
{

//return 0;

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
  _slave_outport.Content() = tmpBottle;
  _slave_outport.Write();
  
  return 0;
  
}

int SendHandPositions(double * dThumb, double * dIndex, double * dMiddle)
{

    YVector handCmd(6);
    YARPBabyBottle tmpBottle;
    tmpBottle.setID(YBVMotorLabel); 

    // send command to the hand
    handCmd(1) = 0.0;         // Thumb inner
    handCmd(2) = 0.0;         // Thumb middle
    handCmd(3) = -dIndex[0];  // Index inner
    handCmd(4) = -dIndex[1];  // Index middle
    handCmd(5) = -dMiddle[0]; // middle-ring-pikkie inner
    handCmd(6) = -dMiddle[1]; // middle-ring-pikkie middle

    tmpBottle.reset();
    tmpBottle.writeVocab(YBVocab(YBVHandNewCmd));
    tmpBottle.writeYVector(handCmd);

    _slave_outport.Content() = tmpBottle;
    _slave_outport.Write();

    return 0;

}

// -----------------------------------
// main loop
// -----------------------------------

int main()
{

  // the streaming thread
  streamingThread myStreamingThread;

  // register communication ports
  registerPorts();

  // launch batch file which yarp-connects ports
  cout << endl << "Now connecting ports. Hit any key upon completion." << endl;
  system(CONNECT_SCRIPT);
  cin.get();

  YARPScheduler::setHighResScheduling();

  // ----------------------------
  // connect to the master
  cout << endl << "Initialising master... ";
  cout.flush();
  _master_cmd_outport.Content() = CCmdConnect;
  _master_cmd_outport.Write();
  _master_cmd_inport.Read();
  if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
    // success!
    cout << "done." << endl;
    // gather which sensors we use
    _master_cmd_inport.Read();
    int reply = _master_cmd_inport.Content();
	// so far, only tracker 0 and glove are used
	if ( reply != (HardwareUseDataGlove|HardwareUseTracker0) ) {
		cout << "must use tracker 0 and dataglove only." << endl;
		unregisterPorts();
		return 0;
	}
    _options.useDataGlove = reply & HardwareUseDataGlove;
    _options.useTracker0  = reply & HardwareUseTracker0;
    _options.useTracker1  = reply & HardwareUseTracker1;
    _options.usePresSens  = reply & HardwareUsePresSens;
    _options.useCamera0   = reply & HardwareUseCamera0;
    _options.useCamera1   = reply & HardwareUseCamera1;
	// and if we use the cameras,
    if ( _options.useCamera0 || _options.useCamera1 ) {
      // gather image size and set up image size
      _master_cmd_inport.Read();
      _options.sizeX = _master_cmd_inport.Content();
      _img0.Resize (_options.sizeX, _options.sizeY);
      _master_cmd_inport.Read();
      _options.sizeY = _master_cmd_inport.Content();
      _img1.Resize (_options.sizeX, _options.sizeY);
      cout << "image size is " << _options.sizeX << "x" << _options.sizeY << endl;
    }
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // ----------------------------------
  // calibration....
  // ----------------------------------

  // ----------------------------
  // gather reference coordinates
  cout << endl << "Now gathering reference frame. Place hand at origin and press any key... ";
  cout.flush();
  cin.get();
  _master_cmd_outport.Content() = CCmdGetData;
  _master_cmd_outport.Write();
  _master_cmd_inport.Read();
  if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
    _master_data_inport.Read();
    CollectorNumericalData tmpData = _master_data_inport.Content();
    refX  = tmpData.tracker0Data.x;
    refY  = tmpData.tracker0Data.y;
    refZ  = tmpData.tracker0Data.z;
    refAz = myDegToRad * tmpData.tracker0Data.azimuth;
    refEl = myDegToRad * tmpData.tracker0Data.elevation;
    refRo = myDegToRad * tmpData.tracker0Data.roll;
    cout << "done." << endl;
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }
  cout << "done. Reference frame is: "
       << refX << ", " << refY << ", " << refZ << ", "
       << refAz/myDegToRad << ", " << refEl/myDegToRad << ", " << refRo/myDegToRad << endl;
  cout.flush();

	// ---------------------------------------------------- 
	// acquire wrist
	int iWristYawDown, iWristYawUp;
	// down
	cout << endl << "Move your hand DOWN and press any key.";
	cout.flush();
	cin.get(); 
	_master_cmd_outport.Content() = CCmdGetData;
	_master_cmd_outport.Write();
	_master_cmd_inport.Read();
	if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
        // get data for open hand
        iWristYawDown= tmpData.gloveData.wrist[0];
    } else {
        cout << "failed." << endl;
        unregisterPorts();
        return 0;
    }
    // up
    cout << endl << "Move your hand UP and press any key.";
    cout.flush(); 
    cin.get(); 
    _master_cmd_outport.Content() = CCmdGetData;
    _master_cmd_outport.Write();
    _master_cmd_inport.Read();
    if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
        // get data for open hand
        iWristYawUp= tmpData.gloveData.wrist[0];
    } else {
        cout << "failed." << endl;
        unregisterPorts();
        return 0;
    }

    cout << "wrist interval: " << iWristYawDown << " to " << iWristYawUp  << endl;

    // evaluate wrist0 and wristF
	dWristF = fabs( (robotWristUp-robotWristDown)/(double)(iWristYawUp-iWristYawDown) );
	dWrist0 = min(robotWristDown,robotWristUp) - (double)min(robotWristDown,robotWristUp)*dWristF;

goto skip_gripper_calibration;

  //---------------------------------------------------- 
  // Data glove calibration
  // gather dataglove calibration: open hand, snapshot, close hand, snapshot
  if (_options.useDataGlove)
  {
      cout << endl << "NOW CALIBRATING DATAGLOVE... ";
      cout.flush();

      //---------------------------------------------------- 
      // acquire open hand data
      cout << endl << "Open hand and press any key.";
      cout.flush(); 
      cin.get(); 

      _master_cmd_outport.Content() = CCmdGetData;
      _master_cmd_outport.Write();
      _master_cmd_inport.Read();

      if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
          _master_data_inport.Read();
          CollectorNumericalData tmpData = _master_data_inport.Content();

          // get data for open hand
          iThumbOpen[0]  = tmpData.gloveData.thumb[0];
          iThumbOpen[1]  = tmpData.gloveData.thumb[1];
          iIndexOpen[0]  = tmpData.gloveData.index[0];
          iIndexOpen[1]  = tmpData.gloveData.index[1];
          iMiddleOpen[0] = tmpData.gloveData.middle[0];
          iMiddleOpen[1] = tmpData.gloveData.middle[1];

          } else {
          cout << "failed." << endl;
          unregisterPorts();
          return 0;
      }

      //---------------------------------------------------- 
      // acquire closed hand data
      cout << endl << "Close hand and press any key... ";
      cout.flush();
      cin.get();

      _master_cmd_outport.Content() = CCmdGetData;
      _master_cmd_outport.Write();
      _master_cmd_inport.Read();

      if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
          _master_data_inport.Read();
          CollectorNumericalData tmpData = _master_data_inport.Content();

          // get data for closed hand
          iThumbClose[0]  = tmpData.gloveData.thumb[0];
          iThumbClose[1]  = tmpData.gloveData.thumb[1];
          iIndexClose[0]  = tmpData.gloveData.index[0];
          iIndexClose[1]  = tmpData.gloveData.index[1];
          iMiddleClose[0] = tmpData.gloveData.middle[0];
          iMiddleClose[1] = tmpData.gloveData.middle[1];

      } else {
          cout << "failed." << endl;
          unregisterPorts();
          return 0;
      }

      cout << endl << "NOW CALIBRATING WRIST SENSOR......."; 
      cout.flush();

      // evaluate gripper factors

      cout << iThumbClose  << " " << iThumbOpen  << " "
          << iIndexClose  << " " << iIndexOpen  << " "
          << iMiddleClose << " " << iMiddleOpen << endl;

      // evaluate hand calibration factor. empirically, the fingers range 0 - 0.87
      // this is the angular range in radians for the babybot hand
      dThumbFactor[0]  = 0.87 / (double)abs(iThumbClose[0]-iThumbOpen[0]);
      dThumbFactor[1]  = 0.87 / (double)abs(iThumbClose[1]-iThumbOpen[1]);
      dIndexFactor[0]  = 0.87 / (double)abs(iIndexClose[0]-iIndexOpen[0]);
      dIndexFactor[1]  = 0.87 / (double)abs(iIndexClose[1]-iIndexOpen[1]);
      dMiddleFactor[0] = 0.87 / (double)abs(iMiddleClose[0]-iMiddleOpen[0]);
      dMiddleFactor[1] = 0.87 / (double)abs(iMiddleClose[1]-iMiddleOpen[1]);

      cout << "Babybot hand calibration done." << endl;
      cout.flush();
  }

skip_gripper_calibration:

  // ----------------------------------
  // activate the streaming!!
  // ----------------------------------

  // ----------------------------
  // activate stream from master: issue streaming command
  cout << "Starting streaming mode (hit a key to stop)... ";
  cout.flush();
  _master_cmd_outport.Content() = CCmdStartStreaming;
  _master_cmd_outport.Write();
  _master_cmd_inport.Read();
  if ( _master_cmd_inport.Content() == CCmdSucceeded) {
    // start streaming thread
    cout << "done." << endl;
    myStreamingThread.Begin();
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // wait for a key to stop streaming mode  
  cin.get();

  // stop streaming
  cout << endl << "Stopping streaming mode...";
  cout.flush();
  _master_cmd_outport.Content() = CCmdStopStreaming;
  _master_cmd_outport.Write();
  myStreamingThread.End();
  _master_cmd_inport.Read();
  if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
    cout << "done." << endl;
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // ----------------------------------
  // shut down....
  // ----------------------------------

  // ----------------------------
  // let the arm go to rest
  YARPBabyBottle tmpBottle;
  tmpBottle.setID(YBVMotorLabel);
  tmpBottle.reset();
  tmpBottle.writeVocab(YBVocab(YBVArmForceResting));
  _slave_outport.Content() = tmpBottle;
  _slave_outport.Write();

  // ----------------------------
  // release MASTER and bail out
  cout << "Now releasing MASTER... ";
  cout.flush();
  _master_cmd_outport.Content() = CCmdDisconnect;
  _master_cmd_outport.Write();
  _master_cmd_inport.Read();
  if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
    cout << "done." << endl;
  } else {
    cout << "failed." << endl;
  }

  // launch batch file which yarp-disconnects ports
  cout << endl << "Now disconnecting ports." << endl;
  system(DISCONNECT_SCRIPT);

  // unregister ports and bail out
  cout << endl << "Unregistering ports and bailing out." << endl;
  unregisterPorts();
  return 0;
  
}
