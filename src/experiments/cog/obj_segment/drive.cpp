#include <stdio.h>

#include "YARPImage.h"
#include "YARPImagePortContent.h"
#include "YARPPort.h"
#include "YARPTime.h"

#include "drive.h"

YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;

void main(int argc, char *argv[])
{
  char buf[256];
  sprintf(buf,"/%s/i:img",GetAppName());
  in_img.Register(buf);
  sprintf(buf,"/%s/o:img",GetAppName());
  out_img.Register(buf);
  
  while (1)
    {
      double now = YARPTime::GetTimeAsSeconds();
      static double last_round = now;
      static double last_reset = 0;
      static double diff_total = 0;
      static int diff_count = 0;
      diff_total += now-last_round;
      diff_count++;
      if (now - last_reset>5)
	{
	  //printf("Time between frames in ms is %g\n", 1000*diff_total/diff_count);
	  diff_count = 0;
	  diff_total = 0;
	  last_reset = now;
	}
      last_round = now;
      
      in_img.Read();
      
      YARPImageOf<YarpPixelBGR> src, dest;
      src.Refer(in_img.Content());
      //src.Resize(128,128);
      out_img.Content().SetID(YARP_PIXEL_BGR);
      SatisfySize(src,out_img.Content());
      dest.Refer(out_img.Content());
      ProcessImage(src,dest);
      out_img.Write(1);
      out_img.FinishSend();
    }
}
