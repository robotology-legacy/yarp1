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
#include "abimport.h"
#include "proto.h"

#include "headcontrol.h"
#include "YARPTime.h"

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */

// globals.
CControlThread	*thread = NULL;
YARPHeadControl *head = NULL;
double 		*encoder = NULL;

char 		INI_FILENAME[256];
char            MEI_DEVICENAME[256];
char            ATTN_CONNECT[256];
char            CTRL_CONNECT[256];
char            GYRO_NAME[256];

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
                                                   sprintf (MEI_DEVICENAME, "%s\0", argv[i+1]);
                                                }
                                                break;

					case 'f':
                                                if (i < argc-1)
                                                {
                                                   sprintf (INI_FILENAME, "%s\0", argv[i+1]);
                                                }
                                                break;

					case 'a':
                                                if (i < argc-1)
                                                {
                                                   sprintf (ATTN_CONNECT, "%s\0", argv[i+1]);
                                                }
                                                break;

					case 'c':
                                                if (i < argc-1)
                                                {
                                                   sprintf (CTRL_CONNECT, "%s\0", argv[i+1]);
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
initialize( int argc, char *argv[] )

	{
		int ret = qnx_scheduler( 0, 0, SCHED_RR, -1, 1 );
		if (ret < 0)
			cout << "cannot set new scheduler policy" << endl;

		memset (INI_FILENAME, 0, 256);
		memset (MEI_DEVICENAME, 0, 256);
		memset (ATTN_CONNECT, 0, 256);
		memset (CTRL_CONNECT, 0, 256);
		memset (GYRO_NAME, 0, 256);

		// default from HeadControl.h
		sprintf (INI_FILENAME, "%s\0", DEFAULTFILENAME);
		sprintf (MEI_DEVICENAME, "%s\0", DEFAULTDEVICENAME);
		sprintf (ATTN_CONNECT, "%s\0", ATTENTIONCONNECTALL);
		sprintf (CTRL_CONNECT, "%s\0", ATTENTIONCONNECTCONTROL);
		sprintf (GYRO_NAME, "%s\0", GYROTESTER);

		ParseCommandLine (argc, argv);

		printf ("headcontrol: ini filename %s MEI device driver %s\n", INI_FILENAME, MEI_DEVICENAME);
		printf ("headcontrol: attn connect script %s control connect script %s\n", ATTN_CONNECT, CTRL_CONNECT);

		head = new YARPHeadControl();
		head->Initialize (MEI_DEVICENAME, INI_FILENAME);

		encoder = new double[head->GetNJoints()];
		assert (encoder != NULL);

		/* eliminate 'unreferenced' warnings */
		argc = argc, argv = argv;

		return( Pt_CONTINUE );
	}

int 
startall (void)
{
	thread = new CControlThread;
	assert (thread != NULL);
	YARPTime::DelayInSeconds (1.0);

        printf ("Head control is started - MEI reset\n");
        printf ("Starting low-level threads\n");

        thread->Start(1);

        printf ("Threads started\n");

        fflush (stdout);

	// init widget values.
        double l,r,t;
        thread->GetFeedbackEyesGain (&l, &r, &t);

        PtArg_t arg;
        PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &l, 0);
        PtSetResources (ABW_base_eyes_pan_gain, 1, &arg);
        PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &t, 0);
        PtSetResources (ABW_base_eyes_tilt_gain, 1, &arg);

	thread->GetSaccadeGain (&l, &t);
        PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &l, 0);
        PtSetResources (ABW_base_sac_pan, 1, &arg);
        PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &t, 0);
        PtSetResources (ABW_base_sac_tilt, 1, &arg);

	thread->GetDisparityGain (&l);
        PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &l, 0);
        PtSetResources (ABW_base_disp_gain, 1, &arg);

	return 0;
}

int
quit( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		if (encoder != NULL)
			delete[] encoder;

		if (thread != NULL)
		{
			thread->Terminate (1);
			delete thread;
		}

		head->Uninitialize ();
	  
	  YARPThread::TerminateAll();
	  YARPTime::DelayInSeconds(0.25);
		
		exit (0);

		/* eliminate 'unreferenced' warnings */
		widget = widget, apinfo = apinfo, cbinfo = cbinfo;

		return( Pt_CONTINUE );
	}

