
#include "YARPClimber.h"


double YARPClimber::Apply(YARPEvaluator& eval, double x0, double x1, 
			  double steps, int recurse)
{
  //printf("%d %g %g\n", recurse, x0, x1);
  double xbest = (x0+x1)/2;
  double vbest = eval.Apply(xbest);
  if (x0>x1)
    {
      double tmp = x0;
      x0 = x1;
      x1 = tmp;
    }
  if (recurse>0)
    {
      double dstep = (x1-x0)/steps;
      double x = x0;
      for (int i=0; i<steps; i++)
	{
	  x += dstep;
	  double v = eval.Apply(x);
	  if (v<vbest)
	    {
	      xbest = x;
	      vbest = v;
	    }
	}
      xbest = Apply(eval,xbest-dstep,xbest+dstep,steps,recurse-1);
    }
  return xbest;
}


