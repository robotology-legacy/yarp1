/* paulfitz Wed May 30 15:36:24 EDT 2001 */

#ifndef YARPFramegrabber_INC
#define YARPFramegrabber_INC

#include "YARPImage.h"
#include "YARPString.h"

class YARPGenericFramegrabber
{
protected:
  int interlaced;
  int height, width;
  double tlx, tly, brx, bry;
  int format;
  double aspect;
  int active;
public:
  enum
  {
    FORMAT_NULL,
    FORMAT_MONO,
    FORMAT_RGB,
  };

  YARPGenericFramegrabber();
  virtual ~YARPGenericFramegrabber();

  virtual string GetFrameIdentifier() { return "unknown"; }

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
};



#endif


