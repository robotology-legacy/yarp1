/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "motortester.h"
#include "abimport.h"
#include "proto.h"

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */


void 
ParseCommandLine(int argc, char *argv[])
{
        int i;

        for(i=1;i<argc;i++)
        {
                switch(argv[i][0])
                {
                        case '-':
                                switch(argv[i][1])
                                {
                                        case 'n':
						if (i < argc-1)
						{
						   sprintf (DEVICENAME, "%s\0", argv[i+1]);
						}
                                                break;

                                        default:
                                                break;
                                }
                                break;
                        default:
                                break;
                }
        }
}


int
init( int argc, char *argv[] )

	{

	memset (DEVICENAME, 0, 256);
	sprintf (DEVICENAME, "%s\0", DEFAULTDEVICENAME);

	ParseCommandLine (argc, argv);
	printf ("Device name is now: %s\n", DEVICENAME);


	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	return( Pt_CONTINUE );

	}

