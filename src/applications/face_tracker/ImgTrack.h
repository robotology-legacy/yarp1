#ifndef ImgTrackTool_INC
#define ImgTrackTool_INC

#include "YARPImage.h"

#define IMG_H 128
#define IMG_W 128
typedef unsigned char ImgInt[128][128];
typedef unsigned char ImgInt3[128][128][3];

class ImgTrackTool
{
private:
  int tx;
  int ty;
  YARPImageOf<YarpPixelMono> prevImg;
  YARPImageOf<YarpPixelBGR> prevImg3;
  int first;
  int delta;
  int blockSize;
  int blockXSize;
  int blockYSize;
  int windowSize;
  int windowXSize;
  int windowYSize;
  int windowXOffset;
  int windowYOffset;
  int normalize;
  float quality;
public:
  
  ImgTrackTool()
    {
      ResetXY();
      quality = 0;
      first = 1;
      blockSize = 9;
      blockXSize = blockSize;
      blockYSize = blockSize;
      windowSize = 5;
      windowXSize = windowSize;
      windowYSize = windowSize;
      windowXOffset = 0;
      windowYOffset = 0;
      normalize = 0;
    }
  
  void SetBlockSize(int size)
    {
      blockSize = size;
      blockXSize = blockSize;
      blockYSize = blockSize;
    }
  
  void SetBlockSize(int dx, int dy)
    {
      blockXSize = dx;
      blockYSize = dy;
    }

  // search will be from -size to +size pixels from the search center
  void SetSearchWindowSize(int size)
    {
      windowSize = windowXSize = windowYSize = size;
    }
  void SetSearchWindowOffset(int dx, int dy)
    {
      windowXOffset = dx;
      windowYOffset = dy;
    }

  void SetSearchWindowSize(int xsize, int ysize)
    {
      windowXSize = xsize;
      windowYSize = ysize;
      windowSize = xsize;
    }
  
  void SetNormalize(int flag=1)
    {
      normalize = flag;
    }
  
  int GetX()
    { return tx; }
  
  int GetY()
    { return ty; }
  
  float GetQuality()
    { return quality; }
  
  void SetXY(int x, int y)
    {
      tx = x; ty = y;
      delta = 1;
    }
  
  void Restart()
    {
      first = 1;
    }
  
  void ResetXY()
    { 
      SetXY(IMG_W/2,IMG_H/2);
    }
  
  void Apply(ImgInt frame1, ImgInt frame2, int x, int y);
  void Apply(YARPImageOf<YarpPixelMono>& src);
  void Apply(YARPImageOf<YarpPixelMono>& src, 
	     YARPImageOf<YarpPixelMono>& dest, int x, int y);

  void Apply(ImgInt3 frame1, ImgInt3 frame2, int x, int y);
  void Apply(YARPImageOf<YarpPixelBGR>& src);
};

#endif









