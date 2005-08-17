
#ifndef __OBREROHEADHH__
#define __OBREROHEADHH__

#include <yarp/YARPErrorCodes.h>
#include <yarp/YARPSemaphore.h>

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
      _mutex.Wait();
      _set((int)delta[0], (int)delta[1]);
      _mutex.Post();
      return YARP_OK;
    }

  int setPosition(int j, double v)
    {
      _mutex.Wait();
      _setSingleJoint(j, (int)v);
      _mutex.Post();
      return YARP_OK;
    }

  int setPositionRelative(int j, double delta);
  int setPositionsRelative(const double *delta);
  int getPosition(int j, double *v)
    {
      _mutex.Wait();
      int iTmp;
      _getSingleJoint(j, &iTmp);
      *v=(double) (iTmp);
      _mutex.Post();
      return YARP_OK;
    }

  int getPositions(double *p)
    {
      int tmpx;
      int tmpy;
      
      _mutex.Wait();
      _get(tmpx, tmpy);
      p[0]=(double) tmpx;
      p[1]=(double) tmpy;

      _mutex.Post();
      return YARP_OK;
    }

  int initialize()
    { 
      _mutex.Wait();
      _init(); 
      _mutex.Post();
      return YARP_OK;
    }

  int getRefPositions(double *v);

private:
  void _init();
  void _set(int x, int y);
  void _setSingleJoint(int j, int v);
  void _getSingleJoint(int j, int *v);
  void _get(int& x, int& y);

  YARPSemaphore _mutex;

  int _nj;
};

#endif

