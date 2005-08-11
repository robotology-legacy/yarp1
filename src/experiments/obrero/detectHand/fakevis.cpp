#include "fakevis.h"
#include <stdlib.h>


void CopyToImage(MVisRGBImage &image_out, unsigned char * buf, int buf_w, int buf_h)
{
  image_out.Create(buf_h, buf_w);
  
  int w,h;
  for(h = 0; h < buf_h; h++)
    {
      for(w = 0; w < buf_w; w++)
	{
	  MVisRGBPixel & pix = image_out.Pixel(w, h);
	  int t;
	  t = ((h*buf_w)+w)*3;
	  pix.Set(
		  buf[t],
		  buf[t+1],
		  buf[t+2]
		  );
	}
    }
}


void CopyToBuffer(MVisRGBImage &image_in, unsigned char * & buf, int & buf_w, int &buf_h)
{
  buf_w = image_in.W();
  buf_h = image_in.H();

  if(buf!=NULL) { delete [] buf; buf = NULL; }
  buf = new unsigned char[buf_h * buf_w * 3];
  
  int w,h;
  for(h = 0; h < buf_h; h++)
    {
      for(w = 0; w < buf_w; w++)
	{
	  MVisRGBPixel & pix = image_in.Pixel(w, h);
	  int t;
	  t = ((h*buf_w)+w)*3;
	  buf[t]=pix.R();
	  buf[t+1]=pix.G();
	  buf[t+2]=pix.B();
	}
    }
}




int GetChannels(const std::type_info &t_info)
{	
  if(t_info == typeid(char)) return(1);
  if(t_info == typeid(unsigned char)) return(1);
  if(t_info == typeid(signed char)) return(1);
  if(t_info == typeid(signed short int)) return(1);
  if(t_info == typeid(unsigned short int)) return(1);
  if(t_info == typeid(int)) return(1);
  if(t_info == typeid(unsigned int)) return(1);
  if(t_info == typeid(signed int)) return(1);
  if(t_info == typeid(long int)) return(1);
  if(t_info == typeid(unsigned long int)) return(1);
  if(t_info == typeid(signed long int)) return(1);
  if(t_info == typeid(float)) return(1);
  if(t_info == typeid(double)) return(1);

  if(t_info == typeid(MVisRGBABytePixel)) return(4);
  if(t_info == typeid(MVisRGBBytePixel)) return(3);
	
  return(-1);
}


//	return IPL_StsOk == iplGetErrStatus();

IplImage * MVisImageBase::MyCreateImageHeader(CSize &size, int depth, int channels) 
{
	
  //if number of channels = 4 we might
  //want to use the alpha channel
	
  char color_model[8];
  char color_order[8];
  if(channels == 4) sprintf(color_model, "RGBA");
  if(channels == 3) sprintf(color_model, "RGB");
  if(channels == 1) sprintf(color_model, "GRAY");
	
  if(channels == 4) sprintf(color_order, "RGBA");
  if(channels == 3) sprintf(color_order, "RGB");
  if(channels == 1) sprintf(color_order, "G");
	
  int alpha_channel;
  if(channels == 4) alpha_channel = 4;
  else alpha_channel = 0;
	
  IplImage * temp_image;
	
  temp_image = iplCreateImageHeader(
				    channels, // number of channels
				    alpha_channel, // the number of the alpha channel, 0 means no alpha channel
				    depth, // data of byte type
				    color_model, // color model
				    color_order, // color order
				    IPL_DATA_ORDER_PIXEL,	// channel arrangement
				    IPL_ORIGIN_TL,			// top left orientation
				    IPL_ALIGN_QWORD,		// 8 byte align, align on quad word boundaries, 64 bits
				    //IPL_ALIGN_DWORD,		// 4 byte align, align to double word boundaries, 32 bits
				    size.cx,	// image width
				    size.cy,	// image height
				    NULL,	// no ROI
				    NULL,	// no mask ROI
				    NULL,	// no image ID
				    NULL);	// not tiled
	
  if( temp_image == NULL ) 
    {
      printf("ERROR: unable to allocate image with iplCreateImageHeader\n");
      exit(1);
    }
	
  return(temp_image);
}

void MVisImageBase::MyCreateAndSetImageData(IplImage *image) 
{
	
  if( ((image->depth) == 32) && ((image->nChannels) == 1) )
    {
      //this is a float image
      iplAllocateImageFP(
			 image, 
			 1,	//int doFill, if 0 then don't fill pixels with fillValue
			 0.0f //float fillValue,	value used to fill the pixels if doFill != 0
			 );
    } else
      {
	iplAllocateImage(
			 image, 
			 1, //int doFill
			 0  //int fillValue
			 );
      }
}

void MVisImageBase::MyDeleteIPLBuffer(IplImage *image)
{
  if((image->imageData) != NULL) iplDeallocateImage(image);
}

void MVisImageBase::MyReleaseImageHeader(IplImage *image)
{
  //iplDeallocate( image, IPL_IMAGE_DATA );
  //iplDeallocate( image, IPL_IMAGE_HEADER );
  iplDeallocate( image, IPL_IMAGE_ALL );
}



void MVisImageBase::Create(int nh, int nw, int pixel_size_in_bytes_i, int channels)
{
	
  if(channels == -1)
    {
      ipl_compatible = false;
      channels = 1;
		
      assert(channels >= 1);
      assert(nh>0 && nw>0 && pixel_size_in_bytes_i>0);
      Destroy();
      h = nh;
      w = nw;
      x0 = 0;  y0 = 0;
      x1 = w-1;  y1 = h-1;
      pixel_size_in_bytes = pixel_size_in_bytes_i;
		
      int pixel_bit_depth;
      CSize ipl_size;
      ipl_size.cy = h;
      ipl_size.cx = w;
      pixel_bit_depth = (pixel_size_in_bytes / channels) * 8;
		
      ipl_image = NULL;
		
      buffer = new char[h * w * pixel_size_in_bytes];
      int i;
      for(i=0; i < h*w*pixel_size_in_bytes; i++)
	buffer[i] = 0;
      
      row_alignment_offset_in_bytes = 0;
      row_step_in_bytes = w * pixel_size_in_bytes;
      actual_buffer_size_in_bytes = h * w * pixel_size_in_bytes;
		
      assert(buffer!=NULL);
      SetRowIndex(h, buffer, row_step_in_bytes);
		
      SetBufferType(NON_IPL_BUFFER);
      SetBufferOwnership(INTERNAL_OWNERSHIP);
    } else
      {
		
	assert(channels >= 1);
	assert(nh>0 && nw>0 && pixel_size_in_bytes_i>0);
	Destroy();
	h = nh;
	w = nw;
	x0 = 0;  y0 = 0;
	x1 = w-1;  y1 = h-1;
	pixel_size_in_bytes = pixel_size_in_bytes_i;
		
	int pixel_bit_depth;
	CSize ipl_size;
	ipl_size.cy = h;
	ipl_size.cx = w;
	pixel_bit_depth = (pixel_size_in_bytes / channels) * 8;
		
	ipl_image = MyCreateImageHeader(ipl_size, pixel_bit_depth, channels);
	MyCreateAndSetImageData(ipl_image);
		
	//I'm not completely sure about the difference between the 
	//
	//			imageData   and  imageDataOrigin
	//
	//my best guess: 
	//
	//maybe the memory allocation routine they use
	//forces them to get some non-aligned preceding bytes,
	//for example:
	//	 most memory allocators probably give a pointer that
	//	 will sit on any 32-bit boundary, yet we want a 64-bit
	//	 boundary, the second pointer allows us to deal with this
		
	buffer = ipl_image->imageData;
	row_alignment_offset_in_bytes = ipl_image->widthStep - w;
	row_step_in_bytes = ipl_image->widthStep;
	actual_buffer_size_in_bytes = ipl_image->imageSize;
		
	assert(buffer!=NULL);
	SetRowIndex(h, buffer, row_step_in_bytes);
		
	SetBufferType(IPL_BUFFER);
	SetBufferOwnership(INTERNAL_OWNERSHIP);
      }
}


void MVisImageBase::Refer(char *nbuffer, int nh, int nw, int pixel_size_in_bytes_i, int channels)
{
  if(channels <= 0) printf("ERROR: unknown pixel type\n");
  assert(channels >= 1);
  assert(nh>0 && nw>0 && pixel_size_in_bytes_i>0 && nbuffer!=NULL);
  Destroy();
  h = nh;
  w = nw;
  x0 = 0;  y0 = 0;
  x1 = w-1;  y1 = h-1;
  pixel_size_in_bytes = pixel_size_in_bytes_i;
  buffer = nbuffer;
	
  int depth;
  CSize ipl_size;
  ipl_size.cy = h;
  ipl_size.cx = w;
  depth = (pixel_size_in_bytes/channels) * 8;
	
  ipl_image = MyCreateImageHeader(ipl_size, depth, channels);
	
  //convert the header to be external
  ipl_image->imageData = buffer;
  row_alignment_offset_in_bytes = 0;
  row_step_in_bytes = w;
  actual_buffer_size_in_bytes = h*w*pixel_size_in_bytes;
	
  //I'm not sure if this will work
  //		IPL_ALIGN_DWORD indicates 32-bit alignment, 4 byte
  //I should experiment with actually creating the image header
  //with the IPL_ALIGN_DWORD setting, if nothing else
  //to confirm how it works
  ipl_image->align = IPL_ALIGN_DWORD;
  ipl_image->imageSize = actual_buffer_size_in_bytes;
  ipl_image->widthStep = row_step_in_bytes;
	
  SetRowIndex(h,buffer,row_step_in_bytes);
	
  //by default assume that the buffer 
  //		has external ownership
  //		is not an ipl type of buffer
	
  SetBufferType(NON_IPL_BUFFER);
  SetBufferOwnership(EXTERNAL_OWNERSHIP);
}

void MVisImageBase::Destroy()
{
  if( ownership == INTERNAL_OWNERSHIP )
    {
      switch(buffer_type)
	{
	case IPL_BUFFER:
	  if(ipl_image != NULL)
	    {
	      //	MyDeleteIPLBuffer(ipl_image);
	      MyReleaseImageHeader(ipl_image);
	    }
	  ipl_image = NULL;
	  break;
	case NON_IPL_BUFFER:
	  if(ipl_compatible)
	    {
	      if( buffer != NULL )
		{
		  ipl_image = NULL;
		  delete [] buffer;
		  MyReleaseImageHeader(ipl_image);
		}
	    } else
	      {
		if( buffer != NULL )
		  {
		    delete [] buffer;
		  }
	      }
	  break;
	}
    }
	
  buffer = NULL;
  ipl_image = NULL;
  h = w = pixel_size_in_bytes = 0;
  ClearRowIndex();
}

void MVisImageBase::SetRect(CRect r)
{
  int lx0 = r.r1.cx;
  int ly0 = r.r1.cy;
  int lx1 = r.r2.cx;
  int ly1 = r.r2.cy;
  int tmp;
  if (lx1<lx0)
    {
      tmp = lx0;
      lx0 = lx1;
      lx1 = tmp;
    }
  if (ly1<ly0)
    {
      tmp = ly0;
      ly0 = ly1;
      ly1 = tmp;
    }
  x0 = lx0;
  y0 = ly0;
  x1 = lx1;
  y1 = ly1;
  assert(x0>=0 && x0<w);
  assert(x1>=0 && x1<w);
  assert(y0>=0 && y0<h);
  assert(y1>=0 && y1<h);
	
  if(ipl_compatible)
    {
      // 0 means all channels
      const int channel_of_interest = 0;
		
      //I need to test to make sure these
      //are the correct settings
      iplSetROI(
		ipl_image->roi, channel_of_interest,
		x0, y0, 
		x1-x0, y1-y0);
    }
}
