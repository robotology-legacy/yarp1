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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"
#define PX_IMAGE_MODULES
#define PX_GIF_SUPPORT
#define PX_JPG_SUPPORT
#define PX_TIFF_SUPPORT
#define PX_BMP_SUPPORT
#include <photon/PxImage.h>

char fname[150];

void *ImageFile_alloc(long nbytes, int type)
{
  // cout << "Called alloc " << endl;
  if ((type == PX_IMAGE) || (type == PX_NORMAL))
    return(calloc(1, nbytes));
  else
    return(NULL);
}

void *ImageFile_free(void *mem, int type)
{
  // cout << "Free called " << endl;
  free(mem);
  return(NULL);
}

void *ImageFile_error(char *msg)
{
  ApError(ABW_base, errno, "Cogcam", msg, __FILE__);
  PtFlush();
  return(NULL);
}

void *ImageFile_warning(char *msg)
{
  ApError(ABW_base, errno, "Cogcam", msg, __FILE__);
  PtFlush();
  return(NULL);
}

void *ImageFile_progress(int percent)
{
  PtArg_t args[1];
  int perc;
  
  perc = percent >> 16;
  
  PtSetArg(&args[0], Pt_ARG_GAUGE_VALUE, perc, 0);
  PtSetResources(ABW_progress_chart, 1, &args[0]);
  PtFlush();
  
  return(NULL);
}

int
save( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PxMethods_t meths;
  char *fname;
  PtArg_t args[1];
  char labelfn[300];
  
  memset(&meths, 0, sizeof(PxMethods_t));
  meths.flags = PX_LOAD;
  meths.px_alloc = ImageFile_alloc;
  meths.px_free = ImageFile_free;
  meths.px_warning = ImageFile_warning;
  meths.px_error = ImageFile_error;
  meths.px_progress = ImageFile_progress;
    
//  cout << "writing to file <" << global_filename << ">" << endl;
  
  sprintf(labelfn, "Saving File : %s", global_filename);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, labelfn, 0);
  PtSetResources(ABW_progress_label, 1, &args[0]);
  
  ImageFile_progress(0);

  PxWriteImage(global_filename, 
	       &ptest, 
	       &meths,
	       PX_IMAGE_BMP,
	       PX_IMAGE);

  PtUnrealizeWidget(ABW_progress_bar);
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
  
}


int
load( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PxMethods_t meths;
  char *fname;
  PtArg_t args[1];
  char labelfn[300];
  PhImage_t *img;
  
  memset(&meths, 0, sizeof(PxMethods_t));
  meths.flags = PX_LOAD;
  meths.px_alloc = ImageFile_alloc;
  meths.px_free = ImageFile_free;
  meths.px_warning = ImageFile_warning;
  meths.px_error = ImageFile_error;
  meths.px_progress = ImageFile_progress;
    
  // cout << "reading from file <" << global_filename << ">" << endl;
  
  sprintf(labelfn, "Loading File : %s", global_filename);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, labelfn, 0);
  PtSetResources(ABW_progress_label, 1, &args[0]);
  
  ImageFile_progress(0);
  
  img = PxLoadImage(global_filename, &meths);
  
  // printf( "Image width:   %d\n", img->size.w );
  // printf( "Image height:  %d\n", img->size.h );
  // printf( "Image BPL:     %d\n", img->bpl );
  // printf( "Image colors:  %d\n", img->colors );
  // printf( "Image type:    %d\n", img->type );
  
  PtUnrealizeWidget(ABW_progress_bar);

  /*
   if ((img->size.w != FRAME_WIDTH) || (img->size.h != FRAME_HEIGHT))
    {
      ImageFile_error("Invalid image size to load.  Can only load 128x128 images. ");
      ImageFile_free(img->image, 0);
      return(Pt_CONTINUE);
    }
   */
  
  image_scale = img->size.w / FRAME_WIDTH;
  set_size();
  
  ind = img->image;
  ImageLoaded = true;
  set_freeze();
  
  ImageFile_free(img->image, 0);
    
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
      
  return( Pt_CONTINUE );
  
}

