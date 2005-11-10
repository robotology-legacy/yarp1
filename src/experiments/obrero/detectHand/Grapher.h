#include <yarp/YARPConfig.h>
#include <yarp/YARPImage.h>

class Grapher 
{
 public:
  Grapher(int sizeX, int sizeY, const char *cap=0);
  ~Grapher();
  void Apply(YARPImageOf<YarpPixelRGB>& dest);
 private:
  void *helper;
};
