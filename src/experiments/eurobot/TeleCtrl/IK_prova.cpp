// standard
#include <iostream>
#include <math.h>

#define SIZE_BUF 128
#define NO_ITERATIONS 5

#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPBabyBottle.h>

// Numerical Recipes --- this is for the downhill simplex method
// used in evaluating inverse kinematics
#include "nr.h"

// -----------------------------------
// globals
// -----------------------------------

using namespace std;

// evaluation of inverse kinematics
// desired configuration - as a global, so far
YVector desired_cfg(3);

void forward_kinematics(YVector&, YVector&);
double evaluate_error(YVector&);
void inverse_kinematics(YVector&, YVector&, YVector&);
void NR::amoeba(Mat_IO_DP&, Vec_IO_DP&, const DP, DP funk(Vec_I_DP&), int&);
DP NR::amotry(Mat_IO_DP&, Vec_O_DP&, Vec_IO_DP&, DP funk(Vec_I_DP&),const int, const DP);

int main(void);

// -----------------------------------
// streaming thread. this gathers data from the MASTER,
// processes them and the feeds the SLAVE the
// appropriate commands.
// -----------------------------------

class streamingThread : public YARPThread {
public:
  virtual void Body (void);
};

// streaming thread
void streamingThread::Body (void)
{

  // for the inverse kinematics
  YVector desired_X(3), required_Q(3), starting_Q(3);
  starting_Q[0] = 0; starting_Q[1] = 0; starting_Q[2] = 0;

  // loop until the thread is terminated ( thread.End() )
  while ( !IsTerminated() ) {

    // gather glove, tracker and pressens data
    if ( _options.useDataGlove || _options.useTracker || _options.usePresSens ) {

      // output to screen what we are actually sending
      cout.precision(5);
      // now evaluate inverse kinematics according to tracker position
      desired_X[0] = frX; desired_X[1] = frY; desired_X[2] = frZ;
      inverse_kinematics(desired_X, required_Q, starting_Q);
      starting_Q = required_Q;
      cout << "Arm:\t"
           << "X\t" << floor(desired_X[0])  << "\t" << floor(desired_X[1]) << "\t"  << floor(desired_X[2]) << "\t"
           << "Q\t" << floor(required_Q[0]) << "\t" << floor(required_Q[1]) << "\t" << floor(required_Q[2]) << "   \r";
      cout.flush();

    }

  }
    
  return;

}

// -----------------------------------
// functions & procedures
// -----------------------------------

// functions which realise the downhill simplex method (taken from Numerical Recipes)
// they are added to the namespace NR

namespace NR {
  
  inline void get_psum(Mat_I_DP& p, Vec_O_DP& psum) {
    
    int i,j;
    DP sum;
    
    int mpts=p.nrows();
    int ndim=p.ncols();
    
    for (j=0;j<ndim;j++) {
      for (sum=0.0,i=0;i<mpts;i++) {
        sum += p[i][j];
      }
      psum[j]=sum;
    }

  }
  
  void amoeba(Mat_IO_DP &p, Vec_IO_DP &y, const DP ftol, DP funk(Vec_I_DP &), int &nfunk)
  {
    
    const int NMAX=5000;
    const DP TINY=1.0e-10;
    int i,ihi,ilo,inhi,j;
    DP rtol,ysave,ytry;
    
    int mpts=p.nrows();
    int ndim=p.ncols();
    
    Vec_DP psum(ndim);
    nfunk=0;
    get_psum(p,psum);
    
    for (;;) {
      ilo=0;
      ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
      for (i=0;i<mpts;i++) {
        if (y[i] <= y[ilo]) ilo=i;
        if (y[i] > y[ihi]) {
          inhi=ihi;
          ihi=i;
        } else if (y[i] > y[inhi] && i != ihi) {
          inhi=i;
        }
      }
      
      rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo])+TINY);
      
      if (rtol < ftol) {
        SWAP(y[0],y[ilo]);
        for (i=0;i<ndim;i++) SWAP(p[0][i],p[ilo][i]);
        break;
      }
      
      if (nfunk >= NMAX) {
        //        nrerror("NMAX exceeded");
        return;
      }
      
      nfunk += 2;
      ytry=amotry(p,y,psum,funk,ihi,-1.0);
      
      if (ytry <= y[ilo]) {
        ytry=amotry(p,y,psum,funk,ihi,2.0);
      } else if (ytry >= y[inhi]) {
        ysave=y[ihi];
        ytry=amotry(p,y,psum,funk,ihi,0.5);
        if (ytry >= ysave) {
          for (i=0;i<mpts;i++) {
            if (i != ilo) {
              for (j=0;j<ndim;j++) {
                p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
              }
              y[i]=funk(psum);
            }
          }
          nfunk += ndim;
          get_psum(p,psum);
        }
      } else {
        --nfunk;
      }
    }
    
  }
  
  DP amotry(Mat_IO_DP &p, Vec_O_DP &y, Vec_IO_DP &psum, DP funk(Vec_I_DP &),const int ihi, const DP fac)
  {
    
    int j;
    DP fac1,fac2,ytry;
    
    int ndim=p.ncols();
    Vec_DP ptry(ndim);
    
    fac1=(1.0-fac)/ndim;
    fac2=fac1-fac;
    
    for (j=0;j<ndim;j++)
      ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
    
    ytry=funk(ptry);
    
    if (ytry < y[ihi]) {
      y[ihi]=ytry;
      for (j=0;j<ndim;j++) {
        psum[j] += ptry[j]-p[ihi][j];
        p[ihi][j]=ptry[j];
      }
    }
    return ytry;
    
  }
  
} // end of namespace NR

// so you give me the forward kinematics in this function. it takes as input
// the vector Q of joint coordinates and writes into X the Cartesian-space
// position of the end-effector
void forward_kinematics(YVector& Q, YVector& X)
{
  // forward kinematics for the PUMA 200 robotic arm

  // length of the joints
  const unsigned short L1 = 10, L2 = 5, L3 = 3;

  // allocate configuration matrices
  YMatrix e_csi_1(4,4), e_csi_2(4,4), e_csi_3(4,4), g_st_0(4,4), T(4,4);
  YVector zero(4);
  zero[0] = 0; zero[1] = 0; zero[2] = 0; zero[3] = 1;

  // turn degrees into radiants
  Q[0] = Q[0] * myDegToRad;
  Q[1] = Q[1] * myDegToRad;
  Q[2] = Q[2] * myDegToRad;

  // evaluate configurations:
  // first joint (rotation about X axis)
  e_csi_1[0][0] =  cos(Q[0]); e_csi_1[0][1] = 0;          e_csi_1[0][2] = -sin(Q[0]); e_csi_1[0][3] = 0;
  e_csi_1[1][0] =          0; e_csi_1[1][1] = 1;          e_csi_1[1][2] =          0; e_csi_1[1][3] = 0;
  e_csi_1[2][0] =  sin(Q[0]); e_csi_1[2][1] = 0;          e_csi_1[2][2] =  cos(Q[0]); e_csi_1[2][3] = 0;
  e_csi_1[3][0] =          0; e_csi_1[3][1] = 0;          e_csi_1[3][2] =          0; e_csi_1[3][3] = 1;
  // second joint (rotation about Z axis)
  e_csi_2[0][0] =  cos(Q[1]); e_csi_2[0][1] = -sin(Q[1]); e_csi_2[0][2] =          0; e_csi_2[0][3] = L1*sin(Q[1]);
  e_csi_2[1][0] =  sin(Q[1]); e_csi_2[1][1] =  cos(Q[1]); e_csi_2[1][2] =          0; e_csi_2[1][3] = L1*(1-cos(Q[1]));
  e_csi_2[2][0] =          0; e_csi_2[2][1] =          0; e_csi_2[2][2] =          1; e_csi_2[2][3] = 0;
  e_csi_2[3][0] =          0; e_csi_2[3][1] =          0; e_csi_2[3][2] =          0; e_csi_2[3][3] = 1;
  // third joint (again, rotation about Z axis)
  e_csi_3[0][0] =  cos(Q[2]); e_csi_3[0][1] = -sin(Q[2]); e_csi_3[0][2] =          0; e_csi_3[0][3] = (L1+L2)*sin(Q[2]);
  e_csi_3[1][0] =  sin(Q[2]); e_csi_3[1][1] =  cos(Q[2]); e_csi_3[1][2] =          0; e_csi_3[1][3] = (L1+L2)*(1-cos(Q[2]));
  e_csi_3[2][0] =          0; e_csi_3[2][1] =          0; e_csi_3[2][2] =          1; e_csi_3[2][3] = 0;
  e_csi_3[3][0] =          0; e_csi_3[3][1] =          0; e_csi_3[3][2] =          0; e_csi_3[3][3] = 1;
  // starting configuration
  g_st_0[0][0] = 1; g_st_0[0][1] = 0; g_st_0[0][2] = 0; g_st_0[0][3] = 0;
  g_st_0[1][0] = 0; g_st_0[1][1] = 1; g_st_0[1][2] = 0; g_st_0[1][3] = L1+L2+L3;
  g_st_0[2][0] = 0; g_st_0[2][1] = 0; g_st_0[2][2] = 1; g_st_0[2][3] = 0;
  g_st_0[3][0] = 0; g_st_0[3][1] = 0; g_st_0[3][2] = 0; g_st_0[3][3] = 1;
  // final configuration is obtained by chain-multiplying
  T = e_csi_1.Transposed() * (e_csi_2 * (e_csi_3 * g_st_0));

  // give me end-effetor's position (that is, (0,0,0) in the e.e.'s frame)
  YVector tmpX = T * zero;
  // project on first three cordinates (we are not interested in the last 1)
  X[0] = tmpX[0]; X[1] = tmpX[1]; X[2] = tmpX[2];

}

// given the forward_kinematics function, this evaluates the
// norm of the error w.r.t. a given desired position
DP evaluate_error(Vec_I_DP& Q_NR)
{

  // translate from NR's vector representation to YARP's
  YVector X(3), Q(3);
  Q[0] = Q_NR[0]; Q[1] = Q_NR[1]; Q[2] = Q_NR[2];

  // evaluate forward kinematics
  forward_kinematics(Q,X);

  // return norm of distance to desired position
  return (X - desired_cfg).norm2();

}

// here we are. you get me the desired Cartesian position of the e.e., X,
// and a starting point, init, and I am going to get you the appropriate Qs
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

// -----------------------------------
// main loop
// -----------------------------------

int main()
{

  myStreamingThread.Begin();
  cin.get(); cout << endl << "Stopping streaming mode..."; cout.flush();
  myStreamingThread.End();
  return 0;
  
}
