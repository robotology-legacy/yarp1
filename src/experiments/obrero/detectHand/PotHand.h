
#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif

class PotHand {
public:
  int rset, rox, roy, row, roh;

  PotHand() { rset = 0; }

  void Apply(YARPImageOf<YarpPixelRGB>& src, YARPImageOf<YarpPixelRGB>& dest,
	     int cx=-1, int cw=-1, int cw=-1, int ch=-1);
  void ApplyFreq(YARPImageOf<YarpPixelRGB>& src, 
		 YARPImageOf<YarpPixelRGB>& dest);
};

