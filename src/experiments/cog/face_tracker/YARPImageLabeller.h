#ifndef YARPIMAGELABELLER_INC
#define YARPIMAGELABELLER_INC

#include "YARPImage.h"

class YARPImageLabeller
{
public:
    YARPImageOf<int> xstack;
    YARPImageOf<int> ystack;
    int bestId;
  
    int Apply(YARPImageOf<int>& src, YARPImageOf<int>& dest);
};


#endif


