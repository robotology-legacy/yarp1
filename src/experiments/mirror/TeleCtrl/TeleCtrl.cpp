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
/// $Id: TeleCtrl.cpp,v 1.15 2006-03-16 08:29:30 claudio72 Exp $
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
double refX = 0.0, refY = 0.0, refZ = 0.0, refAz = 0.0, refEl = 0.0, refRo = 0.0;

// degrees to radiants
const double myDegToRad = 2*M_PI/360.0;
// inches to cms
const double myInchToCM = 2.45;

// movement tolerance (inches)
const double moveTol = 0.5/myInchToCM;
// wrist roll tolerance (radians)
const double wrRollTol = 2.0*myDegToRad;
// wrist yaw tolerance (glove units)
const double wrYawTol = 2.0;
// streaming frequency
const double streamingFrequency = 1.0/10.0;

// inverse kinematics desired configuration - as a global, so far
YVector desired_cfg(3);

// values to send to the robot hand (radians)
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
const double robotWristDown = -90.0*myDegToRad;
const double robotWristUp   =  90.0*myDegToRad;
double dWrist0 = 0.0;
double dWristF = 0.0;

//Data glove transformation factors
double dThumbFactor[2]  = {0.0, 0.0};
double dThumb0[2]       = {0.0, 0.0};
double dIndexFactor[2]  = {0.0, 0.0};
double dIndex0[2]       = {0.0, 0.0};
double dMiddleFactor[2] = {0.0, 0.0};
double dMiddle0[2]      = {0.0, 0.0};

//Fingers angles
const double robotFingerOpen  = 0.0;
const double robotFingerClose = 90.0*myDegToRad;

// arm joints initial position (in degrees). the arm is initially
// stretched with the hand down (looks like the Fascist salutation, unluckily)
const double armOffsetQ0 =   20.0;
const double armOffsetQ1 =   25.0;
const double armOffsetQ2 =  -40.0;
const double armOffsetQ3 =    0.0;
const double armOffsetQ4 =    0.0;
const double armOffsetQ5 = -160.0;

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
	double prevDx = 0.0, prevDy = 0.0, prevDz = 0.0, prevRo = 0.0, prevWr = 0.0;
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
		double ds = sqrt( (dx-prevDx)*(dx-prevDx) + (dy-prevDy)*(dy-prevDy) + (dz-prevDz)*(dz-prevDz) );
		double dRo = fabs( frRo-prevRo );
		double dWr = fabs( (double)_data.gloveData.wrist[0]-prevWr );
		if ( ds > moveTol || dRo > wrRollTol || dWr > wrYawTol ) {
			prevDx = dx; prevDy = dy; prevDz = dz;
			prevRo = frRo; prevWr = _data.gloveData.wrist[0];
			// compute arm joint angles: set the desired_X (in cms)
		    desired_X[0] = frX*myInchToCM; desired_X[1] = -frY*myInchToCM; desired_X[2] = frZ*myInchToCM;
		    // evaluate inverse kinematics according to tracker position
		    inverse_kinematics(desired_X, required_Q, starting_Q);
		    // let next starting point be the one we've just found
		    starting_Q = required_Q; // all Qs are in DEGREES
			// compute glove wrist position
			double dWristYaw = dWrist0+_data.gloveData.wrist[0]*dWristF;
		    cout 
//				<< "X\t" << desired_X[0]  << "\t" << desired_X[1] << "\t"  << desired_X[2] << "\t"
//				<< "Q\t" << required_Q[0] << "\t" << required_Q[1] << "\t" << required_Q[2] << "\t"
//				<< "roll " << frRo/myDegToRad << "\tyaw " << armOffsetQ4+40-dWristYaw/myDegToRad << "\t"
//				<< armOffsetQ0-required_Q[0] << "\t" << armOffsetQ1-required_Q[1] << "\t" << armOffsetQ2-required_Q[2] << "\t";
				<< "       \r";
//			cout.flush();
			// send commands to the arm, after correction (armOffsets and signs)
			SendArmPositions(
				(armOffsetQ0-required_Q[0])*myDegToRad,
				(armOffsetQ1-required_Q[1])*myDegToRad,
				(armOffsetQ2-required_Q[2])*myDegToRad,
				armOffsetQ3*myDegToRad+frRo,
				(armOffsetQ4+20)*myDegToRad-dWristYaw,
				armOffsetQ5*myDegToRad );
		}
		// send commands to the hand
        dThumb[0]  = dThumb0[0]  + _data.gloveData.thumb[0]  * dThumbFactor[0];
        dThumb[1]  = dThumb0[1]  + _data.gloveData.thumb[1]  * dThumbFactor[1];
        dIndex[0]  = dIndex0[0]  + _data.gloveData.index[0]  * dIndexFactor[0];
        dIndex[1]  = dIndex0[1]  + _data.gloveData.index[1]  * dIndexFactor[1];
        dMiddle[0] = dMiddle0[0] + _data.gloveData.middle[0] * dMiddleFactor[0];
        dMiddle[1] = dMiddle0[1] + _data.gloveData.middle[1] * dMiddleFactor[1];
		// output to screen
//		cout << "Hand:\t" << dIndex[0]/myDegToRad << "\t" << dMiddle[0]/myDegToRad << "\t" << dThumb[0]/myDegToRad << "\t"  
//			 << dIndex[1]/myDegToRad << "\t" << dMiddle[1]/myDegToRad << "\t" << dThumb[1]/myDegToRad << "       \r";
//		cout.flush();
        dIndex[0]  = 0.0;
        dIndex[1]  = 0.0;
        SendHandPositions(dThumb, dIndex, dMiddle);
	}
    
	return;

} // streamingThread::Body() 

}; // class stramingThread

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
//       startup phase (check the main() procedure and the armOffsetQx globals)

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

int SendHandPositions(double* dThumb, double* dIndex, double* dMiddle)
{

//return 0;

    YVector handCmd(6);
    YARPBabyBottle tmpBottle;
    tmpBottle.setID(YBVMotorLabel); 

    // send command to the hand
    handCmd(1) = dThumb[0];   // Thumb inner
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

	cout.precision(2); cout.setf(ios::fixed);

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
	// tell collector to activate the sensors
	cout << endl << "Initialising collector... "; cout.flush();
	_master_cmd_outport.Content() = CCmdConnect;
	_master_cmd_outport.Write();
	_master_cmd_inport.Read();
	if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		cout << "done." << endl;
		// gather which sensors we use
		_master_cmd_inport.Read();
		if ( _master_cmd_inport.Content() != (HardwareUseDataGlove|HardwareUseTracker0) ) {
			cout << "must use tracker 0 and dataglove only." << endl;
			unregisterPorts();
			return 0;
		}
		// so far, use only tracker0 and glove
		_options.useDataGlove = _options.useTracker0  = 1;
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
	cout << endl << "Now gathering reference frame. Place hand at origin and press any key... "; cout.flush();
	cin.get();
	_master_cmd_outport.Content() = CCmdGetData;
	_master_cmd_outport.Write();
	_master_cmd_inport.Read();
	if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
		CollectorNumericalData tmpData = _master_data_inport.Content();
		refX  = tmpData.tracker0Data.x;
		refY  = tmpData.tracker0Data.y;
		// takes into account that the tracker is mounted about 2cm above the hand!
		refZ  = tmpData.tracker0Data.z - 2.5/myInchToCM;
		refAz = myDegToRad * tmpData.tracker0Data.azimuth;
		refEl = myDegToRad * tmpData.tracker0Data.elevation;
		refRo = myDegToRad * tmpData.tracker0Data.roll;
		cout << "done." << endl;
	} else {
		cout << "failed." << endl;
		unregisterPorts();
		return 0;
	}
	cout << "done. (" << refX << " " << refY << " " << refZ << " "
         << refAz/myDegToRad << " " << refEl/myDegToRad << " " << refRo/myDegToRad << endl;
	cout.flush();

	// ---------------------------------------------------- 
	// calibrate wrist
	int iWristDown, iWristUp;
	// down
	cout << endl << "Move your hand DOWN and press any key."; cout.flush();
	cin.get(); 
	_master_cmd_outport.Content() = CCmdGetData;
	_master_cmd_outport.Write();
	_master_cmd_inport.Read();
	if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
        iWristDown= tmpData.gloveData.wrist[0];
    } else {
        cout << "failed." << endl;
        unregisterPorts();
        return 0;
    }
    // up
    cout << endl << "Move your hand UP and press any key."; cout.flush(); 
    cin.get(); 
    _master_cmd_outport.Content() = CCmdGetData;
    _master_cmd_outport.Write();
    _master_cmd_inport.Read();
    if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
        iWristUp= tmpData.gloveData.wrist[0];
    } else {
        cout << "failed." << endl;
        unregisterPorts();
        return 0;
    }
    cout << "wrist interval: " << iWristDown << " to " << iWristUp  << endl;
    // evaluate wrist0 and wristF
	dWristF = fabs( (robotWristUp-robotWristDown)/(double)(iWristUp-iWristDown) );
	dWrist0 = min(robotWristDown,robotWristUp) - (double)min(robotWristDown,robotWristUp)*dWristF;

	//---------------------------------------------------- 
	// calibrate fingers
	cout << endl << "Now calibrating dataglove. ";
	cout.flush();

	//---------------------------------------------------- 
	// acquire open hand data
	cout << endl << "Open hand and press any key."; cout.flush(); 
    cin.get(); 
    _master_cmd_outport.Content() = CCmdGetData;
    _master_cmd_outport.Write();
    _master_cmd_inport.Read();
    if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
		_master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
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
    cout << endl << "Close hand and press any key. "; cout.flush();
    cin.get();
    _master_cmd_outport.Content() = CCmdGetData;
    _master_cmd_outport.Write();
    _master_cmd_inport.Read();
    if ( _master_cmd_inport.Content() == CCmdSucceeded ) {
        _master_data_inport.Read();
        CollectorNumericalData tmpData = _master_data_inport.Content();
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

    // this is the angular range in radians for the babybot hand
    double robotFingerRange = fabs(robotFingerClose-robotFingerOpen);
    dThumbFactor[0]  = robotFingerRange / (double)abs(iThumbClose[0]-iThumbOpen[0]);
    dThumbFactor[1]  = robotFingerRange / (double)abs(iThumbClose[1]-iThumbOpen[1]);
    dIndexFactor[0]  = robotFingerRange / (double)abs(iIndexClose[0]-iIndexOpen[0]);
    dIndexFactor[1]  = robotFingerRange / (double)abs(iIndexClose[1]-iIndexOpen[1]);
    dMiddleFactor[0] = robotFingerRange / (double)abs(iMiddleClose[0]-iMiddleOpen[0]);
    dMiddleFactor[1] = robotFingerRange / (double)abs(iMiddleClose[1]-iMiddleOpen[1]);
    // compute de 0 componets (robot_d - finger_close * fingerFactor)
    double minRobotFingerAngle = min(robotFingerClose,robotFingerOpen);
    dThumb0[0]  = minRobotFingerAngle - (double)min(iThumbClose[0],iThumbOpen[0]) * dThumbFactor[0];
    dThumb0[1]  = minRobotFingerAngle - (double)min(iThumbClose[1],iThumbOpen[1]) * dThumbFactor[1];
    dIndex0[0]  = minRobotFingerAngle - (double)min(iIndexClose[0],iIndexOpen[0]) * dIndexFactor[0];
    dIndex0[1]  = minRobotFingerAngle - (double)min(iIndexClose[1],iIndexOpen[1]) * dIndexFactor[1];
    dMiddle0[0] = minRobotFingerAngle - (double)min(iMiddleClose[0],iMiddleOpen[0]) * dMiddleFactor[0];
    dMiddle0[1] = minRobotFingerAngle - (double)min(iMiddleClose[1],iMiddleOpen[1]) * dMiddleFactor[1];
    cout << "Babybot hand calibration done." << endl; cout.flush();

	// ----------------------------------
	// streaming
	// ----------------------------------

	// have arm and hand positioned in the starting position
	SendArmPositions(armOffsetQ0*myDegToRad,armOffsetQ1*myDegToRad,armOffsetQ2*myDegToRad,armOffsetQ3*myDegToRad,armOffsetQ4*myDegToRad,armOffsetQ5*myDegToRad );
    cout << endl << "Positioning. Press any key when finished. "; cout.flush(); cin.get();
		
	// ----------------------------
	// activate stream from master
	cout << "Starting streaming mode (hit a key to stop)... "; cout.flush();
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
	cout << endl << "Stopping streaming mode..."; cout.flush();
	myStreamingThread.End();
	_master_cmd_outport.Content() = CCmdStopStreaming;
	_master_cmd_outport.Write();
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
	// reset hand
	{ 
		double zero1[2] = { 0.0, 0.0 };
		double zero2[2] = { 0.0, 0.0 };
		double zero3[2] = { 0.0, 0.0 };
		SendHandPositions(zero1,zero2,zero3);
	}

	// ----------------------------
	// let the arm go to rest
	YARPBabyBottle tmpBottle;
	tmpBottle.setID(YBVMotorLabel);
	tmpBottle.reset();
	tmpBottle.writeVocab(YBVocab(YBVArmForceResting));
	_slave_outport.Content() = tmpBottle;
	_slave_outport.Write();

	// ----------------------------
	// release collector and bail out
	cout << "Now releasing collector. "; cout.flush();
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
	unregisterPorts();

	return 0;
  
}
