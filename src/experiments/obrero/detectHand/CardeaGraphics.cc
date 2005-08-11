

#include "CardeaGraphics.h"

#define HAVE_GRAPHICS

#ifdef HAVE_GRAPHICS
#include "SDL.h"   /* All SDL App's need this */
#include "sdl_canvas.h"
#endif

typedef void *PVoid;

#ifdef HAVE_GRAPHICS
MyCanvas& GetCanvas(PVoid& pcanvas)
{
  if (pcanvas==NULL)
    {
      pcanvas = new MyCanvas;
    }
  return (*((MyCanvas*)pcanvas));
}
#endif

#define canvas GetCanvas(pcanvas)

CardeaGraphics::~CardeaGraphics()
{
#ifdef HAVE_GRAPHICS
  if (pcanvas!=NULL) delete ((MyCanvas*)pcanvas);
#endif
}

void CardeaGraphics::Body()
{
  int done = 0;
  while (!done)
    {
      available.Wait();
      while (available.PollingWait()) {}
      show_mutex.Wait();
      running = 1;
      if (estop) done = 1;
      show_mutex.Post();
      if (!done)
	{
	  ShowDirect(panes[0],panes[1]);
	}
      show_mutex.Wait();
      running = 0;
      show_mutex.Post();
    }
  stopped.Post();
}

void CardeaGraphics::Init(bool n_text_mode, int w, int h)
{
  out1_image_buffer = new unsigned char [h * w * 3];
  out2_image_buffer = new unsigned char [h * w * 3];
  image_height = h;
  image_width = w;
#ifdef HAVE_GRAPHICS
  //MyCanvas canvas(160, 120, 5, 5); 
  canvas.Init(160, 120, 5, 5); 
  
  const SDL_VideoInfo *narf;
  SDL_Surface *imsurf;
#endif


  use_sdl = !n_text_mode;
  int sdl_w, sdl_h;

  sdl_w = 700;
  sdl_h = 300;
  //sdl_w = 1200;
  //sdl_h = 1050;
  //sdl_w = 1400;
  //sdl_h = 1050;
  
  //if(text_mode) use_sdl = false;

#ifdef HAVE_GRAPHICS
  if(use_sdl)
    {
      printf("Initializing SDL.\n");
      /* Initialize defaults, Video and Audio */
      if((SDL_Init(SDL_INIT_VIDEO)==-1)) { 
	printf("Could not initialize SDL: %s.\n", SDL_GetError());
	use_sdl = false;
      }
      printf("SDL initialized.\n");
      narf=SDL_GetVideoInfo();
      printf("Is it possible to  create hardware surfaces?\n\t");
      printf("hw_available=%d\n",narf->hw_available);
      printf("Is there a window manager available?\n\t");
      printf("wm_available=%d\n",narf->wm_available);
      printf("Are hardware to hardware blits accelerated?\n\t");
      printf("blit_hw=%d\n",narf->blit_hw);
      printf("Are hardware to hardware colorkey blits accelerated?\n\t");
      printf("blit_hw_CC=%d\n",narf->blit_hw_CC);
      printf("Are hardware to hardware alpha bits accelerated?\n\t");
      printf("blit_hw_A=%d\n",narf->blit_hw_A);
      printf("Are software to hardware blits accelerated?\n\t");
      printf("blit_sw=%d\n",narf->blit_sw);
      printf("Are software to hardware colorkey blits accelerated?\n\t");
      printf("blit_sw_CC=%d\n",narf->blit_sw_CC);
      printf("Are software to hardware alpha blits accelerated?\n\t");
      printf("blit_sw_A=%d\n",narf->blit_sw_A);
      printf("Are color fills accelerated?\n\t");
      printf("blit_fill=%d\n",narf->blit_fill);
      printf("\n\n\t\tTotal amount of video_mem: %d\n",narf->video_mem);
       
      if((screen = SDL_SetVideoMode(sdl_w,sdl_h,24,SDL_SWSURFACE | SDL_ANYFORMAT))==NULL) 
	{
	  fprintf(stderr,"\n\t Could NOT set video mode %s\n\n",SDL_GetError());
	  use_sdl = false;
	}
    }

  /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
     as expected by OpenGL for textures */
  Uint32 rmask, gmask, bmask, amask;
	
  /* SDL interprets each pixel as a 32-bit number, so our masks must depend
     on the endianness (byte order) of the machine */
	
  //   //SDL_BIG_ENDIAN
  //   rmask = 0xff000000;
  //   gmask = 0x00ff0000;
  //   bmask = 0x0000ff00;
  //   amask = 0x00000000;
	
  //little endian like Intel
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0x00000000;
#endif
}



void CardeaGraphics::Deinit()
{
  if (backgrounded)
    {
      estop = 1;
      available.Post();
      stopped.Wait();
    }

#ifdef HAVE_GRAPHICS
  if(use_sdl)
    {  
      printf("Quitting SDL...\n");
      /* Shutdown all subsystems */
      SDL_Quit();
      printf("done.\n\n");
    }
#endif
}


void CardeaGraphics::Show(YARPImageOf<YarpPixelBGR>& dest,
			  YARPImageOf<YarpPixelBGR>& dest2)
{
  if (!backgrounded)
    {
      ShowDirect(dest,dest2);
    }
  else
    {
      show_mutex.Wait();
      if (!running)
	{
	  panes[0] = dest;
	  panes[1] = dest2;
	  available.Post();
	}
      show_mutex.Post();
    }
}

void CardeaGraphics::ShowDirect(YARPImageOf<YarpPixelBGR>& dest,
				YARPImageOf<YarpPixelBGR>& dest2)
{
#ifdef HAVE_GRAPHICS
  {
    unsigned char *pix = out1_image_buffer;
    for(int y=0; y<image_height; y++)
      {
	for(int x=0; x<image_width; x++) 
	  {
	    *pix = dest.Pixel(x,y).r;
	    pix++;
	    *pix = dest.Pixel(x,y).g;
	    pix++;
	    *pix = dest.Pixel(x,y).b;
	    pix++;
	  }
      }
  }

  {
    unsigned char *pix = out2_image_buffer;
    for(int y=0; y<image_height; y++)
      {
	for(int x=0; x<image_width; x++) 
	  {
	    *pix = dest2.Pixel(x,y).r;
	    pix++;
	    *pix = dest2.Pixel(x,y).g;
	    pix++;
	    *pix = dest2.Pixel(x,y).b;
	    pix++;
	  }
      }
  }

    RGBIm flrim, hdim, doorim, hsvim;
    CopyToImage(flrim, out1_image_buffer, image_width, image_height);
    CopyToImage(hdim, out2_image_buffer, image_width, image_height);

    //printf("Ping....\n");
    if(use_sdl) {
      //printf("Allegedly displaying\n");
      canvas.MagnifyOn(2.0f, 0, 0);  
      canvas.Display(flrim, 0, 0);
      canvas.MagnifyOn(2.0f, 2, 0);  
      canvas.Display(hdim, 2, 0);
      //canvas.MagnifyOn(2.0f, 2, 2);  
      //canvas.Display(doorim, 2, 2);
      canvas.OnPaint((SDL_Surface*)screen);
    }
#endif
}
