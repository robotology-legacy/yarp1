/* Header "shlc.h" for shlc Application */

#ifndef __shlch__
#define __shlch__

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>
#include <photon/PhRender.h>

// sys includes.
#include <stdio.h>
#include <sys/sched.h>
#include <process.h>

#include "YARPImageServices.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPVelocityField.h"

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

#include "classes.h"

// from poking library.
#include "YARPCanonicalData.h"
#include "YARPVisualContact.h"
#include "YARPVisualSearch.h"
#include "YARPFlowTracker.h"
#include "YARPObjectContainer.h"
#include "YARPBottle.h"


/* experimental */
#include "YARPGoodActions.h"


// Globals
extern InputManager *manager;
extern ImageReader *reader;
extern MessageReceiver *messenger;

extern bool play_from_memory;
extern bool opticflow_mode;
extern bool secret_mode;
extern bool canonical_mode;

extern int playing_frame;

extern YARPOutofScreenImage *oos;
extern YARPOutofScreenImage *oosd;
extern YARPOutofScreenImage **oosi;
extern YARPOutofScreenImage **ooso;

extern char genericbuf[];
extern char globaldirname[];

extern PlotVectors *plot_vectors;
extern PlotVectors *plot_vectors2;

extern YARPVelocityField *velocity_field;
extern YARPImageOf<YarpPixelMono> *mono;
extern YARPImageOf<YarpPixelBGR> *output;

extern YARPGoodActionMapArray *goodaction;
extern YARPObjectContainer *objects;


/* Prototypes */
int render_cur_frame (void);
int render_cur_output_frame (void);
int render_sequence (void);
int render_output_sequence (void);
int render_opticflow (void);
int alloc_secret (void);
int free_secret (void);
int processing (void);

/* Canonical neurons-like representation */
int Display (YARPFlowTracker& tracker);
int BrowserDisplay (void);
int BrowserInit (void);
int BrowserNext (void);

int addcanonical (void);
int reset_canonical_mode (void);

/* Good-bad actions representation */
int buildactionmap (int ret);
int queryactionmap (float theta, float phi, float& torso, int& action_no);


/* Use of canonical/mirror representation */
int find_object (int& bestaction);



/* Constants */
#define CANONICALFILENAME "/ai/ai/mb/cdp/src/YARP/state/canonical/canonical"
#define GOODMAPFILENAME "/ai/ai/mb/cdp/src/YARP/state/goodaction/goodaction"
#define CSTATSFILENAME "/ai/ai/mb/cdp/src/YARP/state/canonical/stats"
#define CIMAGESFILENAME "/ai/ai/mb/cdp/src/YARP/state/canonical/tmp_img"


const int		WIDTH = 128;
const int		HEIGHT = 128;

const int		NEURONS = 40;
const int		STATNBINS = 8;
const int		MAXNUMBEROFFRAMES = FRAMES2STORE;	// already defined in classes.h

	
#endif