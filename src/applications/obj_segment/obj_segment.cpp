#include <iostream>

#include "drive.h"

#include "YARPVisualContact.h"
#include "YARPTime.h"
#include "YARPBottle.h"

#define JUST_MOTION

YARPOutputPortOf<YARPBottle> out_bot;

const char *GetAppName()
{
  return "obj_segment";
}

void ProcessImage(YARPImageOf<YarpPixelBGR>& src,
		  YARPImageOf<YarpPixelBGR>& dest)
{
  static int first = 1;
  if (first)
    {
      out_bot.Register("/obj_segment/o:bot");
      first = 0;
    }
  static double tap_time = -1000.0;
  static YARPVisualContact contact;
  static YARPImageOf<YarpPixelBGR> fix_image;
  static YARPImageOf<YarpPixelFloat> dest_motion;
  static int since_tap = 0;
  int tap = 0;
  float motion = 0;

#ifndef JUST_MOTION
  tap = contact.Apply(src,dest);
#else
  motion = contact.GetMotion(src,dest,dest_motion);
  float thet = 5;
  if (motion>thet)
    {
      //printf("motion is %g\n", motion);
      YARPBottleIterator it(out_bot.Content());
      it.Reset();
      it.WriteVocab(YBC_IMAGE_MOTION);
      out_bot.Write();
    }
#endif

  double now = YARPTime::GetTimeAsSeconds();
  if (tap)
    {
      contact.Reset();
      fix_image.PeerCopy(dest);
      tap_time = now;
    }
  if (now-tap_time<3)
    {
      dest.PeerCopy(fix_image);
      contact.Reset();
    }
}
