
#ifndef MOTIONGROUP_INC
#define MOTIONGROUP_INC

class MotionGroup {
public:
  int x, y;
  double v;

  MotionGroup() {
    v = -1;
  }

  void Apply(YARPImageOf<YarpPixelRGB>& src,
	     YARPImageOf<YarpPixelRGB>& dest);
  
  int GetX() { return x; }
  int GetY() { return y; }

  double GetStrength() { return v; }

  int IsValid() { return v>-0.5; }
};


#endif
