#include "CogGaze.h"

#include <stdio.h>

void main()
{
  CogGaze gaze;
  JointPos jp;
  gaze.Apply(jp);
  
  float x1 = 100, y1 = 40;
  float t1, p1, v1, v2, v3;

  gaze.DeIntersect(x1,y1,v1,v2,v3);

  gaze.DeIntersect(x1,y1,t1,p1);
  printf("%g %g // %g %g %g\n", x1, y1, v1, v2, v3);
  printf("%g %g // %g %g\n", x1, y1, t1, p1);
  gaze.Intersect(t1,p1,x1,y1);
  printf("%g %g // %g %g\n", x1, y1, t1, p1);
  gaze.DeIntersect(x1,y1,t1,p1);
  printf("%g %g // %g %g\n", x1, y1, t1, p1);
}

