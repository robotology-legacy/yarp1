/* Header "displayimg.h" for shlc Application */

#ifndef __displayimgh__
#define __displayimgh__

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

#include "YARPImageServices.h"
#include "YARPPort.h"
#include "YARPImagePort.h"

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

//#include "portnames.h"

// types.
class YARPOutofScreenImage
{
protected:
    PmMemoryContext_t *DBMem;
    PhImage_t im;
//    YARPImageOf<YarpPixelBGR> tmp;

    virtual void DrawSomething(void) {}

public:
    YARPOutofScreenImage (int width, int height)
    {
      // only display type is BGR.
      im.type = Pg_IMAGE_DIRECT_888;
      im.bpl = width * 3;
      im.size.w = width;
      im.size.h = height;
      im.palette_tag = 0;
      im.colors = 0;
      im.xscale = 1;
      im.yscale = 1;
      im.format = NULL;
      im.flags = NULL;
      im.ghost_bitmap = NULL;
      im.spare1 = NULL;
      im.ghost_bpl = 0;
      im.mask_bpl = 0;
      im.mask_bm = NULL;
      im.palette = NULL;
      im.image = (char *) PgShmemCreate(width*height*3,NULL);

      // out of screen mem context.
      PhDim_t dim;
      dim.w = width;
      dim.h = height;

      PhPoint_t trans = { 0, 0 };
      DBMem = PmMemCreateMC(&im, &dim, &trans);

//      tmp.Resize(width,height);
    }

    virtual ~YARPOutofScreenImage()
    {
      PmMemReleaseMC(DBMem);

      // need to free the shmem?
	  PgShmemCleanup();

//      tmp.Cleanup();
    }

    // display plain image.
    virtual void Display (PtWidget_t *widget, YARPGenericImage& image)
    {
		if (image.GetID() != YARP_PIXEL_BGR)
		{
			printf ("aahhhh: this class can only display BGR images\n");
			return;
		}
#if 0
		SatisfySize (image, tmp);

	   // copying to tmp is not really needed.
       if (image.GetID() != YARP_PIXEL_BGR)
       {
          tmp.CastCopy(image);
       }
       else
       {
          tmp.PeerCopy(image);
       }
       memcpy (im.image, tmp.GetRawBuffer(), tmp.GetWidth()*tmp.GetHeight()*3);
#endif

	   memcpy (im.image, image.GetRawBuffer(), image.GetWidth()*image.GetHeight()*3);

       PmMemStart(DBMem);
       DrawSomething();
       PmMemFlush(DBMem, &im);
       PmMemStop(DBMem);

       PtArg_t args[1];
       PtSetArg( &args[0], Pt_ARG_LABEL_DATA, &im, sizeof(PhImage_t) );
       PtSetResources(widget, 1, args);
    }
};

// prototypes.
//int display_img (PtWidget_t *widget, YARPGenericImage& generic);

#endif
