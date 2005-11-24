#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif



#include "Grapher.h"

#include <yarp/YARPImageDraw.h>

#include <SDL/SDL.h>


#include <yarp/begin_pack_for_net.h>

struct YarpPixelBGRA {
  char b, g, r, a;
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>

class SDL_Helper {
public:
  SDL_Helper() {
    screen = NULL;
    pf = NULL;
    screenWidth = 720;
    screenHeight = 480;
    done = false;
  }

  SDL_Surface *screen;
  SDL_Event event;
  SDL_PixelFormat *pf;
  Uint32 black;
  Uint32 red;
  Uint32 green;
  Uint32 blue;
  
  int screenWidth;
  int screenHeight;

  bool done;




  void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
  {
    Uint8 *ubuff8;
    Uint16 *ubuff16;
    Uint32 *ubuff32;
    Uint32 color;
    char c1, c2, c3;
    
    /* Lock the screen, if needed */
    if(SDL_MUSTLOCK(screen)) {
      if(SDL_LockSurface(screen) < 0) 
	return;
    }
    
    /* Get the color */
    color = SDL_MapRGB( screen->format, r, g, b );
    
    /* How we draw the pixel depends on the bitdepth */
    switch(screen->format->BytesPerPixel) 
      {
      case 1: 
	ubuff8 = (Uint8*) screen->pixels;
	ubuff8 += (y * screen->pitch) + x; 
	*ubuff8 = (Uint8) color;
	break;
	
      case 2:
	ubuff8 = (Uint8*) screen->pixels;
	ubuff8 += (y * screen->pitch) + (x*2);
	ubuff16 = (Uint16*) ubuff8;
	*ubuff16 = (Uint16) color; 
	break;  
	
      case 3:
	ubuff8 = (Uint8*) screen->pixels;
	ubuff8 += (y * screen->pitch) + (x*3);
	
	
	if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
	  c1 = (color & 0xFF0000) >> 16;
	  c2 = (color & 0x00FF00) >> 8;
	  c3 = (color & 0x0000FF);
	} else {
	  c3 = (color & 0xFF0000) >> 16;
	  c2 = (color & 0x00FF00) >> 8;
	  c1 = (color & 0x0000FF);	
	}
	
	ubuff8[0] = c3;
	ubuff8[1] = c2;
	ubuff8[2] = c1;
	break;
	
      case 4:
	ubuff8 = (Uint8*) screen->pixels;
	ubuff8 += (y*screen->pitch) + (x*4);
	ubuff32 = (Uint32*)ubuff8;
	*ubuff32 = color;
	break;
	
      default:
	fprintf(stderr, "Error: Unknown bitdepth!\n");
      }
  
    /* Unlock the screen if needed */
    if(SDL_MUSTLOCK(screen)) {
      SDL_UnlockSurface(screen);
    }
  }



  void init() {
  
    // Try to initialize SDL. If it fails, then give up.
    
    if (-1 == SDL_Init(SDL_INIT_EVERYTHING))
      {
	printf("Can't initialize SDL\n");
	exit(1);
      }
    
    // Safety first. If the program exits in an unexpected
    // way the atexit() call should ensure that SDL will be
    // shut down properly and the screen returned to a
    // reasonable state.
    
    //atexit(SDL_Quit);
    
    // Initialize the display. Here I'm asking for a 640x480
    // window with any pixel format and any pixel depth. If
    // you uncomment SDL_FULLSCREEN you should get a 640x480
    // full screen display.
    

      printf("SDL initialized.\n");
      const SDL_VideoInfo *narf=SDL_GetVideoInfo();
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

      if((screen = SDL_SetVideoMode(screenWidth,screenHeight,24,SDL_SWSURFACE | SDL_ANYFORMAT))==NULL)
        {
          fprintf(stderr,"\n\t Could NOT set video mode %s\n\n",SDL_GetError());
          //use_sdl = false;
        }

  /*

    screen = SDL_SetVideoMode(screenWidth, 
			      screenHeight, 
			      0, 
			      SDL_ANYFORMAT |
			      //SDL_FULLSCREEN |
			      SDL_SWSURFACE
			      );
  */
    
    if (NULL == screen)
      {
	printf("Can't set video mode\n");
	exit(1);
      }
    
    // Grab the pixel format for the screen. SDL_MapRGB()
    // needs the pixel format to create pixels that are laid
    // out correctly for the screen.
    
    pf = screen->format;
    
    //Create the pixel values used in the program. Black is
    //for clearing the background and the other three are
    //for line colors. Note that in SDL you specify color
    //intensities in the rang 0 to 255 (hex ff). That
    //doesn't mean that you always get 24 or 32 bits of
    //color. If the format doesn't support the full color
    //range, SDL scales it to the range that is correct for
    //the pixel format.
    
    black = SDL_MapRGB(pf, 0x00, 0x00, 0x00);
    red = SDL_MapRGB(pf, 0xff, 0x00, 0x00);
    green = SDL_MapRGB(pf, 0x00, 0xff, 0x00);
    blue = SDL_MapRGB(pf, 0x00, 0x00, 0xff);
    
    // Set the window caption and the icon caption for the
    // program. In this case I'm just setting it to whatever
    // the name of the program happens to be.
    
    SDL_WM_SetCaption("obrero-cam", "obrero-cam");
  }


  void show(YARPImageOf<YarpPixelRGB>& src) {
    if (screen==NULL) {
      init();
    }
    /*
    if (screen!=NULL) {
      IMGFOR(src,x,y) {
	YarpPixelRGB& pix = src(x,y);
	setpixel(screen, x, y, pix.r, pix.g, pix.b);
      }
      SDL_Flip(screen);
    }
    */

    /* Draw bands of color on the raw surface */
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 )
	  exit(1);
    }
    /*
    for (int i=0; i<screen->h; ++i ) {
        memset(buffer,(i*255)/screen->h,
               screen->w*screen->format->BytesPerPixel);
               buffer += screen->pitch;
    }
    buffer=(Uint8 *)screen->pixels;
    */
    for (int y=0; y<src.GetHeight(); y++) {
      YarpPixelRGB *pix0 = &src(0,y);
      Uint8 *buffer=(Uint8 *)screen->pixels + y*screen->pitch;
      YarpPixelBGRA *pix1 = (YarpPixelBGRA*)buffer;
      for (int x=0; x<src.GetWidth(); x++) {
	pix1->r = pix0->r;
	pix1->g = pix0->g;
	pix1->b = pix0->b;
	pix0++;
	pix1++;
      }
    }
    if ( SDL_MUSTLOCK(screen) ) {
      SDL_UnlockSurface(screen);
    }
    SDL_Flip(screen);
  }
  

};


static SDL_Helper helper;

void Grapher::Apply(YARPImageOf<YarpPixelRGB>& src) {
  helper.show(src);
}

void Grapher::End() {
  SDL_Quit();
}






