// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  init.c
// 
//     Description: These are the init and send bottle functions  
// 
//         Version:  $Id: init.c,v 1.2 2004-01-21 20:21:58 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <./conf/YARPVocab.h>

/* Application Options string */
const char ApOptions[] = AB_OPTIONS ""; /* Add your options in the "" */

//----------------------------------------------------------------------
// Declare some global variables 
//----------------------------------------------------------------------
int bright = 0;
int hue = 0;
int contrast = 216;
int satu = 254;
int satv = 180;
int lnotch = 0;
int ldec = 0;
int peak = 0;
int cagc = 0;
int ckill = 0;
int range = 0;
int ysleep = 0;
int csleep = 0;
int crush = 0;
int _gamma = 0;
int dithf = 0;

//----------------------------------------------------------------------
// The output port ans the bottle 
//----------------------------------------------------------------------
YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
YARPBottle bottle;

// ===  FUNCTION  ======================================================================
// 
//         Name:  init
// 
//  Description: Initialize the photon application fgadjuster 
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
int
init( int argc, char *argv[] )
{
	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	_outPort.Register("/framegrab/o","Net0");
	bottle.setID(YBVMotorLabel);

	return( Pt_CONTINUE );
}

// ===  FUNCTION  ======================================================================
// 
//         Name: sendfginfo 
// 
//  Description: This function send the bottle data.
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
int
sendfginfo()
{
	bottle.reset();
	bottle.writeInt(bright);
	bottle.writeInt(hue);
	bottle.writeInt(contrast);
	bottle.writeInt(satu);
	bottle.writeInt(satv);
	bottle.writeInt(lnotch);
	bottle.writeInt(ldec);
	bottle.writeInt(peak);
	bottle.writeInt(cagc);
	bottle.writeInt(ckill);
	bottle.writeInt(range);
	bottle.writeInt(ysleep);
	bottle.writeInt(csleep);
	bottle.writeInt(crush);
	bottle.writeInt(_gamma);
	bottle.writeInt(dithf);

	_outPort.Content() = bottle;
	_outPort.Write();

	return 1;
}

