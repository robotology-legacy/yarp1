// Yo Emacs!  Parse this file in -*- C++ -*- mode.

#ifndef SDL_CANVAS
#define SDL_CANVAS

#include "PPMImage.h"
#include "SDL.h"   /* All SDL App's need this */
#include "fakevis.h"
#include "cv.h"
#include "VisMapOperators.h"

//////////////////////////////////////////////////////


#define NUM_SCREENS_WIDE 6
#define NUM_SCREENS_TALL 6  
#define MAX_STRING_LENGTH 100

class MyScreen
{
public:

  MyScreen() : magnify_on(false), mag(1.0f), buffer_size(-1), buffer_width(-1), buffer_height(-1), buffer(NULL), imsurf(NULL),iter(0), nframes(0), write_to_disk(false), display_update(false), display_on(false),display_image(false), display_text(false), clear_screen(true), pos_x(0), pos_y(0), cursor_height(0), line_height(15), screen_width(64), screen_height(64) {

    // SDL interprets each pixel as a 32-bit number, so our masks must depend
    //   on the endianness (byte order) of the machine 
	
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

  bg_color.Set(0,0,0);
  }

  void SetBGColor(RGBPix & bg_color_in) { bg_color = bg_color_in; }
  void SetPosition(int x, int y) {pos_x = x; pos_y = y;} 
  void SetScreenSize(int w, int h) { screen_width = w; screen_height = h;}
  void SetBorder(int width, int height) { border_width = width; border_height = height; }
  void SetScreenNum(int screen_num_x_in, int screen_num_y_in){
    screen_num_x = screen_num_x_in; screen_num_y = screen_num_y_in;}

  void SetToClear() { clear_screen = true;}
  void TurnOnMagnify(float mag_in) { magnify_on = true; mag = mag_in;}
  void TurnOffMagnify() {magnify_on = false; }

  void SetWriteToDisk(int nframes_in=1) {write_to_disk = true; nframes=nframes_in;}

  void SetText(char * str_in)
  {
    string_copy( str, str_in );
    display_text = true;
    display_on = true;
  }	
 

 
 void LendBufferToScreen(unsigned char * buff, int im_width, int im_height);

  void CopyBufferToScreen(unsigned char * buff, int im_width, int im_height);
  
  void CopyImageToScreen(RGBIm & rgb_image);

  void OnPaint(SDL_Surface * & screen, bool update_all = false);

private:

  void Clear(SDL_Surface * & screen)
  {
    //    Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b);
    //    int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);

    Uint32 sdl_bg_color;
    SDL_Rect clear_rect;
    clear_rect.x = pos_x - myround(float(border_width)/2.0) ;
    clear_rect.y = pos_y - myround(float(border_width)/2.0) ;
    clear_rect.w = screen_width + border_width;
    clear_rect.h = screen_height + border_height;
    sdl_bg_color = SDL_MapRGB(screen->format, bg_color.R(), bg_color.G(), bg_color.B());
    SDL_FillRect(screen, &clear_rect, sdl_bg_color);
  }


  void string_copy(char *dest_str, char *source_str)
  {
    int index = 0;

    while ((source_str[index] != '\0') && (index < (MAX_STRING_LENGTH-1)))
      {
	dest_str[index] = source_str[index];
	index++;
      }
    dest_str[index] = '\0';
  }

  PPMImage ppmim;

  char str[MAX_STRING_LENGTH];
  int iter;
  int pos_x, pos_y;
  bool display_on, display_image, display_text, display_update;
  bool clear_screen;
  bool write_to_disk;
  int nframes;
  int cursor_height;
  int line_height;
  int screen_width, screen_height;
  int border_width, border_height;
  int screen_num_x, screen_num_y;

  float mag;
  bool magnify_on;

  unsigned char * buffer;
  int buffer_width, buffer_height, buffer_size;

  // Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
  //   as expected by OpenGL for textures 
  Uint32 rmask, gmask, bmask, amask;
  CvFont * font;

  RGBPix bg_color;
  
  SDL_Surface *imsurf;
};


class EdgeData;



//////////////////////////////////////////////////////

class MyCanvas
{
public:
  MyCanvas(); 
  MyCanvas(int screen_widthi, int screen_heighti,
	   int screen_width_borderi, int screen_height_borderi); 
  ~MyCanvas();

  void Draw(SDL_Surface * & screen, bool update_all = false){
    OnPaint(screen, update_all);
  } 

  void OnPaint(SDL_Surface * & screen, bool update_all = false) { 
    int w, h;
    int x, y;
    for(w = 0; w<NUM_SCREENS_WIDE; w++)
      {
	for(h = 0; h<NUM_SCREENS_TALL; h++)
	  {
	    screens[w][h].OnPaint(screen, update_all);
	  }
      }
  }

  void Init(
	    int screen_widthi, int screen_heighti,
	    int screen_width_borderi, int screen_height_borderi
	    );
	
  int width, height;
  int framecount;
  bool initstream;
    
  MyScreen screens[NUM_SCREENS_WIDE][NUM_SCREENS_TALL];

public:

  void WriteToDisk( int screen_index_x, int screen_index_y, int nframes=1 );

  void DisplayText( char * str, //int x, int y,
		    int screen_index_x, int screen_index_y);


  void Display(unsigned char * buffer, 
	       int im_width, int im_height, 
	       int screen_x_index, int screen_y_index)
  {
    DisplayRawRGBBuffer(buffer, 
			im_width, im_height, 
			screen_x_index, screen_y_index);
  }

  void Display(MVisImage<MVisBytePixel> & byte_image,
	       int screen_x_index, int screen_y_index)
  {
    DisplayByteImage(byte_image,
		     screen_x_index, screen_y_index);
  }
	
  void Display(MVisImage<MVisFloatPixel> & float_image,
	       int screen_x_index, int screen_y_index)
  {
    DisplayFloatImage(float_image,
		      screen_x_index, screen_y_index);
  }


  void Display(DIm & double_image,
	       int screen_x_index, int screen_y_index)
  {
    DisplayDoubleImage(double_image,
		      screen_x_index, screen_y_index);
  }
	
  void Display(MVisImage<MVisRGBBytePixel> & rgb_image, 
	       int screen_x_index, int screen_y_index)
  {
    DisplayRGBImage( rgb_image, 
		     screen_x_index, screen_y_index);
  }
	
  void Display(MVisImage<MVisRGBABytePixel> & rgb_image, 
	       int screen_x_index, int screen_y_index)
  {
    DisplayRGBAImage( rgb_image, 
		      screen_x_index, screen_y_index);
  }
	
  void Display(
	       MVisImage<MVisIntPixel> & int_image, 
	       int screen_x_index, int screen_y_index)
  {
    DisplayIntImage(
		    int_image, 
		    screen_x_index, screen_y_index);
  }

  void Display(MVisImage<S2Pix> & s2int_image, 
	       int screen_x_index, int screen_y_index)
  {	
    DisplayS2IntImage(s2int_image, 
		      screen_x_index, screen_y_index);
  }
	
  void Display(MVisImage<MVisUSIntPixel> & usint_image, 
	       int screen_x_index, int screen_y_index)
  {	
    DisplayUSIntImage(usint_image, 
		      screen_x_index, screen_y_index);
  }

  void Display(MVisImage<MVisSignedCharPixel> & schar_image, 
	       int screen_x_index, int screen_y_index)
  {	
    DisplaySignedCharImage(schar_image, 
		      screen_x_index, screen_y_index);
  }
       
	
  //////////////////////////////////////
  //General Functions for Display
  void ClearBorders(MVisByteImage & image, int border_size);
  void ClearRGBBorders(MVisImage<MVisRGBBytePixel> & image, int border_size);
	
  CvPoint GetScreenCoordinates(int screen_x_index, int screen_y_index);
  CvPoint GSC(int screen_x_index, int screen_y_index);
	
  void DrawRectangle(
		     CvPoint center, CvSize size, MVisRGBBytePixel color, int width,
		     int screen_index_x, int screen_index_y); 
	
  void DrawLine(
		CvPoint p1, CvPoint p2, 
		MVisRGBBytePixel color, int width,
		int screen_index_x, int screen_index_y);	
	
  void DrawVectorField(
		       MVisImage<MVisFloatPixel> & dx_image,
		       MVisImage<MVisFloatPixel> & dy_image,
		       RGBIm & background_image,
		       int num_vectors, float scale,
		       MVisRGBBytePixel & color, int width,
		       float magnify,
		       int screen_x_index, int screen_y_index);
  
  void ClearScreen( int screen_index_x, int screen_index_y );
  void MagnifyOn( float mag_in, int screen_index_x, int screen_index_y ) {
    screens[screen_index_x][screen_index_y].TurnOnMagnify(mag_in);
  }
  void MagnifyOff( int screen_index_x, int screen_index_y ) {
    screens[screen_index_x][screen_index_y].TurnOffMagnify();
  }
	
	
  void DisplayQuantizedAngle( MVisImage<MVisBytePixel> & angle_map,
			      MVisImage<MVisRGBBytePixel> & image_for_display);

  void DisplayQuantizedFloatAngle( MVisImage<MVisFloatPixel> & angle_map,
				   MVisImage<MVisRGBBytePixel> & image_for_display);
	

private:


  void DisplayRawRGBBuffer(unsigned char *buffer, int im_width, int im_height, 
			   int screen_x_index, int screen_y_index);
  void DisplayByteImage(MVisImage<MVisBytePixel> & byte_image,
			int screen_x_index, int screen_y_index);
  void DisplayFloatImage(MVisImage<MVisFloatPixel> & float_image,
			 int screen_x_index, int screen_y_index);
  void DisplayDoubleImage(DIm & double_image,
			 int screen_x_index, int screen_y_index);
  void DisplayRGBImage(MVisImage<MVisRGBBytePixel> & rgb_image, 
		       int screen_x_index, int screen_y_index);
  void DisplayRGBAImage(MVisImage<MVisRGBABytePixel> & rgb_image, 
			int screen_x_index, int screen_y_index);
  void DisplayIntImage(MVisImage<MVisIntPixel> & int_image, 
		       int screen_x_index, int screen_y_index);
  void DisplayUSIntImage(MVisImage<MVisUSIntPixel> & usint_image, 
			 int screen_x_index, int screen_y_index);
  void DisplayS2IntImage(MVisImage<S2Pix> & s2int_image, 
			 int screen_x_index, int screen_y_index);
  void DisplaySignedCharImage(MVisImage<MVisSignedCharPixel> & signedchar_image,
			      int screen_x_index, int screen_y_index);
  void DisplayOutput(MVisImage<MVisRGBBytePixel> & rgb_image, int x, int y);
	

  ////////////////////


  SDL_Surface *screen;
  int screen_width, screen_height, screen_width_border, screen_height_border;
	
    };


#endif
//////////////////////////////////////////////////////
