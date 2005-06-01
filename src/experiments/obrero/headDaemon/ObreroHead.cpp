// Class to control Obrero Head. Recycled from Paul and Eduardo's prev. code.
// by nat

/* This program tests the output of the EPP protocol.
   Changing the mode the writing can be done to the data or the address.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "SPICommBoard.h"
#include "HeadChannel.h"
#define MUTE_MOTORS
#include "ObreroHead.h"

const int ED_MIDX = 0x70;
const int ED_MIDY = 0x80;
const int X_HI = 0xC2;
const int X_LO = 0x27;
const int Y_HI = 0xBA;
const int Y_LO = 0x5A;

//#define __NAT_DEBUG__

const int HEAD_ADDR = 0x00;
const int ARM_ADDR = 0x10;

class EdHeadHelper {
public:
  SPICommBoard	ObreroSPICommBoard;
  HeadChannel	ObreroHead;

  EdHeadHelper() :
    ObreroHead(ARM_ADDR,&ObreroSPICommBoard)
  {
    ObreroSPICommBoard.openport("/dev/parport0");
  }

  ~EdHeadHelper() {
    ObreroSPICommBoard.closeport();
  }

  void transformToMotor(int& x, int& y) {
    #ifndef __NAT_DEBUG__
    int midx = ED_MIDX;
    int midy = ED_MIDY;
    int xhi = X_HI;
    int xlo = X_LO;
    int yhi = Y_HI;
    int ylo = Y_LO;
    int nx = midx - x;
    int ny = midy + y;
    if (nx>xhi) nx = xhi;
    if (nx<xlo) nx = xlo;
    if (ny>yhi) ny = yhi;
    if (ny<ylo) ny = ylo;
    x = nx;
    y = ny;
    #endif
  }

  void transformFromMotor(int& x, int& y) {
    #ifndef __NAT_DEBUG__
    int midx = ED_MIDX;
    int midy = ED_MIDY;
    x = -x+midx;
    y = y-midy;
    #endif
  }

  void set(int x, int y) {
    int nx = x;
    int ny = y;
    printf("Go to ext %d %d\n", nx, ny);
    transformToMotor(nx,ny);
    printf("Go to %x %x\n", nx, ny);
    ObreroHead.send_setpoint_pot(0x01,nx);
    ObreroHead.send_setpoint_pot(0x00,ny);
  }

  void setSingleJoint(int j, int v)
  {
    int *nx;
    int *ny;
    int dummy = 0;
    int address;
    if (j==0)
      {
	address = 0x01;
	nx=&v;
	ny=&dummy;
      }
    else
      {
	address = 0x00;
	nx=&dummy;
	ny=&v;
      }
     
    printf("%d go to ext %d\n", j, v);
    transformToMotor(*nx, *ny);
    printf("%d go to %d\n", j, v);
    ObreroHead.send_setpoint_pot(address, v);
  }

  void getSingleJoint(int j, int *v)
  {
    int x,y;
    ObreroHead.update_buffer();
    x = ObreroHead.BufferRX[2];
    y = ObreroHead.BufferRX[0];
    transformFromMotor(x,y);
    
    if (j==0)
      *v=x;
    else
      *v=y;
  }
    

  void get(int& x, int& y) {
    ObreroHead.update_buffer();
   
    // printf("%d %d %d %d   ",
    //   ObreroHead.BufferRX[0],
    //   ObreroHead.BufferRX[1],
    //   ObreroHead.BufferRX[2],
    //   ObreroHead.BufferRX[3]);
   
    x = ObreroHead.BufferRX[2];
    y = ObreroHead.BufferRX[0];
    transformFromMotor(x,y);
  }

};

ObreroHead::ObreroHead() {
  system = NULL;
  _nj = 2;
}

ObreroHead::~ObreroHead() {
  if (system!=NULL) {
    delete (EdHeadHelper*)system;
  }
}

void ObreroHead::check() {
  if (system==NULL) {
    system = new EdHeadHelper();
  }
  assert(system!=NULL);
}

void ObreroHead::init() {
  check();
}

void ObreroHead::set(int x, int y) {
  //printf("EDHEAD set %d %d\n", x, y);
  check();
  ((EdHeadHelper*)system)->set(x,y);
}


void ObreroHead::get(int& x, int& y) {
  check();
  ((EdHeadHelper*)system)->get(x,y);
  //  printf("EDHEAD get %d %d\n", x, y);
}

void ObreroHead::setSingleJoint(int j, int v) 
{
  check();
  ((EdHeadHelper*)system)->setSingleJoint(j,v);
}

void ObreroHead::getSingleJoint(int j, int *v)
{
  check();
  ((EdHeadHelper*)system)->getSingleJoint(j,v);
}

int ObreroHead::setPositionRelative(int j, double delta)
{
  check();
  
  int at;
  getSingleJoint(j,&at);
  setSingleJoint(j,(at+(int) delta));

  return YARP_OK;
}

int ObreroHead::setPositionsRelative(const double *delta)
{
  int atX, atY;
  get(atX, atY);

  set(atX+(int) delta[0], atY+(int) delta[1]);

  return YARP_OK;
}
