
#ifndef _PIC_MSG_H_
#define _PIC_MSG_H_

#include "DataIO.h"

#define _D_SERVOSETGAIN			0
#define _D_MOVEPOSNOWAIT		1
#define _D_MOVEPOSWAIT			2
#define _D_MOVEPWM			3
#define _D_READENCODER			4
#define _D_READAD		        5
#define _D_READDIGITAL			6
#define _D_CALIBRATE_PWM		7
#define _D_CALIBRATE_POS		8
#define _D_READALL			9
#define _D_RESETENCODER			10
#define _D_GETNUMMOTORS                 11
#define _D_HARDRESET                    12
#define _D_ISMOVEDONE                   13

typedef unsigned char byte;

/* Message Types */

struct _type_servosetgain
{
	unsigned char addr;
	short int kp;
	short int kd;
	short int ki;
	short int il;
	unsigned char ol;
	unsigned char cl;
	short int el;
	unsigned char sr;
	unsigned char dc;
};

struct _type_servoloadtraj
{
	byte addr; 
	byte mode; 
	long pos; 
	long vel; 
	long acc; 
	byte pwm;
	int dir;
};

struct _type_readencoder
{
	byte addr; 
	long pos; 
	char set;
};

struct _type_readanalogdigital
{
	byte addr; 
	byte ADval[3]; 
	char set;
};

struct _type_readdigital
{
	byte addr; 
	char nbit; 
	char set;
	char value;
};

struct _type_calibrate
{
	byte addr; 
	int pwm;
	int mindesp;
	long int despla;
	int maxerror;
	
};

struct _type_getnummotors
{
  int nmotors;
};

struct _type_hardreset
{

};

struct _type_ismovedone
{
  byte addr;
  int done;
};

/*
 This union encompasses all the possible message types that the IO manager
 will receive. The replies are also contained in this union.
 */

struct DaemonPICMessage
{
  short unsigned id;
  union data{
    
    struct _type_servosetgain		fdServoSetGain;
    struct _type_servoloadtraj		fdServoLoadTraj;
    struct _type_readencoder		fdReadEncoder;
    struct _type_readanalogdigital	fdReadAD;
    struct _type_readdigital		fdReadDigital;
    struct _type_calibrate			fdCalibrate;
    struct _type_readall			fdReadAll;
    struct _type_getnummotors                     fdGetNumMotors;
    struct _type_ismovedone                       fdIsMoveDone;
  }msg;

};




#endif 

