/*
paulfitz Thu May 31 11:59:04 EDT 2001
*/
#ifndef YARPDisplay_INC
#define YARPDisplay_INC

#include "YARPImage.h"

class YARPDisplay
{
protected:
  void *system_resource;
public:
  YARPDisplay();
  virtual ~YARPDisplay();

  void Show(const YARPImageOf<PixelMono>& src);
  void Show(const YARPImageOf<PixelRGB>& src);
};

#endif
