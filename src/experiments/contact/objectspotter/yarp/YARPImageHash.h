
#ifndef YARP_IMAGE_HASH_INC
#define YARP_IMAGE_HASH_INC

#include <yarp/YARPImage.h>
#include <yarp/YARPNetworkTypes.h>

class YARPImageHash
{
public:
  static NetInt32 GetHash(YARPImageOf<YarpPixelBGR>& src);
};

#endif

