#include <yarp/YARPPort.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageDraw.h>

int main() {
  YARPImageOf<YarpPixelRGB> prev;
  YARPInputPortOf<YARPGenericImage> in_img;
  in_img.Register("/motion");
  
  while (1) {
    in_img.Read();
    static long int ct = 0;
    static long int idx = 0;
    ct++;
    YARPImageOf<YarpPixelRGB> curr;
    curr.CastCopy(in_img.Content());
    if (prev.GetWidth()<=0) {
      prev = curr;
    }
    int pct = 0;
    double total = 0;
    IMGFOR(curr,x,y) {
      YarpPixelRGB& pix1 = curr(x,y);
      YarpPixelRGB& pix2 = prev(x,y);
      float v = fabs(pix1.r-pix2.r);
      total += v;
      pct++;
    }
    total /= pct;

    if (total>=10) {
      printf("Change! %d  %g\n", idx, total);
      char buf[256];
      sprintf(buf,"/scratch/log/%06d.ppm",idx);
      YARPImageFile::Write(buf,in_img.Content());
      idx = (idx+1)%10000;
      ct = 0;
    }
    prev = curr;
  }

  return 0;
}

