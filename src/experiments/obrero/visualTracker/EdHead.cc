/* This program tests the output of the EPP protocol.
   Changing the mode the writing can be done to the data or the address.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


//#include "ParallelPort.h"

#include "SPICommBoard.h"

#include "HeadChannel.h"

#define MUTE_MOTORS

#include "EdHead.h"

const int ED_MIDX = 0x70;
const int ED_MIDY = 0x80;

class EdHeadHelper {
public:
  SPICommBoard	ObreroSPICommBoard;
  HeadChannel		ObreroHead;

  EdHeadHelper() :
    ObreroHead(0x00,&ObreroSPICommBoard)
  {
    ObreroSPICommBoard.openport("/dev/parport0");
  }

  ~EdHeadHelper() {
    ObreroSPICommBoard.closeport();
  }

  void transformToMotor(int& x, int& y) {
    int midx = ED_MIDX;
    int midy = ED_MIDY;
    int xhi = 0xC2;
    int xlo = 0x27;
    int yhi = 0xBA;
    int ylo = 0x5D;
    int nx = midx - x;
    int ny = midy + y;
    if (nx>xhi) nx = xhi;
    if (nx<xlo) nx = xlo;
    if (ny>yhi) ny = yhi;
    if (ny<ylo) ny = ylo;
    x = nx;
    y = ny;
  }

  void transformFromMotor(int& x, int& y) {
    int midx = ED_MIDX;
    int midy = ED_MIDY;
    x = -x+midx;
    y = y-midy;
  }

  void set(int x, int y) {
    int nx = x;
    int ny = y;
    //printf("Go to ext %d %d\n", nx, ny);
    transformToMotor(nx,ny);
    //printf("Go to %x %x\n", nx, ny);
    ObreroHead.send_setpoint_pot(0x01,nx);
    ObreroHead.send_setpoint_pot(0x00,ny);
  }

  void get(int& x, int& y) {
    ObreroHead.update_buffer();
    /*
    printf("%d %d %d %d   ",
	   ObreroHead.BufferRX[0],
	   ObreroHead.BufferRX[1],
	   ObreroHead.BufferRX[2],
	   ObreroHead.BufferRX[3]);
    */
    x = ObreroHead.BufferRX[2];
    y = ObreroHead.BufferRX[0];
    transformFromMotor(x,y);
  }

};


EdHead::EdHead() {
  system = NULL;
}

EdHead::~EdHead() {
  if (system!=NULL) {
    delete (EdHeadHelper*)system;
  }
}

void EdHead::check() {
  if (system==NULL) {
    system = new EdHeadHelper();
  }
  assert(system!=NULL);
}

void EdHead::init() {
  check();
}

void EdHead::set(int x, int y) {
  //printf("EDHEAD set %d %d\n", x, y);
  check();
  ((EdHeadHelper*)system)->set(x,y);
}


void EdHead::get(int& x, int& y) {
  check();
  ((EdHeadHelper*)system)->get(x,y);
  //printf("EDHEAD get %d %d\n", x, y);
}


/*
int main(int argc, char* argv[])
{

	SPICommBoard	ObreroSPICommBoard;
	HeadChannel		ObreroHead(0x00,&ObreroSPICommBoard);
	//ParallelPort	ObreroComm;
	int i,j,k=0,m=0;
	int delta=0x10;
	int setpoint;

	int setpoints_ud[4]={0x94,0x74,0x74,0x94};
	int setpoints_lr[4]={0x90,0x60,0x90,0x60};

	ObreroSPICommBoard.openport("/dev/parport0");


	for(j=0;j<1000;j++)
	{
		
		k++;
		if(k==10)
		{
			k=0;
			setpoint=0x80+delta;
			delta=-delta;

			ObreroHead.send_setpoint_pot(0x01,setpoints_lr[m]);
			ObreroHead.send_setpoint_pot(0x00,setpoints_ud[m]);
			m++;
			if(m==4) m=0;
			
		}
		
		ObreroHead.update_buffer();
		
		
		printf("[%4d]POT0= 0x%2X %2X POT1= 0x%2X %2X \n",j,ObreroHead.BufferRX[0],ObreroHead.BufferRX[1],ObreroHead.BufferRX[2],ObreroHead.BufferRX[3]);
		//printf("ENC0= 0x%2X %2X ENC1= 0x%2X %2X\n",buffread[4],buffread[5],buffread[6],buffread[7]);
		
		usleep(100000);
	}
	
	ObreroSPICommBoard.closeport();
	
	
	return 1;
}
*/

