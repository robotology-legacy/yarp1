/* M a i n l i n e                                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#ifdef __USAGE
%C - This is a QNX/Photon Application.
%C [options]

Options:
  -s server   Server node or device name
  -x x        Initial x position
  -y y        Initial y position
  -h h        Initial h dimension
  -w w        Initial w dimension

Examples:
%C -s4
  Run using Photon server on node 4

%C -s//4/dev/photon
  Same as above

%C -x10 -y10 -h200 -w300
  Run at initial position 10,10 with initial 
  dimension of 200x300.
#endif

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "abwidgets.h"
#include "abdefine.h"
#include "abevents.h"
#include "ablinks.h"
#include "abvars.h"

int
main ( int argc, char *argv[] )

	{

	_Ap_.Ap_winpos.x  = _Ap_.Ap_winpos.y  = AB_LOCKED;
	_Ap_.Ap_winsize.y = _Ap_.Ap_winsize.x = AB_LOCKED;

	/* AppBuilder Initialization */
	ApInitialize( argc, argv, &AbContext );

	/* this is only temporary - it will be removed in the 6.1.2 */
	PgSetDrawBufferSize( 0xffff );

	init( argc, argv );

	/* Display main window */
	ApLinkWindow( NULL, &AbApplLinks[0], NULL );

	/* Loop until user quits application */
	PtMainLoop( );
	PtExit( 0 );

	return 0;
	}

static const ApClassTab_t ClassTable[] = {
	{ "PtWindow", &PtWindow },
	{ "PtOSContainer", &PtOSContainer },
	{ "PtGroup", &PtGroup },
	{ "PtRaw", &PtRaw },
	{ "PtText", &PtText },
	{ "PtTimer", &PtTimer },
	{ "PtMenuBar", &PtMenuBar },
	{ "PtMenuButton", &PtMenuButton },
	{ "PtMenu", &PtMenu },
	{ NULL, NULL }
	};

ApContext_t AbContext = { ClassTable, 1, AbWidgets };


