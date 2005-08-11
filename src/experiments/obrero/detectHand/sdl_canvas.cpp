// Yo Emacs!  Parse this file in -*- C++ -*- mode.

#ifndef CARDEA_SIM

#include "sdl_canvas.h"


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


  void MyScreen::CopyImageToScreen(RGBIm & rgb_image)
  {
    RGBIm mag_image;

    int width;
    int height;

    width = rgb_image.Width();
    height = rgb_image.Height();
    
    if(magnify_on) {
      int new_width, new_height;
      new_width = myround(((float) width) * mag); 
      new_height = myround(((float) height) * mag); 
      mag_image.Create(new_height, new_width);
      //IPL_INTER_NN, IPL_INTER_CUBIC
      iplResizeFit(rgb_image.GetIPL(), mag_image.GetIPL(), IPL_INTER_NN); 
      width = new_width;
      height = new_height;
    }

    if(imsurf != NULL) {
      SDL_FreeSurface(imsurf);
      imsurf = NULL;
    }

    if(buffer_size < (width*height*3)) {
      if(buffer != NULL) delete [] buffer;
      buffer_width = width; buffer_height = height;
      buffer = new unsigned char [width*height*3];
      buffer_size = width*height*3;
    }
    
    if(magnify_on) {
      CopyToBuffer(mag_image, buffer, width, height);
    } else {
      CopyToBuffer(rgb_image, buffer, width, height);
    }
    
    imsurf = SDL_CreateRGBSurfaceFrom(buffer, width, height, 24, 
				      3*width, rmask, gmask, bmask, amask);

    display_on = true;
    display_image = true;
    display_update = true;
  }
  

 void MyScreen::LendBufferToScreen(unsigned char * buff, int im_width, int im_height)
  {

    //The data stored in pixels is assumed to be of the depth specified in the parameter 
    //list. The pixel data is not copied into the SDL_Surface structure so it should not 
    //be freed until the surface has been freed with a called to SDL_FreeSurface. pitch 
    //is the length of each scanline in bytes.

    if(imsurf != NULL) {
      SDL_FreeSurface(imsurf);
      imsurf = NULL;
    }

    imsurf = SDL_CreateRGBSurfaceFrom(buff, im_width, im_height, 24, 
				      3*im_width, rmask, gmask, bmask, amask);

    display_on = true;
    display_image = true;
    display_update = true;
  }


  void MyScreen::OnPaint(SDL_Surface * & screen, bool update_all)
  {

    if(display_on && (display_update||update_all))
      {
	iter++;

	if(clear_screen)
	  {
	    Clear(screen);
	    clear_screen = false;
	  }

	if(display_image)
	  {
	    //display
	    
	  SDL_Rect sub_screen_loc;
	  sub_screen_loc.x = pos_x;
	  sub_screen_loc.y = pos_y;

	    if(SDL_BlitSurface(imsurf, NULL, screen, &sub_screen_loc) < 0)
	      fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
	   
	    //printf("x=%d, y=%d, w=%d, h=%d\n", pos_x, pos_y, buffer_width, buffer_height);
	    SDL_UpdateRect(screen, pos_x, pos_y, buffer_width, buffer_height);
	    
	    if(write_to_disk)
	      {	
		if(nframes>0)
		  {
		    char text_out[100];
		    sprintf(text_out, "./%d_%d_%05d.ppm", screen_num_x, screen_num_y, iter);
		    ppmim.CopyFrom(buffer, buffer_width, buffer_height);
		    ppmim.Write(text_out);
		    nframes--;
		  } else
		    {
		      write_to_disk=false;
		    }
	      }
	  }

	if(display_text)
	  {
	   
	    if( (((cursor_height + (2*line_height)) % screen_height) - cursor_height) <= 0 )
	      {
		//we have wrapped around so clear the screen

		cursor_height = 0;
	      }

	    if( (cursor_height == 0) && (!display_image) ) Clear(screen);

	    //cv write text
	    //void cvPutText( IplImage* img, const char* text, CvPoint org, CvFont* font,int color );
	    //  CvPoint text_loc;
	    //  text_loc.x = ;
	    //  text_loc.y = ;
	    //  cvPutText(, str, );
	    display_text = false;

	    cursor_height = cursor_height + line_height;
	  }

      }

    display_update=false;
  }


  void MyScreen::CopyBufferToScreen(unsigned char * buff, int im_width, int im_height)
  {

    //The data stored in pixels is assumed to be of the depth specified in the parameter 
    //list. The pixel data is not copied into the SDL_Surface structure so it should not 
    //be freed until the surface has been freed with a called to SDL_FreeSurface. pitch 
    //is the length of each scanline in bytes.

    if(imsurf != NULL) {
      SDL_FreeSurface(imsurf);
      imsurf = NULL;
    }

    if(buffer_size < (im_width*im_height*3)) {
      if(buffer != NULL) delete [] buffer;
      buffer_width = im_width; buffer_height = im_height;
      buffer = new unsigned char [im_width*im_height*3];
      buffer_size = im_width*im_height*3;
    }

    int i;
    for(i=0;i<buffer_size;i++) buffer[i] = buff[i];
    
    imsurf = SDL_CreateRGBSurfaceFrom(buffer, im_width, im_height, 24, 
				      3*im_width, rmask, gmask, bmask, amask);

    display_on = true;
    display_image = true;
    display_update = true;
  }


//////////////////////////////////////////////////////



MyCanvas::MyCanvas() {
  Init(190,150,3,3);
  //200, 200, //200, 250 //180, 150
};


MyCanvas::MyCanvas(int screen_widthi, int screen_heighti,
		   int screen_width_borderi, int screen_height_borderi) 
{
  Init(screen_widthi, screen_heighti, screen_width_borderi, screen_height_borderi);
};



MyCanvas::~MyCanvas() {}


void MyCanvas::Init(	  int screen_widthi, int screen_heighti,
			  int screen_width_borderi, int screen_height_borderi
			  )
{
  //  void cvInitFont( CvFont* font, CvFontFace fontFace, float hscale, float vscale, float italicScale, int thickness );
  
  screen_width = screen_widthi; 
  screen_height = screen_heighti; 
  screen_width_border = screen_width_borderi; 
  screen_height_border = screen_height_borderi;

  int w, h;

  for(w = 0; w<NUM_SCREENS_WIDE; w++)
    {
      for(h = 0; h<NUM_SCREENS_TALL; h++)
	{
	  screens[w][h].SetPosition(GSC(w,h).x, GSC(w,h).y);
	  screens[w][h].SetScreenSize(screen_width, screen_height);
	  screens[w][h].SetBorder(screen_width_border, screen_height_border);
	  screens[w][h].SetScreenNum(w,h);
	}
    }
}

void MyCanvas::WriteToDisk( int screen_index_x, int screen_index_y, int nframes)
{
  screens[screen_index_x][screen_index_y].SetWriteToDisk(nframes);
}

void MyCanvas::DisplayOutput(
			     MVisImage<MVisRGBBytePixel> & rgb_image,
			     int screen_index_x, int screen_index_y )
{	
  screens[screen_index_x][screen_index_y].CopyImageToScreen(rgb_image);
}


void MyCanvas::DisplayRawRGBBuffer(
				   unsigned char *buffer, int im_width, int im_height, 
				   int screen_index_x, int screen_index_y) 
{
  screens[screen_index_x][screen_index_y].CopyBufferToScreen(buffer, im_width, im_height);
 }

void MyCanvas::DisplayText(
			   char * str, 
			   int screen_index_x, int screen_index_y 
			   )
{
  screens[screen_index_x][screen_index_y].SetText(str);
}


CvPoint MyCanvas::GetScreenCoordinates(int screen_x_index, int screen_y_index)
{
  CvPoint screen_loc;
	
  screen_loc.x = 
    (screen_x_index * (screen_width + screen_width_border)) + screen_width_border;
  screen_loc.y =
    (screen_y_index * (screen_height + screen_height_border)) + screen_height_border;
	
  return(screen_loc);
}

CvPoint MyCanvas::GSC(int screen_x_index, int screen_y_index)
{
  return(GetScreenCoordinates(screen_x_index, screen_y_index));
}


void MyCanvas::DrawRectangle(
			     CvPoint center, CvSize size, MVisRGBBytePixel color, int width,
			     int screen_index_x, int screen_index_y)
{
  CvPoint offset;
  offset.x = GSC(screen_index_x, screen_index_y).x;
  offset.y = GSC(screen_index_x, screen_index_y).y;
  //add cvdrawing code
  //void cvRectangle( IplImage* img, CvPoint pt1, CvPoint pt2, int color, int thickness );
  //
}



void MyCanvas::DrawLine(CvPoint p1, CvPoint p2, 
			MVisRGBBytePixel color, int width,
			int screen_index_x, int screen_index_y)
{
  CvPoint offset;
  offset.x = GSC(screen_index_x, screen_index_y).x;
  offset.y = GSC(screen_index_x, screen_index_y).y;
  //add cvdrawing code	
  //void cvLine( IplImage* img, CvPoint pt1, CvPoint pt2,intcolor,int thickness=1 );
  //void cvLineAA( IplImage* img, CvPoint pt1, CvPoint pt2,intcolor,intscale=0 );
}


void MyCanvas::ClearScreen( int screen_index_x, int screen_index_y ) {
  screens[screen_index_x][screen_index_y].SetToClear();
}


void MyCanvas::DrawVectorField(
			       MVisImage<MVisFloatPixel> & dx_image,
			       MVisImage<MVisFloatPixel> & dy_image,
			       RGBIm & background_image,
			       int num_vectors, float scale,
			       MVisRGBBytePixel & color, int width, float magnify,
			       int screen_index_x, int screen_index_y)
{	
  float ratio, area_per_vector;
  float inc;
  int im_width, im_height;
	
  im_width = dx_image.Width();
  im_height = dx_image.Height();
	
  RGBIm dispimage(im_height, im_width);

  dispimage.Copy(background_image);
	

  ratio = (float) im_height/(float) im_width;
	
  area_per_vector = ((float) (im_width*im_height)/(float) num_vectors);
  inc = sqrt(area_per_vector);
	
  CvPoint p1, p2;
  float x,y;
  for(y = 0; y < im_height; y = y+inc)
    {
      for(x = 0; x < im_width; x = x+inc)
	{
	  p1.x = myround(x * magnify);
	  p1.y = myround(y * magnify);
	  p2.x = myround((x + (dx_image.Pixel(x,y) * scale)) * magnify);
	  p2.y = myround((y + (dy_image.Pixel(x,y) * scale)) * magnify);
		
	  CvPoint pt1, pt2;
	  pt1.x = p1.x; pt1.y = p1.y;
	  pt2.x = p2.x; pt2.y = p2.y;
	  	  
	  int color_int;
	  color_int = CV_RGB(color.B(), color.G(), color.R()); 

	  int thickness;
	  thickness = 1;

	  //cvLineAA
	  cvLine(dispimage.GetIPL(), pt1, pt2, color_int, thickness);
	
		 //	  DrawLine(
		 //		   p1, p2, 
		 //		   color, width,
		 //		   screen_index_x, screen_index_y);	
	}
    }
  
  DisplayOutput(
		dispimage, 
		screen_index_x, 
		screen_index_y);
}


void MyCanvas::DisplayRGBImage(
			       MVisImage<MVisRGBBytePixel> & rgb_image,
			       int screen_x_index, int screen_y_index)
{
  DisplayOutput(
		rgb_image, 
		screen_x_index, 
		screen_y_index);
}

struct CopyRGBAToRGB
{
  inline void operator() (MVisRGBABytePixel & in, MVisRGBBytePixel & out)
  {
    out.Set(in.R(), in.G(), in.B());
  };
};


void MyCanvas::DisplayRGBAImage(MVisImage<MVisRGBABytePixel> & rgb_image, 
				int screen_x_index, int screen_y_index)
{
  int width, height;
	
  width = rgb_image.Width();
  height = rgb_image.Height();
	
  MVisImage<MVisRGBBytePixel> dispimage(height, width);
  
  CopyRGBAToRGB op;
  VisMap2(op, rgb_image, dispimage);
	
  DisplayOutput(
		dispimage, 
		screen_x_index, 
		screen_y_index);
}

void MyCanvas::DisplayFloatImage(
				 MVisImage<MVisFloatPixel> & float_image,
				 int screen_x_index, int screen_y_index)
{	
  float maxpix, minpix;
  int width, height;
	
  width = float_image.Width();
  height = float_image.Height();
	
  MVisImage<MVisRGBBytePixel> dispimage(height, width);
	
  MaxMin op2; //0,0?
  VisMap1(op2, float_image);
  maxpix = op2.maxpix;
  minpix = op2.minpix;

  FloatToDisplay op(minpix, maxpix);
  VisMap2(op, float_image, dispimage);
	
  DisplayOutput(
		dispimage, 
		screen_x_index, 
		screen_y_index);
}


class MaxMinDouble
{
public:
	
  double max;
  double min;
	
  inline MaxMinDouble()
  {
    //DBL_MIN is smallest positive double
    //    max = DBL_MIN;
    //not sure about this
    max = -DBL_MAX;
    min = DBL_MAX;
  };
	
  inline void operator() (const DPix &in)
  {
    if(in > max) max = in;
    if(in < min) min = in;
  };
};


class DoubleToFloat
{
public:
	
  double max, min;
  double divisor;
	
  inline DoubleToFloat(double min_in, double max_in)
  {
    max = max_in;
    min = min_in;
    if( max-min == 0.0 ) divisor = 1.0;
    else divisor = max-min;
  };
	
  inline void operator() (const DPix &in, MVisFloatPixel &out)
  {
    out = (float) ((in - min)/divisor);
  };
};

void MyCanvas::DisplayDoubleImage(DIm & double_image,
			 int screen_x_index, int screen_y_index)
{
  int width, height;
	
  width = double_image.Width();
  height = double_image.Height();
	
  FIm dispimage(height, width);
	
  MaxMinDouble mmd_op;
  VisMap1 (mmd_op, double_image);
	
  DoubleToFloat op(mmd_op.min, mmd_op.max);
  VisMap2 ( op, double_image, dispimage);
	
  DisplayFloatImage(dispimage,
		    screen_x_index, screen_y_index);
}


class MaxMinInt
{
public:
	
  float max;
  float min;
	
  inline MaxMinInt()
  {
    max = INT_MIN;
    min = INT_MAX;
  };
	
  inline void operator() (const MVisIntPixel &in)
  {
    if(in > max) max = in;
    if(in < min) min = in;
  };
};


class IntToFloat
{
public:
	
  float max, min;
	
  inline IntToFloat(float min_in, float max_in)
  {
    max = max_in;
    min = min_in;
    if(max <= min)
      {
	max = 1.0f;
	min = 0.0f;
      }
  };
	
  inline void operator() (const MVisIntPixel &in, MVisFloatPixel &out)
  {
    out = (in - min)/(max-min);
  };
};



void MyCanvas::DisplayIntImage(
			       MVisImage<MVisIntPixel> & int_image, 
			       int screen_x_index, int screen_y_index)
{	
  int width, height;
	
  width = int_image.Width();
  height = int_image.Height();
	
  MVisImage<MVisFloatPixel> dispimage(height, width);
	
	
  MaxMinInt mmi_op;
  VisMap1 (mmi_op, int_image);
	
  IntToFloat op(mmi_op.min, mmi_op.max);
  VisMap2 ( op, int_image, dispimage);
	
  DisplayFloatImage(
		    dispimage,
		    screen_x_index, screen_y_index);
}

class MaxMinSignedChar
{
public:
	
  float max;
  float min;
	
  inline MaxMinSignedChar(signed char init_pix)
  {
    max = init_pix;
    min = init_pix;
  };
	
  inline void operator() (const MVisSignedCharPixel &in)
  {
    if(in > max) max = in;
    if(in < min) min = in;
  };
};


class SignedCharToRGB
{public:
  float mn; float mx;
  SignedCharToRGB(float min_in, float max_in):mn(min_in),mx(max_in){}
  inline void operator()(MVisSignedCharPixel & in, MVisRGBPixel & out)
  {
    char val;
    val = (char) myround((((float) in)/(mx - mn))*255.0f);
      out.Set(val,val,val);
  }
};


class MaxMinS2Int
{
public:
	
  float max;
  float min;
	
  inline MaxMinS2Int(S2Pix init_pix)
  {
    max = init_pix;
    min = init_pix;
  };
	
  inline void operator() (const S2Pix &in)
  {
    if(in > max) max = in;
    if(in < min) min = in;
  };
};


class S2IntToRGB
{public:
  float mn; float mx;
  S2IntToRGB(float min_in, float max_in):mn(min_in),mx(max_in){}
  inline void operator()(S2Pix & in, MVisRGBPixel & out)
  {
    char val;
    val = (char) myround((((float) in)/(mx - mn))*255.0f);
      out.Set(val,val,val);
  }
};

void MyCanvas::DisplaySignedCharImage(
				 MVisImage<MVisSignedCharPixel> & schar_image, 
				 int screen_x_index, int screen_y_index)
{	
  int width, height;
	
  width = schar_image.Width();
  height = schar_image.Height();
	
  MVisImage<MVisRGBPixel> dispimage(height, width);
	
  MaxMinSignedChar mm(schar_image.Pixel(0,0));
  VisMap1(mm, schar_image);

  SignedCharToRGB todisp(mm.min, mm.max);	
  VisMap2 ( todisp, schar_image, dispimage);
	
  DisplayOutput(
		    dispimage,
		    screen_x_index, screen_y_index);
}

void MyCanvas::DisplayS2IntImage(MVisImage<S2Pix> & s2int_image, 
			 int screen_x_index, int screen_y_index)
{
  int width, height;
	
  width = s2int_image.Width();
  height = s2int_image.Height();
	
  MVisImage<MVisRGBPixel> dispimage(height, width);
	
  MaxMinS2Int mm(s2int_image.Pixel(0,0));
  VisMap1(mm, s2int_image);

  S2IntToRGB todisp(mm.min, mm.max);	
  VisMap2 ( todisp, s2int_image, dispimage);
	
  DisplayOutput(
		    dispimage,
		    screen_x_index, screen_y_index);
}


void MyCanvas::DisplayUSIntImage(
				 MVisImage<MVisUSIntPixel> & usint_image, 
				 int screen_x_index, int screen_y_index)
{	
  int width, height;
	
  width = usint_image.Width();
  height = usint_image.Height();
	
  MVisImage<MVisFloatPixel> dispimage(height, width);
	
  //2^16 = 65536
  //so the max value should be 65535
  USIntToFloat op(65535);
	
  VisMap2 ( op, usint_image, dispimage);
	
  DisplayFloatImage(
		    dispimage,
		    screen_x_index, screen_y_index);
}

void MyCanvas::DisplayByteImage(
				MVisImage<MVisBytePixel> & byte_image, 
				int screen_x_index, int screen_y_index)
{	
  int width, height;
	
  width = byte_image.Width();
  height = byte_image.Height();
	
  MVisImage<MVisRGBBytePixel> dispimage(height, width);
	
  Byte_to_RGB op;
  VisMap2 (op, byte_image, dispimage);
	
  DisplayOutput(
		dispimage, 
		screen_x_index, 
		screen_y_index);
}

void MyCanvas::DisplayQuantizedAngle( MVisImage<MVisBytePixel> & angle_map,
				      MVisImage<MVisRGBBytePixel> & image_for_display)
{
  int x,y;
  unsigned char angle;
	
  for(y=0; y<angle_map.Height(); y++)
    for(x=0; x<angle_map.Width(); x++)
      {
	//assume that 255 means don't display
	angle = angle_map.Pixel(x,y) ;
	if( angle < 64 ) image_for_display.Pixel(x,y).SetR(255);
	else if( angle < 128 ) image_for_display.Pixel(x,y).SetG(255);
	else if( angle < 192 ) image_for_display.Pixel(x,y).SetB(255);
	else if( angle < 255 ) 
	  {
	    image_for_display.Pixel(x,y).SetG(255);
	    image_for_display.Pixel(x,y).SetR(255);
	  }
      }
}

void MyCanvas::DisplayQuantizedFloatAngle( MVisImage<MVisFloatPixel> & angle_map,
					   MVisImage<MVisRGBBytePixel> & image_for_display)
{
  int x,y;
  float angle;
	
  for(y=0; y<angle_map.Height(); y++)
    for(x=0; x<angle_map.Width(); x++)
      {
	//assume that 255 means don't display
	angle = angle_map.Pixel(x,y) ;
	if(angle != -10.0f)
	  {
	    if( (angle> -PI - 0.001) && (angle < -PI/2.0f) ) 
	      {
		image_for_display.Pixel(x,y).Set(255,0,0);
	      }
	    else if( angle < 0.0f ) 
	      {
		image_for_display.Pixel(x,y).Set(0,255,0);
	      }
	    else if( angle < PI/2.0f ) 
	      {
		image_for_display.Pixel(x,y).Set(0,0,255);
	      }
	    else if( angle < PI + 0.001 ) 
	      {
		image_for_display.Pixel(x,y).Set(255,255,0);
	      } else
		{
		  image_for_display.Pixel(x,y).Set(0,255,255);
		}
	  } else
	    {
	      image_for_display.Pixel(x,y).Set(255,0,255);
	    }
      }
}


void MyCanvas::ClearBorders(MVisByteImage & image, int border_size)
{			
  //set borders of the image to be displayed to 0
  //to avoid any sneaky boundary problems
	
  int i,j;
  int width, height;
  width = image.Width();
  height = image.Height();
	
  for( i=0; i < border_size; i++)
    { 
      for( j=0; j<height; j++)
	{
	  image.Pixel(i, j)  = 0;
	  image.Pixel((width - border_size) + i, j)  = 0;
	}
    }
	
  for( j=0; j < border_size; j++)
    { 
      for( i=0; i<width ; i++)
	{
	  image.Pixel(i, j)  = 0;
	  image.Pixel(i, (height - border_size) + j)  = 0;
	}
    }	
}


void MyCanvas::ClearRGBBorders(MVisImage<MVisRGBBytePixel> & image, int border_size)
{			
  //set borders of the image to be displayed to 0
  //to avoid any sneaky boundary problems
	
  int i,j;
  int width, height;
  width = image.Width();
  height = image.Height();
	
  for( i=0; i < border_size; i++)
    { 
      for( j=0; j<height; j++)
	{
	  image.Pixel(i, j).Set(0,0,0);
	  image.Pixel((width - border_size) + i, j).Set(0,0,0);
	}
    }
	
  for( j=0; j < border_size; j++)
    { 
      for( i=0; i<width ; i++)
	{
	  image.Pixel(i, j).Set(0,0,0);
	  image.Pixel(i, (height - border_size) + j).Set(0,0,0);
	}
    }	
}



#endif

