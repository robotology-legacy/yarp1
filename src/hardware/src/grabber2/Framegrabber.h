#ifndef __FRAMEGRABBER_INC
#define __FRAMEGRABBER_INC

#include "portability.h"

//#include "Image.h"
#include "YARPImage.h"

//#include "bool.h"

#ifndef true
#define bool int
#define true 1
#define false 0
#endif

class Frame : public YARPGenericImage
{
public:
  int id;
  Frame() {}
private:
};


class Framegrabber
{
protected:

  int height, width;
  int format;
  int depth;
  double top, left, bottom, right, dleft, dtop;
  int active;
  bool interlaced;
  bool continuous;

public:
  enum ErrorCode
  {
    OK = 0,
    BAD = -1,
    NOT_IMPLEMENTED = -2,
  };

  enum PixelFormat
  {
    PIXEL_NULL = YARP_PIXEL_INVALID,
    PIXEL_Y8 = YARP_PIXEL_MONO,
    PIXEL_MONO = PIXEL_Y8,
    PIXEL_RGB = YARP_PIXEL_RGB,
    PIXEL_BGR = YARP_PIXEL_BGR,
  };

  Framegrabber()
    {
      active = 0;
      depth = 0;
      left = 0;
      top = 0;
      bottom = 1;
      right = 1;
    }

  virtual int GetMaxHeight() { return 0; }
  virtual int GetMaxWidth() { return 0; }

  virtual ErrorCode Init()   { return NOT_IMPLEMENTED; }
  virtual ErrorCode AddSignalHandler()   { return NOT_IMPLEMENTED; }
  virtual ErrorCode Deinit() { return NOT_IMPLEMENTED; }

  virtual ErrorCode SetSize(int n_height, int n_width)
    {
      height = n_height;
      width = n_width;
      if (active)
	{
	  return ChangeSize(height,width);
	}
      return OK;
    }

  virtual ErrorCode SetFormat(PixelFormat n_format)
    {
      format = n_format;
      if (active)
	{
	  return ChangeFormat(n_format);
	}
      return OK;
    }

  virtual ErrorCode ChangeSize(int n_height, int n_width)
    { return NOT_IMPLEMENTED; }

  virtual ErrorCode ChangeFormat(PixelFormat n_format)
    { return NOT_IMPLEMENTED; }

  virtual ErrorCode SetSubImage(double n_top, double n_left, 
				double n_bottom, double n_right,
				double n_dleft, double n_dtop)
    {
      top = n_top;
      left = n_left;
      bottom = n_bottom;
      right = n_right;
      dleft = n_dleft;
      dtop = n_dtop;
      if (active)
	{
	  return NOT_IMPLEMENTED;
	}
      return OK;
    }

  virtual ErrorCode SetInterlaced(bool separate_interlaced_frames)
    {
      return NOT_IMPLEMENTED;
    }

  virtual ErrorCode SetContinuous(bool continuous_grab_on)
    {
      return NOT_IMPLEMENTED;
    }
  
  virtual ErrorCode CopyFrameToImage(YARPGenericImage& image)
    { return NOT_IMPLEMENTED; }
  
  //virtual const Frame& AcquireFrame() = 0; 
  virtual ErrorCode AcquireFrame(Frame& frame, int wait, int fnum)
    { return NOT_IMPLEMENTED; }
  // Must return the old frame in order to get a new frame.

  virtual ErrorCode RelinquishFrame(Frame& frame, int fnum) 
    { return NOT_IMPLEMENTED; }
  // Used to return a frame -- should be done in a timely manner.
  

  // for pxc framegrabber, need to give main process to hardware --
  // fork threads for yourself, then call this.  It never returns.
  virtual void Main() {}
private:
};


class FramegrabberThread;

class PXCFramegrabber : public Framegrabber
{
public:
  FramegrabberThread *server1;
  FramegrabberThread *server2;

  PXCFramegrabber();

  virtual ~PXCFramegrabber();

  virtual int GetMaxHeight();
  virtual int GetMaxWidth();

  virtual ErrorCode Init();
  virtual ErrorCode AddSignalHandler();
  virtual ErrorCode Deinit();

  virtual ErrorCode ChangeSize(int n_height, int n_width);

  virtual ErrorCode CopyFrameToImage(YARPGenericImage& image, int fnum);
  
  //  virtual const Frame& AcquireFrame(); 
  virtual ErrorCode AcquireFrame(Frame& frame, bool wait, int fnum);

  virtual ErrorCode RelinquishFrame(Frame& frame, int fnum); 

  virtual void Main();
  
  void EmergencyStop();

};


/*

User of the framegrabber 
can only read the image buffer

should only read the image buffer when the framegrabber is not using it

a semaphore must exist that is associated with the buffer

it can either poll to see if the resource is available

paul says don't poll!!

or it can wait for a signal that the resource is available
forget all this, just wait!!! it won't be long - honest", so says paul

from the user's perspective there is only one buffer that is quickly available


	//framegrabber has control of the user image buffer. 
	//Only it can write in it,
	//only it can allocate it
	//Only it can deallocate it
	//Only it can give permission to read the user image buffer
	//it's not all powerful! - it can't write to the user buffer when the user has control!!!
	
	//this control is implemented with a semaphore
	//the framegrabber has control of this semaphore

	// size{height,width}, pixel type, continuous vs the other, subimage to image, interlaced or not
	// test pattern
*/

#endif
