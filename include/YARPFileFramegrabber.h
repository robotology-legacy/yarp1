/* paulfitz Wed May 30 15:36:24 EDT 2001 */

#ifndef YARPFileFramegrabber_INC
#define YARPFileFramegrabber_INC

#include "YARPFramegrabber.h"
#include "YARPString.h"

class YARPFileFramegrabber : public YARPGenericFramegrabber
{
protected:
  string id;
  void *system_resource;
  int repeat;
  int bounce;
public:

  YARPFileFramegrabber();
  virtual ~YARPFileFramegrabber();

  virtual string GetFrameIdentifier();

  // this might be a slot number or file name
  virtual void Attach(const char *resource_identifier);

  virtual YARPGenericImage *AcquireFrame(int wait = 1);
  virtual void RelinquishFrame();

  virtual void SetInterlaced(int n_interlaced);
  virtual void SetSubImage(double n_tlx, double n_tly, 
			   double n_brx, double n_bry);
  virtual void SetBandwidth(int n_height, int n_width, 
			    int n_preserve_aspect_ratio);
  virtual void SetFormat(int n_format);
  virtual void SetAspect(double n_aspect);

  void SetRepeatMode(int n_repeat=1, int n_bounce=0)
    {
      repeat = n_repeat;
      bounce = n_bounce;
    }
};

#endif
