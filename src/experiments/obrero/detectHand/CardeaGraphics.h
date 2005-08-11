
#ifndef CARDEA_GRAPHICS_INC
#define CARDEA_GRAPHICS_INC

#include <yarp/YARPImage.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>

class CardeaGraphics : public YARPThread
{
public:
  bool use_sdl;
  bool backgrounded;
  int text_mode;
  int estop;
  int image_width, image_height;
  void *pcanvas;
  void *screen;
  YARPSemaphore show_mutex;
  YARPSemaphore available;
  YARPSemaphore stopped;
  int running;

  YARPImageOf<YarpPixelBGR> panes[2];

  unsigned char * out1_image_buffer;
  unsigned char * out2_image_buffer;

  CardeaGraphics() :
    use_sdl(false),
    show_mutex(1),
    stopped(0),
    available(0)
    {
      screen = NULL;
      pcanvas = NULL;
      out1_image_buffer = out2_image_buffer = NULL;
      image_width = image_height = 0;
      backgrounded = false;
      running = 0;
      estop = 0;
    }

  ~CardeaGraphics();

  void Begin()
    {
      backgrounded = true;
      YARPThread::Begin();
    }

  void Init(bool n_text_mode, int w, int h);

  void Deinit();

  void ShowDirect(YARPImageOf<YarpPixelBGR>& dest,
		  YARPImageOf<YarpPixelBGR>& dest2);

  void Show(YARPImageOf<YarpPixelBGR>& dest,
	    YARPImageOf<YarpPixelBGR>& dest2);

  virtual void Body();
};


#endif

