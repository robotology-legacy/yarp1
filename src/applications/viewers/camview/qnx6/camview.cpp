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

#include <YARPEurobotGrabber.h>

#include <assert.h>


#include <bttv.h>

/* For "devctl()" */
#include <devctl.h>
#include <sys/dcmd_chr.h>

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPDIBConverter.h>
#include <YARPLogpolar.h>
#include <iostream>


#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>
#include <YARPImageFile.h>



// we'll be testing three different types of blits, defaulting to the first
static long blittype=0;
unsigned char * temp_buffer_p;

// this structure will contain our image data
typedef struct
{
	unsigned char *buffer;   // raw RGB565 data
	long width;               // width of image
	long height;              // height of image
	long pitch;               // pitch (bytes per row) of image
	PdOffscreenContext_t *ctx;

} CoolImage;


//#define WH 256              // width & height (we'll use a square buffer)
//#define WH 128

int WH = 128;
int _board_no = 0; 
int _video_mo = 0;
char _name[512];
bool _client = false;
bool _simu = false;
bool _logp = false;

#define RAD (WH>>1)         // 1/2 the width/height for computing animation
#define REPS 100            // how many times we want to blit each image

int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/i:img\0", argv[0]);
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+')
		{
			ACE_OS::sprintf (_name, "/%s/o:img\0", argv[i]+1);
		}
		else
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'w':
				WH = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 'h':
				WH = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 'b':
				_board_no = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 't':
				ACE_OS::fprintf (stdout, "grabber acting as a receiver client...\n");
				ACE_OS::sprintf (_name, "%s\0", argv[i+1]);
				i++;
				_client = true;
				_simu = false;
				break;

			case 'v':
				_video_mo = ACE_OS::atoi(argv[i+1]);
				ACE_OS::fprintf (stdout, "video mode %d...\n",_video_mo);
				break;

			case 'l':
				ACE_OS::fprintf (stdout, "logpolar mode\n");
				_logp = true;
				break;
			}
		}
		else
		{
			ACE_OS::fprintf (stderr, "unrecognized parameter %d:%s\n", i, argv[i]);
		}
	}

	return YARP_OK; 
}


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
		//i->pitch=w * 4;
		//if (i->buffer=(unsigned char*)malloc(w*h*deep))
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
		if (i->ctx=PdCreateOffscreenContext(Pg_IMAGE_DIRECT_888,w,h,Pg_OSC_MEM_PAGE_ALIGN))
		{
			i->pitch=i->ctx->pitch;
			//i->ctx->pitch = 1024 * 2;
			i->buffer=(unsigned char *)PdGetOffscreenContextPtr(i->ctx);
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
	PhImage_t phimage;
	// For blit type 0, we use PgDrawImagemx(). We have to make sure 
	// to set the region to the windows region first.  Don't forget
	// to flush! :)
	if (blittype==0 || blittype==1)
	{
		PhPoint_t pos={0,0};
		PhDim_t size;
		
		size.w = WH;
		size.h = WH;

		PgSetRegion(PtWidgetRid(win)); 
		//PgDrawImagemx(i->buffer,Pg_IMAGE_DIRECT_555,&pos,&size,i->pitch,0);
		////PgDrawImagemx(i->buffer,Pg_IMAGE_DIRECT_888,&pos,&size,i->pitch,0);

		////PgFlush();
		
		phimage.size=size;
		phimage.bpl = WH*deep;
		phimage.type = Pg_IMAGE_DIRECT_888;
		phimage.image =  (char *)i->buffer;
		PgDrawPhImagemx( &pos, &phimage, 0  );
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

inline void SaveImage(int cnt, CoolImage *i)
{	
	char file_name[100] = "image";
	char temp[100];
	int fd;
	
	strcat(file_name,itoa(cnt,temp,10));
	strcat(file_name,".raw");
	fd = open(file_name,O_WRONLY | O_CREAT | O_APPEND,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
        | S_IROTH | S_IWOTH );
        
	write(fd, i->buffer, WH*WH*3);
	close(fd);
}

inline void LoadImage(int cnt, CoolImage *i)
{	
	char file_name[100] = "image";
	char temp[100];
	int fd;
	
	strcat(file_name,itoa(cnt,temp,10));
	strcat(file_name,".raw");
	fd = open(file_name,O_RDONLY ,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
        | S_IROTH | S_IWOTH );
        
	read(fd, i->buffer, WH*WH*3);
	printf("%s\n",file_name);
	fflush(stdout);
	close(fd);
}



int RunRemote()
{
	using namespace _logpolarParams;
	
	CoolImage *image;
	int x,y;
	int i,j;
	PtWidget_t *win;
	PtArg_t args[3];
	PhDim_t dim={WH,WH};
	PhPoint_t pos={50,50};
	int fd; //Bt878 driver file descriptor
	int fd_temp;
	int size_read;
	struct timeval tv;
    fd_set rfd;
    int n;
	int error;
	int counter = 0;
	int file = 0;


	//Timing calculation
    uint64_t cps, cycle1, cycle2, ncycles;
    float sec;
    
    //YARPImageOf<YarpPixelBGR> img;
    YARPGenericImage img;
    YARPGenericImage m_flipped;
	YARPGenericImage m_remapped;
	YARPImageOf<YarpPixelBGR> m_colored;

	YARPLogpolar m_mapper;

	//YARPDIBConverter m_converter;
	//YARPInputPortOf<YARPGenericImage> inport (YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> inport;

	inport.Register ("/images:i");
    

	// initialize our connection to Photon, and create/realize a window 
	PtInit("/dev/photon");
	//PtInit("/dev/photon");
	PtSetArg(&args[0],Pt_ARG_POS,&pos,0);
	PtSetArg(&args[1],Pt_ARG_DIM,&dim,0);
	win=PtCreateWidget(PtWindow,Pt_NO_PARENT,2,args);
	PtRealizeWidget(win);


	/*
     *    Set a 5 second timeout.
     */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	image = AllocBuffer(W,H,fd);	
	//assert(image!=0);
		
	while(1)
	{
	
		   inport.Read ();	  
		   img.Refer (inport.Content());

		if (!_logp)
		{
		   image->buffer = (unsigned char *)img.GetRawBuffer();
		   BlitBuffer(win,image);
		   
		   cycle2=ClockCycles( );
		   counter++;

		   
		   ncycles=cycle2-cycle1;
		   /* find out how many cycles per second */
		
		   cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		   sec=(float)ncycles/cps;
		   
		 }else //Log polar case
		 {
		 	//if (img.GetWidth() != _stheta || img.GetHeight() != _srho - _sfovea)
			//{
				/// falls back to cartesian mode.
			//	_logp = false;
			//	continue;
			//}
			
			//YARPImageFile::Write("image0.PGM",img);

			if (m_remapped.GetWidth() != 256 || m_remapped.GetHeight() != 256)
			{
				//m_remapped.Resize (256, 256, YARP_PIXEL_BGR);
				m_remapped.Resize (256, 256, YARP_PIXEL_RGB);
			}

			if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho - _sfovea)
			{
				m_colored.Resize (_stheta, _srho-_sfovea);
			}

			if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
			{
				m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
			}

			m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)img, m_colored);
			m_mapper.Logpolar2Cartesian (m_colored, m_remapped);
			//YARPSimpleOperation::Flip (m_remapped, m_flipped);
			
			//YARPImageFile::Write("remapped0.PPM",m_remapped);
			//exit(0);
			
			 image->buffer = (unsigned char *)m_remapped.GetRawBuffer();
			 
			 
			 
			  BlitBuffer(win,image);

			//m_mutex.Wait();
			//if (m_flipped.GetWidth() != m_x || m_flipped.GetHeight() != m_y)
			//{
			//	m_converter.Resize (m_flipped);
			//	m_x = m_flipped.GetWidth ();
			//	m_y = m_flipped.GetHeight ();
			//}

			//if (!m_frozen)
			//{
				/// prepare the DIB to display.
			//	m_converter.ConvertToDIB (m_flipped);
			//}
		 }
	}


	// now free the images
	FreeBuffer(image);
	close( fd );
	/// hide the window and destroy it.
	PtUnrealizeWidget(win);
	PtDestroyWidget(win);

}

int RunLocally()
{
	CoolImage *image;
	int x,y;
	int i,j;
	PtWidget_t *win;
	PtArg_t args[3];
	PhDim_t dim={W,H};
	PhPoint_t pos={50,50};
	int fd; //Bt878 driver file descriptor
	int fd_temp;
	int size_read;
	struct timeval tv;
    fd_set rfd;
    int n;
	int error;
	int counter = 0;
	int file = 0;


	//Timing calculation
    uint64_t cps, cycle1, cycle2, ncycles;
    float sec;
    
    YARPEurobotGrabber grabber;
    
    
    grabber.initialize (0, 128);

	// initialize our connection to Photon, and create/realize a window 
	PtInit("/net/irene2/dev/photon");
	//PtInit("/dev/photon");
	PtSetArg(&args[0],Pt_ARG_POS,&pos,0);
	PtSetArg(&args[1],Pt_ARG_DIM,&dim,0);
	win=PtCreateWidget(PtWindow,Pt_NO_PARENT,2,args);
	PtRealizeWidget(win);


	/*
     *    Set a 5 second timeout.
     */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	image = AllocBuffer(W,H,fd);	
	//assert(image!=0);
		
	while(1)
	{
	
		   if (file != 2)
		   {
		   grabber.waitOnNewFrame ();
		   grabber.acquireBuffer (&image->buffer);
		   cycle1=ClockCycles( );
		    }
		   
		   switch(file)
		   {
		   	case 0:
		   		BlitBuffer(win,image);
		   		break;
		   	case 1:
		   		SaveImage(counter,image);
		   		break;
		   	case 2:
		   		
		   		LoadImage(counter,image);
		   		BlitBuffer(win,image);
		   		getchar();
		   		break;
		   };
		   
		   if (file!=2)
		   	grabber.releaseBuffer ();
		   cycle2=ClockCycles( );
		   counter++;

		   
		   ncycles=cycle2-cycle1;
		   //printf("%lld cycles elapsed \n", ncycles);

		   /* find out how many cycles per second */
		
		   cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		   //printf( "This system has %lld cycles/sec.\n",cps );
		   sec=(float)ncycles/cps;
		   //printf("The cycles in seconds is %f \n",sec);
		//}else
			//sleep(2);
		}

	//printf("Blitted %d frames using method %d\n",REPS*NUMIMAGES,blittype);

	// now free the images
	FreeBuffer(image);
	close( fd );
	/// hide the window and destroy it.
	PtUnrealizeWidget(win);
	PtDestroyWidget(win);
}

main (int argc, char *argv[])
{

	//if (argc>1) blittype=atoi(argv[1]);
	blittype = 0;
	ParseParams (argc, argv);
	RunRemote();
}




