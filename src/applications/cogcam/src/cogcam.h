/* Header "cogcam.h" for cogcam Application */
#include <sys/kernel.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <iostream.h>
#include <math.h>
//#include "Image.h"
//#include "ImageIO.h"
#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePortContent.h"
//#include <mem.h>
#include <photon/PhRender.h>
//#include "trajectory.h"
//#include "tobyExperts.h"

#define FRAME_HEIGHT 128
#define FRAME_WIDTH  128


// Command-line flags (flurry_init.cpp)
extern int Verbosity;
extern int PrintTimingCycles;
extern char *my_name;

// Ports (flurry_init.cpp)
//extern InputPortOf<GenericImage> InPortImage;
extern YARPBasicInputPort<YARPImagePortContent> InPortImage;
//extern InputPortOf<TrajectorySetSendable> InPortTraj;

// timing variables (viewer_draw.cc)
extern double starttime, currenttime, fps;
extern PhImage_t ptest;
extern void setup_DB(int w, int h);
extern void SetupImage(PhImage_t *im, int w, int h, int bytes_per_pixel);
extern char *ind;
extern int ImageLoaded;

// resize.cpp
extern int image_scale;
#define MAX_SCALE 4

// freeze.cc
extern int enable_freeze;
extern void set_freeze();

// file.cc and set_file.cc
extern char global_filename[200];
extern char *read_text_from_widget(PtWidget_t *widget);

// trajectory_color.cc
#define COLOR_BY_AGE      0 
#define COLOR_BY_TRACE    1
#define COLOR_BY_ANIMACY  2
#define COLOR_BY_HANIMACY 3
#define COLOR_BY_EXPERT   4
#define COLOR_BY_DISPARITY 5
extern int color_mode;
