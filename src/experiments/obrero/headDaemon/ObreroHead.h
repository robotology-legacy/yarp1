
#ifndef __OBREROHEADHH__
#define __OBREROHEADHH__

#include <yarp/YARPErrorCodes.h>

class ObreroHead {
private:
  void *system;
  void check();
public:
  ObreroHead();
  virtual ~ObreroHead();

  int nj()
    { return _nj;}

  int setPositions(const double *delta)
    {
      set((int)delta[0], (int)delta[1]);
      return YARP_OK;
    }

  int setPosition(int j, double v)
    {
      setSingleJoint(j, (int)v);
      return YARP_OK;
    }

  int setPositionRelative(int j, double delta);
  int setPositionsRelative(const double *delta);
  int getPosition(int j, double *v)
    {
      int iTmp;
      getSingleJoint(j, &iTmp);
      *v=(double) (iTmp);
      return YARP_OK;
    }

  int getPositions(double *p)
    {
      int tmpx;
      int tmpy;
      
      get(tmpx, tmpy);
      p[0]=(double) tmpx;
      p[1]=(double) tmpy;
      
      return YARP_OK;
    }

  int initialize()
    { 
      init(); 
      return YARP_OK;
    }

private:
  void init();
  void set(int x, int y);
  void setSingleJoint(int j, int v);
  void getSingleJoint(int j, int *v);
  void get(int& x, int& y);

  int _nj;
};

#endif

