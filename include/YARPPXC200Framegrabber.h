/* paulfitz Wed May 30 15:36:24 EDT 2001 */

#ifndef YARPPXC200Framegrabber_INC
#define YARPPXC200Framegrabber_INC

#include "YARPFramegrabber.h"
#include "YARPString.h"

class YARPPXC200Framegrabber : public YARPGenericFramegrabber
{
protected:
  void *system_resource;
public:

  YARPPXC200Framegrabber();
  virtual ~YARPPXC200Framegrabber();

  virtual string GetFrameIdentifier();

  // this might be a slot number or file name
  virtual void Attach(const char *resource_identifier);

  virtual YARPGenericImage *AcquireFrame(int wait = 1);
  virtual void RelinquishFrame();

  virtual void SetInterlaced(int n_interlaced);
  virtual void SetSubImage(double n_tlx, double n_tly, 
			   double n_brx, double n_bry);
  virtual void SetBandwidth(int n_width, int n_height, 
			    int n_preserve_aspect_ratio = 0);
  virtual void SetFormat(int n_format);
  virtual void SetAspect(double n_aspect);
};

#endif
