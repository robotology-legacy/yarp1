/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"
#include<signal.h>

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */

// Command-line flags (flurry_init.cc)
int Verbosity=0;
int PrintTimingCycles=0;
char *my_name="/cogcam";
sigset_t set[2];

// Ports (flurry_init.cc)
//InputPortOf<GenericImage> InPortImage;
YARPBasicInputPort<YARPImagePortContent> InPortImage;
//InputPortOf<TrajectorySetSendable> InPortTraj;

void ParseCommandLine(int argc, char *argv[])
{
  int i;

  for(i=1;i<argc;i++)
    {
      switch(argv[i][0])
	{
	case '-':
	  switch(argv[i][1])
	    {
	    case 't':
	      // -timing xxx for timing info every xxx frames
	      assert(i+1 < argc);
	      PrintTimingCycles = atoi(argv[i+1]);
	      i++;
	      break;
	    case 'v':
	      // -verbose xxx 
	      assert(i+1 < argc);
	      Verbosity = atoi(argv[i+1]);
	      i++;
	      break;
	    case 'h':
	    case '-':
	      cout << endl;
	      cout << "Command line arguments:" << endl;
	      cout << "  -timing   xx : print timing info every xx frames" << endl;
	      cout << "  -verbose  xx : Verbosity level xx" << endl;
	      cout << "  +/xxx        : set process name to /xxx " << endl;
	      cout << endl;
	      exit(0);
	      break;
	    default:
	      break;
	    }
	  break;
	case '+':
	  my_name = &argv[i][1];
	  break;
	default:
	  break;
	}
    }

  if (Verbosity)
    {
      cout << "Cogcam name is <" << my_name << ">" << endl;
      cout << "Verbosity Level " << Verbosity << endl;
      cout << "Print timing every " << PrintTimingCycles <<  " cycles" << endl;
    }
}

int ExitSignalHandler(int signum, void *data)
{
  PgShmemCleanup();
  PtAppRemoveSignalProc(NULL, set);
  exit(0);
  return(Pt_CONTINUE);
}

int
flurry_init( int argc, char *argv[] )
{
  int errcd;
  char nn[100];
  
  // setup the exit signal handler
  sigemptyset(set);
  sigaddset(set, SIGTERM);
  
  errcd = PtAppAddSignalProc(NULL, set, 
			     (PtSignalProcF_t) ExitSignalHandler, NULL);
  if (errcd != 0)
    {
      printf("Error in attaching signal handler.\n");
      exit(1);
    }
  
  sprintf(global_filename, "/tmp/zzz.bmp");
  ParseCommandLine(argc, argv);
  
  InPortImage.Register(my_name);
  //sprintf(nn, "%s/traj", my_name);
  //InPortTraj.SetName(nn);
  
  /* eliminate 'unreferenced' warnings */
  argc = argc, argv = argv;

  PhTile_t *tile;

  return( Pt_CONTINUE );
  
}

