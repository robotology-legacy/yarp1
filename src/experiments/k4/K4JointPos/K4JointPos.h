#ifndef _K4JOINTPOS_H_
#define _K4JOINTPOS_H_

#define CONVERT_FILENAME "/ai/ai/mb/cdp/src/YARP/conf/K4convert.ini"
#include "conf/K4_data.h"

/* this class is used to encode motors' encoder values.
this is currently used as a structure passed by YAPOC's encoder thread to 
clients.
it contains function that clients can use to convert encoder values
(ticks) to other units (i.e. radians) */

class K4JointPos
{
public:
  float positions[MAXVIRTUALAXES];      //contains encoder values
  double m_etoangles[MAXVIRTUALAXES];   //conversion parameters
                                        //these values are read from a file
                                        //during construction of K4JointPos
  
  K4JointPos();
  ~K4JointPos();

  //utility function
  void ReadConvertParams(const char *filename);
  void SetPosition(int axis, float newpos); //set one position
  void SetPosition(float *newpos); //set all positions
  
  //conversion function
  void ConvertToRadians();
};

#endif 

