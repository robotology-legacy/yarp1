#include <stdio.h>
#include <stdlib.h>
#include <photon/PtWidget.h>
#include <photon/PtWindow.h>
#include <photon/PdDirect.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/neutrino.h>
#include <inttypes.h>
#include <sys/syspage.h>

#include <unistd.h>
#include <sys/time.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>


#include "bttvx.h"

/* For "devctl()" */
#include <devctl.h>
#include <sys/dcmd_chr.h>


// we'll be testing three different types of blits, defaulting to the first
static long blittype=0;

// this structure will contain our image data
typedef struct
{
	unsigned char *buffer;   // raw RGB565 data
	long width;               // width of image
	long height;              // height of image
	long pitch;               // pitch (bytes per row) of image
	PdOffscreenContext_t *ctx;

} CoolImage;


#define WH 256              // width & height (we'll use a square buffer)


#define RAD (WH>>1)         // 1/2 the width/height for computing animation
#define REPS 100            // how many times we want to blit each image

// returns the allocated buffer
CoolImage *AllocBuffer(long w,long h,int fd)
{
	CoolImage *i=(CoolImage*)malloc(sizeof(*i));


	if (!i) return 0;

	// the width/height are always what we're passed in
	i->width=w;
	i->height=h;

	// our blit type 0 is a straight memory blit
	if (blittype==0)
	{
		//i->pitch=w*2;
		i->pitch=w * deep;
		if (i->buffer=(unsigned char*)malloc(w*h*deep))
			return i;
	}else
	if (blittype==1)
	{
		i->pitch=w * deep;
		if (i->buffer=(unsigned char*)PgShmemCreate(w*h*deep,NULL))
		    return i;
	}else
	if (blittype==2)
	{
		if (i->ctx=PdCreateOffscreenContext(Pg_IMAGE_DIRECT_565,768,h,Pg_OSC_MEM_PAGE_ALIGN))
		{
			i->pitch=i->ctx->pitch;
			//i->ctx->pitch = 1024 * 2;
			i->buffer=PdGetOffscreenContextPtr(i->ctx);
			return i;
		}
	}

	// if we fail, free the CoolImage structure, and return 0
	free(i);
	return 0;
}

// this function frees the image given
void FreeBuffer(CoolImage *i)
{
	// for blit type 0, we just free the memory previously malloced
	if (blittype==0)
		free(i->buffer);
	else
	if (blittype==1)
		PgShmemDestroy(i->buffer);
	else
	if (blittype==2)
		PhDCRelease(i->ctx);


	// free the structure as well
	free(i);
}

// this function blits the given buffer using our blit type method
inline void BlitBuffer(PtWidget_t *win,CoolImage *i)
{
	// For blit type 0, we use PgDrawImagemx(). We have to make sure 
	// to set the region to the windows region first.  Don't forget
	// to flush! :)
	if (blittype==0 || blittype==1)
	{
		PhPoint_t pos={0,0};
		PhDim_t size;
		
		//size.w = 768;
		size.w = W;
		size.h = H;
		//size.h = 576;

		PgSetRegion(PtWidgetRid(win)); 
		//PgDrawImagemx(i->buffer,Pg_IMAGE_DIRECT_565,&pos,&size,i->pitch,0);
		PgDrawImagemx(i->buffer,Pg_IMAGE_DIRECT_888,&pos,&size,i->pitch,0);

		PgFlush();
	}else
    if (blittype == 2)
	{
		PhRect_t r={{0,0},{i->width,i->height}};
		PgSetRegion(PtWidgetRid(win));
		PgContextBlit(i->ctx,&r,NULL,&r);
		PgFlush();
	}

}

main(int argc,char *argv[])
{
	CoolImage *image;
	int x,y;
	int i,j;
	PtWidget_t *win;
	PtArg_t args[3];
	PhDim_t dim={W,H};
	PhPoint_t pos={50,50};
	int fd; //Bt878 driver file descriptor
	int size_read;
	struct timeval tv;
    fd_set rfd;
    int n;
	int error;


	//Timing calculation
    uint64_t cps, cycle1, cycle2, ncycles;
    float sec;

	// if a paramater was passed, grab it as the blit type 
	if (argc>1) blittype=atoi(argv[1]);

	// initialize our connection to Photon, and create/realize a window 
	PtInit("/dev/photon");
	PtSetArg(&args[0],Pt_ARG_POS,&pos,0);
	PtSetArg(&args[1],Pt_ARG_DIM,&dim,0);
	win=PtCreateWidget(PtWindow,Pt_NO_PARENT,2,args);
	PtRealizeWidget(win);

	// Allocate and fill a series of NUMIMAGES images with a little 
	// fading type animation.  Put your own animation in here if you like.


	/*
     *    Set a 5 second timeout.
     */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	image = AllocBuffer(W,H,fd);			

	while(1)
	{
		fd = open("/dev/bttvx0",O_RDWR);
		if ( fd > 0 )
		{
			FD_ZERO( &rfd );
			FD_SET( fd, &rfd );
			switch ( n = select( 1 + max( fd,0 ),
			   &rfd, 0, 0, &tv ) ) 
			{
			  case -1:
				perror( "select" );
				return EXIT_FAILURE;
			  case  0:
				puts( "select timed out" );
				break;
			  default:
				//printf( "descriptor ready ...\n");
				//if( FD_ISSET( console, &rfd ) )
				//  puts( " -- console descriptor has data pending" );
				//if( FD_ISSET( serial, &rfd ) )
				//  puts( " -- serial descriptor has data pending" );
				/* Read the text */

				//cycle1=ClockCycles( );

				size_read = read( fd, image->buffer, W*H*deep );

				//cycle2=ClockCycles( );

				////if ( !size_read )
				/////	printf("Attention: read 0 bytes");
    
				//printf("READ: %d",size_read);
				/* Test for error */
				/*
			   if( size_read == -1 ) 
			   {
				 perror( "Error reading myfile.dat" );
				 return EXIT_FAILURE;
			   }*/

			}

		   close( fd );

		   cycle1=ClockCycles( );
		   
		   BlitBuffer(win,image);

		   cycle2=ClockCycles( );

		   
		   ncycles=cycle2-cycle1;
		   //printf("%lld cycles elapsed \n", ncycles);

		   /* find out how many cycles per second */
		
		   cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		   //printf( "This system has %lld cycles/sec.\n",cps );
		   sec=(float)ncycles/cps;
		   //printf("The cycles in seconds is %f \n",sec);
		}else
			sleep(2);
	}

	//printf("Blitted %d frames using method %d\n",REPS*NUMIMAGES,blittype);

	// now free the images
	FreeBuffer(image);
	close( fd );
	/// hide the window and destroy it.
	PtUnrealizeWidget(win);
	PtDestroyWidget(win);
}
