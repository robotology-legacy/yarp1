#ifndef YARPIMAGELABEL_INC
#define YARPIMAGELABEL_INC

#include "YARPImage.h"

class YARPImageLabel
{
private:
  int alloc_len;
  int *xstack;
  int *ystack;
public:
  YARPImageLabel()
    {
      xstack = ystack = NULL;
      alloc_len = -1;
    }

  virtual ~YARPImageLabel()
    {
      Clean();
    }

  void Clean()
    {
      if (xstack!=NULL) delete[] xstack;
      if (ystack!=NULL) delete[] ystack;
      xstack = ystack = NULL;
    }

  int Apply(YARPImageOf<YarpPixelMono>& src, 
	    YARPImageOf<YarpPixelMono>& dest);


  int ApplySimilarity(YARPImageOf<YarpPixelInt>& src, 
		      YARPImageOf<YarpPixelInt>& dest);

  // answers
  int bestId;
};

#endif
