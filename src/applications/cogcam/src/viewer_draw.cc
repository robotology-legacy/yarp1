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
//#include "attndepth.h"

#include "YARPTime.h"

PhImage_t ptest;

PmMemoryContext_t * DBMem;

PhDim_t dim;

int NoImagesYet = true;
int SetName = false;
YARPImageOf<YarpPixelBGR> IncomingImage;
char *ind;
int gtime;

// timing variables
double starttime, currenttime, fps;
int iterations = 0;

// trajectory variables
int ImageLoaded = false;
int NoTrajYet = true;
int BeenCleared = false;
#if 0
//iDLIST_OF< TARGET_TRACK > * targetTrackArray = NULL;
//iDLIST_OF< FALARM > * falarmArray = NULL;
//int num_target_tracks, num_falarms;
PgColor_t TrajectoryColors[14] = {
    Pg_RED, Pg_BLUE, Pg_GREEN, Pg_YELLOW, Pg_MAGENTA,
    Pg_CYAN, Pg_DGREEN, Pg_DCYAN, Pg_DBLUE, Pg_BROWN,
    Pg_PURPLE, Pg_CELIDON, Pg_WHITE, Pg_GRAY
};
#endif



void SetupImage(PhImage_t *im, int w, int h, int bytes_per_pixel)
{
  int i, j;
  static int done_im_init = false;

  // cout << "SetupImage size " << w << " " << h << endl;
  
  im->type =  Pg_IMAGE_DIRECT_888;
  im->bpl = w * bytes_per_pixel;
  im->size.w = w;
  im->size.h = h;
  im->palette_tag = 0;
  im->colors = 0;
  im->xscale = 1;
  im->yscale = 1;
  im->format = NULL;
  im->flags = NULL;
  im->ghost_bitmap = NULL;
  im->spare1 = NULL;
  im->ghost_bpl = 0;
  im->mask_bpl = 0;
  im->mask_bm = NULL;
  im->palette = NULL;
  if (!done_im_init)
    PgShmemCleanup();
  
  im->image = (char *) PgShmemCreate(sizeof(unsigned char)*h*w*bytes_per_pixel,
				     NULL);
  done_im_init = true;
  // im->image_tag = PxCRC(im->image, im->size.h * im->bpl);
}

void setup_DB(int w, int h)
{
  static int done_init = false;
  PhDim_t dim = {FRAME_WIDTH*image_scale, FRAME_HEIGHT*image_scale};
  PhPoint_t trans = {0,0};

  if (done_init)
    PmMemReleaseMC(DBMem);
  DBMem = PmMemCreateMC(&ptest, &dim, &trans);
  done_init = true;
}



void DrawTrajectories(int isClear)
{
#if 0
  PTR_INTO_iDLIST_OF< TARGET_TRACK > targetTrack;
  PTR_INTO_iDLIST_OF< TARGET_TRACK_ELEMENT > targetTrackEl;
  PTR_INTO_iDLIST_OF< FALARM > falarm;
  int numpts, id, lastx, lasty, newx, newy, nc;
  int len, maxlen;
  int ycen, xcen;
  float tconst;

  tconst = 255.0 / (TrajectoryLength+1);
  
  if (!isClear)
    {
      PgSetStrokeColor(Pg_BLACK);
      PgSetFillColor(Pg_BLACK);
      PgDrawIRect(0, 0, 10, 10, Pg_DRAW_FILL_STROKE);
      PgDrawIRect(0, 0, 127*image_scale, 127*image_scale, Pg_DRAW_FILL_STROKE);
    }
  
  LOOP_DLIST( falarm, *falarmArray )
    {
      if (color_mode == COLOR_BY_ANIMACY)
	{
	  PgSetStrokeColor(Pg_MAGENTA);
	  PgSetFillColor(Pg_MAGENTA);
	}
      else if (color_mode == COLOR_BY_HANIMACY)
	{
	  PgSetStrokeColor(Pg_MAGENTA);
	  PgSetFillColor(Pg_MAGENTA);
	}
      else if (color_mode == COLOR_BY_EXPERT)
	{
	  PgSetStrokeColor(Pg_BLACK);
	  PgSetFillColor(Pg_BLACK);
	}
      else if (color_mode == COLOR_BY_DISPARITY)
	{
	  PgSetStrokeColor(Pg_MAGENTA);
	  PgSetFillColor(Pg_MAGENTA);
	}
      else
	{
	  nc = 255 - (tconst*( gtime - (*falarm).frameNo));
	  if (nc > 255) nc = 255;
	  PgSetStrokeColor(PgRGB(nc, 0, 0));
	  PgSetFillColor(PgRGB(nc, 0, 0));
	}
      
      ycen = (*falarm).rY * image_scale;
      xcen = (*falarm).rX * image_scale;
      // cout << "falarm at " << xcen << " " << ycen << endl;
      PgDrawIRect(ycen - 1, xcen - 1,
		  ycen + 1, xcen + 1,
		  Pg_DRAW_FILL_STROKE);
    }
  
  maxlen = 0;
  LOOP_DLIST( targetTrack, *targetTrackArray )
    {
      id = (*targetTrack).id;
      numpts = (int) (*targetTrack).list.getLength();
      len = 0;
      
      if (color_mode == COLOR_BY_TRACE)
	{
	  PgSetStrokeColor( TrajectoryColors[id % 14] );
	  PgSetFillColor( TrajectoryColors[id % 14] );
	}
      else if (color_mode == COLOR_BY_ANIMACY)
	{
	  switch((*targetTrack).AnimacyCode)
	    {
	    case IS_INANIMATE:
	      PgSetStrokeColor( Pg_RED );
	      PgSetFillColor( Pg_RED );
	      break;
	    case IS_ANIMATE:
	      PgSetStrokeColor( Pg_GREEN );
	      PgSetFillColor( Pg_GREEN );
	      break;
	    case IS_UNDEFINED_ANIMATE:
	    default:
	      PgSetStrokeColor( Pg_BLUE );
	      PgSetFillColor( Pg_BLUE);
	      break;
	    }
	}
      else if (color_mode == COLOR_BY_HANIMACY)
	{
	  // range from 1 to 2
	  if ((*targetTrack).HistAnimacyScore > 0)
	    {
	      nc = 255 * ( (*targetTrack).HistAnimacyScore/2 + 1) / 2;
	      if (nc > 255) nc = 255;
	      PgSetStrokeColor( PgRGB(0, nc, 0) );
	      PgSetFillColor( PgRGB(0, nc, 0) );
	    }
	  else
	    {
	      PgSetStrokeColor( PgRGB(128, 0, 0) );
	      PgSetFillColor( PgRGB(128, 0, 0) );
	    }
	}
      else if (color_mode == COLOR_BY_EXPERT)
	{
	  id = (*targetTrack).AnimacyExpert;
	  if ((id >= 0) && (id < NUM_TOBY_EXPERTS))
	    {
	      PgSetStrokeColor( TobyColors[id] );
	      PgSetFillColor( TobyColors[id] );
	    }
	  else
	    {
	      PgSetStrokeColor( Pg_BLACK );
	      PgSetFillColor( Pg_BLACK );
	    }
	}
	  
      lastx = lasty = -1;
      LOOP_DLIST( targetTrackEl, (*targetTrack).list )
	{
	  if ((*targetTrackEl).hasReport)
	    {
	      newx = (*targetTrackEl).rx * image_scale;
	      newy = (*targetTrackEl).ry * image_scale;
	      // cout << "target at " << newx << " " << newy << endl;
	      if (color_mode == COLOR_BY_AGE)
		{
		  nc = 255 - tconst * (gtime - (*targetTrackEl).time);
		  if (nc > 255) nc = 255;
		  PgSetStrokeColor( PgRGB(0, nc, 0) );
		  PgSetFillColor( PgRGB(0, nc, 0) );
		}
	      else if (color_mode == COLOR_BY_DISPARITY)
		{
		  // use the constant from attndepth.h
		  nc =((*targetTrackEl).depth_rating*7)/8 + 32;
		  if (nc > 255) nc = 255;
		  PgSetStrokeColor( PgRGB(0, nc, 0) );
		  PgSetFillColor( PgRGB(nc, 0, 0) );
		}
		
	      PgDrawIRect(newy - 1, newx - 1,
			  newy + 1, newx + 1,
			  Pg_DRAW_FILL_STROKE);
	      if (lastx >= 0)
		{
		  len += abs(newy-lasty) + abs(newx-lastx);
		  PgDrawILine(lasty, lastx, newy, newx);
		}
	      lastx = newx;
	      lasty = newy;
	    }
	}
      if (len > maxlen) maxlen = len;
    }
#endif
}




int
viewer_draw( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PhRect_t raw_canvas; 
  int i,j,k;
  char pixR, pixG, pixB;
  int ptest_index, ind_index;
  PtArg_t args[1];
    
  if (! SetName)
    {
      SetupImage(&ptest, FRAME_WIDTH, FRAME_HEIGHT, 3);
      setup_DB(ptest.size.w, ptest.size.h);
      starttime = YARPTime::GetTimeAsSeconds();
      PtSetArg(&args[0], Pt_ARG_TEXT_STRING, my_name, 0);
      PtSetResources(ABW_target_label,1,&args[0]);
      SetName = true;
      IncomingImage.Resize(128,128);
    }


  if (!enable_freeze)
    {
      NoImagesYet = true;
      // check for new image
      if (InPortImage.Read(0))
	{
	  YARPGenericImage & generic = InPortImage.Content();
  	  if (generic.GetID() != YARP_PIXEL_BGR)
	    {
	      IncomingImage.CastCopy(generic);
	      ind = IncomingImage.GetRawBuffer();
	    }
	  else
	    ind = generic.GetRawBuffer();
	  
	  NoImagesYet = false;
	}
     
#if 0 
      // check for new trajectories 
      if (InPortTraj.Read(0))
	{
	  if (targetTrackArray) delete targetTrackArray;
	  if (falarmArray) delete falarmArray;
	  
	  targetTrackArray = new iDLIST_OF< TARGET_TRACK >;
	  falarmArray = new iDLIST_OF< FALARM >;
	  gtime = InPortTraj.Content().UnPackInto(targetTrackArray,
						  falarmArray,
						  &num_target_tracks,
						  &num_falarms);
	  NoTrajYet = false;
	  BeenCleared = false;
	}
#endif
    }

  //YARPTime::DelayInSeconds(2);
  //printf("Boing\n");
  

  // copy the image to the buffer
  if (!NoImagesYet)
    {
      BeenCleared = true;
      if (ImageLoaded)
	{
	  memcpy(ptest.image, ind, ptest.size.h*ptest.bpl);
	}
      else if (image_scale == 1)
	{
	  memcpy(ptest.image, ind, FRAME_WIDTH*FRAME_HEIGHT*3);
	}
      else
	{
	  ind_index = 0;
	  ptest_index = 0;
	  for(i=0;i<FRAME_HEIGHT-1;i++)
	    {
	      ptest_index = ptest.bpl*i*image_scale;
	      for(j=0;j<FRAME_WIDTH;j++)
		{
		  pixR = ind[ind_index++];
		  pixG = ind[ind_index++];
		  pixB = ind[ind_index++];
		  for(k=0;k<image_scale;k++)
		    {
		      ptest.image[ptest_index++] = pixR;
		      ptest.image[ptest_index++] = pixG;
		      ptest.image[ptest_index++] = pixB;
		    }
		}
	      ptest_index = ptest.bpl*i*image_scale;
	      for(k=0;k<image_scale;k++)
		{
		  memcpy(&ptest.image[ptest_index+k*ptest.bpl], 
			 &ptest.image[ptest_index], 
			 ptest.bpl);
		}
	    }
	}
    }

  if (!NoImagesYet)
    {
      if (!NoTrajYet && !ImageLoaded)
	{
	  PmMemStart(DBMem);
	  DrawTrajectories(BeenCleared);
	  PmMemFlush(DBMem, &ptest);
	  PmMemStop(DBMem);
	}
    }
  
  if (!NoImagesYet)
    {
      PtSetArg( &args[0], Pt_ARG_LABEL_DATA, &ptest, sizeof(PhImage_t) );
      PtSetResources(ABW_raw_label, 1, args);
    }

  if (PrintTimingCycles)
    {
      iterations++;
      if (iterations >= PrintTimingCycles)
	{
	  currenttime = YARPTime::GetTimeAsSeconds();
	  fps = PrintTimingCycles/(currenttime-starttime);
	  cout << my_name << " timed " << PrintTimingCycles << " iterations: ";
	  cout << fps << " frames/sec" << endl;
	  starttime = currenttime;
	  iterations = 0;
	}
    }
  
  
  return(Pt_CONTINUE);
  // DBMem = PmMemCreateMC(&ptest, &dim, &trans);
  // need a PmMemRelease(DBMem); in the exit function  
}

