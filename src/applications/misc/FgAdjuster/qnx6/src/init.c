/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

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

int bright = 0;
int hue = 0;
int contrast = 216;
int satu = 254;
int satv = 180;
int lnotch = 0;
int ldec = 0;
int crush = 0;
YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
YARPBottle bottle;

int
init( int argc, char *argv[] )
{

	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	_outPort.Register("/framegrab/o","Net0");
	bottle.setID(YBVMotorLabel);

	return( Pt_CONTINUE );

}

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
	bottle.writeInt(crush);

	_outPort.Content() = bottle;
	_outPort.Write();

	return 1;
}

