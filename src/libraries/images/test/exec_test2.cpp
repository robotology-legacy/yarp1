#include <iostream.h>

#include "YARPImage.h"
#include "YARPImageFile.h"
#include "CRLFaceFinderTool.h"

int main()
{
  YARPImageOf<YarpPixelRGB> img;
  YARPImageFile::Read("test1.ppm",img);
  CRLFaceFinderTool face_finder;
  YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(img);
  face_finder.Apply(mono);
  cout << "Found " << face_finder.size() << " face(s)" << endl;
  face_finder.Decorate(img,YarpPixelRGB(0,255,0));
  YARPImageFile::Write("test1_out.ppm",img);
  return 0;
}

